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

#include "jessilib/word_split.hpp"
#include "test_split.hpp"

using namespace jessilib;
using namespace std::literals;

TEST(jessi, lazy) {
	std::string sentence = "   this           is a    sentence ";
	auto split_result = word_split(sentence, ' ');
	EXPECT_EQ(split_result.size(), 4);
	EXPECT_EQ(split_result[0], "this");
	EXPECT_EQ(split_result[1], "is");
	EXPECT_EQ(split_result[2], "a");
	EXPECT_EQ(split_result[3], "sentence");
}

using char_types = ::testing::Types<char, unsigned char, signed char, wchar_t, char8_t, char16_t, char32_t>;

template<typename T>
class WordSplitSVTest : public ::testing::Test {
public:
};
TYPED_TEST_SUITE(WordSplitSVTest, char_types);

template<typename T>
class WordSplitStringTest : public ::testing::Test {
public:
};
TYPED_TEST_SUITE(WordSplitStringTest, char_types);

template<typename T>
class WordSplitOnceTest : public ::testing::Test {
public:
};
TYPED_TEST_SUITE(WordSplitOnceTest, char_types);

template<typename T>
class WordSplitNTest : public ::testing::Test {
public:
};
TYPED_TEST_SUITE(WordSplitNTest, char_types);

TYPED_TEST(WordSplitSVTest, empty) {
	std::basic_string_view<TypeParam> empty;
	std::vector<std::basic_string<TypeParam>> split_result = word_split(empty, default_delim<TypeParam>);
	EXPECT_TRUE(split_result.empty());
}

TYPED_TEST(WordSplitSVTest, single_word) {
	std::basic_string_view<TypeParam> single_word = make_word_view<TypeParam>();
	std::vector<std::basic_string<TypeParam>> split_result = word_split(single_word, default_delim<TypeParam>);
	EXPECT_EQ(split_result.size(), 1);
	EXPECT_EQ(split_result[0].size(), 8);
}

TYPED_TEST(WordSplitSVTest, single_word_trailing_delim) {
	auto word = make_word<TypeParam>();
	word += default_delim<TypeParam>;
	std::basic_string_view<TypeParam> single_word = word;
	std::vector<std::basic_string<TypeParam>> split_result = word_split(single_word, default_delim<TypeParam>);
	EXPECT_EQ(split_result.size(), 1);
	EXPECT_EQ(split_result[0].size(), 8);
}

TYPED_TEST(WordSplitSVTest, single_word_prefix_delim) {
	std::basic_string<TypeParam> word;
	word += default_delim<TypeParam>;
	word += make_word<TypeParam>();
	std::basic_string_view<TypeParam> single_word = word;
	std::vector<std::basic_string<TypeParam>> split_result = word_split(single_word, default_delim<TypeParam>);
	EXPECT_EQ(split_result.size(), 1);
	EXPECT_EQ(split_result[0].size(), 8);
}

TYPED_TEST(WordSplitSVTest, single_word_surround_delim) {
	std::basic_string<TypeParam> word;
	word += default_delim<TypeParam>;
	word += make_word<TypeParam>();
	word += default_delim<TypeParam>;
	std::basic_string_view<TypeParam> single_word = word;
	std::vector<std::basic_string<TypeParam>> split_result = word_split(single_word, default_delim<TypeParam>);
	EXPECT_EQ(split_result.size(), 1);
	EXPECT_EQ(split_result[0].size(), 8);
}

TYPED_TEST(WordSplitSVTest, two_words) {
	auto word = make_word<TypeParam>();
	word += default_delim<TypeParam>;
	word += make_word<TypeParam>();
	std::basic_string_view<TypeParam> words = word;
	std::vector<std::basic_string<TypeParam>> split_result = word_split(words, default_delim<TypeParam>);
	EXPECT_EQ(split_result.size(), 2);
	EXPECT_EQ(split_result[0].size(), 8);
	EXPECT_EQ(split_result[1].size(), 8);
}

