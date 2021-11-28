/**
 * Copyright (C) 2018 Jessica James.
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

#include "jessilib/unicode.hpp"
#include <fstream>
#include <charconv>
#include "jessilib/split.hpp"
#include "test.hpp"

using namespace jessilib;
using namespace std::literals;

/** encode_codepoint */

TEST(UTF8Test, encode_codepoint) {
	EXPECT_EQ(encode_codepoint_u8(U'\0'), u8"\0"sv);
	EXPECT_EQ(encode_codepoint_u8(U'A'), u8"A"sv);
	EXPECT_EQ(encode_codepoint_u8(U'z'), u8"z"sv);
	EXPECT_EQ(encode_codepoint_u8(U'\u007F'), u8"\u007F"sv);
	EXPECT_EQ(encode_codepoint_u8(U'\u0080'), u8"\u0080"sv);
	EXPECT_EQ(encode_codepoint_u8(U'\u07FF'), u8"\u07FF"sv);
	EXPECT_EQ(encode_codepoint_u8(U'\u0800'), u8"\u0800"sv);
	EXPECT_EQ(encode_codepoint_u8(U'\uFFFF'), u8"\uFFFF"sv);
	EXPECT_EQ(encode_codepoint_u8(U'\U00010000'), u8"\U00010000"sv);
	EXPECT_EQ(encode_codepoint_u8(U'\U0010FFFF'), u8"\U0010FFFF"sv);
	EXPECT_EQ(encode_codepoint_u8(U'\U0001F604'), u8"\U0001F604"sv);
}

TEST(UTF16Test, encode_codepoint) {
	EXPECT_EQ(encode_codepoint_u16(U'\0'), u"\0"sv);
	EXPECT_EQ(encode_codepoint_u16(U'A'), u"A"sv);
	EXPECT_EQ(encode_codepoint_u16(U'z'), u"z"sv);
	EXPECT_EQ(encode_codepoint_u16(U'\u007F'), u"\u007F"sv);
	EXPECT_EQ(encode_codepoint_u16(U'\u0080'), u"\u0080"sv);
	EXPECT_EQ(encode_codepoint_u16(U'\u07FF'), u"\u07FF"sv);
	EXPECT_EQ(encode_codepoint_u16(U'\u0800'), u"\u0800"sv);
	EXPECT_EQ(encode_codepoint_u16(U'\uFFFF'), u"\uFFFF"sv);
	EXPECT_EQ(encode_codepoint_u16(U'\U00010000'), u"\U00010000"sv);
	EXPECT_EQ(encode_codepoint_u16(U'\U0010FFFF'), u"\U0010FFFF"sv);
	EXPECT_EQ(encode_codepoint_u16(U'\U0001F604'), u"\U0001F604"sv);
}

TEST(UTF32Test, encode_codepoint) {
	EXPECT_EQ(encode_codepoint_u32(U'\0'), U"\0"sv);
	EXPECT_EQ(encode_codepoint_u32(U'A'), U"A"sv);
	EXPECT_EQ(encode_codepoint_u32(U'z'), U"z"sv);
	EXPECT_EQ(encode_codepoint_u32(U'\u007F'), U"\u007F"sv);
	EXPECT_EQ(encode_codepoint_u32(U'\u0080'), U"\u0080"sv);
	EXPECT_EQ(encode_codepoint_u32(U'\u07FF'), U"\u07FF"sv);
	EXPECT_EQ(encode_codepoint_u32(U'\u0800'), U"\u0800"sv);
	EXPECT_EQ(encode_codepoint_u32(U'\uFFFF'), U"\uFFFF"sv);
	EXPECT_EQ(encode_codepoint_u32(U'\U00010000'), U"\U00010000"sv);
	EXPECT_EQ(encode_codepoint_u32(U'\U0010FFFF'), U"\U0010FFFF"sv);
	EXPECT_EQ(encode_codepoint_u32(U'\U0001F604'), U"\U0001F604"sv);
}

/** decode_codepoint */

#define DECODE_CODEPOINT_TEST(IN_STR, IN_CODEPOINT, IN_UNITS) \
	EXPECT_EQ(decode_codepoint( IN_STR ).codepoint, IN_CODEPOINT); \
	EXPECT_EQ(decode_codepoint( IN_STR ).units, IN_UNITS)

