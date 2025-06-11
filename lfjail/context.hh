/*
 * This source code is released into the public domain.
 */

#ifndef	LFJAIL_CONTEXT_HH
#define	LFJAIL_CONTEXT_HH

namespace lfjail {

/*
 * The runtime context.  This is mainly changed via command-line options.
 */
struct context {
	// Path to the database directory (-d).
	std::string dbdir = LFJAIL_DBDIR;

	// Print more detailed messages (-v).
	bool verbose = false;

	// Return the path for a particular file, relative to dbdir.
	auto path(this context const &, std::string_view what) -> std::string;
};

} // namespace lfjail

#endif	// !LFJAIL_CONTEXT_HH
