/*
 * This source code is released into the public domain.
 */

#include <string>

#include <atf-c++.hpp>

#include "string_utils.hh"

using namespace std::literals;
using namespace lfjail;

#define TEST_CASE(name)				\
	ATF_TEST_CASE_WITHOUT_HEAD(name)	\
	ATF_TEST_CASE_BODY(name)

TEST_CASE(skipws) {
	std::string_view text;

	text = "foo"sv;
	skipws(&text);
	ATF_REQUIRE_EQ(text, "foo");

	text = "  \tfoo"sv;
	skipws(&text);
	ATF_REQUIRE_EQ(text, "foo");

	text = "   \t";
	skipws(&text);
	ATF_REQUIRE_EQ(text, "");

	text = "";
	skipws(&text);
	ATF_REQUIRE_EQ(text, "");
}

TEST_CASE(next_word) {
	std::string_view text, word;

	text = "foo    bar baz "sv;
	ATF_REQUIRE_EQ(text, "foo    bar baz ");

	word = next_word(&text);
	ATF_REQUIRE_EQ(word, "foo");

	word = next_word(&text);
	ATF_REQUIRE_EQ(word, "bar");

	word = next_word(&text);
	ATF_REQUIRE_EQ(word, "baz");

	word = next_word(&text);
	ATF_REQUIRE_EQ(word, "");
}

ATF_INIT_TEST_CASES(tcx) {
	ATF_ADD_TEST_CASE(tcx, skipws);
	ATF_ADD_TEST_CASE(tcx, next_word);
}
