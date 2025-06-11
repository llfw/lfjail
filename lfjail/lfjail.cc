/*
 * This source code is released into the public domain.
 */

#include <cstdlib>
#include <format>
#include <iostream>
#include <unordered_map>
#include <print>

#include <sys/stat.h>
#include <unistd.h>

#include "commands.hh"
#include "command_map.hh"
#include "config_error.hh"
#include "config_store.hh"
#include "ensure_dir.hh"
#include "usage_error.hh"

using namespace lfjail;
using namespace std::literals;

auto main(int argc, char **argv) -> int 
try {
	context ctx;

	// Make sure argv[0] is set so we can call setprogname();
	if (argc < 1)
		throw usage_error();

	::setprogname(argv[0]);

	// Parse command-line arguments.
	int ch;
	while ((ch = getopt(argc, argv, "d:v")) != -1) {
		switch (ch) {
		case 'd':
			ctx.dbdir = ::optarg;
			break;
		case 'v':
			ctx.verbose = true;
			break;
		case '?':
		default:
			throw usage_error();
		}
	}
	argc -= ::optind;
	argv += ::optind;

	// Make sure our database directory exists.
	ensure_dir(ctx, ctx.dbdir);

	// Load the configuration.
	config::store::init(ctx);

	// Dispatch the top-level command.
	return command::get_root_node().invoke(ctx, argc, argv);

} catch (usage_error const &) {
	command::get_root_node().print_usage("usage: lfjail");
	return 1;

} catch (config::error const &exc) {
	std::print(std::cerr, "{0}\n", exc.what());
	return 1;

} catch (std::runtime_error const &exc) {
	std::print(std::cerr, "{0}\n", exc.what());
	return 1;
}

