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

#include "test_split.hpp"

using namespace jessilib;
using namespace std::literals;

using char_types = ::testing::Types<char, unsigned char, signed char, wchar_t, char8_t, char16_t, char32_t>;

template<typename T>
class SplitSVTest : public ::testing::Test {
public:
};
TYPED_TEST_SUITE(SplitSVTest, char_types);

template<typename T>
class SplitStringTest : public ::testing::Test {
public:
};
TYPED_TEST_SUITE(SplitStringTest, char_types);

template<typename T>
class SplitOnceTest : public ::testing::Test {
public:
};
TYPED_TEST_SUITE(SplitOnceTest, char_types);

template<typename T>
class SplitNTest : public ::testing::Test {
public:
};
TYPED_TEST_SUITE(SplitNTest, char_types);

TYPED_TEST(SplitSVTest, empty) {
	std::basic_string_view<TypeParam> empty;
	std::vector<std::basic_string<TypeParam>> split_result = split(empty, default_delim<TypeParam>);
	EXPECT_TRUE(split_result.empty());
}

TYPED_TEST(SplitSVTest, single_word) {
	std::basic_string_view<TypeParam> single_word = make_word_view<TypeParam>();
	std::vector<std::basic_string<TypeParam>> split_result = split(single_word, default_delim<TypeParam>);
	EXPECT_EQ(split_result.size(), 1);
	EXPECT_EQ(split_result[0].size(), 8);
}

TYPED_TEST(SplitSVTest, single_word_trailing_delim) {
	auto word = make_word<TypeParam>();
	word += default_delim<TypeParam>;
	std::basic_string_view<TypeParam> single_word = word;
	std::vector<std::basic_string<TypeParam>> split_result = split(single_word, default_delim<TypeParam>);
	EXPECT_EQ(split_result.size(), 2);
	EXPECT_EQ(split_result[0].size(), 8);
	EXPECT_EQ(split_result[1].size(), 0);
}

TYPED_TEST(SplitSVTest, single_word_prefix_delim) {
	std::basic_string<TypeParam> word;
	word += default_delim<TypeParam>;
	word += make_word<TypeParam>();
	std::basic_string_view<TypeParam> single_word = word;
	std::vector<std::basic_string<TypeParam>> split_result = split(single_word, default_delim<TypeParam>);
	EXPECT_EQ(split_result.size(), 2);
	EXPECT_EQ(split_result[0].size(), 0);
	EXPECT_EQ(split_result[1].size(), 8);
}

TYPED_TEST(SplitSVTest, single_word_surround_delim) {
	std::basic_string<TypeParam> word;
	word += default_delim<TypeParam>;
	word += make_word<TypeParam>();
	word += default_delim<TypeParam>;
	std::basic_string_view<TypeParam> single_word = word;
	std::vector<std::basic_string<TypeParam>> split_result = split(single_word, default_delim<TypeParam>);
	EXPECT_EQ(split_result.size(), 3);
	EXPECT_EQ(split_result[0].size(), 0);
	EXPECT_EQ(split_result[1].size(), 8);
	EXPECT_EQ(split_result[2].size(), 0);
}

TYPED_TEST(SplitSVTest, two_words) {
	auto word = make_word<TypeParam>();
	word += default_delim<TypeParam>;
	word += make_word<TypeParam>();
	std::basic_string_view<TypeParam> words = word;
	std::vector<std::basic_string<TypeParam>> split_result = split(words, default_delim<TypeParam>);
	EXPECT_EQ(split_result.size(), 2);
	EXPECT_EQ(split_result[0].size(), 8);
	EXPECT_EQ(split_result[1].size(), 8);
}

TYPED_TEST(SplitSVTest, three_words) {
	auto word = make_word<TypeParam>(3);
	word += default_delim<TypeParam>;
	word += make_word<TypeParam>(5);
	word += default_delim<TypeParam>;
	word += make_word<TypeParam>(9);
	std::basic_string_view<TypeParam> words = word;
	std::vector<std::basic_string<TypeParam>> split_result = split(words, default_delim<TypeParam>);
	EXPECT_EQ(split_result.size(), 3);
	EXPECT_EQ(split_result[0].size(), 3);
	EXPECT_EQ(split_result[1].size(), 5);
	EXPECT_EQ(split_result[2].size(), 9);
}

/** SplitOnceTest */

TYPED_TEST(SplitOnceTest, random) {
	RandomTestData<TypeParam> data{};
	std::pair<std::basic_string<TypeParam>, std::basic_string<TypeParam>> split_result = split_once(data.m_str, default_delim<TypeParam>);

	EXPECT_EQ(split_result.first, data.m_tokens[0]);
	EXPECT_EQ(split_result.second, data.get_remainder(1));
}

TYPED_TEST(SplitOnceTest, random_vector) {
	using vector_type = std::vector<TypeParam>;
	RandomTestData<TypeParam, vector_type> data{};
	std::pair<vector_type, vector_type> split_result = split_once<vector_type>(data.m_str, default_delim<TypeParam>);

	EXPECT_EQ(split_result.first, data.m_tokens[0]);
	EXPECT_EQ(split_result.second, data.get_remainder(1));
}

TYPED_TEST(SplitOnceTest, random_view) {
	RandomTestData<TypeParam> data{};
	std::pair<std::basic_string_view<TypeParam>, std::basic_string_view<TypeParam>> split_result = split_once_view(data.m_str, default_delim<TypeParam>);

	EXPECT_EQ(split_result.first, data.m_tokens[0]);
	EXPECT_EQ(split_result.second, data.get_remainder(1));
}

/** SplitNTest */