TEST(UTF8Test, decode_codepoint) {
	DECODE_CODEPOINT_TEST(u8""sv, U'\0', 0U);
	DECODE_CODEPOINT_TEST(u8"\0"sv, U'\0', 1U);
	DECODE_CODEPOINT_TEST(u8"A"sv, U'A', 1U);
	DECODE_CODEPOINT_TEST(u8"z"sv, U'z', 1U);
	DECODE_CODEPOINT_TEST(u8"\u007F"sv, U'\u007F', 1U);
	DECODE_CODEPOINT_TEST(u8"\u0080"sv, U'\u0080', 2U);
	DECODE_CODEPOINT_TEST(u8"\u07FF"sv, U'\u07FF', 2U);
	DECODE_CODEPOINT_TEST(u8"\u0800"sv, U'\u0800', 3U);
	DECODE_CODEPOINT_TEST(u8"\uFFFF"sv, U'\uFFFF', 3U);
	DECODE_CODEPOINT_TEST(u8"\U00010000"sv, U'\U00010000', 4U);
	DECODE_CODEPOINT_TEST(u8"\U0010FFFF"sv, U'\U0010FFFF', 4U);
	DECODE_CODEPOINT_TEST(u8"\U0001F604"sv, U'\U0001F604', 4U);
}

TEST(UTF16Test, decode_codepoint) {
	DECODE_CODEPOINT_TEST(u""sv, U'\0', 0U);
	DECODE_CODEPOINT_TEST(u"\0"sv, U'\0', 1U);
	DECODE_CODEPOINT_TEST(u"A"sv, U'A', 1U);
	DECODE_CODEPOINT_TEST(u"z"sv, U'z', 1U);
	DECODE_CODEPOINT_TEST(u"\u007F"sv, U'\u007F', 1U);
	DECODE_CODEPOINT_TEST(u"\u0080"sv, U'\u0080', 1U);
	DECODE_CODEPOINT_TEST(u"\u07FF"sv, U'\u07FF', 1U);
	DECODE_CODEPOINT_TEST(u"\u0800"sv, U'\u0800', 1U);
	DECODE_CODEPOINT_TEST(u"\uD7FF"sv, U'\uD7FF', 1U);
	DECODE_CODEPOINT_TEST(u"\uE000"sv, U'\uE000', 1U);
	DECODE_CODEPOINT_TEST(u"\uFFFF"sv, U'\uFFFF', 1U);
	DECODE_CODEPOINT_TEST(u"\U00010000"sv, U'\U00010000', 2U);
	DECODE_CODEPOINT_TEST(u"\U0010FFFF"sv, U'\U0010FFFF', 2U);
	DECODE_CODEPOINT_TEST(u"\U0001F604"sv, U'\U0001F604', 2U);
}

TEST(UTF32Test, decode_codepoint) {
	DECODE_CODEPOINT_TEST(U""sv, U'\0', 0U);
	DECODE_CODEPOINT_TEST(U"\0"sv, U'\0', 1U);
	DECODE_CODEPOINT_TEST(U"A"sv, U'A', 1U);
	DECODE_CODEPOINT_TEST(U"z"sv, U'z', 1U);
	DECODE_CODEPOINT_TEST(U"\u007F"sv, U'\u007F', 1U);
	DECODE_CODEPOINT_TEST(U"\u0080"sv, U'\u0080', 1U);
	DECODE_CODEPOINT_TEST(U"\u07FF"sv, U'\u07FF', 1U);
	DECODE_CODEPOINT_TEST(U"\u0800"sv, U'\u0800', 1U);
	DECODE_CODEPOINT_TEST(U"\uFFFF"sv, U'\uFFFF', 1U);
	DECODE_CODEPOINT_TEST(U"\U00010000"sv, U'\U00010000', 1U);
	DECODE_CODEPOINT_TEST(U"\U0010FFFF"sv, U'\U0010FFFF', 1U);
	DECODE_CODEPOINT_TEST(U"\U0001F604"sv, U'\U0001F604', 1U);
}

