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

#include <cstddef>
#include <charconv>
#include <string>
#include <algorithm>
#include <bit>

/** Macros */

#define JESSILIB_IMPL_TYPE_LITERAL(IN, ...) __VA_ARGS__ ## IN
#define JESSILIB_TYPE_LITERAL(IN, ...) JESSILIB_IMPL_TYPE_LITERAL(IN, __VA_ARGS__)

#define JESSILIB_FILENAME(...) \
	(::jessilib::impl::filename_from_string(JESSILIB_TYPE_LITERAL(__FILE__, __VA_ARGS__)))

#define JESSILIB_FILENAME_VIEW(...) (::jessilib::make_string_view(JESSILIB_FILENAME(__VA_ARGS__)))

namespace jessilib {

template<typename CharT>
std::basic_string_view<CharT> make_string_view(const CharT* in_ntstring) {
	return in_ntstring;
}

template<typename CharT, typename NumberT>
const CharT* parse_decimal_part(const CharT* in_str, const CharT* in_str_end, NumberT& out_value) {
	int denominator = 10;
	while (in_str != in_str_end) {
		switch (*in_str) {
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				if (out_value >= 0.0) {
					out_value += (static_cast<NumberT>(*in_str - '0') / denominator);
				}
				else {
					out_value -= (static_cast<NumberT>(*in_str - '0') / denominator);
				}
				denominator *= 10;
				break;

			default:
				return in_str;
		}

		++in_str;
	}

	return in_str;
}

template<typename CharT>
struct from_chars_result {
	const CharT* ptr;
	std::errc ec;
};

template<typename CharT, typename NumberT,
	std::enable_if_t<sizeof(CharT) == 1>* = nullptr>
from_chars_result<CharT> from_chars(const CharT* in_str, const CharT* in_str_end, NumberT& out_value) {
	std::from_chars_result std_result{};

	// TODO: use std::from_chars when available for floating point types
	if constexpr (std::is_floating_point<NumberT>::value) {
		// Read integer portion
		long long integer_value{};
		std_result = std::from_chars(reinterpret_cast<const char*>(in_str), reinterpret_cast<const char*>(in_str_end), integer_value);
		out_value = integer_value;

		// Read decimal portion (if one exists)
		if (std_result.ptr != in_str_end && *std_result.ptr == '.') {
			++std_result.ptr;
			std_result.ptr = parse_decimal_part(std_result.ptr, in_str_end, out_value);
			std_result.ec = std::errc{};
		}

		// TODO: Read exponents
	}
	else {
		std_result = std::from_chars(reinterpret_cast<const char*>(in_str), reinterpret_cast<const char*>(in_str_end), out_value);
	}

	return { reinterpret_cast<const CharT*>(std_result.ptr), std_result.ec };
}

// All characters passed in MUST be in: [U'0', U'9'], '.'
template<typename CharT, typename NumberT,
	std::enable_if_t<sizeof(CharT) != 1>* = nullptr>
from_chars_result<CharT> from_chars(CharT* in_str, CharT* in_str_end, NumberT& out_value) {
	// Copy our string into a temporary buffer, then use from_chars on that
	char buffer[256]; // TODO: get some sort of metrics on this to figure out if this is acceptable temporary approach
	size_t distance = static_cast<size_t>(in_str_end - in_str);
	if (distance > sizeof(buffer)) {
		// Way too much data; just slice it off, maybe add a debug assertion. This method's supposed to be temporary.
		distance = sizeof(buffer);
		in_str_end = in_str + distance;
	}

	// Copy the view into the buffer
	char* itr = buffer;
	while (in_str != in_str_end) {
		*itr = static_cast<char>(*in_str);
		++itr;
		++in_str;
	}

	// leverage from_chars
	char* buffer_end = buffer + distance;
	auto char_result = from_chars(buffer, buffer_end, out_value);
	distance = char_result.ptr - buffer;
	return { in_str + distance, char_result.ec };
}

template<typename T>
constexpr T square(T in_value) {
	return in_value * in_value;
}

#ifdef __cpp_lib_byteswap
using std::byteswap;
#else

template<typename IntegerT>
constexpr IntegerT byteswap(IntegerT in_integer) { // TODO: Remove w/ C++23
	static_assert(sizeof(IntegerT) > 1, "byteswap on single byte does nothing");

	if constexpr (sizeof(IntegerT) == 2) {
		return (in_integer << 8) | (in_integer >> 8);
	}
	else if constexpr (sizeof(IntegerT) == 4) {
		return (in_integer << 24)
			| ((in_integer & 0xFF00) << 8)
			| ((in_integer & 0xFF0000) >> 8)
			| (in_integer >> 24);
	}
	else if constexpr (sizeof(IntegerT) == 8) {
		return (in_integer << 56)
			| ((in_integer & 0xFF00) << 40)
			| ((in_integer & 0xFF0000) << 24)
			| ((in_integer & 0xFF000000) << 8)
			| ((in_integer & 0xFF00000000) >> 8)
			| ((in_integer & 0xFF0000000000) >> 24)
			| ((in_integer & 0xFF000000000000) >> 40)
			| (in_integer >> 56);
	}
	else {
		// Fallback to std::reverse for exotic integer types
		std::reverse(reinterpret_cast<unsigned char*>(&in_integer),
			reinterpret_cast<unsigned char*>(&in_integer) + sizeof(IntegerT));
		return in_integer;
	}
}

#endif // __cpp_lib_byteswap

template<typename CharT>
void array_byteswap(CharT* begin, CharT* end) {
	while (begin != end) {
		*begin = byteswap(*begin);
		++begin;
	}
}

template<typename CharT>
void string_byteswap(std::string& in_string) {
	// Ensure correct number of bytes for underlying representation type
	if (in_string.size() % sizeof(CharT) != 0) {
		return;
	}

	for (auto itr = in_string.begin(); itr != in_string.end();) {
		auto unit_end = itr + sizeof(CharT);
		std::reverse(itr, unit_end);
		itr = unit_end;
	}
}

/** Implementation details */

namespace impl {

template<typename CharT, size_t in_filename_length>
constexpr const CharT* filename_from_string(const CharT (&in_filename)[in_filename_length]) {
	const CharT* filename_itr = in_filename;
	const CharT* filename_end = filename_itr + in_filename_length;
	const CharT* result = filename_itr;

	while (filename_itr != filename_end) {
		if (*filename_itr == U'/' || *filename_itr == U'\\') {
			++filename_itr;
			result = filename_itr;
		}
		else {
			++filename_itr;
		}
	}

	return result;
}

template<typename CharT, size_t in_filename_length>
constexpr const CharT* file_extension_from_string(const CharT (&in_filename)[in_filename_length]) {
	const CharT* filename_itr = in_filename;
	const CharT* filename_end = filename_itr + in_filename_length;
	const CharT* result = filename_end;

	while (filename_itr != filename_end) {
		if (*filename_itr == U'/' || *filename_itr == U'\\') {
			++filename_itr;
			result = filename_end;
		}
		else {
			if (*filename_itr == '.') {
				result = filename_itr;
			}

			++filename_itr;
		}
	}

	return result;
}

} // namespace impl
} // namespace jessilib
