/*
 * This source code is released into the public domain.
 */

#include "ensure_dir.hh"
#include "jail.hh"
#include "generic_error.hh"
#include "read_file.hh"
#include "write_file.hh"
#include "zfs.hh"

#include <ucl++.h>

namespace {

using namespace lfjail;

// Get the jails storage directory.
std::string get_jails_dir(context const &ctx) {
	return ctx.path("jails");
}

// Get the config file for a jail.
std::string jail_config_file(context const &ctx, std::string_view jailname) {
	return std::format("{}/{}.ucl", get_jails_dir(ctx), jailname);
}

// Make sure our jails config path exists.
void ensure_jails_dir(context const &ctx) {
	ensure_dir(ctx, get_jails_dir(ctx));
}

} // anonymous namespace

namespace lfjail {

std::optional<jail>
get_jail_config(context const &ctx, std::string_view jailname) {
	ensure_jails_dir(ctx);

	auto file = jail_config_file(ctx, jailname);

	// Load the configuration text.
	std::string config_text;
	try {
		read_file(file, std::back_inserter(config_text));
	} catch (file_read_error const &exc) {
		if (exc.error == ENOENT)
			return {};
		throw;
	}

	// Parse the UCL.

	std::string err;
	auto uclconfig = ucl::Ucl::parse(config_text, err);
	jail ret;
	ret.name = jailname;

	if (!uclconfig)
		throw generic_error("{0}: {1}", file, err);

	for (auto const &uclvalue : uclconfig) {
		throw generic_error("{}: unknown option '{}'",
				    file, uclvalue.key());
	}

	return {ret};
}

void set_jail_config(context const &ctx, jail const &jailconf) {
	// The UCL C++ API doesn't seem to support creating new objects
	// from scratch, so we use the C API here.

	auto ucl = ::ucl_object_typed_new(UCL_OBJECT);
	auto ucl_guard = guard([ucl] { ::ucl_object_unref(ucl); });

	// We could add some jail options here...

	// Dump the UCL object to a string.
	auto *ucl_c_text = reinterpret_cast<char *>(
				::ucl_object_emit(ucl, UCL_EMIT_CONFIG));
	auto ucl_text_guard = guard([ucl_c_text] { ::free(ucl_c_text); });
	std::string ucl_text(ucl_c_text);

	// Write the object to a file.
	auto file = jail_config_file(ctx, jailconf.name);

	safe_write_file(file, ucl_text);
}

bool jail_exists(context const &ctx, std::string_view name) {
	auto jail = get_jail_config(ctx, name);
	if (jail)
		return true;
	else
		return false;
}

void jail_destroy(context const &ctx, jail const &jailconf) {
	auto file = jail_config_file(ctx, jailconf.name);

	::unlink(file.c_str());
	zfs::destroy_for_jail(ctx, jailconf);
}

} // namespace lfjail
