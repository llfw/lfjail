/*
 * This source code is released into the public domain.
 */

#ifndef	LFJAIL_ZFS_HH
#define	LFJAIL_ZFS_HH

#include "config_string.hh"
#include "context.hh"
#include "jail.hh"

namespace lfjail::zfs {

// Create ZFS filesystems for the given jail.
void create_for_jail(context const &ctx, jail const &);

// Destroy ZFS filesystems for the given jail.
void destroy_for_jail(context const &ctx, jail const &);

// Return the Unix path under which jails will be created.
// This is the mountpoint of the ZFS filesystem 'filesystem'.
std::string jail_root(context const &ctx);

} // namespace lfjail::zfs

#endif	// LFJAIL_ZFS_HH
