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
 * Unicode-aware escape sequence parsing utilities
 */

#pragma once

#include <map>
#include "unicode.hpp"

namespace jessilib {

// CharT is the codepoint which has just been read, in_write_head is a writeable string buffer, read_view points to remainder
template<typename CharT>
using shrink_sequence_tree_action = bool(*)(CharT*& in_write_head, std::basic_string_view<CharT>& read_view);

template<typename CharT>
using shrink_sequence_tree = const std::pair<char32_t, shrink_sequence_tree_action<CharT>>[];

template<typename CharT>
using shrink_sequence_tree_member = const std::pair<char32_t, shrink_sequence_tree_action<CharT>>;

template<typename CharT>
bool shrink_tree_member_compare(const shrink_sequence_tree_member<CharT>& in_lhs, const char32_t in_rhs) {
	return in_lhs.first < in_rhs;
}

// Only use for ASTs where each character process is guaranteed to write at most 1 character for each character consumed
template<typename CharT, const shrink_sequence_tree<CharT> SequenceTreeBegin, size_t SequenceTreeSize>
bool apply_shrink_sequence_tree(std::basic_string<CharT>& inout_string) {
	if (inout_string.empty()) {
		// Nothing to parse
		return true;
	}

	std::basic_string_view<CharT> read_view = inout_string;
	CharT* write_head = inout_string.data();
	get_endpoint_result decode;

	constexpr auto SubTreeEnd = SequenceTreeBegin + SequenceTreeSize;
	while ((decode = decode_codepoint(read_view)).units != 0) { // TODO: make constexpr
		auto parser = std::lower_bound(SequenceTreeBegin, SubTreeEnd, decode.codepoint, &shrink_tree_member_compare<CharT>);
		if (parser == SubTreeEnd || parser->first != decode.codepoint) {
			// Just a normal character; write it over
			while (decode.units != 0) {
				*write_head = read_view.front();
				++write_head;
				--decode.units;
				read_view.remove_prefix(1);
			}

			continue;
		}

		// This is a parsed sequence; pass it to the parser instead
		read_view.remove_prefix(decode.units);
		if (!(parser->second)(write_head, read_view)) {
			// Bad input received; strip off whatever we haven't parsed
			size_t length = write_head - inout_string.data();
			inout_string.erase(length);
			return false;
		}
	}

	// We've finished parsing successfully; strip off the extraneous codepoints
	size_t length = write_head - inout_string.data();
	inout_string.erase(length);
	return true;
}

// Only for codepoints representable w/ char8_t (i.e: \n)
template<typename CharT, char32_t InCodepointV, char8_t OutCodepointV>
constexpr shrink_sequence_tree_member<CharT> make_simple_sequence_pair() {
	return {
		InCodepointV,
		[](CharT*& in_write_head, std::basic_string_view<CharT>&) constexpr {
			*in_write_head = static_cast<CharT>(OutCodepointV);
			++in_write_head;
			return true;
		}
	};
}

// Skips a character (i.e: skipping/ignoring newlines)
template<typename CharT, char32_t InCodepointV>
constexpr shrink_sequence_tree_member<CharT> make_noop_sequence_pair() {
	return {
		InCodepointV,
		[](CharT*&, std::basic_string_view<CharT>&) constexpr {
			return true;
		}
	};
}

// Skips a character or two (i.e: skipping/ignoring newlines)
template<typename CharT, char32_t InCodepointV, char32_t InOptionalTrailing>
constexpr shrink_sequence_tree_member<CharT> make_noop_sequence_pair() {
	return {
		InCodepointV,
		[](CharT*&, std::basic_string_view<CharT>& read_view) constexpr {
			// Strip trailing 'InTrailing', if it's present
			auto decode = decode_codepoint(read_view);
			if (decode.units != 0
				&& decode.codepoint == InOptionalTrailing) {
				read_view.remove_prefix(decode.units);
			}

			return true;
		}
	};
}

template<typename CharT, char32_t InCodepointV, size_t MaxDigitsV, bool ExactDigitsV, bool IsUnicode>
constexpr shrink_sequence_tree_member<CharT> make_octal_sequence_pair() {
	static_assert(MaxDigitsV > 0); // Use noop instead
	static_assert((MaxDigitsV == 2 && InCodepointV >= U'0' && InCodepointV <= U'7')
		|| (MaxDigitsV == 3 && InCodepointV >= U'0' && InCodepointV <= U'3')); // Only currently support single-octet octal values

	// Must have at least 1 octal digit (this one), but may not have more than 3 (2 more).
	return {
		InCodepointV,
		[](CharT*& in_write_head, std::basic_string_view<CharT>& read_view) constexpr {
			// Read in first octal character from InCodepointV
			unsigned int out_value = InCodepointV - U'0'; // Set initial value
			if (read_view.empty()) {
				*in_write_head = out_value;
				++in_write_head;
				return true;
			}

			// Read is second octal unit from front; octal characters are always 1 unit
			int octal_value = as_base(read_view.front(), 8);
			if (octal_value < 0) {
				if constexpr (ExactDigitsV) {
					// Expected 2-3 digits, received 1
					return false;
				}

				// Not an octal character; write & return
				*in_write_head = out_value;
				++in_write_head;
				return true;
			}

			out_value <<= 3;
			out_value |= octal_value;
			read_view.remove_prefix(1);

			if constexpr (MaxDigitsV == 2) {
				// We've read in both digits; go ahead and write & return
				*in_write_head = out_value;
				++in_write_head;
				return true;
			}

			if (read_view.empty()) {
				if constexpr (ExactDigitsV) {
					// Expected 3 digits, received 2
					return false;
				}

				*in_write_head = out_value;
				++in_write_head;
				return true;
			}

			// Read in third octal unit from front; octal characters are always 1 unit
			octal_value = as_base(read_view.front(), 8);
			if (octal_value < 0) {
				// Not an octal character; push what we have and handle this
				return true;
			}

			out_value <<= 3;
			out_value |= octal_value;
			read_view.remove_prefix(1);

			// Write & return
			*in_write_head = out_value;
			++in_write_head;
			return true;
		}
	};
}

template<typename CharT, char32_t InCodepointV, size_t MaxDigitsV, bool ExactDigitsV, bool IsUnicode>
constexpr shrink_sequence_tree_member<CharT> make_hex_sequence_pair() {
	static_assert(MaxDigitsV > 0);

	return {
		InCodepointV,
		[](CharT*& in_write_head, std::basic_string_view<CharT>& read_view) constexpr {
			// Does not modify
			auto read_hex = [](uint32_t& out_value, std::basic_string_view<CharT> in_view, size_t max_digits) {
				size_t result{};
				int hex_value;
				out_value = 0;
				while (result != max_digits
					&& !in_view.empty()) {
					hex_value = as_base(in_view.front(), 16); // hexadecimal characters are always 1 unit
					if (hex_value < 0) {
						// Not a hexadecimal character; push what we have and handle this
						return result;
					}

					out_value <<= 4;
					out_value |= hex_value;

					in_view.remove_prefix(1);
					++result;
				}

				// Number of elements that are hexadecimal digits
				return result;
			};

			// Read in hex value
			uint32_t hex_value;
			size_t units_read = read_hex(hex_value, read_view, MaxDigitsV);

			// Sanity check digits read
			if constexpr(ExactDigitsV) {
				if (units_read != MaxDigitsV) {
					// We expected example MaxDigitsV digits; fail
					return false;
				}
			}
			else {
				if (units_read == 0) {
					// We didn't read any digits; fail
					return false;
				}
			}

			// We read an acceptable number of digits; write the unit and call it a day
			read_view.remove_prefix(units_read);
			if constexpr (IsUnicode) {
				in_write_head += encode_codepoint(in_write_head, hex_value);
			}
			else {
				static_assert(MaxDigitsV <= sizeof(CharT) * 2);
				*in_write_head = static_cast<CharT>(hex_value);
				++in_write_head;
			}

			return true;
		}
	};
}

// Calls into another tree with the next character
template<typename CharT, char32_t InCodepointV, const shrink_sequence_tree<CharT> SubTreeBegin, size_t SubTreeSize, bool FailNotFound = true>
constexpr shrink_sequence_tree_member<CharT> make_tree_sequence_pair() {
	return { InCodepointV, [](CharT*& in_write_head, std::basic_string_view<CharT>& read_view) {
		auto decode = decode_codepoint(read_view); // TODO: make constexpr

		constexpr auto SubTreeEnd = SubTreeBegin + SubTreeSize;
		auto parser = std::lower_bound(SubTreeBegin, SubTreeEnd, decode.codepoint, &shrink_tree_member_compare<CharT>);
		if (parser == SubTreeEnd || parser->first != decode.codepoint) {
			if constexpr (FailNotFound) {
				// Code not found; fail
				return false;
			}

			// Just a normal character; write it over
			while (decode.units != 0) {
				*in_write_head = read_view.front();
				++in_write_head;
				--decode.units;
				read_view.remove_prefix(1);
			}

			return true;
		}

		// This is a parsed sequence; pass it to the parser
		read_view.remove_prefix(decode.units);
		return (parser->second)(in_write_head, read_view);
	} };
}

// Lessers on left
template<typename CharT, const shrink_sequence_tree<CharT> SubTreeBegin, size_t SubTreeSize>
constexpr bool is_sorted() {
	auto head = SubTreeBegin;
	constexpr auto end = SubTreeBegin + SubTreeSize;

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

template<typename CharT>
static constexpr shrink_sequence_tree<CharT> cpp_escapes_main_tree{
	/** Newline skippers; not actually a C++ thing, but I want it */
	make_noop_sequence_pair<CharT, U'\n', U'\r'>(),
	make_noop_sequence_pair<CharT, U'\r', U'\n'>(),

	/** Simple quote escape sequences */
	make_simple_sequence_pair<CharT, U'\"', '\"'>(),
	make_simple_sequence_pair<CharT, U'\'', '\''>(),

	// Octal (Single byte value only); should we support octal escapes in sequence?
	make_octal_sequence_pair<CharT, U'0', 3, false, false>(),
	make_octal_sequence_pair<CharT, U'1', 3, false, false>(),
	make_octal_sequence_pair<CharT, U'2', 3, false, false>(),
	make_octal_sequence_pair<CharT, U'3', 3, false, false>(),
	make_octal_sequence_pair<CharT, U'4', 2, false, false>(),
	make_octal_sequence_pair<CharT, U'5', 2, false, false>(),
	make_octal_sequence_pair<CharT, U'6', 2, false, false>(),
	make_octal_sequence_pair<CharT, U'7', 2, false, false>(),

	/** Simple escape sequence (question mark) */
	make_simple_sequence_pair<CharT, U'?', '\?'>(),

	/** Uppercase escapes */
	make_hex_sequence_pair<CharT, U'U', 8, true, true>(),

	/** Simple escape sequence (backslash) */
	make_simple_sequence_pair<CharT, U'\\', '\\'>(),

	/** Lowercase escapes */
	make_simple_sequence_pair<CharT, U'a', '\a'>(),
	make_simple_sequence_pair<CharT, U'b', '\b'>(),
	make_simple_sequence_pair<CharT, U'f', '\f'>(),
	make_simple_sequence_pair<CharT, U'n', '\n'>(),
	make_simple_sequence_pair<CharT, U'r', '\r'>(),
	make_simple_sequence_pair<CharT, U't', '\t'>(),
	make_hex_sequence_pair<CharT, U'u', 4, true, true>(),
	make_simple_sequence_pair<CharT, U'v', '\v'>(),

	// Hexadecimal; should we support hex escapes in sequence? (i.e: \x00FF == \x00\xFF, which is only true for char/char8_t atm)
	make_hex_sequence_pair<CharT, U'x', sizeof(CharT) * 2, false, false>(),
};

template<typename CharT>
static constexpr shrink_sequence_tree<CharT> cpp_escapes_root_tree{
	make_tree_sequence_pair<CharT, U'\\', cpp_escapes_main_tree<CharT>, std::size(cpp_escapes_main_tree<CharT>)>()
};

// Return true for valid sequences, false otherwise
template<typename CharT>
bool apply_cpp_escape_sequences(std::basic_string<CharT>& inout_string) {
	static_assert(is_sorted<CharT, cpp_escapes_root_tree<CharT>, std::size(cpp_escapes_root_tree<CharT>)>(), "Tree must be pre-sorted");
	static_assert(is_sorted<CharT, cpp_escapes_main_tree<CharT>, std::size(cpp_escapes_main_tree<CharT>)>(), "Tree must be pre-sorted");

	return apply_shrink_sequence_tree<CharT, cpp_escapes_root_tree<CharT>, std::size(cpp_escapes_root_tree<CharT>)>(inout_string);
}

} // namespace jessilib
