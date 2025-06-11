/*
 * This source code is released into the public domain.
 */

#include <sys/stat.h>

#include <cerrno>
#include <iostream>
#include <print>

#include "ensure_dir.hh"
#include "generic_error.hh"

namespace lfjail {

void ensure_dir(context const &ctx, std::string_view dir) {
	auto cdir = std::string(dir);
	struct ::stat st;

	int err = ::stat(cdir.c_str(), &st);
	if (err != 0) {
		if (errno != ENOENT)
			throw generic_error("{}: stat: {}",
					    dir, ::strerror(errno));

		err = ::mkdir(cdir.c_str(), 0700);
		if (err != 0)
			throw generic_error("{}: mkdir: {}",
					    dir, ::strerror(errno));

		if (ctx.verbose)
			std::print(std::cerr, "{}: created {}\n",
				   getprogname(), dir);
	}
}

} // namespace lfjail
