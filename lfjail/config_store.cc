/*
 * This source code is released into the public domain.
 */

#include <string>

#include <fcntl.h>
#include <unistd.h>

#include <ucl++.h>

#include "config_error.hh"
#include "config_store.hh"
#include "guard.hh"
#include "read_file.hh"
#include "write_file.hh"

namespace {

/*
 * The global configuration store, created by config::init() and accessed via
 * config::get().
 */
lfjail::config::store *config_store = nullptr;

} // anonymous namespace

namespace lfjail::config {

/*
 * Initialise the global config store.
 */
void store::init(context const &ctx) {
	std::string config_text;

	// Load the configuration text.
	auto config_path = ctx.path("config.ucl");
	try {
		read_file(config_path, std::back_inserter(config_text));
	} catch (file_read_error const &exc) {
		// Ignore ENOENT, it simply means we haven't created the
		// config file yet, so default values will be used.
		if (exc.error == ENOENT)
			return;
		throw;
	}

	// Parse the UCL.

	std::string err;
	auto uclconfig = ucl::Ucl::parse(config_text, err);

	if (!uclconfig)
		throw error("{0}: {1}", config_path, err);

	store &cfg = get();
	for (auto const &uclvalue : uclconfig) {
		auto &value = cfg.fetch(uclvalue.key());

		switch (uclvalue.type()) {
		case UCL_INT:
			value.integer(uclvalue.int_value());
			break;
		case UCL_STRING:
			value.string(uclvalue.string_value());
			break;
		default:
			throw error(
				"INTERNAL ERROR: unknown value type {0}",
				static_cast<int>(uclvalue.type()));
		}
	}
}

/*
 * Get the global config store.  We expect init() has already been called.
 */
auto store::get() -> store& {
	if (config_store == nullptr)
		config_store = new store;

	return *config_store;
}

/*
 * Register a new value with the config store.
 */
auto store::create(this store &self, value *object) -> void {
	auto [it, okay] = self.options.insert(std::pair{object->name, object});

	if (!okay)
		throw error(
			"INTERNAL ERROR: attempt to register duplicate config"
			" value '{0}'", object->name);
}

/*
 * Fetch an existing value in the config store.
 */
auto store::fetch(this store const &self, std::string_view name)
	-> value &
{
	if (auto it = self.options.find(name); it != self.options.end())
		return *it->second;
	else
		throw unknown_setting(name);
}

/*
 * Fetch all values in the configuration store.
 */
auto store::fetch_all(this store const &self) -> std::generator<value const &>
{
	for (auto const &it : self.options)
		co_yield *it.second;
}

/*
 * Write all config values (except defaults) to disk.
 */
void store::write_all(this store const &self, context const &ctx) {
	// The UCL C++ API doesn't seem to support creating new objects
	// from scratch, so we use the C API here.

	auto ucl = ::ucl_object_typed_new(UCL_OBJECT);
	auto ucl_guard = guard([ucl] { ::ucl_object_unref(ucl); });

	// Add all the options to the UCL object.
	for (auto const &option : self.fetch_all()) {
		if (option.is_default)
			continue;

		option.add_to_ucl(ucl);
	}

	// Dump the UCL object to a string.
	auto *ucl_c_text = reinterpret_cast<char *>(
				::ucl_object_emit(ucl, UCL_EMIT_CONFIG));
	auto ucl_text_guard = guard([ucl_c_text] { ::free(ucl_c_text); });
	std::string ucl_text(ucl_c_text);

	// Write the object to a file.
	auto config_path = ctx.path("config.ucl");

	try {
		safe_write_file(config_path, ucl_text);
	} catch (file_write_error const &exc) {
		throw error("{}", exc.what());
	}
}

} // namespace lfjail::config
