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

#include "fmt/xchar.h" // fmt::format
#include "jessilib/parser.hpp"
#include "jessilib/unicode.hpp" // join
#include "jessilib/unicode_syntax.hpp" // syntax trees
#include "jessilib/unicode_sequence.hpp" // apply_cpp_escape_sequences
#include "jessilib/util.hpp" // from_chars

namespace jessilib {

class json_parser : public parser {
public:
	/** deserialize/serialize overrides */
	object deserialize_bytes(bytes_view_type in_data, encoding in_write_encoding) override;
	std::string serialize_bytes(const object& in_object, encoding in_write_encoding) override;

	std::u8string serialize_u8(const object& in_object) override { return serialize_impl<char8_t>(in_object); }
	std::u16string serialize_u16(const object& in_object) override { return serialize_impl<char16_t>(in_object); }
	std::u32string serialize_u32(const object& in_object) override { return serialize_impl<char32_t>(in_object); }
	std::wstring serialize_w(const object& in_object) override { return serialize_impl<wchar_t>(in_object); }

	template<typename CharT, typename ResultCharT = CharT>
	std::basic_string<ResultCharT> serialize_impl(const object& in_object) {
		std::basic_string<ResultCharT> result;
		serialize_impl<CharT, ResultCharT>(result, in_object);
		return result;
	}

