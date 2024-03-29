/**
 * Copyright (C) 2019-2021 Jessica James.
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

#pragma once

#include "jessilib/io/message.hpp"

namespace jessilib {
namespace io {
namespace irc {

// Message wrapper to allow passing messages into fmt::format
class text_wrapper : public text {};

// Color table
static constexpr color s_irc_colors[] {
	// Basic 16 colors (0-15)
	0xFFFFFF, 0x000000, 0x00007F, 0x009300, 0xFF0000, 0x7F0000, 0x9C009C, 0xFC7F00,
	0xFFFF00, 0x00FC00, 0x009393, 0x00FFFF, 0x0000FC, 0xFF00FF, 0x7F7F7F, 0xD2D2D2,

#ifndef JESSILIB_IRC_SIMPLE_COLORS
	// Extended colors (16-98, making a total of 99 color choices)
	0x470000, 0x472100, 0x474700, 0x324700, 0x004700, 0x00472C, 0x004747, 0x002747, 0x000047, 0x2E0047, 0x470047, 0x47002A,
	0x740000, 0x743a00, 0x747400, 0x517400, 0x007400, 0x007449, 0x007474, 0x004074, 0x000074, 0x4b0074, 0x740074, 0x740045,
	0xb50000, 0xb56300, 0xb5b500, 0x7db500, 0x00b500, 0x00b571, 0x00b5b5, 0x0063b5, 0x0000b5, 0x7500b5, 0xb500b5, 0xb5006b,
	0xff0000, 0xff8c00, 0xffff00, 0xb2ff00, 0x00ff00, 0x00ffa0, 0x00ffff, 0x008cff, 0x0000ff, 0xa500ff, 0xff00ff, 0xff0098,
	0xff5959, 0xffb459, 0xffff71, 0xcfff60, 0x6fff6f, 0x65ffc9, 0x6dffff, 0x59b4ff, 0x5959ff, 0xc459ff, 0xff66ff, 0xff59bc,
	0xff9c9c, 0xffd39c, 0xffff9c, 0xe2ff9c, 0x9cff9c, 0x9cffdb, 0x9cffff, 0x9cd3ff, 0x9c9cff, 0xdc9cff, 0xff9cff, 0xff94d3,
	0x000000, 0x131313, 0x282828, 0x363636, 0x4d4d4d, 0x656565, 0x818181, 0x9f9f9f, 0xbcbcbc, 0xe2e2e2, 0xffffff
#endif // JESSILIB_IRC_SIMPLE_COLORS
};

static constexpr std::u8string_view s_irc_color_codes[] {
	// Basic 16 colors (0-15)
	u8"00", u8"01", u8"02", u8"03", u8"04", u8"05", u8"06", u8"07", u8"08", u8"09", u8"10", u8"11", u8"12", u8"13", u8"14", u8"15",

#ifndef JESSILIB_IRC_SIMPLE_COLORS
	// Extended colors (16-98, making a total of 99 color choices)
	u8"16", u8"17", u8"18", u8"19", u8"20", u8"21", u8"22", u8"23", u8"24", u8"25", u8"26", u8"27",
	u8"28", u8"29", u8"30", u8"31", u8"32", u8"33", u8"34", u8"35", u8"36", u8"37", u8"38", u8"39",
	u8"40", u8"41", u8"42", u8"43", u8"44", u8"45", u8"46", u8"47", u8"48", u8"49", u8"50", u8"51",
	u8"52", u8"53", u8"54", u8"55", u8"56", u8"57", u8"58", u8"59", u8"60", u8"61", u8"62", u8"63",
	u8"64", u8"65", u8"66", u8"67", u8"68", u8"69", u8"70", u8"71", u8"72", u8"73", u8"74", u8"75",
	u8"76", u8"77", u8"78", u8"79", u8"80", u8"81", u8"82", u8"83", u8"84", u8"85", u8"86", u8"87",
	u8"88", u8"89", u8"90", u8"91", u8"92", u8"93", u8"94", u8"95", u8"96", u8"97", u8"98"
#endif // JESSILIB_IRC_SIMPLE_COLORS
};

static_assert(sizeof(s_irc_colors) / sizeof(color) == sizeof(s_irc_color_codes) / sizeof(std::string_view), "Color tables not aligned");
static constexpr size_t s_irc_colors_length = sizeof(s_irc_colors) / sizeof(color);

constexpr color to_color(uint8_t in_irc_color) {
	// Safety check
	if (in_irc_color >= s_irc_colors_length) {
		return {};
	}

	return s_irc_colors[in_irc_color];
}

constexpr uint8_t from_color(color in_color) {
	uint8_t best_match{};
	uint32_t best_match_distance_sq{ 0xFFFFFFFF };
	uint32_t distance_sq{};

	for (uint8_t index = 0; index != s_irc_colors_length; ++index) {
		distance_sq = in_color.distance_sq(s_irc_colors[index]);
		if (distance_sq < best_match_distance_sq) {
			best_match_distance_sq = distance_sq;
			best_match = index;

			if (distance_sq == 0) { // TODO: figure out smallest possible distance between any two points in the color table
				// Exact match; stop searching
				break;
			}
		}
	}

	return best_match;
}

constexpr color normalize_color(color in_color) {
	return s_irc_colors[from_color(in_color)];
}

constexpr std::u8string_view color_to_code(color in_color) {
	return s_irc_color_codes[from_color(in_color)];
}

static constexpr uint8_t BOLD{ 0x02 };
static constexpr uint8_t ITALIC{ 0x1D };
static constexpr uint8_t UNDERLINE{ 0x1F };
static constexpr uint8_t STRIKETHROUGH{ 0x1E };
static constexpr uint8_t MONOSPACE{ 0x11 };
static constexpr uint8_t COLOR{ 0x03 };
static constexpr uint8_t COLOR_HEX{ 0x04 };
static constexpr uint8_t REVERSE{ 0x16 };
static constexpr uint8_t NORMAL{ 0x0F };

inline text::property properties_to_toggle(text::property in_active_properties, text::property in_text_properties, uint8_t in_active_color, uint8_t in_text_color, uint8_t in_active_color_bg, uint8_t in_text_color_bg) {
	text::property_backing_t active_properties_backing = static_cast<text::property_backing_t>(in_active_properties);
	text::property_backing_t text_properties_backing = static_cast<text::property_backing_t>(in_text_properties);

	text::property_backing_t result{ active_properties_backing ^ text_properties_backing };

	// If both inputs had an active color, and that color isn't the same, explicitly force colored property.
	if (text_properties_backing & static_cast<text::property_backing_t>(text::property::colored)
		&& (active_properties_backing & static_cast<text::property_backing_t>(text::property::colored))
		&& in_active_color != in_text_color) {
			result |= static_cast<text::property_backing_t>(text::property::colored);
	}

	// If both inputs had an active background color, and that color isn't the same, explicitly force colored_bg property.
	if (text_properties_backing & static_cast<text::property_backing_t>(text::property::colored_bg)
		&& (active_properties_backing & static_cast<text::property_backing_t>(text::property::colored_bg))
		&& in_active_color_bg != in_text_color_bg) {
		result |= static_cast<text::property_backing_t>(text::property::colored_bg);
	}

	return text::property{ result };
}

} // namespace irc

template<>
inline std::u8string text_to_string<irc::text_wrapper>(const irc::text_wrapper& in_text) {
	std::u8string result;
	result.reserve(in_text.string().size() + 8);

	// Prepend properties
	if (in_text.has_property(text::property::bold)) {
		result += irc::BOLD;
	}
	if (in_text.has_property(text::property::italic)) {
		result += irc::ITALIC;
	}
	if (in_text.has_property(text::property::underline)) {
		result += irc::UNDERLINE;
	}
	if (in_text.has_property(text::property::strikethrough)) {
		result += irc::STRIKETHROUGH;
	}

	// Prepend color (if there is one)
	if (in_text.has_property(text::property::colored)) {
		result += irc::COLOR;
		result += irc::color_to_code(in_text.get_color());
	}

	// Append textual string
	result += in_text.string();

	// Reset (if needed)
	if (in_text.properties() != text::property::normal) {
		// Result any properties which were set
		result += irc::NORMAL;
	}

	return result;
}

} // namespace io
} // namespace jessilib

template<>
struct fmt::formatter<jessilib::io::irc::text_wrapper, char8_t> : formatter<std::u8string, char8_t> {
	template <typename FormatContext>
	auto format(const jessilib::io::irc::text_wrapper& in_text, FormatContext& in_context) {
		// Pass result to base
		return formatter<std::u8string, char8_t>::format(jessilib::io::text_to_string(in_text), in_context);
	}
};