using char_types = ::testing::Types<char, char8_t, char16_t, char32_t>;
using char_type_combos = ::testing::Types<
	std::pair<char, char>, std::pair<char, char8_t>, std::pair<char, char16_t>, std::pair<char, char32_t>,
	std::pair<char8_t, char>, std::pair<char8_t, char8_t>, std::pair<char8_t, char16_t>, std::pair<char8_t, char32_t>,
	std::pair<char16_t, char>, std::pair<char16_t, char8_t>, std::pair<char16_t, char16_t>, std::pair<char16_t, char32_t>,
	std::pair<char32_t, char>, std::pair<char32_t, char8_t>, std::pair<char32_t, char16_t>, std::pair<char32_t, char32_t>>;

template<typename T>
class UnicodeAbcdTest : public ::testing::Test {
public:
};
TYPED_TEST_SUITE(UnicodeAbcdTest, char_type_combos);

template<typename CharT, size_t InLength>
std::basic_string<CharT> make_str(const char32_t (&in_str)[InLength]) {
	std::basic_string<CharT> result;
	auto in_str_end = std::end(in_str) - 1; // ignore null terminator
	for (auto itr = std::begin(in_str); itr != in_str_end; ++itr) {
		jessilib::encode_codepoint(result, *itr);
	}
	return result;
}

/** string_cast */

TYPED_TEST(UnicodeAbcdTest, string_cast) {
	auto abcd_str = make_str<typename TypeParam::first_type>(U"ABCD");
	std::basic_string_view<typename TypeParam::first_type> abcd_string_view = abcd_str;

	EXPECT_TRUE(equals(abcd_str,
		string_cast<typename TypeParam::second_type>(abcd_str)));
	EXPECT_TRUE(is_valid(abcd_str));

	EXPECT_TRUE(equals(abcd_string_view,
		string_cast<typename TypeParam::second_type>(abcd_string_view)));
	EXPECT_TRUE(is_valid(abcd_string_view));
}

TEST(UTF8Test, string_view_cast) {
	auto abcd_str = make_str<char8_t>(U"ABCD");
	auto view = string_view_cast<char>(abcd_str);
	EXPECT_TRUE(equals(view, abcd_str));
}

/** equals */

TYPED_TEST(UnicodeAbcdTest, equals) {
	// TypeParam::first_type == TypeParam::second_type
	EXPECT_TRUE(equals(make_str<typename TypeParam::first_type>(U"ABCD"),
		make_str<typename TypeParam::second_type>(U"ABCD")));
	EXPECT_TRUE(equals(make_str<typename TypeParam::first_type>(U"abcd"),
		make_str<typename TypeParam::second_type>(U"abcd")));
	EXPECT_FALSE(equals(make_str<typename TypeParam::first_type>(U"ABCD"),
		make_str<typename TypeParam::second_type>(U"abcd")));
	EXPECT_FALSE(equals(make_str<typename TypeParam::first_type>(U"abcd"),
		make_str<typename TypeParam::second_type>(U"ABCD")));
	EXPECT_FALSE(equals(make_str<typename TypeParam::first_type>(U"ABcd"),
		make_str<typename TypeParam::second_type>(U"abCD")));
}

/** equalsi */

TYPED_TEST(UnicodeAbcdTest, equalsi) {
	// TypeParam::first_type == TypeParam::second_type
	EXPECT_TRUE(equalsi(make_str<typename TypeParam::first_type>(U"ABCD"),
		make_str<typename TypeParam::second_type>(U"ABCD")));
	EXPECT_TRUE(equalsi(make_str<typename TypeParam::first_type>(U"abcd"),
		make_str<typename TypeParam::second_type>(U"abcd")));
	EXPECT_TRUE(equalsi(make_str<typename TypeParam::first_type>(U"ABCD"),
		make_str<typename TypeParam::second_type>(U"abcd")));
	EXPECT_TRUE(equalsi(make_str<typename TypeParam::first_type>(U"abcd"),
		make_str<typename TypeParam::second_type>(U"ABCD")));
	EXPECT_TRUE(equalsi(make_str<typename TypeParam::first_type>(U"ABcd"),
		make_str<typename TypeParam::second_type>(U"abCD")));
}

/** starts_with */

