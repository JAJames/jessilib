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
#include <random>
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
class SplitOnceTest : public ::testing::Test {
public:
};
TYPED_TEST_SUITE(SplitOnceTest, char_types);

template<typename T>
class SplitNTest : public ::testing::Test {
public:
};
TYPED_TEST_SUITE(SplitNTest, char_types);

template<typename T>
std::basic_string<T> make_word(size_t length = 8, T delim = static_cast<T>(0)) {
	std::basic_string<T> result;

	if (length == 0) {
		return {};
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

template<typename T>
struct RandomTestData {
	RandomTestData(T in_delim)
		: m_delim{ in_delim } {
		operator()();
	}

	void operator()() {
		m_tokens.clear();
		m_str.clear();

		std::mt19937 randgen(static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count()));
		std::uniform_int_distribution<uint32_t> word_count_distribution(5, 64);
		std::uniform_int_distribution<uint32_t> word_length_distribution(0, 16);

		auto random_words = word_count_distribution(randgen);
		while (m_tokens.size() < random_words) {
			m_tokens.push_back(make_word<T>(word_length_distribution(randgen)));
			m_str += m_tokens.back();
			if (m_tokens.size() < random_words) {
				m_str += m_delim;
			}
		}
	}

	std::basic_string<T> get_remainder(size_t in_times_split) {
		std::basic_string<T> result;
		while (in_times_split < m_tokens.size()) {
			result += m_tokens[in_times_split];
			++in_times_split;
			if (in_times_split < m_tokens.size()) {
				result += m_delim;
			}
		}

		return result;
	}

	T m_delim;
	std::basic_string<T> m_str;
	std::vector<std::basic_string<T>> m_tokens;
};

TYPED_TEST(SplitSVTest, empty) {
	std::basic_string_view<TypeParam> empty;
	std::vector<std::basic_string<TypeParam>> split_result = split(empty, static_cast<TypeParam>(0));
	EXPECT_TRUE(split_result.empty());
}

TYPED_TEST(SplitSVTest, single_word) {
	std::basic_string_view<TypeParam> single_word = make_word_view<TypeParam>();
	std::vector<std::basic_string<TypeParam>> split_result = split(single_word, static_cast<TypeParam>(0));
	EXPECT_EQ(split_result.size(), 1);
	EXPECT_EQ(split_result[0].size(), 8);
}

TYPED_TEST(SplitSVTest, single_word_trailing_delim) {
	auto word = make_word<TypeParam>();
	word += static_cast<TypeParam>(0);
	std::basic_string_view<TypeParam> single_word = word;
	std::vector<std::basic_string<TypeParam>> split_result = split(single_word, static_cast<TypeParam>(0));
	EXPECT_EQ(split_result.size(), 2);
	EXPECT_EQ(split_result[0].size(), 8);
	EXPECT_EQ(split_result[1].size(), 0);
}

TYPED_TEST(SplitSVTest, single_word_prefix_delim) {
	std::basic_string<TypeParam> word;
	word += static_cast<TypeParam>(0);
	word += make_word<TypeParam>();
	std::basic_string_view<TypeParam> single_word = word;
	std::vector<std::basic_string<TypeParam>> split_result = split(single_word, static_cast<TypeParam>(0));
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
	std::vector<std::basic_string<TypeParam>> split_result = split(single_word, static_cast<TypeParam>(0));
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
	std::vector<std::basic_string<TypeParam>> split_result = split(words, static_cast<TypeParam>(0));
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
	std::vector<std::basic_string<TypeParam>> split_result = split(words, static_cast<TypeParam>(0));
	EXPECT_EQ(split_result.size(), 3);
	EXPECT_EQ(split_result[0].size(), 3);
	EXPECT_EQ(split_result[1].size(), 5);
	EXPECT_EQ(split_result[2].size(), 9);
}

/** SplitOnceTest */

TYPED_TEST(SplitOnceTest, random) {
	RandomTestData<TypeParam> data{ static_cast<TypeParam>(0) };
	std::pair<std::basic_string<TypeParam>, std::basic_string<TypeParam>> split_result = split_once(data.m_str, data.m_delim);

	EXPECT_EQ(split_result.first, data.m_tokens[0]);
	EXPECT_EQ(split_result.second, data.get_remainder(1));
}

TYPED_TEST(SplitOnceTest, random_view) {
	RandomTestData<TypeParam> data{ static_cast<TypeParam>(0) };
	std::pair<std::basic_string_view<TypeParam>, std::basic_string_view<TypeParam>> split_result = split_once_view(data.m_str, data.m_delim);

	EXPECT_EQ(split_result.first, data.m_tokens[0]);
	EXPECT_EQ(split_result.second, data.get_remainder(1));
}

/** SplitNTest */

TYPED_TEST(SplitNTest, random) {
	RandomTestData<TypeParam> data{ static_cast<TypeParam>(0) };
	constexpr size_t n = 4;
	std::vector<std::basic_string<TypeParam>> split_result = split_n(data.m_str, data.m_delim, n);

	// Tokens shall be same up until last one (n + 1)
	EXPECT_EQ(split_result.size(), n + 1);
	for (size_t index = 0; index != n; ++index) {
		EXPECT_EQ(split_result[index], data.m_tokens[index]);
	}

	EXPECT_EQ(split_result[n], data.get_remainder(n));
}

TYPED_TEST(SplitNTest, random_view) {
	RandomTestData<TypeParam> data{ static_cast<TypeParam>(0) };
	constexpr size_t n = 4;
	std::vector<std::basic_string_view<TypeParam>> split_result = split_n_view(data.m_str, data.m_delim, n);

	// Tokens shall be same up until last one (n + 1)
	EXPECT_EQ(split_result.size(), n + 1);
	for (size_t index = 0; index != n; ++index) {
		EXPECT_EQ(split_result[index], data.m_tokens[index]);
	}

	EXPECT_EQ(split_result[n], data.get_remainder(n));
}

/** std::string split test, really just testing compilation and returned types */

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

TYPED_TEST(SplitSringTest, random) {
	RandomTestData<TypeParam> data{ static_cast<TypeParam>(0) };
	auto split_result = split(data.m_str, data.m_delim);
	EXPECT_EQ(split_result, data.m_tokens);
}

/** Some basic tests for compiling with different containers */

TYPED_TEST(SplitSVTest, empty_deque) {
	std::basic_string_view<TypeParam> empty;
	std::deque<std::basic_string<TypeParam>> split_result = split<std::deque>(empty, static_cast<TypeParam>(0));
	EXPECT_TRUE(split_result.empty());
}

TYPED_TEST(SplitSVTest, empty_list) {
	std::basic_string_view<TypeParam> empty;
	std::list<std::basic_string<TypeParam>> split_result = split<std::list>(empty, static_cast<TypeParam>(0));
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

TYPED_TEST(SplitViewStringTest, random) {
	RandomTestData<TypeParam> data{ static_cast<TypeParam>(0) };
	auto split_result = split_view(data.m_str, data.m_delim);
	EXPECT_EQ(split_result.size(), data.m_tokens.size());
}
