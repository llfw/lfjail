/*
 * This source code is released into the public domain.
 */

#ifndef	LFJAIL_GENERIC_ERROR_HH
#define	LFJAIL_GENERIC_ERROR_HH

#include <format>
#include <stdexcept>

namespace lfjail {

struct generic_error : std::runtime_error {
	template<typename... Args>
	generic_error(std::format_string<Args...> fmt, Args &&...args)
		: std::runtime_error(std::format(fmt, std::forward<Args>(args)...))
	{}
};

} // namespace lfjail

#endif	// !LFJAIL_GENERIC_ERROR_HH
