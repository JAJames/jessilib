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

#include "unicode.hpp"

namespace jessilib {

/** encode_codepoint */

template<typename T>
void append_helper(std::basic_string<T>& out_string, T in_value) {
	out_string += in_value;
}

template<typename T>
void append_helper(std::basic_ostream<T>& out_string, T in_value) {
	out_string << in_value;
}

template<typename T>
size_t encode_codepoint_utf8(T& out_destination, char32_t in_codepoint) {
	if (in_codepoint > 0x10FFFF) {
		return 0;
	}

	if (in_codepoint <= 0x007F) {
		// 1-byte sequence (7 bits)
		append_helper(out_destination, static_cast<char>(in_codepoint));
		return 1;
	}

	if (in_codepoint <= 0x07FF) {
		// 2-byte sequence (11 bits; 5 + 6)
		append_helper(out_destination, static_cast<char>(0xC0 | ((in_codepoint >> 6) & 0x1F)));
		append_helper(out_destination, static_cast<char>(0x80 | (in_codepoint & 0x3F)));
		return 2;
	}

	if (in_codepoint <= 0xFFFF) {
		// 3-byte sequence (16 bits; 4 + 6 + 6)
		append_helper(out_destination, static_cast<char>(0xE0 | ((in_codepoint >> 12) & 0x0F)));
		append_helper(out_destination, static_cast<char>(0x80 | ((in_codepoint >> 6) & 0x3F)));
		append_helper(out_destination, static_cast<char>(0x80 | (in_codepoint & 0x3F)));
		return 3;
	}

	// 4-byte sequence (21 bits; 3 + 6 + 6 + 6)
	append_helper(out_destination, static_cast<char>(0xF0 | ((in_codepoint >> 18) & 0x07)));
	append_helper(out_destination, static_cast<char>(0x80 | ((in_codepoint >> 12) & 0x3F)));
	append_helper(out_destination, static_cast<char>(0x80 | ((in_codepoint >> 6) & 0x3F)));
	append_helper(out_destination, static_cast<char>(0x80 | (in_codepoint & 0x3F)));
	return 4;
}

template<typename T>
size_t encode_codepoint_utf16(T& out_destination, char32_t in_codepoint) {
	if (in_codepoint > 0x10FFFF) {
		return 0;
	}

	if (in_codepoint <= 0xFFFF) {
		// 1-unit sequence
		append_helper(out_destination, static_cast<char16_t>(in_codepoint));
		return 1;
	}

	// 2-unit sequence
	in_codepoint -= 0x10000;
	append_helper(out_destination, static_cast<char16_t>((in_codepoint >> 10) + 0xD800));
	append_helper(out_destination, static_cast<char16_t>((in_codepoint & 0x03FF) + 0xDC00));
	return 2;
}

template<typename T>
size_t encode_codepoint_utf32(T& out_destination, char32_t in_codepoint) {
	if (in_codepoint > 0x10FFFF) {
		return 0;
	}

	append_helper(out_destination, in_codepoint);
	return 1;
}

size_t encode_codepoint(std::string& out_string, char32_t in_codepoint) {
	return encode_codepoint_utf8(out_string, in_codepoint);
}

size_t encode_codepoint(std::u16string& out_string, char32_t in_codepoint) {
	return encode_codepoint_utf16(out_string, in_codepoint);
}

size_t encode_codepoint(std::u32string& out_string, char32_t in_codepoint) {
	return encode_codepoint_utf32(out_string, in_codepoint);
}

size_t encode_codepoint(std::basic_ostream<char>& out_stream, char32_t in_codepoint) {
	return encode_codepoint_utf8(out_stream, in_codepoint);
}

size_t encode_codepoint(std::basic_ostream<char16_t>& out_stream, char32_t in_codepoint) {
	return encode_codepoint_utf16(out_stream, in_codepoint);
}

size_t encode_codepoint(std::basic_ostream<char32_t>& out_stream, char32_t in_codepoint) {
	return encode_codepoint_utf32(out_stream, in_codepoint);
}

std::string encode_codepoint_u8(char32_t in_codepoint) {
	std::string result;
	encode_codepoint(result, in_codepoint);
	return result;
}

std::u16string encode_codepoint_u16(char32_t in_codepoint) {
	std::u16string result;
	encode_codepoint(result, in_codepoint);
	return result;
}

std::u32string encode_codepoint_u32(char32_t in_codepoint) {
	std::u32string result;
	encode_codepoint(result, in_codepoint);
	return result;
}

/** decode_codepoint */

get_endpoint_result decode_codepoint(const std::string_view& in_string) {
	get_endpoint_result result{ 0, 0 };

	if (in_string.empty()) {
		return result;
	}

	if ((in_string.front() & 0x80) != 0) { // UTF-8 sequence{
		// Validity check
		if (in_string.size() < 2
			|| (in_string.front() & 0x40) == 0) {
			// This is an invalid 1 byte sequence
			return result;
		}

		// get codepoint value
		if ((in_string.front() & 0x20) != 0) {
			// This is a 3+ byte sequence
			if (in_string.size() < 3) {
				// Invalid sequence; too few characters available
				return result;
			}

			if ((in_string.front() & 0x10) != 0) {
				// This is a 4 byte sequence
				if (in_string.size() < 4) {
					// Invalid sequence; too few characters available
					return result;
				}

				result.codepoint = static_cast<char32_t>(in_string[0] & 0x0F) << 18;
				result.codepoint += static_cast<char32_t>(in_string[1] & 0x3F) << 12;
				result.codepoint += static_cast<char32_t>(in_string[2] & 0x3F) << 6;
				result.codepoint += static_cast<char32_t>(in_string[3] & 0x3F);
				result.units = 4;
				return result;
			}

			// this is a 3 byte sequence
			result.codepoint = static_cast<char32_t>(in_string[0] & 0x0F) << 12;
			result.codepoint += static_cast<char32_t>(in_string[1] & 0x3F) << 6;
			result.codepoint += static_cast<char32_t>(in_string[2] & 0x3F);
			result.units = 3;
			return result;
		}

		// This is a 2 byte sequence
		result.codepoint = static_cast<char32_t>(in_string[0] & 0x1F) << 6;
		result.codepoint += static_cast<char32_t>(in_string[1] & 0x3F);
		result.units = 2;
		return result;
	}

	// This is a valid 1 byte sequence
	result.codepoint = static_cast<char32_t>(in_string.front());
	result.units = 1;

	return result;
}

get_endpoint_result decode_codepoint(const std::u16string_view& in_string) {
	if (in_string.empty()) {
		return { 0, 0 };
	}

	if (is_high_surrogate(in_string.front()) // If this is a high surrogate codepoint...
		&& in_string.size() > 1 // And a codepoint follows this surrogate..
		&& is_low_surrogate(in_string[1])) { // And that codepoint is a low surrogate...
		// We have a valid surrogate pair; decode it into a codepoint and return
		char32_t codepoint { static_cast<char32_t>(
			((in_string.front() - 0xD800U) * 0x400U) // high surrogate magic
			+ (in_string[1] - 0xDC00U) // low surrogate magic
			+ 0x10000ULL // more magic
		) };

		return { codepoint, 2 };
	}

	// Codepoint is a single char16_t; return codepoint directly
	return { in_string.front(), 1 };
}

get_endpoint_result decode_codepoint(const std::u32string_view& in_string) {
	if (in_string.empty()) {
		return { 0, 0 };
	}

	return { in_string.front(), 1 };
}

bool is_high_surrogate(char32_t in_codepoint) {
	return in_codepoint >= 0xD800 && in_codepoint <= 0xDBFF;
}

bool is_low_surrogate(char32_t in_codepoint) {
	return in_codepoint >= 0xDC00 && in_codepoint <= 0xDFFF;
}

get_endpoint_result decode_surrogate_pair(char16_t in_high_surrogate, char16_t in_low_surrogate) {
	if (is_high_surrogate(in_high_surrogate)
		&& is_low_surrogate((in_low_surrogate))) {
		// We have a valid surrogate pair; decode it into a codepoint and return
		char32_t codepoint { static_cast<char32_t>(
			((in_high_surrogate - 0xD800U) * 0x400U) // high surrogate magic
			+ (in_low_surrogate - 0xDC00U) // low surrogate magic
			+ 0x10000ULL // more magic
		) };

		return { codepoint, 2 };
	}

	return { 0, 0 };
}

} // namespace jessilib