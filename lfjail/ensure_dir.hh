/*
 * This source code is released into the public domain.
 */

#ifndef	LFJAIL_ENSURE_DIR_HH
#define	LFJAIL_ENSURE_DIR_HH

#include <string>

#include "context.hh"

namespace lfjail {

void ensure_dir(context const &ctx, std::string_view dir);

} // namespace lfjail

#endif	// !LFJAIL_ENSURE_DIR_HH
