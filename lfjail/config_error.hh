/*
 * This source code is released into the public domain.
 */

#ifndef	LFJAIL_CONFIG_ERROR_HH
#define	LFJAIL_CONFIG_ERROR_HH

#include <format>
#include <stdexcept>

namespace lfjail::config {

/*
 * Exception thrown when an issue occurs with the configuration.
 */
struct error : std::runtime_error {
	template<typename... Args>
	error(std::format_string<Args...> fmt, Args &&...args)
		: std::runtime_error(std::format(fmt, std::forward<Args>(args)...))
	{}
};

struct unknown_setting final : error {
	std::string varname;

	unknown_setting(std::string_view varname_)
		: error("unknown configuration variable '{0}'", varname_)
		, varname(varname_)
	{}
};

} // namespace lfjail::config

#endif	// !LFJAIL_CONFIG_ERROR_HH
