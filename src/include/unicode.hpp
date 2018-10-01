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

#include <string>
#include <string_view>
#include <ostream>

namespace jessilib {

/** encode_codepoint */

size_t encode_codepoint(std::string& out_string, char32_t in_codepoint);
size_t encode_codepoint(std::u16string& out_string, char32_t in_codepoint);
size_t encode_codepoint(std::u32string& out_string, char32_t in_codepoint);
size_t encode_codepoint(std::basic_ostream<char>& out_stream, char32_t in_codepoint);
size_t encode_codepoint(std::basic_ostream<char16_t>& out_stream, char32_t in_codepoint);
size_t encode_codepoint(std::basic_ostream<char32_t>& out_stream, char32_t in_codepoint);
std::string encode_codepoint_u8(char32_t in_codepoint);
std::u16string encode_codepoint_u16(char32_t in_codepoint);
std::u32string encode_codepoint_u32(char32_t in_codepoint);

/** decode_codepoint */

struct get_endpoint_result {
	char32_t codepoint{};
	size_t units{};
};

get_endpoint_result decode_codepoint(const std::string_view& in_string); // UTF-8
get_endpoint_result decode_codepoint(const std::u16string_view& in_string); // UTF-16
get_endpoint_result decode_codepoint(const std::u32string_view& in_string); // UTF-32

/** advance_codepoint */

template<typename T>
char32_t advance_codepoint(std::basic_string_view<T>& in_string) {
	auto result = decode_codepoint(in_string);
	in_string.remove_prefix(result.units);
	return result.codepoint;
}

/** next_codepoint */

template<typename T>
std::string_view next_codepoint(const std::basic_string_view<T>& in_string) {
	return in_string.substr(decode_codepoint(in_string).units);
}

/** is_valid_codepoint */

template<typename T>
bool is_valid_codepoint(const std::basic_string_view<T>& in_string) {
	return decode_codepoint(in_string).units != 0;
}

/** utf-16 surrogate helpers */

bool is_high_surrogate(char32_t in_codepoint);
bool is_low_surrogate(char32_t in_codepoint);
get_endpoint_result decode_surrogate_pair(char16_t in_high_surrogate, char16_t in_low_surrogate);

} // namespace jessilib
