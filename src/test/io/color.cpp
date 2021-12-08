/**
 * Copyright (C) 2020-2021 Jessica James.
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

#include "jessilib/io/color.hpp"
#include "test.hpp"

using namespace jessilib;
using namespace jessilib::io;
using namespace std::literals;

static constexpr color g_constexpr_color{ 0x123456 };

TEST(ColorTest, red) {
	color color_value{ 0xFF0000 };
	color color_rgb{ 0xFF, 0, 0 };

	EXPECT_EQ(color_value, color_rgb);
	EXPECT_EQ(color_value.value(), 0xFF0000U);
	EXPECT_EQ(color_value.red(), 0xFF);
	EXPECT_EQ(color_value.green(), 0x00);
	EXPECT_EQ(color_value.blue(), 0x00);
}

TEST(ColorTest, green) {
	color color_value{ 0xFF00 };
	color color_rgb{ 0, 0xFF, 0 };

	EXPECT_EQ(color_value, color_rgb);
	EXPECT_EQ(color_value.value(), 0xFF00U);
	EXPECT_EQ(color_value.red(), 0x00);
	EXPECT_EQ(color_value.green(), 0xFF);
	EXPECT_EQ(color_value.blue(), 0x00);
}

TEST(ColorTest, blue) {
	color color_value{ 0xFF };
	color color_rgb{ 0, 0, 0xFF };

	EXPECT_EQ(color_value, color_rgb);
	EXPECT_EQ(color_value.value(), 0xFFU);
	EXPECT_EQ(color_value.red(), 0x00);
	EXPECT_EQ(color_value.green(), 0x00);
	EXPECT_EQ(color_value.blue(), 0xFF);
}

TEST(ColorTest, mixed) {
	color color_value{ 0x123456 };
	color color_rgb{ 0x12, 0x34, 0x56 };

	EXPECT_EQ(color_value, color_rgb);
	EXPECT_EQ(color_value.value(), 0x123456U);
	EXPECT_EQ(color_value.red(), 0x12);
	EXPECT_EQ(color_value.green(), 0x34);
	EXPECT_EQ(color_value.blue(), 0x56);
}

TEST(ColorTest, copy) {
	color color_value{ g_constexpr_color };
	color color_rgb{ 0x12, 0x34, 0x56 };

	EXPECT_EQ(color_value, color_rgb);
	EXPECT_EQ(color_value.value(), g_constexpr_color.value());
	EXPECT_EQ(color_value.red(), 0x12);
	EXPECT_EQ(color_value.green(), 0x34);
	EXPECT_EQ(color_value.blue(), 0x56);
}

TEST(ColorTest, distance) {
	EXPECT_EQ(color{}.distance({}), 0);
	EXPECT_EQ(color{ 0xFF }.distance({}), 0xFF);
	EXPECT_EQ(color{ 0xFF00 }.distance({}), 0xFF);
	EXPECT_EQ(color{ 0xFF0000 }.distance({}), 0xFF);
}