TYPED_TEST(WordSplitSVTest, three_words) {
	auto word = make_word<TypeParam>(3);
	word += default_delim<TypeParam>;
	word += make_word<TypeParam>(5);
	word += default_delim<TypeParam>;
	word += default_delim<TypeParam>;
	word += make_word<TypeParam>(9);
	std::basic_string_view<TypeParam> words = word;
	std::vector<std::basic_string<TypeParam>> split_result = word_split(words, default_delim<TypeParam>);
	EXPECT_EQ(split_result.size(), 3);
	EXPECT_EQ(split_result[0].size(), 3);
	EXPECT_EQ(split_result[1].size(), 5);
	EXPECT_EQ(split_result[2].size(), 9);
}

/** WordSplitOnceTest */

TYPED_TEST(WordSplitOnceTest, random) {
	RandomWordTestData<TypeParam> data{};
	std::pair<std::basic_string<TypeParam>, std::basic_string<TypeParam>> split_result = word_split_once(data.m_str, default_delim<TypeParam>);

	EXPECT_EQ(split_result.first, data.m_tokens[0]);
	EXPECT_EQ(split_result.second, data.get_remainder(1));
}

TYPED_TEST(WordSplitOnceTest, random_vector) {
	using vector_type = std::vector<TypeParam>;
	RandomWordTestData<TypeParam, vector_type> data{};
	std::pair<vector_type, vector_type> split_result = word_split_once<vector_type>(data.m_str, default_delim<TypeParam>);

	EXPECT_EQ(split_result.first, data.m_tokens[0]);
	EXPECT_EQ(split_result.second, data.get_remainder(1));
}

TYPED_TEST(WordSplitOnceTest, random_view) {
	RandomWordTestData<TypeParam> data{};
	std::pair<std::basic_string_view<TypeParam>, std::basic_string_view<TypeParam>> split_result = word_split_once_view(data.m_str, default_delim<TypeParam>);

	EXPECT_EQ(split_result.first, data.m_tokens[0]);
	EXPECT_EQ(split_result.second, data.get_remainder(1));
}

/** SplitNTest */

TYPED_TEST(WordSplitNTest, random) {
	RandomWordTestData<TypeParam> data{};
	constexpr size_t n = 4;
	std::vector<std::basic_string<TypeParam>> split_result = word_split_n(data.m_str, default_delim<TypeParam>, n);

	// Tokens shall be same up until last one (n + 1)
	EXPECT_EQ(split_result.size(), n + 1);
	for (size_t index = 0; index != n; ++index) {
		EXPECT_EQ(split_result[index], data.m_tokens[index]);
	}

	EXPECT_EQ(split_result[n], data.get_remainder(n));
}

TYPED_TEST(WordSplitNTest, random_vector) {
	RandomWordTestData<TypeParam, std::vector<TypeParam>> data{};
	constexpr size_t n = 4;
	std::vector<std::vector<TypeParam>> split_result = word_split_n<std::vector, std::vector<TypeParam>>(data.m_str, default_delim<TypeParam>, n);

	// Tokens shall be same up until last one (n + 1)
	EXPECT_EQ(split_result.size(), n + 1);
	for (size_t index = 0; index != n; ++index) {
		EXPECT_EQ(split_result[index], data.m_tokens[index]);
	}

	EXPECT_EQ(split_result[n], data.get_remainder(n));
}

TYPED_TEST(WordSplitNTest, random_view) {
	RandomWordTestData<TypeParam> data{};
	constexpr size_t n = 4;
	std::vector<std::basic_string_view<TypeParam>> split_result = word_split_n_view(data.m_str, default_delim<TypeParam>, n);

	// Tokens shall be same up until last one (n + 1)
	EXPECT_EQ(split_result.size(), n + 1);
	for (size_t index = 0; index != n; ++index) {
		EXPECT_EQ(split_result[index], data.m_tokens[index]);
	}

	EXPECT_EQ(split_result[n], data.get_remainder(n));
}

