/*
 * This source code is released into the public domain.
 */

#include <string>
#include <vector>

#include <sys/wait.h>
#include <fcntl.h>
#include <poll.h>
#include <unistd.h>

#include "exec.hh"
#include "generic_error.hh"

namespace lfjail {

namespace detail {

void set_nonblock(int fd) {
	int flags;

	flags = fcntl(fd, F_GETFL);
	if (flags == -1)
		throw generic_error("fcntl(F_GETFL): {}", strerror(errno));

	flags |= O_NONBLOCK;
	flags = fcntl(fd, F_SETFL, flags);
	if (flags == -1)
		throw generic_error("fcntl(F_SETFL): {}", strerror(errno));
}

bool fd_copy(int fd, cexec_write_fn writer) {
	constexpr std::size_t bufsize = 1024;
	auto buf = std::array<char, bufsize>();
	int err;
	
	while ((err = ::read(fd, buf.data(), buf.size())) > 0) {
		auto data = std::span(buf).subspan(0, err);
		writer(data);
	}

	if (err == 0)
		return false;

	if (errno == EWOULDBLOCK)
		return true;

	throw generic_error("read: {}", ::strerror(errno));
}

int cexec(cexec_write_fn outwriter,
	  cexec_write_fn errwriter,
	  std::string const &cmd,
	  std::vector<std::string> args)
{
	auto c_args = std::vector<char const *>();
	auto stdout_pipe = std::array<int, 2>{};
	auto stderr_pipe = std::array<int, 2>{};

	if (::pipe(&stdout_pipe[0]) != 0)
		throw generic_error("cexec: pipe: {}", strerror(errno));
	set_nonblock(stdout_pipe[0]);

	if (::pipe(&stderr_pipe[0]) != 0)
		throw generic_error("cexec: pipe: {}", strerror(errno));
	set_nonblock(stderr_pipe[0]);

	for (auto const &s : args)
		c_args.push_back(s.c_str());
	c_args.push_back(nullptr);

	auto child_pid = fork();

	if (child_pid == -1)
		// fork() failed
		throw generic_error("fork: {}", ::strerror(errno));

	if (child_pid == 0) {
		// We are in the child.
		::close(stdout_pipe[0]);
		::close(stderr_pipe[0]);

		::dup2(stdout_pipe[1], STDOUT_FILENO);
		::close(stdout_pipe[1]);
		::dup2(stderr_pipe[1], STDERR_FILENO);
		::close(stderr_pipe[1]);

		int devnull = ::open("/dev/null", O_RDONLY);
		if (devnull == -1)
			_exit(1);
		::dup2(devnull, STDIN_FILENO);
		::close(devnull);

		::execv(cmd.c_str(), const_cast<char *const *>(c_args.data()));
		_exit(1);
	}

	// We are in the parent.
	::close(stdout_pipe[1]);
	::close(stderr_pipe[1]);

	for (;;) {
		std::vector<pollfd> pollfds;

		if (stdout_pipe[0] != -1) {
			auto pfd = pollfd{};
			pfd.fd = stdout_pipe[0];
			pfd.events = POLLIN;
			pollfds.push_back(pfd);
		}

		if (stderr_pipe[0] != -1) {
			auto pfd = pollfd{};
			pfd.fd = stderr_pipe[0];
			pfd.events = POLLIN;
			pollfds.push_back(pfd);
		}

		if (pollfds.empty())
			break;

		int err = ::poll(&pollfds[0], pollfds.size(), 0);
		if (err == -1)
			throw generic_error("poll: {}", ::strerror(errno));

		for (auto const &pfd : pollfds) {
			if ((pfd.revents & (POLLIN|POLLHUP|POLLERR)) == 0)
				continue;

			if (pfd.fd == stdout_pipe[0]) {
				bool ret = fd_copy(pfd.fd, outwriter);
				if (!ret) {
					::close(stdout_pipe[0]);
					stdout_pipe[0] = -1;
				}
			} else if (pfd.fd == stderr_pipe[0]) {
				bool ret = fd_copy(pfd.fd, errwriter);
				if (!ret) {
					::close(stderr_pipe[0]);
					stderr_pipe[0] = -1;
				}
			}
		}
	}

	int status;
	waitpid(child_pid, &status, WEXITED);
	return WEXITSTATUS(status);
}

} // namespace detail

} // namespace lfjail
