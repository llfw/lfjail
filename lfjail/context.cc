/*
 * This source code is released into the public domain.
 */

#include <format>
#include <string>

#include "context.hh"

namespace lfjail {

auto context::path(this context const &self, std::string_view what)
	-> std::string
{
	return std::format("{0}/{1}", self.dbdir, what);
}

} // namespace lfjail
