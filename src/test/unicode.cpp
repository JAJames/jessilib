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
