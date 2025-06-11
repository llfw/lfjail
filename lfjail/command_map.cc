/*
 * This source code is released into the public domain.
 */

#include "command_map.hh"

namespace lfjail::command {

node &get_root_node() {
	static node root_node("");
	return root_node;
}

node::node(std::string command_)
	: command(command_)
{}

int node::invoke(context const &ctx, int argc, char **argv) const {
	// Look for a subcommand with argv[0].
	auto it = commands.find(argv[0]);
	if (it == commands.end()) {
		std::print(std::cerr, "{}: unknown command\n", ::getprogname());
		throw usage_error();
	}

	auto const &child = it->second;

	// If the child has a handler, invoke it.
	if (child.handler)
		return child.handler(ctx, argc, argv);

	--argc;
	++argv;

	if (argc == 0) {
		std::print(std::cerr, "{}: incomplete command\n", ::getprogname());
		throw usage_error();
	}

	// Otherwise, continue recursing.
	return child.invoke(ctx, argc, argv);
}

node &node::create_node(std::string_view path) {
	auto child = next_word(&path);
	if (child.empty())
		return *this;

	// If the child node doesn't exist, insert an empty node.
	auto it = commands.find(child);
	if (it == commands.end()) {
		std::tie(it, std::ignore) =
			commands.insert(std::pair{child, node(std::string(child))});
	}

	if (path.empty())
		return it->second;

	return it->second.create_node(path);
}

void node::print_usage(std::string prefix) const {
	if (handler)
		std::print("{}{}\n", prefix, command);

	for (auto const &it : commands)
		it.second.print_usage(prefix + command + " ");
}

command::command(std::string_view path, func fn) {
	auto &node = get_root_node().create_node(path);
	node.handler = fn;
}

} // namespace lfjail
