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

/**
 * @file unicode_sequence.hpp
 * @author Jessica James
 *
 * Unicode-aware syntax tree parsing utilities
 */

#pragma once

#include "unicode_syntax.hpp"
#include "unicode_sequence.hpp"
#include "type_traits.hpp"

namespace jessilib {

/**
 * Query string escape sequence parser
 */

template<typename CharT,
	std::enable_if_t<sizeof(CharT) == 1>* = nullptr> // make_hex_sequence_pair isn't going to play well with other types
static constexpr shrink_sequence_tree<CharT> http_query_escapes_root_tree{
	make_hex_sequence_pair<CharT, U'%', 2, true, false>(),
	make_simple_sequence_pair<CharT, U'+', ' '>()
};
static_assert(is_sorted<char, http_query_escapes_root_tree<char>, std::size(http_query_escapes_root_tree<char>)>(), "Tree must be pre-sorted");
static_assert(is_sorted<char8_t, http_query_escapes_root_tree<char8_t>, std::size(http_query_escapes_root_tree<char8_t>)>(), "Tree must be pre-sorted");

template<typename CharT,
	std::enable_if_t<sizeof(CharT) == 1>* = nullptr>
constexpr bool deserialize_http_query(std::basic_string<CharT>& inout_string) {
	return apply_shrink_sequence_tree<CharT, http_query_escapes_root_tree<CharT>, std::size(http_query_escapes_root_tree<CharT>)>(inout_string);
}

// TODO: decide whether to take this approach, where query strings are assumed to represent UTF-8 text data, OR implement
// such that calling deserialize_http_query will assume the relevant encoding (i.e: calling with char16_t would read in
// escaped query values as bytes in codepoint char16_t, rather than utf-8 encoding sequence)
/*template<typename CharT,
	std::enable_if_t<sizeof(CharT) != 1>* = nullptr>
bool deserialize_http_query(std::basic_string<CharT>& inout_string) {
	//TODO: optimize this?
	std::basic_string<char8_t> u8query_string = string_cast<char8_t>(inout_string);
	bool result = deserialize_http_query<char8_t>(u8query_string);
	inout_string = string_cast<CharT>(u8query_string);
	return result;
}*/

/**
 * HTML form parser
 */

template<typename CharT, typename ContainerT>
struct HTMLFormContext {
	using container_type = ContainerT;
	container_type& out_container;
	CharT* write_head;
	const CharT* key_start = write_head;
	const CharT* value_start{}; // value_start is also key_end
};

template<typename CharT, typename ContextT, char32_t InCodepointV>
constexpr syntax_tree_member<CharT, ContextT> make_value_start_pair() {
	// '='
	return { InCodepointV, [](ContextT& inout_context, std::basic_string_view<CharT>&) constexpr {
		if (inout_context.value_start != nullptr) {
			// There's already a value pending; this must just be part of the value.
			inout_context.write_head += encode_codepoint(inout_context.write_head, InCodepointV);
			return true;
		}

		// Start pending_value
		inout_context.value_start = inout_context.write_head;
		return true;
	} };
}

template<typename CharT, typename ContextT>
constexpr bool value_end_action(ContextT& inout_context, std::basic_string_view<CharT>&) {
	const CharT* value_end = inout_context.write_head;
	const CharT* key_start = inout_context.key_start;
	const CharT* value_start = inout_context.value_start;
	if (value_start != nullptr) {
		// Terminate key & value, push them to table
		std::basic_string_view<CharT> key{ key_start, static_cast<size_t>(value_start - key_start) };
		std::basic_string_view<CharT> value{ value_start, static_cast<size_t>(value_end - value_start) };
		jessilib::container::push(inout_context.out_container, key, value);

		// Start reading next key
		inout_context.key_start = value_end;
		inout_context.value_start = nullptr;
		return true;
	}

	// This is a valueless key; terminate the key and push it
	std::basic_string_view<CharT> key{ key_start, static_cast<size_t>(value_end - key_start) };
	jessilib::container::push(inout_context.out_container, key, std::basic_string_view<CharT>{});

	// Start reading next key
	inout_context.key_start = value_end;
	return true;
}

template<typename CharT, typename ContextT, char32_t InCodepointV>
constexpr syntax_tree_member<CharT, ContextT> make_value_end_pair() {
	// '&'
	return { InCodepointV, value_end_action<CharT, ContextT> };
}

template<typename CharT, typename ContextT, char32_t InCodepointV, size_t MaxDigitsV, bool ExactDigitsV, bool IsUnicode>
constexpr syntax_tree_member<CharT, ContextT> make_hex_syntax_shrink_pair() {
	return { InCodepointV, [](ContextT& inout_context, std::basic_string_view<CharT>& inout_read_view) constexpr {
		return hex_shrink_sequence_action<CharT, MaxDigitsV, ExactDigitsV, IsUnicode>(inout_context.write_head, inout_read_view);
	} };
}

template<typename CharT, typename ContextT, char32_t InCodepointV, char8_t OutCodepointV>
constexpr syntax_tree_member<CharT, ContextT> make_simple_shrink_pair() {
	return {
		InCodepointV,
		[](ContextT& inout_context, std::basic_string_view<CharT>&) constexpr {
			*inout_context.write_head = static_cast<CharT>(OutCodepointV);
			++inout_context.write_head;
			return true;
		}
	};
}

template<typename CharT, typename ContextT>
bool html_form_default_action(decode_result decode, ContextT& inout_context, std::basic_string_view<CharT>& inout_read_view) {
	// A regular character; copy it and advance the read/write heads
	CharT*& write_head = inout_context.write_head;
	CharT* write_end = write_head + decode.units;
	while (write_head != write_end) {
		*write_head = inout_read_view.front();
		++write_head;
		inout_read_view.remove_prefix(1);
	}

	return true;
}

template<typename CharT, typename ContainerT,
	std::enable_if_t<sizeof(CharT) == 1>* = nullptr> // make_hex_sequence_pair isn't going to play well with other types
static constexpr syntax_tree<CharT, HTMLFormContext<CharT, ContainerT>> html_form_root_tree{
	make_hex_syntax_shrink_pair<CharT, HTMLFormContext<CharT, ContainerT>, U'%', 2, true, false>(),
	make_value_end_pair<CharT, HTMLFormContext<CharT, ContainerT>, U'&'>(),
	make_simple_shrink_pair<CharT, HTMLFormContext<CharT, ContainerT>, U'+', ' '>(),
	make_value_start_pair<CharT, HTMLFormContext<CharT, ContainerT>, U'='>()
};

template<typename CharT, typename ContainerT,
	std::enable_if_t<sizeof(CharT) == 1>* = nullptr>
constexpr bool deserialize_html_form(ContainerT& out_container, std::basic_string<CharT>& inout_string) {
	if (inout_string.empty()) {
		return true; // even though apply_syntax_tree checks for this, check it here anyways so we don't call value_end_action
	}

	HTMLFormContext<CharT, ContainerT> context{ out_container, inout_string.data() };
	constexpr auto& html_form_tree = html_form_root_tree<CharT, ContainerT>;
	static_assert(is_sorted<char, decltype(context), html_form_tree, std::size(html_form_tree)>(), "Tree must be pre-sorted");

	std::basic_string_view<CharT> read_view{ inout_string };
	if (apply_syntax_tree<CharT, decltype(context), html_form_tree, std::size(html_form_tree), html_form_default_action>
	    (context, read_view)) {
		value_end_action<CharT, decltype(context)>(context, read_view);
		return true;
	}

	return false;
}

} // namespace jessilib
