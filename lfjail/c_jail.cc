/*
 * This source code is released into the public domain.
 */

#include <stdlib.h>

#include "command_map.hh"
#include "jail.hh"
#include "zfs.hh"

using namespace std::literals;
using namespace lfjail;

namespace {

command::command j_show("show"sv,
			[](context const &ctx, int argc, char **argv) {
	if (argc != 2)
		throw usage_error();

	auto jail = get_jail_config(ctx, argv[1]);
	if (!jail) {
		std::print(std::cerr, "{}: jail does not exist: {}\n",
			   ::getprogname(), argv[1]);
		return 1;
	}

	std::print("name: {}\n", jail->name);
	return 0;
});

command::command j_create("create"sv,
			  [](context const &ctx, int argc, char **argv) {
	if (argc != 2)
		throw usage_error();

	auto jailconf = jail{};
	jailconf.name = argv[1];

	if (jail_exists(ctx, argv[1])) {
		std::print(std::cerr, "{}: jail already exists: {}\n",
			   ::getprogname(), jailconf.name);
		return 1;
	}

	zfs::create_for_jail(ctx, jailconf);

	set_jail_config(ctx, jailconf);
	return 0;
});

command::command j_destroy("destroy"sv,
			   [](context const &ctx, int argc, char **argv) {
	if (argc != 2)
		throw usage_error();

	auto jailname = argv[1];
	auto jail = get_jail_config(ctx, jailname);

	if (!jail) {
		std::print(std::cerr, "{}: jail does not exist: {}\n",
			   ::getprogname(), jailname);
		return 1;
	}

	jail_destroy(ctx, *jail);
	return 0;
});

} // anonymous namespace
