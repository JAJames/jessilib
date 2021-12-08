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

#pragma once

#include "jessilib/io/message.hpp"

namespace jessilib {
namespace io {
namespace ansi {

// Message wrapper to allow passing messages into fmt::format
class text_wrapper : public text {};

// Control characters
static constexpr uint8_t ESCAPE_CHR{ 0x1B };
static constexpr std::u8string_view ESCAPE{ u8"\x1B[" };
// ESCAPE + '[' + <color or graphics code list> + 'm'

// Graphics modes
static constexpr uint8_t NORMAL{ '0' };
static constexpr uint8_t BOLD{ '1' };
static constexpr uint8_t UNDERLINE{ '4' };
static constexpr uint8_t BLINK{ '5' };
static constexpr uint8_t REVERSE{ '7' };
static constexpr uint8_t CONCEALED{ '8' };
static constexpr uint8_t GRAPHICS_SEP{ ';' };
static constexpr uint8_t GRAPHICS_END{ 'm' };

static constexpr std::u8string_view COLOR_HEX{ u8"38;2" };
static constexpr std::u8string_view COLOR_DEFAULT{ u8"39" };
static constexpr std::u8string_view COLOR_BG_HEX{ u8"48;2" };
static constexpr std::u8string_view COLOR_BG_DEFAULT{ u8"49" };

} // namespace ansi

template<>
inline std::u8string text_to_string<ansi::text_wrapper>(const ansi::text_wrapper& in_text) {
	std::u8string result;
	result.reserve(in_text.string().size() + 8);

	auto set_graphic_option = [&result](auto in_option) {
		if (result.empty()) {
			// Append escape sequence
			result = ansi::ESCAPE;
		}
		else {
			// Append value separator
			result += ansi::GRAPHICS_SEP;
		}

		// Append graphics option
		if constexpr (std::is_same_v<decltype(in_option), std::string>) {
			result += jessilib::string_view_cast<char8_t>(in_option);
		}
		else {
			result += in_option;
		}
	};

	// Set graphics properties
	if (in_text.has_property(text::property::bold)) {
		set_graphic_option(ansi::BOLD);
	}
	if (in_text.has_property(text::property::underline)) {
		set_graphic_option(ansi::UNDERLINE);
	}

	// Set foreground color (if there is one)
	if (in_text.has_property(text::property::colored)) {
		auto color = in_text.get_color();
		set_graphic_option(ansi::COLOR_HEX);
		set_graphic_option(std::to_string(color.red()));
		set_graphic_option(std::to_string(color.green()));
		set_graphic_option(std::to_string(color.blue()));
	}

	// Set background color (if there is one)
	if (in_text.has_property(text::property::colored_bg)) {
		auto color = in_text.get_color_bg();
		set_graphic_option(ansi::COLOR_BG_HEX);
		set_graphic_option(std::to_string(color.red()));
		set_graphic_option(std::to_string(color.green()));
		set_graphic_option(std::to_string(color.blue()));
	}

	// If any graphics options were set, mark them complete
	if (!result.empty()) {
		result += ansi::GRAPHICS_END;
	}

	// Append textual string
	result += in_text.string();

	// Reset (if needed)
	if (in_text.properties() != text::property::normal) {
		// Result any properties which were set
		result += ansi::ESCAPE;
		result += ansi::NORMAL;
		result += ansi::GRAPHICS_END;
	}

	return result;
}

} // namespace io
} // namespace jessilib

template<>
struct fmt::formatter<jessilib::io::ansi::text_wrapper, char8_t> : formatter<std::u8string, char8_t> {
	template <typename FormatContext>
	auto format(const jessilib::io::ansi::text_wrapper& in_text, FormatContext& in_context) {
		// Pass result to base
		return formatter<std::u8string, char8_t>::format(jessilib::io::text_to_string(in_text), in_context);
	}
};