/** std::string word_split test, really just testing compilation and returned types */

TYPED_TEST(WordSplitStringTest, empty) {
	std::basic_string<TypeParam> empty;
	std::vector<decltype(empty)> split_result = word_split(empty, default_delim<TypeParam>);
	EXPECT_TRUE(split_result.empty());
}

TYPED_TEST(WordSplitStringTest, empty_long) {
	std::basic_string<TypeParam> empty;
	auto delim = make_delim_long<TypeParam>(8);
	std::vector<decltype(empty)> split_result = word_split(empty, delim);
	EXPECT_TRUE(split_result.empty());
}

TYPED_TEST(WordSplitStringTest, single_word) {
	std::basic_string<TypeParam> single_word = make_word<TypeParam>();
	std::vector<decltype(single_word)> split_result = word_split(single_word, default_delim<TypeParam>);
	EXPECT_EQ(split_result.size(), 1);
	EXPECT_EQ(split_result[0].size(), 8);
}

TYPED_TEST(WordSplitStringTest, single_word_long) {
	auto delim = make_delim_long<TypeParam>(8);
	std::basic_string<TypeParam> single_word = make_word<TypeParam>(delim);
	std::vector<decltype(single_word)> split_result = word_split(single_word, delim);
	EXPECT_EQ(split_result.size(), 1);
	EXPECT_EQ(split_result[0].size(), 8);
}

TYPED_TEST(WordSplitStringTest, random) {
	RandomWordTestData<TypeParam> data{};
	std::vector<std::basic_string<TypeParam>> split_result = word_split(data.m_str, default_delim<TypeParam>);
	EXPECT_EQ(split_result, data.m_tokens);
}

TYPED_TEST(WordSplitStringTest, random_long) {
	auto delim = make_delim_long<TypeParam>(8);
	RandomWordTestData<TypeParam> data{ delim };
	std::vector<std::basic_string<TypeParam>> split_result = word_split(data.m_str, delim);
	EXPECT_EQ(split_result, data.m_tokens);
}

TYPED_TEST(WordSplitStringTest, random_vector) {
	RandomWordTestData<TypeParam, std::vector<TypeParam>> data{};
	std::vector<std::vector<TypeParam>> split_result = word_split<std::vector, std::vector<TypeParam>>(data.m_str, default_delim<TypeParam>);
	EXPECT_EQ(split_result, data.m_tokens);
}

TYPED_TEST(WordSplitStringTest, random_long_trailing_delim) {
	auto delim = make_delim_long<TypeParam>(8);
	RandomWordTestData<TypeParam> data{ delim };
	data.m_str += delim;
	std::vector<std::basic_string<TypeParam>> split_result = word_split(data.m_str, delim);
	EXPECT_EQ(split_result, data.m_tokens);
}

TYPED_TEST(WordSplitStringTest, random_long_prefix_delim) {
	auto delim = make_delim_long<TypeParam>(8);
	RandomWordTestData<TypeParam> data{ delim };
	data.m_str = delim + data.m_str;
	std::vector<std::basic_string<TypeParam>> split_result = word_split(data.m_str, delim);
	EXPECT_EQ(split_result, data.m_tokens);
}

TYPED_TEST(WordSplitStringTest, random_long_trailing_two_delim) {
	auto delim = make_delim_long<TypeParam>(8);
	RandomWordTestData<TypeParam> data{ delim };
	data.m_str += delim + delim;
	std::vector<std::basic_string<TypeParam>> split_result = word_split(data.m_str, delim);
	EXPECT_EQ(split_result, data.m_tokens);
}

TYPED_TEST(WordSplitStringTest, random_long_prefix_two_delim) {
	auto delim = make_delim_long<TypeParam>(8);
	RandomWordTestData<TypeParam> data{ delim };
	data.m_str = delim + delim + data.m_str;
	std::vector<std::basic_string<TypeParam>> split_result = word_split(data.m_str, delim);
	EXPECT_EQ(split_result, data.m_tokens);
}
