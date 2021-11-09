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

#include "split.hpp"
#include <cassert>
#include <deque>
#include <list>
#include "test.hpp"

using namespace jessilib;
using namespace std::literals;

// empty strings

using char_types = ::testing::Types<char, unsigned char, signed char, wchar_t, char8_t, char16_t, char32_t>;
using text_char_types = ::testing::Types<char, unsigned char, signed char, wchar_t, char8_t, char16_t, char32_t>;

template<typename T>
class SplitSVTest : public ::testing::Test {
public:
};
TYPED_TEST_SUITE(SplitSVTest, char_types);

template<typename T>
class SplitSringTest : public ::testing::Test {
public:
};
TYPED_TEST_SUITE(SplitSringTest, char_types);

template<typename T>
class SplitViewSVTest : public ::testing::Test {
public:
};
TYPED_TEST_SUITE(SplitViewSVTest, char_types);

template<typename T>
class SplitViewStringTest : public ::testing::Test {
public:
};
TYPED_TEST_SUITE(SplitViewStringTest, char_types);

template<typename T>
std::basic_string<T> make_word(size_t length = 8, T delim = static_cast<T>(0)) {
	std::basic_string<T> result;

	if (length == 0) {
		throw std::runtime_error{ "length == 0" };
	}

	result.push_back(delim + 1);
	while (result.size() < length) {
		auto chr = result.back() + 1;
		if (chr == delim) {
			++chr;
		}
		result.push_back(chr);
	}

	if (result.size() != length) {
		std::string errmsg = std::to_string(result.size()) + " != " + std::to_string(length) + "; result.size() != length";
		throw std::runtime_error{ errmsg };
	}

	return result;
}

template<typename T>
std::basic_string_view<T> make_word_view(size_t length = 8, T delim = static_cast<T>(0)) {
	static std::basic_string<T> s_result;
	s_result = make_word(length, delim);
	return s_result;
}

TYPED_TEST(SplitSVTest, empty) {
	std::basic_string_view<TypeParam> empty;
	std::vector<decltype(empty)> split_result = split(empty, static_cast<TypeParam>(0));
	EXPECT_TRUE(split_result.empty());
}

TYPED_TEST(SplitSVTest, single_word) {
	std::basic_string_view<TypeParam> single_word = make_word_view<TypeParam>();
	std::vector<decltype(single_word)> split_result = split(single_word, static_cast<TypeParam>(0));
	EXPECT_EQ(split_result.size(), 1);
	EXPECT_EQ(split_result[0].size(), 8);
}

TYPED_TEST(SplitSVTest, single_word_trailing_delim) {
	auto word = make_word<TypeParam>();
	word += static_cast<TypeParam>(0);
	std::basic_string_view<TypeParam> single_word = word;
	std::vector<decltype(single_word)> split_result = split(single_word, static_cast<TypeParam>(0));
	EXPECT_EQ(split_result.size(), 2);
	EXPECT_EQ(split_result[0].size(), 8);
	EXPECT_EQ(split_result[1].size(), 0);
}

TYPED_TEST(SplitSVTest, single_word_prefix_delim) {
	std::basic_string<TypeParam> word;
	word += static_cast<TypeParam>(0);
	word += make_word<TypeParam>();
	std::basic_string_view<TypeParam> single_word = word;
	std::vector<decltype(single_word)> split_result = split(single_word, static_cast<TypeParam>(0));
	EXPECT_EQ(split_result.size(), 2);
	EXPECT_EQ(split_result[0].size(), 0);
	EXPECT_EQ(split_result[1].size(), 8);
}

TYPED_TEST(SplitSVTest, single_word_surround_delim) {
	std::basic_string<TypeParam> word;
	word += static_cast<TypeParam>(0);
	word += make_word<TypeParam>();
	word += static_cast<TypeParam>(0);
	std::basic_string_view<TypeParam> single_word = word;
	std::vector<decltype(single_word)> split_result = split(single_word, static_cast<TypeParam>(0));
	EXPECT_EQ(split_result.size(), 3);
	EXPECT_EQ(split_result[0].size(), 0);
	EXPECT_EQ(split_result[1].size(), 8);
	EXPECT_EQ(split_result[2].size(), 0);
}

