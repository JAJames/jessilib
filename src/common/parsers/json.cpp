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

#include "parsers/json.hpp"
#include <charconv>

using namespace std::literals;

namespace jessilib {

std::string make_json_string(std::u8string_view in_string) {
	std::string result;
	result.reserve(in_string.size() + 2);
	result = '\"';

	decode_result decode;
	while ((decode = decode_codepoint(in_string)).units != 0) {
		if (decode.codepoint == U'\\') { // backslash
			result += '\\';
			result += '\\';
		}
		else if (decode.codepoint == U'\"') { // quotation
			result += '\\';
			result += '\"';
		}
		else if (decode.codepoint < 0x20) { // control characters
			result += "\\u0000"sv;

			// overwrite last 2 zeroes with correct hexadecimal sequence
			char* data_end = result.data() + result.size();
			char* data = data_end - 2; // Will only ever use 2 chars
			auto to_chars_result = std::to_chars(data, data_end, static_cast<uint32_t>(decode.codepoint), 16);
			if (to_chars_result.ec == std::errc{} && to_chars_result.ptr != data_end) {
				// Only 1 byte written; shift it over
				*to_chars_result.ptr = *(to_chars_result.ptr - 1);

				// And fill in the zeroes
				*(to_chars_result.ptr - 1) = '0';
			}
		}
		else {
			// Valid UTF-8 sequence; copy it over
			result.append(reinterpret_cast<const char*>(in_string.data()), decode.units);
		}

		in_string.remove_prefix(decode.units);
	}

	result += '\"';
	return result;
}



object json_parser::deserialize(std::string_view in_data) {
	object result;
	deserialize_json<char, true>(result, in_data);
	return result;
}

std::string json_parser::serialize(const object& in_object) {
	static const object::array_type s_null_array;
	static const object::map_type s_null_map;

	switch (in_object.type()) {
		case object::type::null:
			return "null"s;

		case object::type::boolean:
			if (in_object.get<bool>()) {
				return "true"s;
			}
			return "false"s;

		case object::type::integer:
			return std::to_string(in_object.get<intmax_t>());

		case object::type::decimal:
			return std::to_string(in_object.get<long double>());

		case object::type::text:
			return make_json_string(in_object.get<std::u8string>());

		case object::type::array: {
			if (in_object.size() == 0) {
				return "[]"s;
			}

			std::string result;
			result = '[';

			// Serialize all objects in array
			for (auto& obj : in_object.get<object::array_type>(s_null_array)) {
				result += json_parser::serialize(obj);
				result += ',';
			}

			// Return result
			result.back() = ']';
			return result;
		}

		case object::type::map: {
			if (in_object.size() == 0) {
				return "{}"s;
			}

			std::string result;
			result = '{';

			// Serialize all objects in map
			for (auto& item : in_object.get<object::map_type>(s_null_map)) {
				result += make_json_string(item.first);
				result += ":"sv;
				result += json_parser::serialize(item.second);
				result += ',';
			}

			// Return result
			result.back() = '}';
			return result;
		}

		default:
			throw std::invalid_argument{ "Invalid data type: " + std::to_string(static_cast<size_t>(in_object.type())) };
	}
}

} // namespace jessilib
