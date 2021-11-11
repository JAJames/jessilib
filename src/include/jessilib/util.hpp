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

#pragma once

#include <cstddef>
#include <charconv>

/** Macros */

#define JESSILIB_FILENAME \
	(::jessilib::impl::filename_from_string(__FILE__))

namespace jessilib {

template<typename T>
const char* parse_decimal_part(const char* in_str, const char* in_str_end, T& out_value) {
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
					out_value += (static_cast<T>(*in_str - '0') / denominator);
				}
				else {
					out_value -= (static_cast<T>(*in_str - '0') / denominator);
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

template<typename T>
std::from_chars_result from_chars(const char* in_str, const char* in_str_end, T& out_value) {
	// TODO: use std::from_chars when available for floating point types
	if constexpr (std::is_floating_point<T>::value) {
		// Read integer portion
		long long integer_value{};
		std::from_chars_result result{ std::from_chars(in_str, in_str_end, integer_value) };
		out_value = integer_value;

		// Read decimal portion (if one exists)
		if (result.ptr != in_str_end && *result.ptr == '.') {
			++result.ptr;
			result.ptr = parse_decimal_part(result.ptr, in_str_end, out_value);
			result.ec = std::errc{};
		}

		// TODO: Read exponents

		return result;
	}
	else {
		return std::from_chars(in_str, in_str_end, out_value);
	}
}

template<typename T>
constexpr T square(T in_value) {
	return in_value * in_value;
}

/** Implementation details */


namespace impl {

template<size_t in_filename_length>
constexpr const char* filename_from_string(const char (&in_filename)[in_filename_length]) {
	const char* filename_itr = in_filename;
	const char* filename_end = filename_itr + in_filename_length;
	const char* result = filename_itr;

	while (filename_itr != filename_end) {
		if (*filename_itr == '/' || *filename_itr == '\\') {
			++filename_itr;
			result = filename_itr;
		}
		else {
			++filename_itr;
		}
	}

	return result;
}

} // namespace impl
} // namespace jessilib
