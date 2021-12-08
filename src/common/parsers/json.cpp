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
#include "unicode.hpp"
#include "unicode_sequence.hpp"
#include "util.hpp"

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

void advance_whitespace(std::string_view& in_data) {
	while (!in_data.empty()) {
		switch (in_data.front()) {
			case ' ':
			case '\t':
			case '\r':
			case '\n':
				in_data.remove_prefix(1);
				break;

			default:
				return;
		}
	}
}

uint16_t get_codepoint_from_hex(const std::string_view& in_data) {
	uint16_t value{};
	auto data = in_data.data();
	auto end = in_data.data() + 4;
	data = std::from_chars(data, end, value, 16).ptr;
	if (data != end) {
		throw std::invalid_argument{ "Invalid JSON data; unexpected token: '"s + *data + "' when parsing unicode escape sequence" };
	};

	return value;
}

std::u8string read_json_string(std::string_view& in_data) {
	std::u8string result;

	// Remove leading quotation
	in_data.remove_prefix(1);

	if (in_data.empty()) {
		throw std::invalid_argument{ "Invalid JSON data; missing ending quote (\") when parsing string" };
	}

	if (in_data.front() == '\"') {
		in_data.remove_prefix(1);
		advance_whitespace(in_data); // strip trailing spaces
		return result;
	}

	size_t search_start = 1;
	size_t end_pos;
	while ((end_pos = in_data.find('\"', search_start)) != std::string_view::npos) {
		// Quote found; check if it's escaped
		if (in_data[end_pos - 1] != '\\') {
			// Unescaped quote; must be end of string
			break;
		}

		search_start = end_pos + 1;
	}

	if (end_pos == std::string_view::npos) {
		throw std::invalid_argument{ "Invalid JSON data; missing ending quote (\") when parsing string" };
	}

	std::u8string_view string_data = jessilib::string_view_cast<char8_t>(in_data.substr(0, end_pos));
	in_data.remove_prefix(string_data.size() + 1);
	advance_whitespace(in_data); // strip trailing spaces
	result = string_data;
	if (!jessilib::apply_cpp_escape_sequences(result)) {
		throw std::invalid_argument{ jessilib::join<std::string>("Invalid JSON data; invalid token or end of string: "sv, string_data) };
	}

	return result;
}

object read_json_number(std::string_view& in_data) {
	// parse integer
	intmax_t integer_value{};
	const char* from_chars_end = std::from_chars(in_data.data(), in_data.data() + in_data.size(), integer_value).ptr;
	if (in_data.data() == from_chars_end) {
		// Failed to parse integer portion
		throw std::invalid_argument{ "Invalid JSON data; unexpected token: '"s + in_data.front() + "' when parsing number" };
	}

	// Strip integer portion and return if nothing remains
	in_data.remove_prefix(from_chars_end - in_data.data());
	if (in_data.empty() || in_data.front() != '.') {
		return integer_value;
	}

	// Parse decimal portion

	/*
	// std::from_chars method
	long double decimal_value{};
	from_chars_end = std::from_chars(data, data_end, decimal_value).ptr;
	return static_cast<long double>(integer_value) + decimal_value;
	 */

	// parse_decimal_part method
	in_data.remove_prefix(1); // strip leading '.'
	long double decimal_value = static_cast<long double>(integer_value);
	from_chars_end = parse_decimal_part(in_data.data(), in_data.data() + in_data.size(), decimal_value);
	// TODO: parse exponent

	// Strip decimal portion and return
	in_data.remove_prefix(from_chars_end - in_data.data());
	return decimal_value;
}

