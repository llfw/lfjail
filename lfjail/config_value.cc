/*
 * This source code is released into the public domain.
 */

#include "config_error.hh"
#include "config_store.hh"
#include "config_value.hh"

namespace lfjail::config {

value::value(std::string_view name_,
	     std::string_view description_,
	     bool is_required_)
	: name(name_)
	, description(description_)
	, is_required(is_required_)
{
	store::get().create(this);
}

auto value::integer(this value const &self) -> std::int64_t {
	return self.get_integer();
}

void value::integer(this value &self, std::int64_t value) {
	self.set_integer(value);
	self.is_default = false;
}

auto value::string(this value const &self) -> std::string {
	return self.get_string();
}

void value::string(this value &self, std::string_view value) {
	self.set_string(value);
	self.is_default = false;
}

auto value::get_integer() const -> std::int64_t {
	throw error("cannot get '{}' as an integer value", name);
}

void value::set_integer(std::int64_t) {
	throw error("cannot set '{}' to an integer value", name);
}

auto value::get_string() const -> std::string {
	throw error("cannot get '{}' as a string value", name);
}

void value::set_string(std::string_view) {
	throw error("cannot set '{}' to a string value", name);
}

} // namespace lfjail::config
