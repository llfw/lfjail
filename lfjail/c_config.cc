/*
 * This source code is released into the public domain.
 */

// c_config: the 'lfjail config' command

#include <iostream>
#include <print>
#include <string>

#include <unistd.h>

#include "command_map.hh"
#include "config_error.hh"
#include "config_store.hh"
#include "context.hh"
#include "usage_error.hh"

using namespace std::literals;
using namespace lfjail;
using namespace lfjail::config;

namespace {

command::command config_show("config show"sv,
			     [](context const &, int argc, char **) {
	if (argc != 1)
		throw usage_error();

	for (auto const &value : store::get().fetch_all()) {
		std::print("# {}\n", value.description);
		if (value.is_default)
			std::print("#{} = {}\n", value.name, value.string());
		else
			std::print("{} = {}\n", value.name, value.string());
	}

	return 0;
});

command::command config_set("config set"sv,
			    [](context const &ctx, int argc, char **argv) {
	if (argc != 3)
		throw usage_error();

	auto name = std::string(argv[1]);
	auto value = std::string(argv[2]);

	auto &store = store::get();

	try {
		auto &var = store.fetch(name);
		var.string(value);
		store.write_all(ctx);
	} catch (error const &exc) {
		std::print(std::cerr, "{}\n", exc.what());
		return 1;
	}

	return 0;
});

command::command config_unset("config unset"sv,
			      [](context const &ctx, int argc, char **argv) {
	if (argc != 2)
		throw usage_error();

	auto name = std::string(argv[1]);

	auto &store = store::get();

	// "unset" a config variable by setting is_default=true, which will
	// cause it to not be written when we call write_all().
	try {
		auto &var = store.fetch(name);
		var.is_default = true;
		store.write_all(ctx);
	} catch (error const &exc) {
		std::print(std::cerr, "{}\n", exc.what());
		return 1;
	}

	return 0;
});

} // anonymous namespace