object read_json_object(std::string_view& in_data) {
	while (!in_data.empty()) {
		switch (in_data.front()) {
			/** Start of null */
			case 'n':
				if (in_data.substr(0, 4) != "null"sv) {
					throw std::invalid_argument{ "Invalid JSON data; unexpected token: '"s + std::string{ in_data } + "' when parsing null" };
				}

				in_data.remove_prefix(4);
				return {};

			/** Start of boolean (true) */
			case 't':
				if (in_data.substr(0, 4) != "true"sv) {
					throw std::invalid_argument{ "Invalid JSON data; unexpected token: '"s + std::string{ in_data } + "' when parsing boolean" };
				}

				in_data.remove_prefix(4);
				return true;

			/** Start of boolean (false) */
			case 'f':
				if (in_data.substr(0, 5) != "false"sv) {
					throw std::invalid_argument{ "Invalid JSON data; unexpected token: '"s + std::string{ in_data } + "' when parsing boolean" };
				}

				in_data.remove_prefix(5);
				return false;

			/** Whitespace */
			case ' ':
			case '\t':
			case '\r':
			case '\n':
				in_data.remove_prefix(1);
				break;

			/** Start of string */
			case '\"':
				return read_json_string(in_data);

			/** Start of number */
			case '-':
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9': {
				return read_json_number(in_data);
			}

			/** Start of array */
			case '[': {
				// Strip brace and leading whitespace
				in_data.remove_prefix(1);
				advance_whitespace(in_data);

				// Build and populate result
				std::vector<object> result;
				while (true) {
					if (in_data.empty()) {
						throw std::invalid_argument{ "Invalid JSON data; unexpected end of data when parsing object array" };
					}

					if (in_data.front() == ']') {
						// End of array
						in_data.remove_prefix(1);
						return result;
					}

					// We've reached the start of an object; parse it into our array
					result.push_back(read_json_object(in_data));

					// Strip leading whitespace
					advance_whitespace(in_data);

					if (in_data.empty()) {
						throw std::invalid_argument{ "Invalid JSON data; unexpected end of data when parsing object array" };
					}

					if (in_data.front() == ',') {
						// Strip comma and trailing whitespace
						in_data.remove_prefix(1);
						advance_whitespace(in_data);
					}
				}
			}

			/** Start of map */
			case '{': {
				// Strip brace and leading whitespace
				in_data.remove_prefix(1);
				advance_whitespace(in_data);

				// Build and populate result
				object result{ object::map_type{} };
				while (true) {
					if (in_data.empty()) {
						throw std::invalid_argument{ "Invalid JSON data; unexpected end of data when parsing object map" };
					}

					if (in_data.front() == '}') {
						// End of object
						in_data.remove_prefix(1);
						return result;
					}

					// Assert that we've reached the start of a key
					if (in_data.front() != '\"') {
						throw std::invalid_argument{
							"Invalid JSON data; unexpected token: '"s + in_data.front() + "' when parsing object map" };
					}

					// Read in the key and build a value
					auto& value = result[read_json_string(in_data)];

					// Verify next character is ':'
					if (in_data.empty()) {
						throw std::invalid_argument{
							"Invalid JSON data; unexpected end of data after parsing map key; expected ':' followed by value" };
					}
					if (in_data.front() != ':') {
						throw std::invalid_argument{ "Invalid JSON data; unexpected token: '"s + in_data.front()
							+ "' when parsing map key (expected ':' instead)" };
					}
					in_data.remove_prefix(1); // strip ':'

					// We've reached an object value; parse it
					value = read_json_object(in_data);

					// Advance through whitespace to ',' or '}'
					advance_whitespace(in_data);

					if (in_data.empty()) {
						throw std::invalid_argument{
							"Invalid JSON data; unexpected end of data after parsing map value; expected '}'" };
					}

					if (in_data.front() == ',') {
						// Strip comma and trailing whitespace
						in_data.remove_prefix(1);
						advance_whitespace(in_data);
					}
				}

				// Unreachable; above code will always return on success or throw on failure
			}

			default:
				throw std::invalid_argument{ "Invalid JSON data; unexpected token: '"s + in_data.front() + "' when parsing JSON" };
		}
	}

	// No non-whitespace data passed in; return a null object
	return {};
}

object json_parser::deserialize(std::string_view in_data) {
	return read_json_object(in_data);
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
