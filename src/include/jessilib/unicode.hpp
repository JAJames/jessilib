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

/**
 * Encodes a codepoint, and appends it to an output string
 *
 * @param out_string String to append
 * @param in_codepoint Codepoint to encode
 * @return Number of data elements appended to out_string
 */
size_t encode_codepoint(std::string& out_string, char32_t in_codepoint); // DEPRECATED: ASSUMES UTF-8
size_t encode_codepoint(std::u8string& out_string, char32_t in_codepoint);
size_t encode_codepoint(std::u16string& out_string, char32_t in_codepoint);
size_t encode_codepoint(std::u32string& out_string, char32_t in_codepoint);

/**
 * Encodes a codepoint to an output stream
 *
 * @param out_stream Stream to write codepoint to
 * @param in_codepoint Codepoint to encode
 * @return Number of data elements appending to out_stream
 */
size_t encode_codepoint(std::basic_ostream<char>& out_stream, char32_t in_codepoint); // DEPRECATED: ASSUMES UTF-8
size_t encode_codepoint(std::basic_ostream<char8_t>& out_stream, char32_t in_codepoint);
size_t encode_codepoint(std::basic_ostream<char16_t>& out_stream, char32_t in_codepoint);
size_t encode_codepoint(std::basic_ostream<char32_t>& out_stream, char32_t in_codepoint);

/**
 * Encodes a codepoint and returns it as a string
 *
 * @param in_codepoint Codepoint to encode
 * @return A string containing the codepoint encoded to the appropriate underlying CharT type
 */
std::u8string encode_codepoint_u8(char32_t in_codepoint);
std::u16string encode_codepoint_u16(char32_t in_codepoint);
std::u32string encode_codepoint_u32(char32_t in_codepoint);

/** decode_codepoint */

struct get_endpoint_result {
	char32_t codepoint{}; // Codepoint
	size_t units{}; // Number of data units codepoint was represented by, or 0
};

/**
 * Decodes the front codepoint in a string
 *
 * @param in_string String to decode a codepoint from
 * @return A struct containing a valid codepoint and the number of representative data units on success, zero otherwise.
 */
