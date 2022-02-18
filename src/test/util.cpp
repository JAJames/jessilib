/**
 * Copyright (C) 2018-2021 Jessica James.
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

#include "jessilib/util.hpp"
#include "test.hpp"

using namespace jessilib;
using namespace std::literals;

TEST(UtilTest, filename) {
	EXPECT_EQ(JESSILIB_FILENAME_VIEW(), "util.cpp");
	EXPECT_EQ(JESSILIB_FILENAME_VIEW(u8), u8"util.cpp"sv);
	EXPECT_EQ(JESSILIB_FILENAME_VIEW(u), u"util.cpp"sv);
	EXPECT_EQ(JESSILIB_FILENAME_VIEW(U), U"util.cpp"sv);
	EXPECT_EQ(JESSILIB_FILENAME_VIEW(L), L"util.cpp"sv);
	//EXPECT_EQ(JESSILIB_FILENAME(u8), u8".cpp"sv);
}

TEST(UtilTest, byteswap) {
	EXPECT_EQ(byteswap(uint16_t{0x1234}), uint16_t{0x3412})
		<< "hex: " << std::hex << byteswap(uint16_t{0x1234});
	EXPECT_EQ(byteswap(uint32_t{0x12345678UL}), uint32_t{0x78563412UL})
		<< "hex: " << std::hex << byteswap(uint32_t{0x12345678UL});
	EXPECT_EQ(byteswap(uint64_t{0x123456789ABCDEF0ULL}), uint64_t{0xF0DEBC9A78563412ULL})
		<< "hex: " << std::hex << byteswap(uint64_t{0x123456789ABCDEF0ULL});
}

TEST(UtilTest, array_byteswap) {
	uint16_t numbers[]{
		0x1234,
		0x5678,
		0x9ABC,
		0xDEF0
	};

	uint16_t byteswapped_numbers[]{
		0x3412,
		0x7856,
		0xBC9A,
		0xF0DE
	};

	array_byteswap(std::begin(numbers), std::end(numbers));
	EXPECT_EQ(numbers[0], byteswapped_numbers[0]);
	EXPECT_EQ(numbers[1], byteswapped_numbers[1]);
	EXPECT_EQ(numbers[2], byteswapped_numbers[2]);
	EXPECT_EQ(numbers[3], byteswapped_numbers[3]);
}

TEST(UtilTest, string_byteswap) {
	std::string numbers{
		0x12, 0x34,
		0x56, 0x78,
		static_cast<char>(0x9A), static_cast<char>(0xBC),
		static_cast<char>(0xDE), static_cast<char>(0xF0)
	};

	std::string u16byteswapped_numbers{
		0x34, 0x12,
		0x78, 0x56,
		static_cast<char>(0xBC), static_cast<char>(0x9A),
		static_cast<char>(0xF0), static_cast<char>(0xDE)
	};

	string_byteswap<char16_t>(numbers);
	EXPECT_EQ(numbers, u16byteswapped_numbers);
}
