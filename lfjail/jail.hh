/*
 * This source code is released into the public domain.
 */

#ifndef	LFJAIL_JAIL_HH
#define	LFJAIL_JAIL_HH

#include <optional>
#include <string>

#include "context.hh"

namespace lfjail {

struct jail {
	std::string name;
	std::string root_path;
};

// Fetch the configuration for a jail by name.  If the jail doesn't exist,
// returns nothing.  If another error occurs loading the config, throws
// an exception.
std::optional<jail> get_jail_config(context const &, std::string_view name);

// Write (or overwrite) the configuration for a jail by name.
void set_jail_config(context const &, jail const &);

// Return true if a jail by the given name exists.
bool jail_exists(context const &, std::string_view name);

// Destroy a jail.  This removes the configuration file and its datasets.
void jail_destroy(context const &, jail const &);

} // namespace lfjail

#endif	// !LFJAIL_JAIL_HH
