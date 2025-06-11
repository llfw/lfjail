/*
 * This source code is released into the public domain.
 */

#ifndef	LFJAIL_EXEC_HH
#define	LFJAIL_EXEC_HH

#include <concepts>
#include <format>
#include <functional>
#include <span>

namespace lfjail {

namespace detail {

/*
 * Internal implementation of cexec().
 */

using cexec_write_fn = std::function<void (std::span<char>)>;

int cexec(cexec_write_fn outwriter,
	  cexec_write_fn errwriter,
	  std::string const &cmd,
	  std::vector<std::string> args);

} // namespace detail

/*
 * Run a command with the given argument vector, write its output and error to
 * the given output iterator, and return its exit status.
 */
int cexec(std::output_iterator<char> auto &&outit,
	  std::output_iterator<char> auto &&errit,
	  std::string const &cmd,
	  std::vector<std::string> argv)
{
	auto out_writer = [&outit] (std::span<char> chars) {
		std::ranges::copy(chars, outit);
	};

	auto err_writer = [&errit] (std::span<char> chars) {
		std::ranges::copy(chars, errit);
	};

	return detail::cexec(out_writer, err_writer, cmd, argv);
}

template<typename... Args>
int cexecl(std::output_iterator<char> auto &&outit,
	   std::output_iterator<char> auto &&errit,
	   std::string const &cmd,
	   Args&& ...args) {

	std::vector<std::string> argv;
	(argv.push_back(std::format("{}", std::forward<Args>(args))), ...);

	return cexec(std::forward<decltype(outit)>(outit),
		     std::forward<decltype(errit)>(errit),
		     cmd, argv);
}

} // namespace lfjail

#endif	// LFJAIL_EXEC_HH