TYPED_TEST(UnicodeAbcdTest, starts_with) {
	// TypeParam::first_type == TypeParam::second_type
	EXPECT_TRUE(starts_with(make_str<typename TypeParam::first_type>(U"ABCD"),
		make_str<typename TypeParam::second_type>(U"ABCD")));
	EXPECT_TRUE(starts_with(make_str<typename TypeParam::first_type>(U"abcd"),
		make_str<typename TypeParam::second_type>(U"abcd")));
	EXPECT_FALSE(starts_with(make_str<typename TypeParam::first_type>(U"ABCD"),
		make_str<typename TypeParam::second_type>(U"abcd")));
	EXPECT_FALSE(starts_with(make_str<typename TypeParam::first_type>(U"abcd"),
		make_str<typename TypeParam::second_type>(U"ABCD")));
	EXPECT_FALSE(starts_with(make_str<typename TypeParam::first_type>(U"ABcd"),
		make_str<typename TypeParam::second_type>(U"abCD")));

	// TypeParam::first_type starts_with TypeParam::second_type... (always false)
	EXPECT_FALSE(starts_with(make_str<typename TypeParam::first_type>(U"ABCD"),
		make_str<typename TypeParam::second_type>(U"ABCDzz")));
	EXPECT_FALSE(starts_with(make_str<typename TypeParam::first_type>(U"abcd"),
		make_str<typename TypeParam::second_type>(U"abcdzz")));
	EXPECT_FALSE(starts_with(make_str<typename TypeParam::first_type>(U"ABCD"),
		make_str<typename TypeParam::second_type>(U"abcdzz")));
	EXPECT_FALSE(starts_with(make_str<typename TypeParam::first_type>(U"abcd"),
		make_str<typename TypeParam::second_type>(U"ABCDzz")));
	EXPECT_FALSE(starts_with(make_str<typename TypeParam::first_type>(U"ABcd"),
		make_str<typename TypeParam::second_type>(U"abCDzz")));

	// TypeParam::first_type... starts_with TypeParam::second_type (always same results as first)
	EXPECT_TRUE(starts_with(make_str<typename TypeParam::first_type>(U"ABCDzz"),
		make_str<typename TypeParam::second_type>(U"ABCD")));
	EXPECT_TRUE(starts_with(make_str<typename TypeParam::first_type>(U"abcdzz"),
		make_str<typename TypeParam::second_type>(U"abcd")));
	EXPECT_FALSE(starts_with(make_str<typename TypeParam::first_type>(U"ABCDzz"),
		make_str<typename TypeParam::second_type>(U"abcd")));
	EXPECT_FALSE(starts_with(make_str<typename TypeParam::first_type>(U"abcdzz"),
		make_str<typename TypeParam::second_type>(U"ABCD")));
	EXPECT_FALSE(starts_with(make_str<typename TypeParam::first_type>(U"ABcdzz"),
		make_str<typename TypeParam::second_type>(U"abCD")));
}

/** starts_withi */

