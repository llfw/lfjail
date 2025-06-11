/*
 * This source code is released into the public domain.
 */

#ifndef	LFJAIL_COMMAND_MAP_HH
#define	LFJAIL_COMMAND_MAP_HH

/*
 * command_map represents a hierarchical list of commands.  At each level,
 * a command is mapped to a handler, which can either be a function, in
 * which case we execute the function, or another command_map, in which
 * case we invoke the new map
 */

#include <functional>
#include <iostream>
#include <string>
#include <map>
#include <print>

#include "context.hh"
#include "string_utils.hh"
#include "usage_error.hh"

namespace lfjail::command {

using func = std::function<int (context const &, int, char **)>;

// A node in the command hierarchy.
struct node {
	// The command name of this node.
	std::string command;

	// Handler for this node.  May be null, which means this node has
	// sub-commands but isn't a command itself.
	func handler;

	node(std::string command);

	// Run the handler for this node.
	int invoke(context const &ctx, int argc, char **argv) const;

	// Create a new node under this one, or return it if it already exists.
	// If path is empty, return this node.
	node &create_node(std::string_view path);

	void print_usage(std::string prefix = "") const;

private:
	std::map<std::string_view, node> commands;
};

// Get the root command node singleton.
node &get_root_node();

// Declare a global command and add it to the root node.
struct command {
	command(std::string_view path, func fn);
};

} // namespace lfjail

#endif	// !LFJAIL_COMMAND_MAP_HH
