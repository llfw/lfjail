/*
 * This source code is released into the public domain.
 */

#include <atf-c++.hpp>

#include "exec.hh"

#define TEST_CASE(name)				\
	ATF_TEST_CASE_WITHOUT_HEAD(name)	\
	ATF_TEST_CASE_BODY(name)

TEST_CASE(basic) {
	std::string sout, serr;
	int ret = lfjail::cexecl(
		std::back_inserter(sout),
		std::back_inserter(serr),
		"/bin/sh",
		"sh", "-c", "echo test");

	ATF_REQUIRE_EQ(sout, "test\n");
	ATF_REQUIRE_EQ(serr, "");
	ATF_REQUIRE_EQ(ret, 0);
}

TEST_CASE(exit) {
	std::string sout, serr;
	int ret = lfjail::cexecl(
		std::back_inserter(sout),
		std::back_inserter(serr),
		"/bin/sh",
		"sh", "-c", "exit 47");

	ATF_REQUIRE_EQ(sout, "");
	ATF_REQUIRE_EQ(serr, "");
	ATF_REQUIRE_EQ(ret, 47);
}

TEST_CASE(serr) {
	std::string sout, serr;
	int ret = lfjail::cexecl(
		std::back_inserter(sout),
		std::back_inserter(serr),
		"/bin/sh",
		"sh", "-c", "echo test >&2");

	ATF_REQUIRE_EQ(sout, "");
	ATF_REQUIRE_EQ(serr, "test\n");
	ATF_REQUIRE_EQ(ret, 0);
}

ATF_INIT_TEST_CASES(tcx) {
	ATF_ADD_TEST_CASE(tcx, basic);
	ATF_ADD_TEST_CASE(tcx, exit);
	ATF_ADD_TEST_CASE(tcx, serr);
}