TYPED_TEST(UnicodeAbcdTest, starts_withi) {
	// TypeParam::first_type == TypeParam::second_type
	EXPECT_TRUE(starts_withi(make_str<typename TypeParam::first_type>(U"ABCD"),
		make_str<typename TypeParam::second_type>(U"ABCD")));
	EXPECT_TRUE(starts_withi(make_str<typename TypeParam::first_type>(U"abcd"),
		make_str<typename TypeParam::second_type>(U"abcd")));
	EXPECT_TRUE(starts_withi(make_str<typename TypeParam::first_type>(U"ABCD"),
		make_str<typename TypeParam::second_type>(U"abcd")));
	EXPECT_TRUE(starts_withi(make_str<typename TypeParam::first_type>(U"abcd"),
		make_str<typename TypeParam::second_type>(U"ABCD")));
	EXPECT_TRUE(starts_withi(make_str<typename TypeParam::first_type>(U"ABcd"),
		make_str<typename TypeParam::second_type>(U"abCD")));

	// TypeParam::first_type starts_with TypeParam::second_type... (always false)
	EXPECT_FALSE(starts_withi(make_str<typename TypeParam::first_type>(U"ABCD"),
		make_str<typename TypeParam::second_type>(U"ABCDzz")));
	EXPECT_FALSE(starts_withi(make_str<typename TypeParam::first_type>(U"abcd"),
		make_str<typename TypeParam::second_type>(U"abcdzz")));
	EXPECT_FALSE(starts_withi(make_str<typename TypeParam::first_type>(U"ABCD"),
		make_str<typename TypeParam::second_type>(U"abcdzz")));
	EXPECT_FALSE(starts_withi(make_str<typename TypeParam::first_type>(U"abcd"),
		make_str<typename TypeParam::second_type>(U"ABCDzz")));
	EXPECT_FALSE(starts_withi(make_str<typename TypeParam::first_type>(U"ABcd"),
		make_str<typename TypeParam::second_type>(U"abCDzz")));

	// TypeParam::first_type... starts_with TypeParam::second_type (always same results as first)
	EXPECT_TRUE(starts_withi(make_str<typename TypeParam::first_type>(U"ABCDzz"),
		make_str<typename TypeParam::second_type>(U"ABCD")));
	EXPECT_TRUE(starts_withi(make_str<typename TypeParam::first_type>(U"abcdzz"),
		make_str<typename TypeParam::second_type>(U"abcd")));
	EXPECT_TRUE(starts_withi(make_str<typename TypeParam::first_type>(U"ABCDzz"),
		make_str<typename TypeParam::second_type>(U"abcd")));
	EXPECT_TRUE(starts_withi(make_str<typename TypeParam::first_type>(U"abcdzz"),
		make_str<typename TypeParam::second_type>(U"ABCD")));
	EXPECT_TRUE(starts_withi(make_str<typename TypeParam::first_type>(U"ABcdzz"),
		make_str<typename TypeParam::second_type>(U"abCD")));

	EXPECT_TRUE(starts_withi(make_str<typename TypeParam::first_type>(U"Les Bean del Dallas"),
		make_str<typename TypeParam::second_type>(U"les")));
	EXPECT_TRUE(starts_withi(make_str<typename TypeParam::first_type>(U"Les Bean del Dallas"),
		make_str<typename TypeParam::second_type>(U"les Bean")));
	EXPECT_FALSE(starts_withi(make_str<typename TypeParam::first_type>(U"Les Bean del Dallas"),
		make_str<typename TypeParam::second_type>(U"del")));
}


/**
 * Folding test
 */

struct folding_info {
	uint32_t in_codepoint;
	uint32_t out_codepoint;
};

constexpr bool operator<(const folding_info& lhs, uint32_t rhs) {
	return lhs.in_codepoint < rhs;
}

constexpr bool operator<(uint32_t lhs, const folding_info& rhs) {
	return lhs < rhs.in_codepoint;
}

// TODO: make this just download from unicode.org (https://www.unicode.org/Public/UCD/latest/ucd/CaseFolding.txt)
std::vector<folding_info> read_CaseFolding_txt(const char* filename = JESSITEST_DATA_DIR "CaseFolding.txt") {
	std::vector<folding_info> result;
	std::ifstream file{ filename };

	while (file.good() && !file.eof()) {
		std::string line;
		std::getline(file, line);
		auto split_line = jessilib::split(line, "; "sv);
		if (split_line.size() < 3) {
			continue;
		}

		// From CaseFolding.txt usage; we're doing simple case folding:
		// "A. To do a simple case folding, use the mappings with status C + S."
		// "B. To do a full case folding, use the mappings with status C + F."
		if (split_line[1] == "C" || split_line[1] == "S") {
			folding_info info;
			std::from_chars(split_line[0].data(), split_line[0].data() + split_line[0].size(), info.in_codepoint, 16);
			std::from_chars(split_line[2].data(), split_line[2].data() + split_line[2].size(), info.out_codepoint, 16);
			result.push_back(info);
		}
		//else {
		//	std::cout << "ignoring: " << line << std::endl;
		//}
	}

	return result;
}

// describes a range
struct folding_set {
	enum class mode_type {
		constant,
		alternating,
		single
	} mode = mode_type::single;

	uint32_t range_start; // inclusive
	uint32_t range_end; // inclusive
	int64_t diff; // difference between two codepoints

	const char* type() const {
		switch (mode) {
			case mode_type::constant:
				return "constant";
			case mode_type::alternating:
				return "alternating";
			case mode_type::single:
				return "single";
			default:
				break;
		}

		return nullptr;
	}
};

