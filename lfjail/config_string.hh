/*
 * This source code is released into the public domain.
 */

#ifndef	LFJAIL_CONFIG_STRING_HH
#define	LFJAIL_CONFIG_STRING_HH

#include "config_value.hh"

namespace lfjail::config {

struct string_option final : value {
	// Create a required configuration parameter (no default).
	string_option(std::string_view name,
		      std::string_view description);

	// Create a configuration parameter with a default value.
	string_option(std::string_view name,
		      std::string_view description,
		      std::string_view default_value);

	virtual auto get_string() const -> std::string override;
	virtual void set_string(std::string_view value) override;

	virtual void add_to_ucl(ucl_object_t *) const override;

private:
	std::string _value;
};

} // namespace lfjail::config

#endif	// !LFJAIL_CONFIG_STRING_HH