get_endpoint_result decode_codepoint(const std::string_view& in_string); // DEPRECATED: ASSUMES UTF-8
get_endpoint_result decode_codepoint(const std::u8string_view& in_string); // UTF-8
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
std::basic_string_view<T> next_codepoint(const std::basic_string_view<T>& in_string) {
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

/** Utilities */

template<typename InT>
bool is_valid(const InT& in_string) {
	using InCharT = typename InT::value_type;
	using InViewT = std::basic_string_view<InCharT>;

	InViewT in_string_view = static_cast<InViewT>(in_string);
	while (!in_string_view.empty()) {
		get_endpoint_result string_front = decode_codepoint(in_string_view);
		if (string_front.units == 0) {
			return false;
		}
		in_string_view.remove_prefix(string_front.units);
	}

	return true;
}

/**
 * Interprets one string type as another (i.e: reading from a I/O stream or buffer, char -> whatever)
 * NOTE: THIS DOES NOT GUARANTEE THE RESULTING VIEW IS SANE, ONLY THAT IT'S A TECHNICALLY VALID SEQUENCE
 *
 * @tparam OutCharT Output view character type
 * @tparam InT Input string type
 * @param in_string Container holding string data
 * @return A valid view into in_string on success, an empty string_view otherwise
 */
template<typename OutCharT, typename InT>
std::basic_string_view<OutCharT> string_view_cast(const InT& in_string) {
	using InCharT = typename InT::value_type;
	size_t in_string_bytes = in_string.size() * sizeof(InCharT);
	if constexpr (sizeof(OutCharT) > sizeof(InCharT)) {
		// The output type is larger than the input type; verify no partial codepoints
		if (in_string_bytes % sizeof(OutCharT) != 0) {
			// This cannot be used to produce a valid result
			return {};
		}
	}

	size_t out_string_units = in_string_bytes / sizeof(OutCharT);
	const OutCharT* data_begin = reinterpret_cast<const OutCharT*>(in_string.data());
	std::basic_string_view<OutCharT> result{ data_begin, out_string_units };

	if (!is_valid(result)) {
		// Result isn't valid; discard and return empty
		return {};
	}

	return result;
}

template<typename OutCharT, typename InT>
std::basic_string<OutCharT> string_cast(const InT& in_string) {
	using InCharT = typename InT::value_type;
	using InViewT = std::basic_string_view<InCharT>;
	std::basic_string<OutCharT> result;

	// Just do a dumb copy when same type & valid; should be slightly faster than re-encoding
	if constexpr (std::is_same_v<OutCharT, InCharT>) {
		if (is_valid(in_string)) {
			result = in_string;
		}

		return result;
	}

	InViewT in_string_view = static_cast<InViewT>(in_string);
	if constexpr (sizeof(InCharT) <= sizeof(OutCharT)) {
		// When copying to a larger type, we will need _at most_ as many elements as the smaller storage type
		result.reserve(in_string_view.size());
	}
	else {
		result.reserve(in_string_view.size() * (sizeof(OutCharT) / sizeof(InCharT)));
	}

	while (!in_string_view.empty()) {
		get_endpoint_result string_front = decode_codepoint(in_string_view);
		if (string_front.units == 0) {
			return {};
		}
		in_string_view.remove_prefix(string_front.units);

		encode_codepoint(result, string_front.codepoint);
	}

	return result;
}

/** single-unit case folding utilities */
char32_t fold(char32_t in_codepoint); // Folds codepoint for case insensitive checks (not for human output)

/**
 * Checks if two codepoints are equal to eachother (case insensitive)
 *
 * @param lhs First codepoint to compare
 * @param rhs Second codepoint to compare
 * @return True if the characters are equal, false otherwise
 */
inline bool equalsi(char32_t lhs, char32_t rhs) {
	return lhs == rhs
		|| fold(lhs) == fold(rhs);
}

// Should just make these methods container-type agnostic rather than this mess...
#define ADAPT_BASIC_STRING(method) \
	template<typename LhsCharT, typename RhsCharT> \
	auto method(const std::basic_string<LhsCharT>& lhs, std::basic_string_view<RhsCharT> rhs) { \
		return method(static_cast<std::basic_string_view<LhsCharT>>(lhs), rhs); } \
	template<typename LhsCharT, typename RhsCharT> \
	auto method(std::basic_string_view<LhsCharT> lhs, const std::basic_string<RhsCharT>& rhs) { \
		return method(lhs, static_cast<std::basic_string_view<RhsCharT>>(rhs)); } \
	template<typename LhsCharT, typename RhsCharT> \
	auto method(const std::basic_string<LhsCharT>& lhs, const std::basic_string<RhsCharT>& rhs) { \
		return method(static_cast<std::basic_string_view<LhsCharT>>(lhs), static_cast<std::basic_string_view<RhsCharT>>(rhs)); }

/**
 * Checks if two strings are equal
 *
 * @tparam LhsCharT Character type for left-hand parameter
 * @tparam RhsCharT Character type for right-hand parameter
 * @param lhs First string to compare
 * @param rhs Second string to compare against
 * @return True if the strings are equal, false otherwise
 */
template<typename LhsCharT, typename RhsCharT>
bool equals(std::basic_string_view<LhsCharT> lhs, std::basic_string_view<RhsCharT> rhs) {
	// If lhs and rhs are the same type, compare their sizes and quickly return if not same
	if constexpr (std::is_same_v<LhsCharT, RhsCharT>) {
		return lhs == rhs;
	}

	while (!lhs.empty() && !rhs.empty()) {
		auto lhs_front = decode_codepoint(lhs);
		auto rhs_front = decode_codepoint(rhs);

		if (lhs_front.units == 0
			|| rhs_front.units == 0) {
			// Failed to decode front codepoint; bad unicode sequence
			return false;
		}

		if (lhs_front.codepoint != rhs_front.codepoint) {
			// Codepoints aren't the same
			return false;
		}

		// Codepoints are equal; trim off the fronts and continue
		lhs.remove_prefix(lhs_front.units);
		rhs.remove_prefix(rhs_front.units);
	}

	return lhs.empty() && rhs.empty();
}

ADAPT_BASIC_STRING(equals)

/**
 * Checks if two strings are equal (case insensitive)
 *
 * @tparam LhsCharT Character type for left-hand parameter
 * @tparam RhsCharT Character type for right-hand parameter
 * @param lhs First string to compare
 * @param rhs Second string to compare against
 * @return True if the strings are equal, false otherwise
 */
template<typename LhsCharT, typename RhsCharT>
bool equalsi(std::basic_string_view<LhsCharT> lhs, std::basic_string_view<RhsCharT> rhs) {
	// If lhs and rhs are the same type, compare their sizes and quickly return if not same
	if constexpr (std::is_same_v<LhsCharT, RhsCharT>) {
		if (lhs.size() != rhs.size()) {
			return false;
		}
	}

	while (!lhs.empty() && !rhs.empty()) {
		auto lhs_front = decode_codepoint(lhs);
		auto rhs_front = decode_codepoint(rhs);

		if (lhs_front.units == 0
			|| rhs_front.units == 0) {
			// Failed to decode front codepoint; bad unicode sequence
			return false;
		}

		if (!equalsi(lhs_front.codepoint, rhs_front.codepoint)) {
			// Codepoints don't fold to same value
			return false;
		}

		// Codepoints are equal; trim off the fronts and continue
		lhs.remove_prefix(lhs_front.units);
		rhs.remove_prefix(rhs_front.units);
	}

	return lhs.empty() && rhs.empty();
}

ADAPT_BASIC_STRING(equalsi)

/**
 * Checks if a string starts with a substring
 *
 * @tparam LhsCharT Character type for underlying string
 * @tparam RhsCharT Character type for prefix string
 * @param in_string String to check for prefix
 * @param in_prefix Substring prefix to check for
 * @return Data length of in_prefix in terms of LhsCharT if in_string starts with in_prefix, 0 otherwise
 */
template<typename LhsCharT, typename RhsCharT>
size_t starts_with_length(std::basic_string_view<LhsCharT> in_string, std::basic_string_view<RhsCharT> in_prefix) {
	// If in_string and in_prefix are the same type, compare their sizes and quickly return if in_string is too small
	if constexpr (std::is_same_v<LhsCharT, RhsCharT>) {
		if (in_string.size() < in_prefix.size()) {
			return 0;
		}
	}

	size_t codepoints_removed{};
	while (!in_string.empty() && !in_prefix.empty()) {
		get_endpoint_result string_front = decode_codepoint(in_string);
		get_endpoint_result prefix_front = decode_codepoint(in_prefix);

		if (string_front.units == 0
			|| prefix_front.units == 0) {
			// Failed to decode front codepoint; bad unicode sequence
			return 0;
		}

		if (string_front.codepoint != prefix_front.codepoint) {
			// Codepoints aren't the same
			return 0;
		}

		// Codepoints are equal; trim off the fronts and continue
		in_string.remove_prefix(string_front.units);
		in_prefix.remove_prefix(prefix_front.units);
		codepoints_removed += string_front.units;
	}

	if (!in_prefix.empty()) {
		// We reached end of in_string before end of prefix
		return 0;
	}

	return codepoints_removed;
}

ADAPT_BASIC_STRING(starts_with_length)

/**
 * Checks if a string starts with a substring (case insensitive)
 *
 * @tparam LhsCharT Character type for underlying string
 * @tparam RhsCharT Character type for prefix string
 * @param in_string String to check for prefix
 * @param in_prefix Substring prefix to check for
 * @return Data length of in_prefix in terms of LhsCharT if in_string starts with in_prefix, 0 otherwise
 */
template<typename LhsCharT, typename RhsCharT>
size_t starts_with_lengthi(std::basic_string_view<LhsCharT> in_string, std::basic_string_view<RhsCharT> in_prefix) {
	// If in_string and in_prefix are the same type, skip decoding each point
	if constexpr (std::is_same_v<LhsCharT, RhsCharT>) {
		if (in_string.size() < in_prefix.size()) {
			return 0;
		}
	}

	size_t codepoints_removed{};
	while (!in_string.empty() && !in_prefix.empty()) {
		get_endpoint_result string_front = decode_codepoint(in_string);
		get_endpoint_result prefix_front = decode_codepoint(in_prefix);

		if (string_front.units == 0
			|| prefix_front.units == 0) {
			// Failed to decode front codepoint; bad unicode sequence
			return 0;
		}

		if (!equalsi(string_front.codepoint, prefix_front.codepoint)) {
			// Codepoints don't fold to same value
			return 0;
		}

		// Codepoints are equal; trim off the fronts and continue
		in_string.remove_prefix(string_front.units);
		in_prefix.remove_prefix(prefix_front.units);
		codepoints_removed += string_front.units;
	}

	if (!in_prefix.empty()) {
		// We reached end of in_string before end of prefix
		return 0;
	}

	return codepoints_removed;
}

ADAPT_BASIC_STRING(starts_with_lengthi)

/**
 * Checks if a string starts with a substring
 *
 * @tparam LhsCharT Character type for underlying string
 * @tparam RhsCharT Character type for prefix string
 * @param in_string String to check for prefix
 * @param in_prefix Prefix to check for
 * @return True if both strings are valid and in_string starts with in_prefix, false otherwise
 */
template<typename LhsCharT, typename RhsCharT>
bool starts_with(std::basic_string_view<LhsCharT> in_string, std::basic_string_view<RhsCharT> in_prefix) {
	return starts_with_length<LhsCharT, RhsCharT>(in_string, in_prefix) != 0;
}

ADAPT_BASIC_STRING(starts_with)

/**
 * Checks if a string starts with a substring (case insensitive)
 *
 * @tparam LhsCharT Character type for underlying string
 * @tparam RhsCharT Character type for prefix string
 * @param in_string String to check for prefix
 * @param in_prefix Prefix to check for
 * @return True if both strings are valid and in_string starts with in_prefix, false otherwise
 */
template<typename LhsCharT, typename RhsCharT>
bool starts_withi(std::basic_string_view<LhsCharT> in_string, std::basic_string_view<RhsCharT> in_prefix) {
	return starts_with_lengthi<LhsCharT, RhsCharT>(in_string, in_prefix) != 0;
}

ADAPT_BASIC_STRING(starts_withi)

/**
 * Searches a string for a specified substring
 *
 * @tparam LhsCharT Character type of the string being searched
 * @tparam RhsCharT Character type of the substring being searched for
 * @param in_string String to search
 * @param in_substring Substring to search for
 * @return Character data index on success, npos otherwise
 */
template<typename LhsCharT, bool CaseSensitive = true>
size_t find(std::basic_string_view<LhsCharT> in_string, char32_t in_codepoint) {
	// If we don't have anything to search through, there's nothing to be found
	if (in_string.empty()) {
		return decltype(in_string)::npos;
	}

	if constexpr (!CaseSensitive) {
		in_codepoint = fold(in_codepoint);
	}

	size_t codepoints_removed{};
	while (!in_string.empty()) {
		std::basic_string_view<LhsCharT> string = in_string;
		get_endpoint_result string_front = decode_codepoint(string);

		if (string_front.units == 0) {
			// Failed to decode front codepoint; bad unicode sequence
			return decltype(in_string)::npos;
		}

		if constexpr (CaseSensitive) {
			if (string_front.codepoint == in_codepoint) {
				// Match found!
				return codepoints_removed;
			}
		}
		else {
			if (fold(string_front.codepoint) == in_codepoint) {
				// Match found!
				return codepoints_removed;
			}
		}

		// Didn't find a match here; remove the front codepoint and try the next position
		in_string.remove_prefix(string_front.units);
		codepoints_removed += string_front.units;
	}

	// We reached the end of in_string before finding the prefix :(
	return decltype(in_string)::npos;
}

/**
 * Searches a string for a specified substring
 *
 * @tparam LhsCharT Character type of the string being searched
 * @tparam RhsCharT Character type of the substring being searched for
 * @param in_string String to search
 * @param in_substring Substring to search for
 * @return Character data index on success, npos otherwise
 */
template<typename LhsCharT, typename RhsCharT, bool CaseSensitive = true>
size_t find(std::basic_string_view<LhsCharT> in_string, std::basic_string_view<RhsCharT> in_substring) {
	// If we're searching for nothing, then we've found it at the front
	if (in_substring.empty()) {
		return 0;
	}

	// If we don't have anything to search through, there's nothing to be found
	if (in_string.empty()) {
		return decltype(in_string)::npos;
	}

	// TODO: expand this to cover any instance where in_substring is a single codepoint, rather than a single data unit
	if (in_substring.size() == 1) {
		return find<LhsCharT, CaseSensitive>(in_string, in_substring[0]);
	}

	// TODO: optimize for when in_substring is small and of different type, by only decoding it once

	size_t codepoints_removed{};
	while (!in_string.empty()) {
		// If in_string and in_prefix are the same type, compare their sizes and quickly return if in_string is too small
		if constexpr (std::is_same_v<LhsCharT, RhsCharT>) {
			if (in_string.size() < in_substring.size()) {
				return decltype(in_string)::npos;
			}
		}

		std::basic_string_view<LhsCharT> string = in_string;
		std::basic_string_view<RhsCharT> substring = in_substring;
		get_endpoint_result string_front;
		do {
			// TODO: optimize this for when in_string and in_substring are same type, by only decoding in_string, solely
			// to determine number of data units to compare
			string_front = decode_codepoint(string);
			get_endpoint_result prefix_front = decode_codepoint(substring);

			if (string_front.units == 0
				|| prefix_front.units == 0) {
				// Failed to decode front codepoint; bad unicode sequence
				return decltype(in_string)::npos;
			}

			if constexpr (CaseSensitive) {
				if (string_front.codepoint != prefix_front.codepoint) {
					// Codepoints aren't the same; break & try next position
					break;
				}
			}
			else {
				if (!equalsi(string_front.codepoint, prefix_front.codepoint)) {
					// Codepoints don't fold the same; break & try next position
					break;
				}
			}

			// Codepoints are equal; trim off the fronts and continue
			string.remove_prefix(string_front.units);
			substring.remove_prefix(prefix_front.units);

			if (substring.empty()) {
				// We found the substring! We can return our current position
				return codepoints_removed;
			}
		} while (!string.empty());

		// Didn't find a match here; remove the front codepoint and try the next position
		in_string.remove_prefix(string_front.units);
		codepoints_removed += string_front.units;
	}

	// We reached the end of in_string before finding the prefix :(
	return decltype(in_string)::npos;
}

ADAPT_BASIC_STRING(find)

/**
 * Searches a string for a specified substring
 *
 * @tparam LhsCharT Character type of the string being searched
 * @tparam RhsCharT Character type of the substring being searched for
 * @param in_string String to search
 * @param in_substring Substring to search for
 * @return Character data index on success, npos otherwise
 */
template<typename LhsCharT, typename RhsCharT>
size_t findi(std::basic_string_view<LhsCharT> in_string, std::basic_string_view<RhsCharT> in_substring) {
	return find<LhsCharT, RhsCharT, false>(in_string, in_substring);
}

ADAPT_BASIC_STRING(findi)

/** to_lower / to_upper */
//char32_t to_lower(char32_t in_chr); // TODO: implement
//char32_t to_upper(char32_t in_chr); // TODO: implement

} // namespace jessilib