std::vector<folding_set> folding_sets_from_folding_info(const std::vector<folding_info>& in_info) {
	std::vector<folding_set> result;

	folding_set current{};
	uint32_t last_match;
	for (auto& info : in_info) {
		int64_t diff = static_cast<int64_t>(info.out_codepoint) - static_cast<int64_t>(info.in_codepoint);

		if (current.diff == 0) {
			// this is a new set; set range_start and diff
			current.range_start = info.in_codepoint;
			current.diff = diff;
			last_match = info.in_codepoint;
			continue;
		}

		if (current.mode == folding_set::mode_type::single) {
			// We don't know what mode we are yet; check if this is constant
			if (diff == current.diff
				&& info.in_codepoint == last_match + 1) {
				// diff is same for range_start + 1; must be a constant-diff continuous set
				current.mode = folding_set::mode_type::constant;
				last_match = info.in_codepoint;
				continue;
			}

			// Either one-off fold, or alternating set. Check if in_codepoint is last_match + 2
			if (diff == current.diff
				&& info.in_codepoint == last_match + 2) {
				// Looks like an alternating set
				current.mode = folding_set::mode_type::alternating;
				last_match = info.in_codepoint;
				continue;
			}
		}

		if (current.mode == folding_set::mode_type::constant // For constant offset diffs to continue...
			&& current.diff == diff // Diff must be the same
			&& last_match + 1 == info.in_codepoint) { // And current codepoint must be 1 higher than previous
			// This matches the current range pattern; just update last_match and carry on
			last_match = info.in_codepoint;
			continue;
		}

		if (current.mode == folding_set::mode_type::alternating // For alternating sets to continue...
			&& current.diff == diff // Diff must be same
			&& last_match + 2 == info.in_codepoint) { // And current codepoint must be 2 higher than previous
			last_match = info.in_codepoint;
			continue;
		}

		// This isn't a continuation of the current set; push current and start a new set
		current.range_end = last_match;
		result.push_back(current);

		current = folding_set{};
		current.range_start = info.in_codepoint;
		current.diff = diff;
		last_match = info.in_codepoint;
	}

	// Push final range
	if (current.diff != 0) {
		current.range_end = last_match;
		result.push_back(current);
	}

	return result;
}

TEST(jess, unicode_case_folding_txt) {
	auto folded_codepoints = read_CaseFolding_txt();
	ASSERT_FALSE(folded_codepoints.empty());

	// Test that every folded codepoint is equal to its folded equivalent
	for (auto& folded_codepoint : folded_codepoints) {
		EXPECT_EQ(jessilib::fold(folded_codepoint.in_codepoint), folded_codepoint.out_codepoint)
			<< std::hex << std::uppercase << "lhs: " << std::setw(4) << folded_codepoint.in_codepoint << "; rhs: " << std::setw(8) << folded_codepoint.out_codepoint;
		EXPECT_TRUE(jessilib::equalsi(folded_codepoint.in_codepoint, folded_codepoint.out_codepoint));
	}

	// If the above failed, print out what the fold table needs to be replaced with
	if (::testing::Test::HasFailure()) {
		auto folding_sets = folding_sets_from_folding_info(folded_codepoints);
		ASSERT_FALSE(folding_sets.empty());
		for (auto& set : folding_sets) {
			// Generate code in the format: { folding_set::mode_type::constant, 0x1234, 0x2345, 1 },
			std::cout << std::hex << std::uppercase << std::setfill('0') << "{ "
				<< "folding_set::mode_type::" << set.type()
				<< ", "
				<< "0x" << std::setw(4) << set.range_start << ", "
				<< "0x" << std::setw(4) << set.range_end << ", "
				<< std::dec << set.diff
				<< " }," << std::endl;
		}

		std::cout << "folded_codepoints.size(): " << folded_codepoints.size() << "; folding_sets.size(): " << folding_sets.size() << std::endl;
	}

	// Test that every non-folded codepoint in [0, 0xFFFF] is equal to itself; we already tested folded ones above
	for (char32_t codepoint = 0; codepoint != 0x10000; ++codepoint) {
		bool is_folded = std::binary_search(folded_codepoints.begin(), folded_codepoints.end(), codepoint);
		if (!is_folded) {
			EXPECT_EQ(codepoint, jessilib::fold(codepoint));
			EXPECT_TRUE(jessilib::equalsi(codepoint, codepoint));
		}
	}
}
