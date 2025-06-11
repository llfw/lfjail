/*
 * This source code is released into the public domain.
 */

#ifndef	LFJAIL_WRITE_FILE_HH
#define	LFJAIL_WRITE_FILE_HH

#include <ranges>
#include <stdexcept>
#include <vector>

#include <fcntl.h>
#include <unistd.h>

namespace lfjail {

/*
 * Write the contents of a range to a file.  Throws file_write_error on error.
 */

struct file_write_error final : std::runtime_error {
	std::string filename;
	int error;

	file_write_error(std::string filename_, int error_)
		: std::runtime_error(std::format("{0}: {1}",
						 filename_,
						 ::strerror(error_)))
		, filename(filename_)
		, error(error_)
	{}
};

void write_file(std::string filename, std::ranges::range auto &&range) {
	// Convert the range into an array.
	std::vector<char> chars;
	std::ranges::copy(range, std::back_inserter(chars));

	// Open the output file.
	int fd;

	if ((fd = ::open(filename.c_str(), O_CREAT|O_WRONLY, 0777)) == -1)
		throw file_write_error(filename, errno);

	auto fd_guard = guard([fd] { ::close(fd); });

	// Write the data.
	if (int err = ::write(fd, chars.data(), chars.size()); err < 0)
		throw file_write_error(filename, errno);
}

/*
 * Write the contents of a range to a file safely.  The data will be written
 * to "<filename>.tmp", and if the write succeeds, the temporary file will be
 * renamed to the target filename.  If an error occurs, the target file will
 * not be modified.
 */
void safe_write_file(std::string filename, std::ranges::range auto &&range) {
	auto tmpfile = filename + ".tmp";
	auto tmpfile_guard = guard([tmpfile] { ::unlink(tmpfile.c_str()); });

	write_file(tmpfile, range);
	int err = ::rename(tmpfile.c_str(), filename.c_str());
	if (err != 0)
		throw file_write_error(filename, errno);
}

} // namespace lfjail

#endif	// LFJAIL_WRITE_FILE_HH
