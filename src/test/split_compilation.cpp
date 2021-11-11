/**
 * Copyright (C) 2021 Jessica James.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Written by Jessica James <jessica.aj@outlook.com>
 */

/**
* Tests which exist solely test test compilation for split.hpp, because MSVC demands /bigobj, and I don't want to
*/

#include "jessilib/split.hpp"
#include <deque>
#include <list>
#include "test_split.hpp"

using namespace jessilib;
using namespace std::literals;

using char_types = ::testing::Types<char, unsigned char, signed char, wchar_t, char8_t, char16_t, char32_t>;

template<typename T>
class SplitCompilationTest : public ::testing::Test {
public:
};
TYPED_TEST_SUITE(SplitCompilationTest, char_types);

/** Some basic tests for _view variants */

TYPED_TEST(SplitCompilationTest, empty) {
	// string_view w/ short delimiter
	std::basic_string_view<TypeParam> empty;
	auto delim = default_delim<TypeParam>;
	std::vector<decltype(empty)> split_result = split_view(empty, delim);
	EXPECT_TRUE(split_result.empty());
}

TYPED_TEST(SplitCompilationTest, empty_long) {
	// string_view w/ long delimiter
	std::basic_string_view<TypeParam> empty;
	auto delim = make_delim_long<TypeParam>();
	EXPECT_TRUE(split_view(empty, delim).empty());
	EXPECT_TRUE(split_once_view(empty, delim).first.empty());
}

TYPED_TEST(SplitCompilationTest, empty_str) {
	// string w/ short delimiter
	std::basic_string<TypeParam> empty;
	auto delim = default_delim<TypeParam>;
	EXPECT_TRUE(split_view(empty, delim).empty());
	EXPECT_TRUE(split_once_view(empty, delim).first.empty());
}

TYPED_TEST(SplitCompilationTest, empty_str_long) {
	// string w/ long delimiter
	std::basic_string<TypeParam> empty;
	auto delim = make_delim_long<TypeParam>();
	EXPECT_TRUE(split_view(empty, delim).empty());
	EXPECT_TRUE(split_once_view(empty, delim).first.empty());
}

/** Some basic tests for compiling with different containers */

TYPED_TEST(SplitCompilationTest, empty_deque) {
	std::basic_string_view<TypeParam> empty;
	std::deque<std::basic_string<TypeParam>> split_result = split<std::deque>(empty, default_delim<TypeParam>);
	EXPECT_TRUE(split_result.empty());
}

TYPED_TEST(SplitCompilationTest, empty_deque_long) {
	std::basic_string_view<TypeParam> empty;
	auto delim = make_delim_long<TypeParam>();
	EXPECT_TRUE(split<std::deque>(empty, delim).empty());
	EXPECT_TRUE(split_n<std::deque>(empty, delim, 5).empty());
	EXPECT_TRUE(split_view<std::deque>(empty, delim).empty());
	EXPECT_TRUE(split_n_view<std::deque>(empty, delim, 5).empty());
}

TYPED_TEST(SplitCompilationTest, empty_list) {
	std::basic_string_view<TypeParam> empty;
	std::list<std::basic_string<TypeParam>> split_result = split<std::list>(empty, default_delim<TypeParam>);
	EXPECT_TRUE(split_result.empty());
}

TYPED_TEST(SplitCompilationTest, empty_list_long) {
	std::basic_string_view<TypeParam> empty;
	auto delim = make_delim_long<TypeParam>();
	EXPECT_TRUE(split<std::list>(empty, delim).empty());
	EXPECT_TRUE(split_n<std::list>(empty, delim, 5).empty());
	EXPECT_TRUE(split_view<std::list>(empty, delim).empty());
	EXPECT_TRUE(split_n_view<std::list>(empty, delim, 5).empty());
}
