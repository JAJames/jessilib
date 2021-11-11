/**
 * Copyright (C) 2019-2020 Jessica James.
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

#include "io/message.hpp"

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

static constexpr std::string_view s_irc_color_codes[] {
	// Basic 16 colors (0-15)
	"00", "01", "02", "03", "04", "05", "06", "07", "08", "09", "10", "11", "12", "13", "14", "15",

#ifndef JESSILIB_IRC_SIMPLE_COLORS
	// Extended colors (16-98, making a total of 99 color choices)
	"16", "17", "18", "19", "20", "21", "22", "23", "24", "25", "26", "27",
	"28", "29", "30", "31", "32", "33", "34", "35", "36", "37", "38", "39",
	"40", "41", "42", "43", "44", "45", "46", "47", "48", "49", "50", "51",
	"52", "53", "54", "55", "56", "57", "58", "59", "60", "61", "62", "63",
	"64", "65", "66", "67", "68", "69", "70", "71", "72", "73", "74", "75",
	"76", "77", "78", "79", "80", "81", "82", "83", "84", "85", "86", "87",
	"88", "89", "90", "91", "92", "93", "94", "95", "96", "97", "98"
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

constexpr std::string_view color_to_code(color in_color) {
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

text::property properties_to_toggle(text::property in_active_properties, text::property in_text_properties, uint8_t in_active_color, uint8_t in_text_color, uint8_t in_active_color_bg, uint8_t in_text_color_bg) {
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
inline std::string text_to_string<irc::text_wrapper>(const irc::text_wrapper& in_text) {
	std::string result;
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
struct fmt::formatter<jessilib::io::irc::text_wrapper> : formatter<std::string> {
	template <typename FormatContext>
	auto format(const jessilib::io::irc::text_wrapper& in_text, FormatContext& in_context) {
		// Pass result to base
		return formatter<std::string>::format(jessilib::io::text_to_string(in_text), in_context);
	}
};
