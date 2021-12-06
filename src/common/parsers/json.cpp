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

#include "parsers/json.hpp"
#include <charconv>
#include "unicode.hpp"
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

	// Iterate over view until we reach the ending quotation, or the end of the view
	while (!in_data.empty()) {
		switch (in_data.front()) {
			// Escape sequence
			case '\\':
				// strip '\'
				in_data.remove_prefix(1);
				if (in_data.empty()) {
					throw std::invalid_argument{ "Invalid JSON data; unexpected end of data when parsing escape sequence" };
				}

				// Parse escape type
				switch (in_data.front()) {
					// Quote
					case '\"':
						in_data.remove_prefix(1);
						result += u8'\"';
						break;

					// Backslash
					case '\\':
						in_data.remove_prefix(1);
						result += u8'\\';
						break;

					// Forward slash
					case '/':
						in_data.remove_prefix(1);
						result += u8'/';
						break;

					// Backspace
					case 'b':
						in_data.remove_prefix(1);
						result += u8'\b';
						break;

					// Formfeed
					case 'f':
						in_data.remove_prefix(1);
						result += u8'\f';
						break;

					// Newline
					case 'n':
						in_data.remove_prefix(1);
						result += u8'\n';
						break;

					// Carriage return
					case 'r':
						in_data.remove_prefix(1);
						result += u8'\r';
						break;

					// Horizontal tab
					case 't':
						in_data.remove_prefix(1);
						result += u8'\t';
						break;

					// Unicode codepoint
					case 'u': {
						in_data.remove_prefix(1); // strip 'u'
						if (in_data.size() < 4) {
							throw std::invalid_argument{
								"Invalid JSON data; unexpected end of data when parsing unicode escape sequence" };
						}

						char16_t codepoint = get_codepoint_from_hex(in_data);
						in_data.remove_prefix(4); // strip codepoint hex

						if (is_high_surrogate(codepoint) // If we have a high surrogate...
							&& in_data.size() >= 6) { // And we have enough room for "\uXXXX"...
							// Special case: we just parsed a high surrogate. Handle this with the low surrogate, if there is one
							if (in_data.substr(0, 2) == "\\u"sv) {
								// Another codepoint follows; read it in
								in_data.remove_prefix(2); // strip "\u"
								char16_t second_codepoint = get_codepoint_from_hex(in_data);
								in_data.remove_prefix(4); // strip codepoint hex

								if (is_low_surrogate(second_codepoint)) {
									// We've got a valid surrogate pair; serialize the represented codepoint; decode it
									codepoint = static_cast<char16_t>(decode_surrogate_pair(codepoint, second_codepoint).codepoint);
									encode_codepoint(result, codepoint); // serialize the real codepoint
								}
								else {
									// This is not a valid surrogate pair; serialize the codepoints directly
									encode_codepoint(result, codepoint);
									encode_codepoint(result, second_codepoint);
								}
								continue;
							}
						}

						encode_codepoint(result, codepoint);
						continue;
					}

					default:
						throw std::invalid_argument{ "Invalid JSON data; unexpected token: '"s + in_data.front() + "' when parsing escape sequence" };
				}

				break;

			// End of string
			case '\"':
				in_data.remove_prefix(1); // strip trailing quotation
				advance_whitespace(in_data); // strip trailing spaces
				return result;

			// Unicode sequence
			default: {
				auto codepoint = decode_codepoint(in_data);
				if (codepoint.units == 0) {
					// Invalid unicode sequence
					throw std::invalid_argument{ "Invalid JSON data; unexpected token: '"s + in_data.front() + "' when parsing string" };
				}

				// Valid unicode sequence
				result.append(reinterpret_cast<const char8_t*>(in_data.data()), codepoint.units);
				in_data.remove_prefix(codepoint.units);
				break;
			}
		}
	}

	// We reached the end of the string_view before encountering an ending quote
	throw std::invalid_argument{ "Invalid JSON data; missing ending quote (\") when parsing string" };
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