	template<typename CharT, typename ResultCharT = CharT>
	void serialize_impl(std::basic_string<ResultCharT>& out_string, const object& in_object);
};

/**
 * JSON Parse Tree
 */

// TODO: remove this
template<typename CharT>
void advance_whitespace(std::basic_string_view<CharT>& in_data) {
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

template<typename CharT, bool UseExceptionsV = true>
struct json_context {
	object& out_object;
	static constexpr bool use_exceptions{ UseExceptionsV };
};

// Doesn't do decoding, because we know our keyword is all basic latin (1 data unit, regardless of encoding)
template<typename CharT>
constexpr bool starts_with_fast(std::basic_string_view<CharT> in_string, std::u8string_view in_substring) {
	if (in_string.size() < in_substring.size()) {
		return false;
	}

	const CharT* itr = in_string.data();
	for (auto character : in_substring) {
		if (*itr != character) {
			return false;
		}

		++itr;
	}

	return true;
}

template<typename CharT, typename ContextT, char32_t InCodepointV, const std::u8string_view& KeywordRemainderV, typename ValueT, ValueT ValueV>
constexpr syntax_tree_member<CharT, ContextT> make_keyword_value_pair() {
	// null, true, false
	return { InCodepointV, [](ContextT& inout_context, std::basic_string_view<CharT>& inout_read_view) constexpr -> size_t {
		if (starts_with_fast(inout_read_view, KeywordRemainderV)) {
			// This is the keyword; go ahead and chuck it in
			if constexpr (std::is_pointer_v<ValueT> || std::is_null_pointer_v<ValueT>) {
				if constexpr (ValueV == nullptr) {
					inout_context.out_object = object{};
				}
			}
			else {
				inout_context.out_object = ValueV;
			}
			inout_read_view.remove_prefix(KeywordRemainderV.size());
			return 1;
		}

		// Unexpected character; throw if appropriate
		if constexpr (ContextT::use_exceptions) {
			using namespace std::literals;
			throw std::invalid_argument{ jessilib::join_mbstring(u8"Invalid JSON data; unexpected token: '"sv,
				inout_read_view,
				u8"' when parsing null"sv) };
		}

		return std::numeric_limits<size_t>::max();
	} };
}

template<typename CharT, typename ContextT, char32_t InCodepointV>
constexpr syntax_tree_member<CharT, ContextT> make_noop_pair() {
	return { InCodepointV, [](ContextT&, std::basic_string_view<CharT>&) constexpr -> size_t {
		return 0;
	} };
}

template<typename CharT, typename ContextT>
size_t string_start_action(ContextT& inout_context, std::basic_string_view<CharT>& inout_read_view) {
	// Safety check
	if (inout_read_view.empty()) {
		if constexpr (ContextT::use_exceptions) {
			throw std::invalid_argument{ "Invalid JSON data; missing ending quote (\") when parsing string" };
		}

		return std::numeric_limits<size_t>::max();
	}

	// Check if this is just an empty string
	if (inout_read_view.front() == '\"') {
		inout_read_view.remove_prefix(1);
		inout_context.out_object = std::u8string{};
		return 1;
	}

	// Not an empty string; search for the ending quote
	size_t search_start = 1;
	size_t end_pos;
	while ((end_pos = inout_read_view.find('\"', search_start)) != std::string_view::npos) {
		// Quote found; check if it's escaped
		if (inout_read_view[end_pos - 1] != '\\') {
			// Unescaped quote; must be end of string
			break;
		}

		search_start = end_pos + 1;
	}

	// Early out if we didn't find the terminating quote
	if (end_pos == std::string_view::npos) {
		if constexpr (ContextT::use_exceptions) {
			throw std::invalid_argument{ "Invalid JSON data; missing ending quote (\") when parsing string" };
		}

		return std::numeric_limits<size_t>::max();
	}

	// jessilib::object only current accepts UTF-8 text; copy the necessary data instead of sequencing in-place
	// additionally, even when it does accept other encodings, it'll be storing them as UTF-8 as well, though
	// sequencing in-place and recoding the result would still likely be slightly quicker than recoding the input
	std::u8string string_data = jessilib::string_cast<char8_t>(inout_read_view.substr(0, end_pos));
	inout_read_view.remove_prefix(string_data.size() + 1); // Advance the read view to after the terminating quote
	if (!jessilib::apply_cpp_escape_sequences(string_data)) {
		if constexpr (ContextT::use_exceptions) {
			using namespace std::literals;
			throw std::invalid_argument {
				jessilib::join_mbstring(u8"Invalid JSON data; invalid token or end of string: "sv,
					std::u8string_view{ string_data })
			};
		}

		return std::numeric_limits<size_t>::max();
	}

	inout_context.out_object = std::move(string_data);
	return 1;
}

template<typename CharT, typename ContextT, char32_t InCodepointV>
constexpr syntax_tree_member<CharT, ContextT> make_string_start_pair() {
	// no constexpr in this context because gcc
	return { InCodepointV, string_start_action<CharT, ContextT> };
}

template<typename CharT, bool UseExceptionsV = true,
	default_syntax_tree_action<CharT, json_context<CharT, UseExceptionsV>> DefaultActionF = fail_action<CharT, json_context<CharT, UseExceptionsV>, UseExceptionsV>>
bool deserialize_json(object& out_object, std::basic_string_view<CharT>& inout_read_view);

template<typename CharT, typename ContextT>
size_t array_start_action(ContextT& inout_context, std::basic_string_view<CharT>& inout_read_view) {
	std::vector<object> result;

	advance_whitespace(inout_read_view);
	if (inout_read_view.empty()) {
		if constexpr (ContextT::use_exceptions) {
			throw std::invalid_argument{ "Invalid JSON data: unexpected end of data when parsing object array; expected ']'" };
		}

		return std::numeric_limits<size_t>::max();
	}

	// Checking here instead of top of loop means no trailing comma support.
	if (inout_read_view.front() == ']') {
		// End of array; success
		inout_read_view.remove_prefix(1);
		inout_context.out_object = std::move(result);
		return 1;
	}

	do {
		// Read object
		object obj;
		if (!deserialize_json<CharT, ContextT::use_exceptions>(obj, inout_read_view)) {
			// Invalid JSON! Any exception would've been thrown already
			break;
		}
		result.push_back(std::move(obj));

		advance_whitespace(inout_read_view);
		if (inout_read_view.empty()) {
			// Unexpected end of data; missing ']'; fail
			break;
		}

		CharT front = inout_read_view.front();
		if (front == ',') {
			// Strip comma
			inout_read_view.remove_prefix(1);
			advance_whitespace(inout_read_view);

			// Right now there's no trailing comma support; should behavior be a template option?
		}
		else if (front == ']') {
			// End of array; success
			inout_read_view.remove_prefix(1);
			inout_context.out_object = std::move(result);
			return 1;
		}
		else {
			// Invalid JSON!
			if constexpr (ContextT::use_exceptions) {
				using namespace std::literals;
				throw std::invalid_argument{ jessilib::join_mbstring(
					u8"Invalid JSON data: expected ',' or ']', instead encountered: "sv,
					inout_read_view) };
			}

			return std::numeric_limits<size_t>::max();
		}
	} while (!inout_read_view.empty());

	// Invalid JSON encountered
	if constexpr (ContextT::use_exceptions) {
		throw std::invalid_argument{ "Invalid JSON data: unexpected end of data when parsing object array; expected ']'" };
	}

	return std::numeric_limits<size_t>::max();
}

template<typename CharT, typename ContextT, char32_t InCodepointV>
constexpr syntax_tree_member<CharT, ContextT> make_array_start_pair() {
	return { InCodepointV, array_start_action<CharT, ContextT> };
}

template<bool UseExceptionsV>
struct KeyContext {
	std::u8string out_object;
	static constexpr bool use_exceptions = UseExceptionsV;
};

template<typename CharT, typename ContextT>
size_t make_map_start_action(ContextT& inout_context, std::basic_string_view<CharT>& inout_read_view) {
	using namespace std::literals;
	object::map_type result;

	advance_whitespace(inout_read_view);
	KeyContext<ContextT::use_exceptions> key_context;
	while (!inout_read_view.empty()) {
		// inout_read_view now points to either the start of a key, the end of the object, or invalid data
		CharT front = inout_read_view.front();
		if (front == '}') {
			// End of object
			inout_read_view.remove_prefix(1);
			inout_context.out_object = object{ std::move(result) }; // TODO: fix move semantics here
			return 1;
		}

		// Assert that we've reached the start of a key
		if (front != '\"') {
			if constexpr (ContextT::use_exceptions) {
				throw std::invalid_argument{
					jessilib::join_mbstring(u8"Invalid JSON data; unexpected token: '"sv,
					decode_codepoint(inout_read_view).codepoint,
					u8"' when parsing object map (expected '\"' instead)"sv) };
			}

			return std::numeric_limits<size_t>::max();
		}

		// Read in key
		inout_read_view.remove_prefix(1); // front quote
		// TODO: really should be using the escape sequencing method instead of this
		if (string_start_action<CharT, decltype(key_context)>(key_context, inout_read_view) != 1) {
			// Failed to find end of string; any exception would've been thrown in string_start_action
			return std::numeric_limits<size_t>::max();
		}
		advance_whitespace(inout_read_view);

		// Insert our value object
		auto& value = result[key_context.out_object];

		// Verify next character is ':'
		if (inout_read_view.empty()) {
			throw std::invalid_argument{
				"Invalid JSON data; unexpected end of data after parsing map key; expected ':' followed by value" };
		}
		front = inout_read_view.front();
		if (front != ':') {
			throw std::invalid_argument{
				jessilib::join_mbstring(u8"Invalid JSON data; unexpected token: '"sv,
				decode_codepoint(inout_read_view).codepoint,
				u8"' when parsing map key (expected ':' instead)"sv) };
		}
		inout_read_view.remove_prefix(1); // strip ':'

		// We've reached an object value; parse it
		if (!deserialize_json<CharT, ContextT::use_exceptions>(value, inout_read_view)) {
			// Invalid JSON! Any exception would've been thrown already
			break;
		}

		// Advance through whitespace to ',' or '}'
		advance_whitespace(inout_read_view);

		if (inout_read_view.empty()) {
			throw std::invalid_argument{
				"Invalid JSON data; unexpected end of data after parsing map value; expected '}'" };
		}

		if (inout_read_view.front() == ',') {
			// Strip comma and trailing whitespace
			inout_read_view.remove_prefix(1);
			advance_whitespace(inout_read_view);
		}
	}

	if constexpr (ContextT::use_exceptions) {
		throw std::invalid_argument{ "Invalid JSON data: unexpected end of data when parsing object array; expected '}'" };
	}

	return std::numeric_limits<size_t>::max();
}

template<typename CharT, typename ContextT, char32_t InCodepointV>
constexpr syntax_tree_member<CharT, ContextT> make_map_start_pair() {
	return { InCodepointV, make_map_start_action<CharT, ContextT> };
}

template<typename CharT, typename ContextT, char32_t InCodepointV>
constexpr syntax_tree_member<CharT, ContextT> make_number_pair() {
	return { InCodepointV, [](ContextT& inout_context, std::basic_string_view<CharT>& inout_read_view) constexpr -> size_t {
		// parse integer
		const CharT* number_begin = inout_read_view.data() - 1;
		intmax_t integer_value{};
		const CharT* from_chars_end = from_chars(number_begin, inout_read_view.data() + inout_read_view.size(), integer_value).ptr;
		if constexpr (InCodepointV == '-') {
			if (inout_read_view.data() == from_chars_end) {
				// Failed to parse integer portion
				if constexpr (ContextT::use_exceptions) {
					using namespace std::literals;
					throw std::invalid_argument{
						jessilib::join_mbstring(u8"Invalid JSON data; unexpected token: '"sv, decode_codepoint(inout_read_view).codepoint, u8"' when parsing number"sv) };
				}

				return std::numeric_limits<size_t>::max();
			}
		}

		// Strip integer portion and return if nothing remains
		inout_read_view.remove_prefix(from_chars_end - inout_read_view.data());
		if (inout_read_view.empty() || inout_read_view.front() != '.') {
			inout_context.out_object = integer_value;
			return 1;
		}

		// Parse decimal portion

		/*
		// std::from_chars method
		long double decimal_value{};
		from_chars_end = std::from_chars(data, data_end, decimal_value).ptr;
		return static_cast<long double>(integer_value) + decimal_value;
		 */

		// parse_decimal_part method
		inout_read_view.remove_prefix(1); // strip leading '.'
		long double decimal_value = static_cast<long double>(integer_value);
		from_chars_end = parse_decimal_part(inout_read_view.data(), inout_read_view.data() + inout_read_view.size(), decimal_value);
		// TODO: parse exponent

		// Strip decimal portion and return
		inout_read_view.remove_prefix(from_chars_end - inout_read_view.data());
		inout_context.out_object = decimal_value;
		return 1;
	} };
}

static constexpr std::u8string_view json_false_remainder{ u8"alse" };
static constexpr std::u8string_view json_null_remainder{ u8"ull" };
static constexpr std::u8string_view json_true_remainder{ u8"rue" };

template<typename CharT, bool UseExceptionsV>
static constexpr syntax_tree<CharT, json_context<CharT, UseExceptionsV>> json_object_tree{
	make_noop_pair<CharT, json_context<CharT, UseExceptionsV>, U'\t'>(),
	make_noop_pair<CharT, json_context<CharT, UseExceptionsV>, U'\n'>(),
	make_noop_pair<CharT, json_context<CharT, UseExceptionsV>, U'\r'>(),
	make_noop_pair<CharT, json_context<CharT, UseExceptionsV>, U' '>(),
	make_string_start_pair<CharT, json_context<CharT, UseExceptionsV>, U'\"'>(),
	make_number_pair<CharT, json_context<CharT, UseExceptionsV>, U'-'>(),
	make_number_pair<CharT, json_context<CharT, UseExceptionsV>, U'0'>(),
	make_number_pair<CharT, json_context<CharT, UseExceptionsV>, U'1'>(),
	make_number_pair<CharT, json_context<CharT, UseExceptionsV>, U'2'>(),
	make_number_pair<CharT, json_context<CharT, UseExceptionsV>, U'3'>(),
	make_number_pair<CharT, json_context<CharT, UseExceptionsV>, U'4'>(),
	make_number_pair<CharT, json_context<CharT, UseExceptionsV>, U'5'>(),
	make_number_pair<CharT, json_context<CharT, UseExceptionsV>, U'6'>(),
	make_number_pair<CharT, json_context<CharT, UseExceptionsV>, U'7'>(),
	make_number_pair<CharT, json_context<CharT, UseExceptionsV>, U'8'>(),
	make_number_pair<CharT, json_context<CharT, UseExceptionsV>, U'9'>(),
	make_array_start_pair<CharT, json_context<CharT, UseExceptionsV>, U'['>(),
	make_keyword_value_pair<CharT, json_context<CharT, UseExceptionsV>, U'f', json_false_remainder, bool, false>(),
	make_keyword_value_pair<CharT, json_context<CharT, UseExceptionsV>, U'n', json_null_remainder, std::nullptr_t, nullptr>(),
	make_keyword_value_pair<CharT, json_context<CharT, UseExceptionsV>, U't', json_true_remainder, bool, true>(),
	make_map_start_pair<CharT, json_context<CharT, UseExceptionsV>, U'{'>()
};

template<typename CharT, bool UseExceptionsV,
	default_syntax_tree_action<CharT, json_context<CharT, UseExceptionsV>> DefaultActionF>
bool deserialize_json(object& out_object, std::basic_string_view<CharT>& inout_read_view) {
	if (inout_read_view.empty()) {
		// Empty json; false to indicate out_object not modified with any valid data, but no need to throw
		return false;
	}

	json_context<CharT, UseExceptionsV> context{ out_object };
	static_assert(is_sorted<CharT, decltype(context), json_object_tree<CharT, UseExceptionsV>, std::size(json_object_tree<CharT, UseExceptionsV>)>(), "Tree must be pre-sorted");

	return apply_syntax_tree<CharT, decltype(context), json_object_tree<CharT, UseExceptionsV>, std::size(json_object_tree<CharT, UseExceptionsV>), DefaultActionF>
		(context, inout_read_view);
}

template<typename CharT, typename ResultCharT>
void make_json_string(std::basic_string<ResultCharT>& out_string, std::u8string_view in_string) {
	using namespace std::literals;
	out_string.reserve(out_string.size() + in_string.size() + 2);
	simple_append<CharT, ResultCharT>(out_string, '\"');

	decode_result decode;
	while ((decode = decode_codepoint(in_string)).units != 0) {
		if (decode.codepoint == U'\\') { // backslash
			simple_append<CharT, ResultCharT>(out_string, u8"\\\\"sv);
		}
		else if (decode.codepoint == U'\"') { // quotation
			simple_append<CharT, ResultCharT>(out_string, u8"\\\""sv);
		}
		else if (decode.codepoint < 0x20) { // control characters
			simple_append<CharT, ResultCharT>(out_string, u8"\\u00"sv);

			// overwrite last 2 zeroes with correct hexadecimal sequence
			char data[2]; // Will only ever use 2 chars
			char* data_end = data + sizeof(data);
			auto to_chars_result = std::to_chars(data, data_end, static_cast<uint32_t>(decode.codepoint), 16);
			if (to_chars_result.ptr == data) {
				// No bytes written
				simple_append<CharT, ResultCharT>(out_string, u8"00"sv);
			}
			else if (to_chars_result.ptr != data_end) {
				// 1 byte written
				simple_append<CharT, ResultCharT>(out_string, '0');
				simple_append<CharT, ResultCharT>(out_string, data[0]);
			}
			else {
				// 2 bytes written
				simple_append<CharT, ResultCharT>(out_string, std::u8string_view{ reinterpret_cast<char8_t*>(data), sizeof(data) });
			}
		}
		else {
			if constexpr (sizeof(CharT) == sizeof(char8_t) && sizeof(CharT) == sizeof(ResultCharT)) {
				// Valid UTF-8 sequence; copy it over
				out_string.append(reinterpret_cast<const ResultCharT*>(in_string.data()), decode.units);
			}
			else if constexpr (sizeof(CharT) == sizeof(ResultCharT)){
				// Valid UTF-8 codepoint; append it
				encode_codepoint(out_string, decode.codepoint);
			}
			else {
				// Valid UTF-8 codepoint; encode & append it
				encode_buffer_type<CharT> buffer;
				size_t units_written = encode_codepoint(buffer, decode.codepoint);
				out_string.append(reinterpret_cast<ResultCharT*>(buffer), units_written * sizeof(CharT));
			}
		}

		in_string.remove_prefix(decode.units);
	}

	simple_append<CharT, ResultCharT>(out_string, '\"');
}

template<typename CharT>
static constexpr CharT empty_format_arg[3]{ '{', '}', 0 };

template<typename CharT, typename ResultCharT>
void json_parser::serialize_impl(std::basic_string<ResultCharT>& out_string, const object& in_object) {
	using namespace std::literals;
	static const object::array_type s_null_array;
	static const object::map_type s_null_map;

	switch (in_object.type()) {
		case object::type::null:
			simple_append<CharT, ResultCharT>(out_string, u8"null"sv);
			return;

		case object::type::boolean:
			if (in_object.get<bool>()) {
				simple_append<CharT, ResultCharT>(out_string, u8"true"sv);
				return;
			}
			simple_append<CharT, ResultCharT>(out_string, u8"false"sv);
			return;

		case object::type::integer:
			if constexpr (sizeof(CharT) == sizeof(ResultCharT)) {
				out_string += fmt::format(empty_format_arg<ResultCharT>, in_object.get<intmax_t>());
			}
			else if constexpr (std::is_same_v<ResultCharT, char>){
				auto encoded = fmt::format(empty_format_arg<CharT>, in_object.get<intmax_t>());
				out_string.append(reinterpret_cast<ResultCharT*>(encoded.data()), encoded.size() * sizeof(CharT));
			}
			return;

		case object::type::decimal:
			if constexpr (sizeof(CharT) == sizeof(ResultCharT)) {
				out_string += fmt::format(empty_format_arg<ResultCharT>, in_object.get<long double>());
			}
			else if constexpr (std::is_same_v<ResultCharT, char>){
				auto encoded = fmt::format(empty_format_arg<CharT>, in_object.get<long double>());
				out_string.append(reinterpret_cast<ResultCharT*>(encoded.data()), encoded.size() * sizeof(CharT));
			}
			return;

		case object::type::text:
			make_json_string<CharT, ResultCharT>(out_string, in_object.get<std::u8string>());
			return;

		case object::type::array: {
			if (in_object.size() == 0) {
				simple_append<CharT, ResultCharT>(out_string, u8"[]"sv);
			}

			simple_append<CharT, ResultCharT>(out_string, '[');

			// Serialize all objects in array
			for (auto& obj : in_object.get<object::array_type>(s_null_array)) {
				json_parser::serialize_impl<CharT, ResultCharT>(out_string, obj);
				simple_append<CharT, ResultCharT>(out_string, ',');
			}

			// Replace last comma with ']'
			if constexpr (sizeof(CharT) == sizeof(ResultCharT)) {
				out_string.back() = ']';
			}
			else if constexpr (std::is_same_v<ResultCharT, char>) {
				out_string.erase(out_string.size() - sizeof(CharT));
				simple_append<CharT, ResultCharT>(out_string, ']');
			}
			// else // not supported
			return;
		}

		case object::type::map: {
			if (in_object.size() == 0) {
				simple_append<CharT, ResultCharT>(out_string, u8"{}"sv);
			}

			simple_append<CharT, ResultCharT>(out_string, '{');

			// Serialize all objects in map
			for (auto& item : in_object.get<object::map_type>(s_null_map)) {
				make_json_string<CharT, ResultCharT>(out_string, item.first);
				simple_append<CharT, ResultCharT>(out_string, ':');
				json_parser::serialize_impl<CharT, ResultCharT>(out_string, item.second);
				simple_append<CharT, ResultCharT>(out_string, ',');
			}

			// Replace last comma with '}'
			if constexpr (sizeof(CharT) == sizeof(ResultCharT)) {
				out_string.back() = '}';
			}
			else if constexpr (std::is_same_v<ResultCharT, char>) {
				out_string.erase(out_string.size() - sizeof(CharT));
				simple_append<CharT, ResultCharT>(out_string, '}');
			}
			// else // not supported
			return;
		}

		default:
			throw std::invalid_argument{ "Invalid data type: " + std::to_string(static_cast<size_t>(in_object.type())) };
	}
}

} // namespace jessilib
