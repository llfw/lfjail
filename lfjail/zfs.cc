/*
 * This source code is released into the public domain.
 */

/*
 * ZFS handling.  It would be nice 
 */

#include "config_string.hh"
#include "exec.hh"
#include "generic_error.hh"
#include "zfs.hh"

using namespace lfjail;
using namespace std::literals;

namespace {

config::string_option filesystem(
	"zfs.filesystem"sv,
	"The ZFS filesystem under which jails are created"sv);

config::string_option mountpoint(
	"zfs.mountpoint"sv,
	"The location where the ZFS hierarchy will be mounted"sv);

/*
 * Test if the given ZFS dataset exists.
 */
auto dataset_exists(std::string const &ds) -> bool {
	auto sout = std::string();
	auto serr = std::string();

	int ret = lfjail::cexecl(
		std::back_inserter(sout),
		std::back_inserter(serr),
		"/sbin/zfs",
		"zfs", "get", "-H", "-ovalue", "name", ds);

	return (ret == 0);
}

/*
 * Create a new ZFS dataset.
 */
template<typename... Args>
void dataset_create(std::string const &ds, Args&& ...args) {
	std::vector<std::string> argv;
	argv.push_back("zfs");
	argv.push_back("create");
	(argv.push_back(std::format("-o{}", std::forward<Args>(args))), ...);
	argv.push_back(ds);

	auto sout = std::string();
	auto serr = std::string();

	int ret = lfjail::cexec(std::back_inserter(sout),
				std::back_inserter(serr),
				"/sbin/zfs", argv);

	if (!serr.empty())
		throw generic_error("zfs create failed: {}", serr);
	if (ret != 0)
		throw generic_error("zfs create failed: {}", ret);
}

/*
 * Destroy a ZFS datset.
 */
template<typename... Args>
void dataset_destroy(std::string const &ds) {
	auto sout = std::string();
	auto serr = std::string();

	int ret = lfjail::cexecl(std::back_inserter(sout),
				 std::back_inserter(serr),
				 "/sbin/zfs", "zfs", "destroy", "-r", ds);

	if (!serr.empty())
		throw generic_error("zfs destroy failed: {}", serr);
	if (ret != 0)
		throw generic_error("zfs destroy failed: {}", ret);
}

/*
 * Return a ZFS properly for a fileystem.
 */
auto get_property(std::string const &fs, std::string const &prop)
	-> std::string
{
	auto sout = std::string();
	auto serr = std::string();

	int ret = lfjail::cexecl(
		std::back_inserter(sout),
		std::back_inserter(serr),
		"/sbin/zfs",
		"zfs", "get", "-H", "-ovalue", prop, fs);

	if (!serr.empty())
		throw generic_error("zfs get failed: {}", serr);
	if (ret != 0)
		throw generic_error("zfs get failed: {}", ret);
	if (sout[sout.size() - 1] == '\n')
		sout.resize(sout.size() - 1);
	return sout;
}

/*
 * Make sure our top-level ZFS dataset exists.
 */
void ensure_jroot(context const &) {
	auto jroot = filesystem.string();
	auto mntpt = mountpoint.string();

	auto mntptopt = std::format("mountpoint={}", mntpt);
	if (dataset_exists(jroot))
		return;

	dataset_create(jroot, mntptopt);
}

} // anonymous namespace

namespace lfjail::zfs {

void create_for_jail(context const &ctx, jail const &jailconf) {
	if (filesystem.string().empty())
		throw generic_error("zfs.filesystem is not set");

	if (mountpoint.string().empty())
		throw generic_error("zfs.mountpoint is not set");

	ensure_jroot(ctx);
	std::string ds = filesystem.string() + "/" + jailconf.name;
	dataset_create(ds);
}

void destroy_for_jail(context const &, jail const &jailconf) {
	std::string ds = filesystem.string() + "/" + jailconf.name;
	dataset_destroy(ds);
}

std::string jail_root(context const &ctx) {
	std::ignore = ctx;

	auto fs = filesystem.string();
	if (fs.empty())
		throw generic_error("zfs.filesystem not set");

	return get_property(fs, "mountpoing");
}

} // namespace lfjail::zfs
