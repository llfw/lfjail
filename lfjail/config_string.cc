/*
 * This source code is released into the public domain.
 */

#include "config_string.hh"

namespace lfjail::config {

/*
 * config::string, string values.
 */

string_option::string_option(std::string_view name_,
			     std::string_view description_)
	: value(name_, description_, true)
	, _value("")
{
}

string_option::string_option(std::string_view name_,
			     std::string_view description_,
			     std::string_view default_value)
	: value(name_, description_, false)
	, _value(std::from_range, default_value)
{
}

auto string_option::get_string() const -> std::string {
	return _value;
};

void string_option::set_string(std::string_view new_value) {
	_value = new_value;
}

void string_option::add_to_ucl(ucl_object_t *ucl) const {
	auto ucl_value = ucl_object_fromstring_common(
				_value.data(), _value.size(),
				UCL_STRING_RAW);
	ucl_object_insert_key(ucl, ucl_value,
			      name.data(), name.size(), true);
}

} // namespace lfjail::config
