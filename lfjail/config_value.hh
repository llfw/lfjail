/*
 * This source code is released into the public domain.
 */

#ifndef	LFJAIL_CONFIG_VALUE_HH
#define	LFJAIL_CONFIG_VALUE_HH

#include <cstdint>
#include <string>

#include <ucl++.h>

namespace lfjail::config {

// config_value is a single configuration option.  Creating a config_value
// automatically registers it in the configuration store.

struct value {
	// Short name of this option.
	std::string_view name;

	// Human-readable description of this option.
	std::string_view description;

	// If true, this option must be set before using lfjail
	// Does not apply to 'lfjail config' commands.
	bool is_required = false;

	// If true, this option is set to its default value.
	bool is_default = true;

	// Get or set this option as an integer.
	auto integer(this value const &) -> std::int64_t;
	void integer(this value &, std::int64_t);

	// Get or set this option as a string.
	auto string(this value const &) -> std::string;
	void string(this value &, std::string_view value);

	// Add this option to a UCL object.  This is used when writing the
	// configuration file.
	virtual void add_to_ucl(ucl_object_t *) const = 0;

protected:
	value(std::string_view name,
	      std::string_view description,
	      bool is_required);

	virtual auto get_integer() const -> std::int64_t;
	virtual void set_integer(std::int64_t);

	virtual auto get_string() const -> std::string;
	virtual void set_string(std::string_view);
};

} // namespace lfjail::config

#endif	// !LFJAIL_CONFIG_VALUE_HH
