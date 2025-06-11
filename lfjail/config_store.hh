/*
 * This source code is released into the public domain.
 */

#ifndef	LFJAIL_CONFIG_STORE_HH
#define	LFJAIL_CONFIG_STORE_HH

#include <map>
#include <string>

#include "config_value.hh"
#include "context.hh"
#include "generator.hh"

namespace lfjail::config {

/*
 * The configuration store.  There should only be one of these.
 */

struct store final {
	// Initialise the global configuration.
	static auto init(context const &) -> void;

	// Get the global config store.
	static auto get() -> store&;

	// Create a new config value.
	auto create(this store &, value *) -> void;

	// Fetch a config value.
	auto fetch(this store const &, std::string_view name)
		-> value &;

	// Fetch all values.
	auto fetch_all(this store const &) 
		-> std::generator<value const &>;
	
	// Write all values to the config store.  Throws config_error.
	void write_all(this store const &, context const &);

	store(store const &) = delete;
	store(store &&) = delete;
	store& operator=(store const &) = delete;
	store& operator=(store &&) = delete;

private:
	std::map<std::string_view, value *> options;
	store() = default;
};

} // namespace lfjail::config

#endif	// LFJAIL_CONFIG_STORE_HH