TYPED_TEST(SplitSVTest, two_words) {
	auto word = make_word<TypeParam>();
	word += static_cast<TypeParam>(0);
	word += make_word<TypeParam>();
	std::basic_string_view<TypeParam> words = word;
	std::vector<decltype(words)> split_result = split(words, static_cast<TypeParam>(0));
	EXPECT_EQ(split_result.size(), 2);
	EXPECT_EQ(split_result[0].size(), 8);
	EXPECT_EQ(split_result[1].size(), 8);
}

TYPED_TEST(SplitSVTest, three_words) {
	auto word = make_word<TypeParam>(3);
	word += static_cast<TypeParam>(0);
	word += make_word<TypeParam>(5);
	word += static_cast<TypeParam>(0);
	word += make_word<TypeParam>(9);
	std::basic_string_view<TypeParam> words = word;
	std::vector<decltype(words)> split_result = split(words, static_cast<TypeParam>(0));
	EXPECT_EQ(split_result.size(), 3);
	EXPECT_EQ(split_result[0].size(), 3);
	EXPECT_EQ(split_result[1].size(), 5);
	EXPECT_EQ(split_result[2].size(), 9);
}

/** std::string split test, really just testing compilation */

TYPED_TEST(SplitSringTest, empty) {
	std::basic_string<TypeParam> empty;
	std::vector<decltype(empty)> split_result = split(empty, static_cast<TypeParam>(0));
	EXPECT_TRUE(split_result.empty());
}

TYPED_TEST(SplitSringTest, single_word) {
	std::basic_string<TypeParam> single_word = make_word<TypeParam>();
	std::vector<decltype(single_word)> split_result = split(single_word, static_cast<TypeParam>(0));
	EXPECT_EQ(split_result.size(), 1);
	EXPECT_EQ(split_result[0].size(), 8);
}

/** Some basic tests for compiling with different containers */

TYPED_TEST(SplitSVTest, empty_deque) {
	std::basic_string_view<TypeParam> empty;
	std::deque<decltype(empty)> split_result = split<decltype(empty), std::deque<decltype(empty)>>(empty, static_cast<TypeParam>(0));
	EXPECT_TRUE(split_result.empty());
}

TYPED_TEST(SplitSVTest, empty_list) {
	std::basic_string_view<TypeParam> empty;
	std::list<decltype(empty)> split_result = split<decltype(empty), std::list<decltype(empty)>>(empty, static_cast<TypeParam>(0));
	EXPECT_TRUE(split_result.empty());
}

/** SplitViewSVTest, really just compilation tests */

TYPED_TEST(SplitViewSVTest, empty) {
	std::basic_string_view<TypeParam> empty;
	std::vector<decltype(empty)> split_result = split_view(empty, static_cast<TypeParam>(0));
	EXPECT_TRUE(split_result.empty());
}

TYPED_TEST(SplitViewSVTest, single_word) {
	std::basic_string_view<TypeParam> single_word = make_word_view<TypeParam>();
	std::vector<decltype(single_word)> split_result = split_view(single_word, static_cast<TypeParam>(0));
	EXPECT_EQ(split_result.size(), 1);
}

TYPED_TEST(SplitViewStringTest, empty) {
	std::basic_string<TypeParam> empty;
	std::vector<std::basic_string_view<TypeParam>> split_result = split_view(empty, static_cast<TypeParam>(0));
	EXPECT_TRUE(split_result.empty());
}

TYPED_TEST(SplitViewStringTest, single_word) {
	std::basic_string<TypeParam> single_word = make_word<TypeParam>();
	std::vector<std::basic_string_view<TypeParam>> split_result = split_view(single_word, static_cast<TypeParam>(0));
	EXPECT_EQ(split_result.size(), 1);
}
