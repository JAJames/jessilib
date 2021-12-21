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

#pragma once

#include <bit>
#include <string_view>

namespace jessilib {

static constexpr char32_t byte_order_mark_codepoint = 0xFEFF;

static_assert(sizeof(wchar_t) == sizeof(char16_t) || sizeof(wchar_t) == sizeof(char32_t),
	"Unexpected wchar_t size; neither char16 nor char32");

enum class text_encoding {
	utf_8 = 0,

	utf_16_little = 1,
	utf_16_big = 2,
	utf_16_native = (std::endian::native == std::endian::little ? utf_16_little : utf_16_big),
	utf_16_foreign = (std::endian::native == std::endian::little ? utf_16_big : utf_16_little),
	utf_16 = utf_16_native, // Alias for native

	utf_32_little = 3,
	utf_32_big = 4,
	utf_32_native = (std::endian::native == std::endian::little ? utf_32_little : utf_32_big),
	utf_32_foreign = (std::endian::native == std::endian::little ? utf_32_big : utf_32_little),
	utf_32 = utf_32_native, // Alias for native

	wchar = 5, // essentially only really for std::wcout / std::wcout
	multibyte = 6, // essentially only really for std::cout / std::cin
	//wchar = (sizeof(wchar_t) == sizeof(char16_t) ? utf_16 : utf_32),

	unknown
};

template<text_encoding EncodingV>
struct encoding_info;

template<>
struct encoding_info<text_encoding::utf_8> {
	using data_type = char8_t;
	static constexpr bool is_little = false;
	static constexpr bool is_big = false;
	static constexpr bool is_native = true;
	static constexpr bool is_foreign = !is_native;
	static constexpr bool is_agnostic = true;
	static constexpr size_t bom_byte_size = 3;
	static constexpr text_encoding encoding = text_encoding::utf_8;
};

template<>
struct encoding_info<text_encoding::utf_16_little> {
	using data_type = char16_t;
	static constexpr bool is_little = true;
	static constexpr bool is_big = false;
	static constexpr bool is_native = std::endian::native == std::endian::little;
	static constexpr bool is_foreign = !is_native;
	static constexpr bool is_agnostic = false;
	static constexpr size_t bom_byte_size = 2;
	static constexpr text_encoding encoding = text_encoding::utf_16;
};

template<>
struct encoding_info<text_encoding::utf_16_big> {
	using data_type = char16_t;
	static constexpr bool is_little = false;
	static constexpr bool is_big = true;
	static constexpr bool is_native = std::endian::native == std::endian::big;
	static constexpr bool is_foreign = !is_native;
	static constexpr bool is_agnostic = false;
	static constexpr size_t bom_byte_size = 2;
	static constexpr text_encoding encoding = text_encoding::utf_16;
};

template<>
struct encoding_info<text_encoding::utf_32_little> {
	using data_type = char32_t;
	static constexpr bool is_little = true;
	static constexpr bool is_big = false;
	static constexpr bool is_native = std::endian::native == std::endian::little;
	static constexpr bool is_foreign = !is_native;
	static constexpr bool is_agnostic = false;
	static constexpr size_t bom_byte_size = 4;
	static constexpr text_encoding encoding = text_encoding::utf_32;
};

template<>
struct encoding_info<text_encoding::utf_32_big> {
	using data_type = char32_t;
	static constexpr bool is_little = false;
	static constexpr bool is_big = true;
	static constexpr bool is_native = std::endian::native == std::endian::big;
	static constexpr bool is_foreign = !is_native;
	static constexpr bool is_agnostic = false;
	static constexpr size_t bom_byte_size = 4;
	static constexpr text_encoding encoding = text_encoding::utf_32;
};

template<>
struct encoding_info<text_encoding::wchar> {
	using data_type = wchar_t;
	static constexpr bool is_little = std::endian::native == std::endian::little;
	static constexpr bool is_big = std::endian::native == std::endian::big;
	static constexpr bool is_native = true;
	static constexpr bool is_foreign = !is_native;
	static constexpr bool is_agnostic = true; // Not truly agnostic, but wchar_t is for system-local use only... probably.
	static constexpr size_t bom_byte_size = 0; // Not supporting for wchar at this time
	static constexpr text_encoding encoding = text_encoding::wchar;
};

template<>
struct encoding_info<text_encoding::multibyte> {
	using data_type = char;
	static constexpr bool is_little = false;
	static constexpr bool is_big = false;
	static constexpr bool is_native = true;
	static constexpr bool is_foreign = !is_native;
	static constexpr bool is_agnostic = true;
	static constexpr size_t bom_byte_size = 0;
	static constexpr text_encoding encoding = text_encoding::multibyte;
};

template<typename CharT>
struct default_encoding_info;

template<>
struct default_encoding_info<char8_t> : public encoding_info<text_encoding::utf_8> {
};

template<>
struct default_encoding_info<char16_t> : public encoding_info<text_encoding::utf_16> {
};

template<>
struct default_encoding_info<char32_t> : public encoding_info<text_encoding::utf_32> {
};

template<>
struct default_encoding_info<wchar_t> : public encoding_info<text_encoding::wchar> {
};

/** Unicode byte-order markers */

enum class bom_encoding {
	utf_8 = 0,

	utf_16_little = 1,
	utf_16_big = 2,
	utf_16_native = (std::endian::native == std::endian::little ? utf_16_little : utf_16_big),
	utf_16 = utf_16_native,

	utf_32_little = 4,
	utf_32_big = 5,
	utf_32_native = (std::endian::native == std::endian::little ? utf_32_little : utf_32_big),
	utf_32 = utf_32_native,

	unknown = 0xFF
};

// If this results in a non-native encoding, the swaps have to be done passing to decode_character
constexpr bom_encoding peek_bom(std::string_view in_data) {
	if (in_data.size() < 2) {
		// Not enough space for any BOM
		return bom_encoding::unknown;
	}

	// Try UTF-16 BE
	if (in_data[0] == '\xFE' && in_data[1] == '\xFF') {
		return bom_encoding::utf_16_big;
	}

	// Try UTF-16 LE
	if (in_data[0] == '\xFF' && in_data[1] == '\xFE') {
		// Check UTF-32 LE
		if (in_data.size() >= 4
			&& in_data[2] == 0 && in_data[3] == 0) {
			return bom_encoding::utf_32_little;
		}

		return bom_encoding::utf_16_little;
	}

	if (in_data.size() < 3) {
		// Not enough space for any other BOMs
		return bom_encoding::unknown;
	}

	// Try UTF-8
	if (in_data[0] == '\xEF' && in_data[1] == '\xBB' && in_data[2] == '\xBF') {
		return bom_encoding::utf_8;
	}

	if (in_data.size() < 4) {
		// Not enough space for any other BOMs
		return bom_encoding::unknown;
	}

	// Try UTF-32 BE
	if (in_data[0] == 0 && in_data[1] == 0 && in_data[2] == '\xFE' && in_data[3] == '\xFF') {
		return bom_encoding::utf_32_big;
	}

	// No matches; return unknown (mostly likely encoded as UTF-8)
	return bom_encoding::unknown;
}

} // namespace jessilib
