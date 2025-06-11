/*
 * This source code is released into the public domain.
 */

#ifndef	LFJAIL_USAGE_ERROR_HH
#define	LFJAIL_USAGE_ERROR_HH

namespace lfjail {

struct usage_error final : std::runtime_error {
	usage_error() : std::runtime_error("invalid usage") {}
};

} // namespace lfjail

#endif	// !LFJAIL_USAGE_ERROR_HH
