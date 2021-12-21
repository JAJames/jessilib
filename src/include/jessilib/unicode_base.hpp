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

#pragma once

#include <string>
#include <string_view>
#include <ostream>

namespace jessilib {

/** encode_codepoint */

/**
 * Encodes a codepoint, and appends it to an output string
 *
 * @param out_string String to append
 * @param in_codepoint Codepoint to encode
 * @return Number of data elements appended to out_string
 */
template<typename CharT>
constexpr size_t encode_codepoint(std::basic_string<CharT>& out_string, char32_t in_codepoint);

/**
 * Encodes a codepoint to an output stream
 *
 * @param out_stream Stream to write codepoint to
 * @param in_codepoint Codepoint to encode
 * @return Number of data elements appending to out_stream
 */
template<typename CharT>
constexpr size_t encode_codepoint(std::basic_ostream<CharT>& out_stream, char32_t in_codepoint);

/**
 * Encodes a codepoint directly to a character buffer
 * Note: Do not use this without careful consideration; note the size requirements:
 * 1) char8_t may write up to 4 elements
 * 2) char16_t may write up to 2 elements
 * 3) char32_t may write up to 1 element
 * 4) char may write up to 4 elements; provided solely for compatibility/ease of use
 *
 * @param out_buffer Character buffer to write to
 * @param in_codepoint Codepoint to encode
 * @return Number of data elements written to out_buffer
 */
template<typename CharT>
constexpr size_t encode_codepoint(CharT* out_buffer, char32_t in_codepoint);

/**
 * Encodes a codepoint and returns it as a string
 *
 * @param in_codepoint Codepoint to encode
 * @return A string containing the codepoint encoded to the appropriate underlying CharT type
 */
std::u8string encode_codepoint_u8(char32_t in_codepoint);
std::u16string encode_codepoint_u16(char32_t in_codepoint);
std::u32string encode_codepoint_u32(char32_t in_codepoint);
std::wstring encode_codepoint_w(char32_t in_codepoint); // ASSUMES UTF-16 OR UTF-32W

/** decode_codepoint */

struct decode_result {
	char32_t codepoint{}; // Codepoint
	size_t units{}; // Number of data units codepoint was represented by, or 0
};

/**
 * Decodes the front codepoint in a string
 *
 * @param in_string String to decode a codepoint from
 * @return A struct containing a valid codepoint and the number of representative data units on success, zero otherwise.
 */
template<typename CharT>
constexpr decode_result decode_codepoint_utf8(std::basic_string_view<CharT> in_string); // UTF-8
template<typename CharT>
constexpr decode_result decode_codepoint_utf16(std::basic_string_view<CharT> in_string); // UTF-16
template<typename CharT>
constexpr decode_result decode_codepoint_utf32(std::basic_string_view<CharT> in_string); // UTF-32
template<typename CharT>
constexpr decode_result decode_codepoint(std::basic_string_view<CharT> in_string); // ASSUMES UTF-16 OR UTF-32
template<typename CharT>
constexpr decode_result decode_codepoint(const CharT* in_begin, size_t in_length);
template<typename CharT>
constexpr decode_result decode_codepoint(const CharT* in_begin, const CharT* in_end);

/** advance_codepoint */

template<typename T>
char32_t advance_codepoint(std::basic_string_view<T>& in_string) {
	auto result = decode_codepoint(in_string);
	in_string.remove_prefix(result.units);
	return result.codepoint;
}

/** next_codepoint */

template<typename T>
std::basic_string_view<T> next_codepoint(const std::basic_string_view<T>& in_string) {
	return in_string.substr(decode_codepoint(in_string).units);
}

/** is_valid_codepoint */

template<typename T>
bool is_valid_codepoint(const std::basic_string_view<T>& in_string) {
	return decode_codepoint(in_string).units != 0;
}

/** utf-16 surrogate helpers */

constexpr bool is_high_surrogate(char32_t in_codepoint);
constexpr bool is_low_surrogate(char32_t in_codepoint);
constexpr decode_result decode_surrogate_pair(char16_t in_high_surrogate, char16_t in_low_surrogate);

template<typename CharT>
struct unicode_traits : std::false_type {};

#ifdef JESSILIB_CHAR_AS_UTF8
template<>
struct unicode_traits<char> : std::true_type {
	using equivalent_type = char8_t; // DEPRECATE
	static constexpr size_t max_units_per_codepoint = 4;
};
#endif // JESSILIB_CHAR_AS_UTF8

template<>
struct unicode_traits<char8_t> : std::true_type {
#ifdef JESSILIB_CHAR_AS_UTF8
	using equivalent_type = char;
#else // JESSILIB_CHAR_AS_UTF8
	using equivalent_type = char8_t;
#endif // JESSILIB_CHAR_AS_UTF8
	static constexpr size_t max_units_per_codepoint = 4;
};

template<>
struct unicode_traits<char16_t> : std::true_type {
	using equivalent_type = std::conditional_t<sizeof(wchar_t) == sizeof(char16_t), wchar_t, char16_t>;
	static constexpr size_t max_units_per_codepoint = 2;
};

template<>
struct unicode_traits<char32_t> : std::true_type {
	using equivalent_type = std::conditional_t<sizeof(wchar_t) == sizeof(char32_t), wchar_t, char32_t>;
	static constexpr size_t max_units_per_codepoint = 1;
};

template<>
struct unicode_traits<wchar_t> : std::true_type {
	using equivalent_type = std::conditional_t<sizeof(wchar_t) == sizeof(char32_t), char32_t, char16_t>;
	static constexpr size_t max_units_per_codepoint = unicode_traits<equivalent_type>::max_units_per_codepoint;
};

template<typename CharT>
using encode_buffer_type = CharT[unicode_traits<CharT>::max_units_per_codepoint];

/** single-unit helper utilities */
char32_t fold(char32_t in_codepoint); // Folds codepoint for case-insensitive checks (not for human output)
constexpr int as_base(char32_t in_character, unsigned int base); // The value represented by in_character in terms of base if valid, -1 otherwise

/**
 * Checks if two codepoints are equal to each-other (case insensitive)
 *
 * @param lhs First codepoint to compare
 * @param rhs Second codepoint to compare
 * @return True if the characters are equal, false otherwise
 */
inline bool equalsi(char32_t lhs, char32_t rhs) {
	return lhs == rhs
		|| fold(lhs) == fold(rhs);
}

template<char32_t InCodepointV>
struct codepoint_info {
private:
	template<typename CharT>
	static constexpr size_t encode_codepoint_length(char32_t in_codepoint) {
		encode_buffer_type<CharT> buffer{};
		return encode_codepoint(buffer, in_codepoint);
	}

public:
	static constexpr char32_t value = InCodepointV;
	template<typename CharT>
	static constexpr size_t encode_length = encode_codepoint_length<CharT>(InCodepointV);

