/*
 * This source code is released into the public domain.
 */

#ifndef	LFJAIL_READ_FILE_HH
#define	LFJAIL_READ_FILE_HH

#include <array>
#include <format>
#include <span>
#include <stdexcept>
#include <string>

#include <fcntl.h>
#include <unistd.h>

#include "guard.hh"

namespace lfjail {

/*
 * Load the contents of a file into an output iterator.  Throws file_read_error
 * on failure.
 */

struct file_read_error final : std::runtime_error {
	std::string filename;
	int error;

	file_read_error(std::string filename_, int error_)
		: std::runtime_error(std::format("{0}: {1}", filename_, error_))
		, filename(filename_)
		, error(error_)
	{}
};

void read_file(std::string filename,
	       std::output_iterator<char> auto &&iter)
{
	constexpr std::size_t bufsize = 1024;
	std::array<char, bufsize> buffer;
	int fd, err;

	if ((fd = ::open(filename.c_str(), O_RDONLY)) == -1)
		throw file_read_error(filename, errno);

	auto fd_guard = guard([fd] { ::close(fd); });

	while ((err = ::read(fd, &buffer[0], buffer.size())) > 0) {
		auto data = std::span(buffer).subspan(0, err);
		std::ranges::copy(data, iter);
	}

	if (err == -1)
		throw file_read_error(filename, errno);
}

} // namespace lfjail

#endif	// !LFJAIL_READ_FILE_HH
