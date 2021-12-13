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

#include "unicode.hpp"

namespace jessilib {

/**
 * Syntax tree; move this to another file later
 */

template<typename CharT, typename ContextT>
using syntax_tree_action = size_t(*)(ContextT& inout_context, std::basic_string_view<CharT>& inout_read_view);

template<typename CharT, typename ContextT>
using default_syntax_tree_action = size_t(*)(decode_result in_codepoint, ContextT& inout_context, std::basic_string_view<CharT>& inout_read_view);

template<typename CharT, typename ContextT>
using syntax_tree = const std::pair<char32_t, syntax_tree_action<CharT, ContextT>>[];

template<typename CharT, typename ContextT>
using syntax_tree_member = const std::pair<char32_t, syntax_tree_action<CharT, ContextT>>;

template<typename CharT, typename ContextT>
constexpr bool syntax_tree_member_compare(const syntax_tree_member<CharT, ContextT>& in_lhs, const char32_t in_rhs) {
	return in_lhs.first < in_rhs;
}

// Lessers on left
template<typename CharT, typename ContextT, const syntax_tree<CharT, ContextT> TreeBegin, size_t TreeSize>
constexpr bool is_sorted() {
	auto head = TreeBegin;
	constexpr auto end = TreeBegin + TreeSize;

	if (head == end) {
		return true;
	}

	while (head + 1 != end) {
		const auto next = head + 1;
		if (head->first > next->first) {
			return false;
		}

		++head;
	}

	return true;
}

template<typename CharT, typename ContextT, bool UseExceptionsV = false>
size_t fail_action(decode_result, ContextT&, std::basic_string_view<CharT>& in_read_view) {
	using namespace std::literals;
	if constexpr (UseExceptionsV) {
		throw std::invalid_argument{ jessilib::join_mbstring(u8"Invalid parse data; unexpected token: '"sv,
			jessilib::decode_codepoint(in_read_view).codepoint,
			u8"' when parsing data"sv) };
	}
	return std::numeric_limits<size_t>::max();
}

template<typename CharT, typename ContextT>
size_t noop_action(decode_result decode, ContextT&, std::basic_string_view<CharT>& inout_read_view) {
	inout_read_view.remove_prefix(decode.units);
	return 0;
}

template<typename CharT, typename ContextT, const syntax_tree<CharT, ContextT> SubTreeBegin, size_t SubTreeSize, default_syntax_tree_action<CharT, ContextT> DefaultActionF>
constexpr size_t tree_action(ContextT& inout_context, std::basic_string_view<CharT>& inout_read_view) {
	decode_result decode;
	size_t break_stack_depth;
	constexpr syntax_tree_member<CharT, ContextT>* SubTreeEnd = SubTreeBegin + SubTreeSize;
	while ((decode = decode_codepoint(inout_read_view)).units != 0) {
		auto parser = std::lower_bound(SubTreeBegin, SubTreeEnd, decode.codepoint, &syntax_tree_member_compare<CharT, ContextT>);
		if (parser == SubTreeEnd || parser->first != decode.codepoint) {
			break_stack_depth = DefaultActionF(decode, inout_context, inout_read_view);
			if (break_stack_depth == 0) {
				// Don't jump the stack; continue
				continue;
			}

			return break_stack_depth - 1;
		}

		// This is a parsed sequence; pass it to the parser
		inout_read_view.remove_prefix(decode.units);
		break_stack_depth = (parser->second)(inout_context, inout_read_view);
		if (break_stack_depth != 0) {
			return break_stack_depth - 1;
		}
	}

	// decode.units == 0; success if view empty, failure otherwise
	if (inout_read_view.empty()) {
		return 0;
	}

	return std::numeric_limits<size_t>::max();
}

template<typename CharT, typename ContextT, char32_t InCodepointV, const syntax_tree<CharT, ContextT> SubTreeBegin, size_t SubTreeSize, default_syntax_tree_action<CharT, ContextT> DefaultActionF = fail_action<CharT, ContextT>>
constexpr syntax_tree_member<CharT, ContextT> make_tree_pair() {
	return { InCodepointV, tree_action<CharT, ContextT, SubTreeBegin, SubTreeSize, DefaultActionF> };
}

template<typename CharT, typename ContextT, const syntax_tree<CharT, ContextT> SequenceTreeBegin, size_t SequenceTreeSize,
	default_syntax_tree_action<CharT, ContextT> DefaultActionF = noop_action<CharT, ContextT>>
constexpr bool apply_syntax_tree(ContextT& inout_context, std::basic_string_view<CharT>& inout_read_view) {
	if (inout_read_view.empty()) {
		// Nothing to parse
		return true;
	}

	return tree_action<CharT, ContextT, SequenceTreeBegin, SequenceTreeSize, DefaultActionF>(inout_context, inout_read_view) == 0;
}

} // namespace jessilib