	template<typename CharT>
	using encode_buffer = CharT[encode_length<CharT>];

	static constexpr size_t utf8_length = encode_length<char8_t>;
	static constexpr size_t utf16_length = encode_length<char16_t>;
	static constexpr size_t utf32_length = encode_length<char32_t>;
	static constexpr size_t wchar_length = encode_length<wchar_t>;

	using utf8_buffer = char8_t[utf8_length];
	using utf16_buffer = char16_t[utf16_length];
	using utf32_buffer = char32_t[utf32_length];
	using wchar_buffer = wchar_t[wchar_length];

	static constexpr void encode(utf8_buffer& buffer) {
		encode_codepoint(buffer, InCodepointV);
	}

	static constexpr void encode(utf16_buffer& buffer) {
		encode_codepoint(buffer, InCodepointV);
	}

	static constexpr void encode(utf32_buffer& buffer) {
		encode_codepoint(buffer, InCodepointV);
	}

	static constexpr void encode(wchar_buffer& buffer) {
		encode_codepoint(buffer, InCodepointV);
	}
};

/**
 * Inline constexpr encode implementation
 */

/** encode_codepoint */

namespace impl_unicode {

template<typename T>
constexpr void append_helper(std::basic_string<T>& out_string, T in_value) {
	out_string += in_value;
}

template<typename T>
constexpr void append_helper(std::basic_ostream<T>& out_string, T in_value) {
	out_string << in_value;
}

template<typename T>
constexpr void append_helper(T*& out_string, T in_value) {
	*out_string = in_value;
	++out_string;
}

} // namespace impl_unicode

template<typename CharT, typename T>
constexpr size_t encode_codepoint_utf8(T& out_destination, char32_t in_codepoint) {
	if (in_codepoint > 0x10FFFF) {
		return 0;
	}

	if (in_codepoint <= 0x007F) {
		// 1-byte sequence (7 bits)
		impl_unicode::append_helper(out_destination, static_cast<CharT>(in_codepoint));
		return 1;
	}

	if (in_codepoint <= 0x07FF) {
		// 2-byte sequence (11 bits; 5 + 6)
		impl_unicode::append_helper(out_destination, static_cast<CharT>(0xC0 | ((in_codepoint >> 6) & 0x1F)));
		impl_unicode::append_helper(out_destination, static_cast<CharT>(0x80 | (in_codepoint & 0x3F)));
		return 2;
	}

	if (in_codepoint <= 0xFFFF) {
		// 3-byte sequence (16 bits; 4 + 6 + 6)
		impl_unicode::append_helper(out_destination, static_cast<CharT>(0xE0 | ((in_codepoint >> 12) & 0x0F)));
		impl_unicode::append_helper(out_destination, static_cast<CharT>(0x80 | ((in_codepoint >> 6) & 0x3F)));
		impl_unicode::append_helper(out_destination, static_cast<CharT>(0x80 | (in_codepoint & 0x3F)));
		return 3;
	}

	// 4-byte sequence (21 bits; 3 + 6 + 6 + 6)
	impl_unicode::append_helper(out_destination, static_cast<CharT>(0xF0 | ((in_codepoint >> 18) & 0x07)));
	impl_unicode::append_helper(out_destination, static_cast<CharT>(0x80 | ((in_codepoint >> 12) & 0x3F)));
	impl_unicode::append_helper(out_destination, static_cast<CharT>(0x80 | ((in_codepoint >> 6) & 0x3F)));
	impl_unicode::append_helper(out_destination, static_cast<CharT>(0x80 | (in_codepoint & 0x3F)));
	return 4;
}

template<typename CharT, typename T>
constexpr size_t encode_codepoint_utf16(T& out_destination, char32_t in_codepoint) {
	if (in_codepoint > 0x10FFFF) {
		return 0;
	}

	if (in_codepoint <= 0xFFFF) {
		// 1-unit sequence
		impl_unicode::append_helper(out_destination, static_cast<CharT>(in_codepoint));
		return 1;
	}

	// 2-unit sequence
	in_codepoint -= 0x10000;
	impl_unicode::append_helper(out_destination, static_cast<CharT>((in_codepoint >> 10) + 0xD800));
	impl_unicode::append_helper(out_destination, static_cast<CharT>((in_codepoint & 0x03FF) + 0xDC00));
	return 2;
}

template<typename CharT, typename T>
constexpr size_t encode_codepoint_utf32(T& out_destination, char32_t in_codepoint) {
	if (in_codepoint > 0x10FFFF) {
		return 0;
	}

	impl_unicode::append_helper(out_destination, static_cast<CharT>(in_codepoint));
	return 1;
}

template<typename T>
constexpr size_t encode_codepoint_w(T& out_destination, char32_t in_codepoint) {
	if constexpr (std::is_same_v<unicode_traits<wchar_t>::equivalent_type, char16_t>) {
		return encode_codepoint_utf16<wchar_t, T>(out_destination, in_codepoint);
	}

	if constexpr (std::is_same_v<unicode_traits<wchar_t>::equivalent_type, char32_t>) {
		return encode_codepoint_utf32<wchar_t, T>(out_destination, in_codepoint);
	}
}

template<typename CharT, typename T>
constexpr size_t encode_codepoint_utf(T& out_destination, char32_t in_codepoint) {
	if constexpr (std::is_same_v<CharT, char8_t>) {
		return encode_codepoint_utf8<CharT, T>(out_destination, in_codepoint);
	}
	else if constexpr (std::is_same_v<CharT, char16_t>) {
		return encode_codepoint_utf16<CharT, T>(out_destination, in_codepoint);
	}
	else if constexpr (std::is_same_v<CharT, char32_t>) {
		return encode_codepoint_utf32<CharT, T>(out_destination, in_codepoint);
	}
	else if constexpr (std::is_same_v<CharT, wchar_t>) {
		return encode_codepoint_w<T>(out_destination, in_codepoint);
	}
#ifdef JESSILIB_CHAR_AS_UTF8
	else if constexpr (std::is_same_v<CharT, char>) {
		return encode_codepoint_utf8<CharT, T>(out_destination, in_codepoint);
	}
#endif // JESSILIB_CHAR_AS_UTF8
}

template<typename CharT>
constexpr size_t encode_codepoint(std::basic_string<CharT>& out_string, char32_t in_codepoint) {
	return encode_codepoint_utf<CharT>(out_string, in_codepoint);
}

template<typename CharT>
constexpr size_t encode_codepoint(std::basic_ostream<CharT>& out_stream, char32_t in_codepoint) {
	return encode_codepoint_utf<CharT>(out_stream, in_codepoint);
}

template<typename CharT>
constexpr size_t encode_codepoint(CharT* out_buffer, char32_t in_codepoint) {
	return encode_codepoint_utf<CharT>(out_buffer, in_codepoint);
}

/**
 * Inline constexpr decode implementation
 */

/** decode_codepoint */

template<typename CharT>
constexpr decode_result decode_codepoint_utf8(std::basic_string_view<CharT> in_string) {
	decode_result result{ 0, 0 };

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

template<typename CharT>
constexpr decode_result decode_codepoint_utf16(std::basic_string_view<CharT> in_string) {
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

template<typename CharT>
constexpr decode_result decode_codepoint_utf32(std::basic_string_view<CharT> in_string) {
	if (in_string.empty()) {
		return { 0, 0 };
	}

	return { static_cast<char32_t>(in_string.front()), 1 };
}

template<typename CharT>
constexpr decode_result decode_codepoint(std::basic_string_view<CharT> in_string) {
	if constexpr (std::is_same_v<CharT, char8_t>) {
		return decode_codepoint_utf8(in_string);
	}
	else if constexpr (std::is_same_v<CharT, char16_t>) {
		return decode_codepoint_utf16(in_string);
	}
	else if constexpr (std::is_same_v<CharT, char32_t>) {
		return decode_codepoint_utf32(in_string);
	}
	else if constexpr (std::is_same_v<CharT, wchar_t>) {
		if constexpr (std::is_same_v<unicode_traits<wchar_t>::equivalent_type, char16_t>) {
			return decode_codepoint_utf16<wchar_t>(in_string);
		}
		else if constexpr (std::is_same_v<unicode_traits<wchar_t>::equivalent_type, char32_t>) {
			return decode_codepoint_utf32<wchar_t>(in_string);
		}
	}
#ifdef JESSILIB_CHAR_AS_UTF8
	else if constexpr (std::is_same_v<CharT, char>) {
		return decode_codepoint_utf8(in_string);
	}
#endif // JESSILIB_CHAR_AS_UTF8
}

template<typename CharT>
constexpr decode_result decode_codepoint(const CharT* in_begin, size_t in_length) {
	return decode_codepoint<CharT>(std::basic_string_view<CharT>{in_begin, in_length});
}

template<typename CharT>
constexpr decode_result decode_codepoint(const CharT* in_begin, const CharT* in_end) {
	return decode_codepoint<CharT>(std::basic_string_view<CharT>{in_begin, static_cast<size_t>(in_end - in_begin)});
}

constexpr bool is_high_surrogate(char32_t in_codepoint) {
	return in_codepoint >= 0xD800 && in_codepoint <= 0xDBFF;
}

constexpr bool is_low_surrogate(char32_t in_codepoint) {
	return in_codepoint >= 0xDC00 && in_codepoint <= 0xDFFF;
}

constexpr decode_result decode_surrogate_pair(char16_t in_high_surrogate, char16_t in_low_surrogate) {
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

// Maybe this should be moved back to .cpp and provide separate constexpr/non-constexpr variants?
static constexpr unsigned char base_table[]{
	127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
	127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 127, 127, 127, 127, 127, 127,
	127, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 127, 127, 127, 127, 127,
	127, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 127, 127, 127, 127, 127,
};

// If we're already making two comparisons, what would the real impact be of a couple more and removing the lookup table?
constexpr int as_base(char32_t in_character, unsigned int base) {
	if (in_character >= sizeof(base_table)) {
		return -1;
	}

	unsigned int result = base_table[in_character];
	if (result >= base) {
		return -1;
	}

	return base_table[in_character];
}

} // namespace jessilib