TYPED_TEST(SplitNTest, random) {
	RandomTestData<TypeParam> data{};
	constexpr size_t n = 4;
	std::vector<std::basic_string<TypeParam>> split_result = split_n(data.m_str, default_delim<TypeParam>, n);

	// Tokens shall be same up until last one (n + 1)
	EXPECT_EQ(split_result.size(), n + 1);
	for (size_t index = 0; index != n; ++index) {
		EXPECT_EQ(split_result[index], data.m_tokens[index]);
	}

	EXPECT_EQ(split_result[n], data.get_remainder(n));
}

TYPED_TEST(SplitNTest, random_vector) {
	RandomTestData<TypeParam, std::vector<TypeParam>> data{};
	constexpr size_t n = 4;
	std::vector<std::vector<TypeParam>> split_result = split_n<std::vector, std::vector<TypeParam>>(data.m_str, default_delim<TypeParam>, n);

	// Tokens shall be same up until last one (n + 1)
	EXPECT_EQ(split_result.size(), n + 1);
	for (size_t index = 0; index != n; ++index) {
		EXPECT_EQ(split_result[index], data.m_tokens[index]);
	}

	EXPECT_EQ(split_result[n], data.get_remainder(n));
}

TYPED_TEST(SplitNTest, random_view) {
	RandomTestData<TypeParam> data{};
	constexpr size_t n = 4;
	std::vector<std::basic_string_view<TypeParam>> split_result = split_n_view(data.m_str, default_delim<TypeParam>, n);

	// Tokens shall be same up until last one (n + 1)
	EXPECT_EQ(split_result.size(), n + 1);
	for (size_t index = 0; index != n; ++index) {
		EXPECT_EQ(split_result[index], data.m_tokens[index]);
	}

	EXPECT_EQ(split_result[n], data.get_remainder(n));
}

/** std::string split test, really just testing compilation and returned types */

TYPED_TEST(SplitStringTest, empty) {
	std::basic_string<TypeParam> empty;
	std::vector<decltype(empty)> split_result = split(empty, default_delim<TypeParam>);
	EXPECT_TRUE(split_result.empty());
}

TYPED_TEST(SplitStringTest, empty_long) {
	std::basic_string<TypeParam> empty;
	auto delim = make_delim_long<TypeParam>(8);
	std::vector<decltype(empty)> split_result = split(empty, delim);
	EXPECT_TRUE(split_result.empty());
}

TYPED_TEST(SplitStringTest, single_word) {
	std::basic_string<TypeParam> single_word = make_word<TypeParam>();
	std::vector<decltype(single_word)> split_result = split(single_word, default_delim<TypeParam>);
	EXPECT_EQ(split_result.size(), 1);
	EXPECT_EQ(split_result[0].size(), 8);
}

TYPED_TEST(SplitStringTest, single_word_long) {
	std::basic_string<TypeParam> single_word = make_word<TypeParam>();
	auto delim = make_delim_long<TypeParam>(8);
	std::vector<decltype(single_word)> split_result = split(single_word, delim);
	EXPECT_EQ(split_result.size(), 1);
	EXPECT_EQ(split_result[0].size(), 8);
}

TYPED_TEST(SplitStringTest, random) {
	RandomTestData<TypeParam> data{};
	std::vector<std::basic_string<TypeParam>> split_result = split(data.m_str, default_delim<TypeParam>);
	EXPECT_EQ(split_result, data.m_tokens);
}

TYPED_TEST(SplitStringTest, random_long) {
	auto delim = make_delim_long<TypeParam>(8);
	RandomTestData<TypeParam> data{ delim };
	std::vector<std::basic_string<TypeParam>> split_result = split(data.m_str, delim);
	EXPECT_EQ(split_result, data.m_tokens);
}

TYPED_TEST(SplitStringTest, random_vector) {
	RandomTestData<TypeParam, std::vector<TypeParam>> data{};
	std::vector<std::vector<TypeParam>> split_result = split<std::vector, std::vector<TypeParam>>(data.m_str, default_delim<TypeParam>);
	EXPECT_EQ(split_result, data.m_tokens);
}

TYPED_TEST(SplitStringTest, random_long_trailing_delim) {
	auto delim = make_delim_long<TypeParam>(8);
	RandomTestData<TypeParam> data{ delim };
	data.m_str += delim;
	data.m_tokens.emplace_back();
	std::vector<std::basic_string<TypeParam>> split_result = split(data.m_str, delim);
	EXPECT_EQ(split_result, data.m_tokens);
}

TYPED_TEST(SplitStringTest, random_long_prefix_delim) {
	auto delim = make_delim_long<TypeParam>(8);
	RandomTestData<TypeParam> data{ delim };
	data.m_str = delim + data.m_str;
	data.m_tokens.emplace(data.m_tokens.begin());
	std::vector<std::basic_string<TypeParam>> split_result = split(data.m_str, delim);
	EXPECT_EQ(split_result, data.m_tokens);
}

TYPED_TEST(SplitStringTest, random_long_trailing_two_delim) {
	auto delim = make_delim_long<TypeParam>(8);
	RandomTestData<TypeParam> data{ delim };
	data.m_str += delim + delim;
	data.m_tokens.emplace_back();
	data.m_tokens.emplace_back();
	std::vector<std::basic_string<TypeParam>> split_result = split(data.m_str, delim);
	EXPECT_EQ(split_result, data.m_tokens);
}

TYPED_TEST(SplitStringTest, random_long_prefix_two_delim) {
	auto delim = make_delim_long<TypeParam>(8);
	RandomTestData<TypeParam> data{ delim };
	data.m_str = delim + delim + data.m_str;
	data.m_tokens.emplace(data.m_tokens.begin());
	data.m_tokens.emplace(data.m_tokens.begin());
	std::vector<std::basic_string<TypeParam>> split_result = split(data.m_str, delim);
	EXPECT_EQ(split_result, data.m_tokens);
}
