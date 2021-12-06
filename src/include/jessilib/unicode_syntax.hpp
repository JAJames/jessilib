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

#include "unicode_base.hpp"

namespace jessilib {

/**
 * Syntax tree; move this to another file later
 */

template<typename CharT, typename ContextT>
using syntax_tree_action = bool(*)(ContextT& inout_context, std::basic_string_view<CharT>& inout_read_view);

template<typename CharT, typename ContextT>
using default_syntax_tree_action = bool(*)(decode_result in_codepoint, ContextT& inout_context, std::basic_string_view<CharT>& inout_read_view);

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

template<typename CharT, typename ContextT>
bool fail_action(decode_result, ContextT&, std::basic_string_view<CharT>&) {
	return false;
}

template<typename CharT, typename ContextT>
bool noop_action(decode_result decode, ContextT&, std::basic_string_view<CharT>& inout_read_view) {
	inout_read_view.remove_prefix(decode.units);
	return true;
}

template<typename CharT, typename ContextT, char32_t InCodepointV, const syntax_tree<CharT, ContextT> SubTreeBegin, size_t SubTreeSize, default_syntax_tree_action<CharT, ContextT> DefaultActionF = fail_action<CharT, ContextT>>
constexpr syntax_tree_member<CharT, ContextT> make_tree_pair() {
	return { InCodepointV, [](ContextT& inout_context, std::basic_string_view<CharT>& inout_read_view) constexpr {
		auto decode = decode_codepoint(inout_read_view);
		if (decode.units == 0) {
			return false;
		}

		constexpr syntax_tree_member<CharT, ContextT>* SubTreeEnd = SubTreeBegin + SubTreeSize;
		auto parser = std::lower_bound(SubTreeBegin, SubTreeEnd, decode.codepoint, &syntax_tree_member_compare<CharT>);
		if (parser == SubTreeEnd || parser->first != decode.codepoint) {
			return DefaultActionF(decode, inout_context, inout_read_view);
		}

		// This is a parsed sequence; pass it to the parser
		inout_read_view.remove_prefix(decode.units);
		return (parser->second)(inout_context, inout_read_view);
	} };
}

template<typename CharT, typename ContextT, const syntax_tree<CharT, ContextT> SequenceTreeBegin, size_t SequenceTreeSize,
	default_syntax_tree_action<CharT, ContextT> DefaultActionF = noop_action<CharT, ContextT>>
constexpr bool apply_syntax_tree(ContextT& inout_context, std::basic_string_view<CharT>& inout_read_view) {
	if (inout_read_view.empty()) {
		// Nothing to parse
		return true;
	}

	decode_result decode;
	constexpr auto SubTreeEnd = SequenceTreeBegin + SequenceTreeSize;
	while ((decode = decode_codepoint(inout_read_view)).units != 0) {
		auto parser = std::lower_bound(SequenceTreeBegin, SubTreeEnd, decode.codepoint, &syntax_tree_member_compare<CharT, ContextT>);
		if (parser == SubTreeEnd || parser->first != decode.codepoint) {
			// Just a normal character; pass it to the default handler
			if (!DefaultActionF(decode, inout_context, inout_read_view)) {
				return false;
			}

			continue;
		}

		// This is a parsed sequence; pass it to the parser instead
		inout_read_view.remove_prefix(decode.units);
		if (!(parser->second)(inout_context, inout_read_view)) {
			// Bad input received; give up
			return false;
		}
	}

	// We've finished parsing successfully
	return true;
}

} // namespace jessilib
