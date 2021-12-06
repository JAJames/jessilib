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
 * @file unicode_hash.hpp
 * @author Jessica James
 *
 * Unicode-aware encoding-agnostic string comparison & hashing utilities
 */

#pragma once

#include "unicode_base.hpp"

namespace jessilib {

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
		decode_result string_front = decode_codepoint(in_string);
		decode_result prefix_front = decode_codepoint(in_prefix);

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
		decode_result string_front = decode_codepoint(in_string);
		decode_result prefix_front = decode_codepoint(in_prefix);

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
 * Calculates the hash of a string based on its codepoints, such that a unicode string will always produce the same hash
 * regardless of underlying encoding
 *
 * This is not intended for generating hashses of arbitrary data; it's specifically intended for strings of text
 */
struct text_hash {
	using is_transparent = std::true_type;

	template<typename CharT>
	static uint64_t hash(const CharT* data, const CharT* end) {
		uint64_t hash = 14695981039346656037ULL;

		decode_result decode;
		while (data != end) {
			decode = decode_codepoint(data, end);
			if (decode.units == 0) {
				return hash;
			}

			hash = hash ^ decode.codepoint;
			hash = hash * 1099511628211ULL;
			data += decode.units;
		}

		return hash;
	}

	auto operator()(const std::basic_string<char>& in_key) const noexcept { // ASSUMES UTF-8
		return hash(in_key.data(), in_key.data() + in_key.size());
	}

	auto operator()(std::basic_string_view<char> in_key) const noexcept {
		return hash(in_key.data(), in_key.data() + in_key.size());
	}

	auto operator()(const std::basic_string<char8_t>& in_key) const noexcept { // ASSUMES UTF-8
		return hash(in_key.data(), in_key.data() + in_key.size());
	}

	auto operator()(std::basic_string_view<char8_t> in_key) const noexcept {
		return hash(in_key.data(), in_key.data() + in_key.size());
	}

	auto operator()(const std::basic_string<char16_t>& in_key) const noexcept { // ASSUMES UTF-8
		return hash(in_key.data(), in_key.data() + in_key.size());
	}

	auto operator()(std::basic_string_view<char16_t> in_key) const noexcept {
		return hash(in_key.data(), in_key.data() + in_key.size());
	}

	auto operator()(const std::basic_string<char32_t>& in_key) const noexcept { // ASSUMES UTF-8
		return hash(in_key.data(), in_key.data() + in_key.size());
	}

	auto operator()(std::basic_string_view<char32_t> in_key) const noexcept {
		return hash(in_key.data(), in_key.data() + in_key.size());
	}
};

struct text_equal {
	using is_transparent = std::true_type;

	template<typename LhsCharT, typename RhsCharT>
	bool operator()(std::basic_string_view<LhsCharT> in_lhs, std::basic_string_view<RhsCharT> in_rhs) const noexcept {
		return equals<LhsCharT, RhsCharT>(in_lhs, in_rhs);
	}

	template<typename LhsCharT, typename RhsCharT>
	bool operator()(std::basic_string_view<LhsCharT> in_lhs, const std::basic_string<RhsCharT>& in_rhs) const noexcept {
		return equals<LhsCharT, RhsCharT>(in_lhs, in_rhs);
	}

	template<typename LhsCharT, typename RhsCharT>
	bool operator()(const std::basic_string<LhsCharT>& in_lhs, std::basic_string_view<RhsCharT> in_rhs) const noexcept {
		return equals<LhsCharT, RhsCharT>(in_lhs, in_rhs);
	}

	template<typename LhsCharT, typename RhsCharT>
	bool operator()(const std::basic_string<LhsCharT>& in_lhs, const std::basic_string<RhsCharT>& in_rhs) const noexcept {
		return equals<LhsCharT, RhsCharT>(in_lhs, in_rhs);
	}
};

/**
 * Calculates the hash of a string based on its folded codepoints, such that a unicode string will always produce the
 * same hash regardless of underlying encoding or the casing of its values.
 *
 * This is not intended for generating hashses of arbitrary data; it's specifically intended for strings of text
 */
struct text_hashi {
	using is_transparent = std::true_type;

	template<typename CharT>
	static uint64_t hash(const CharT* data, const CharT* end) {
		uint64_t hash = 14695981039346656037ULL;

		decode_result decode;
		while (data != end) {
			decode = decode_codepoint(data, end - data);
			if (decode.units == 0) {
				return hash;
			}

			hash = hash ^ fold(decode.codepoint);
			hash = hash * 1099511628211ULL;
			data += decode.units;
		}

		return hash;
	}

	auto operator()(const std::basic_string<char>& in_key) const noexcept { // ASSUMES UTF-8
		return hash(in_key.data(), in_key.data() + in_key.size());
	}

	auto operator()(std::basic_string_view<char> in_key) const noexcept {
		return hash(in_key.data(), in_key.data() + in_key.size());
	}

	auto operator()(const std::basic_string<char8_t>& in_key) const noexcept { // ASSUMES UTF-8
		return hash(in_key.data(), in_key.data() + in_key.size());
	}

	auto operator()(std::basic_string_view<char8_t> in_key) const noexcept {
		return hash(in_key.data(), in_key.data() + in_key.size());
	}

	auto operator()(const std::basic_string<char16_t>& in_key) const noexcept { // ASSUMES UTF-8
		return hash(in_key.data(), in_key.data() + in_key.size());
	}

	auto operator()(std::basic_string_view<char16_t> in_key) const noexcept {
		return hash(in_key.data(), in_key.data() + in_key.size());
	}

	auto operator()(const std::basic_string<char32_t>& in_key) const noexcept { // ASSUMES UTF-8
		return hash(in_key.data(), in_key.data() + in_key.size());
	}

	auto operator()(std::basic_string_view<char32_t> in_key) const noexcept {
		return hash(in_key.data(), in_key.data() + in_key.size());
	}
};

struct text_equali {
	using is_transparent = std::true_type;

	template<typename LhsCharT, typename RhsCharT>
	bool operator()(std::basic_string_view<LhsCharT> in_lhs, std::basic_string_view<RhsCharT> in_rhs) const noexcept {
		return equalsi<LhsCharT, RhsCharT>(in_lhs, in_rhs);
	}

	template<typename LhsCharT, typename RhsCharT>
	bool operator()(std::basic_string_view<LhsCharT> in_lhs, const std::basic_string<RhsCharT>& in_rhs) const noexcept {
		return equalsi<LhsCharT, RhsCharT>(in_lhs, in_rhs);
	}

	template<typename LhsCharT, typename RhsCharT>
	bool operator()(const std::basic_string<LhsCharT>& in_lhs, std::basic_string_view<RhsCharT> in_rhs) const noexcept {
		return equalsi<LhsCharT, RhsCharT>(in_lhs, in_rhs);
	}

	template<typename LhsCharT, typename RhsCharT>
	bool operator()(const std::basic_string<LhsCharT>& in_lhs, const std::basic_string<RhsCharT>& in_rhs) const noexcept {
		return equalsi<LhsCharT, RhsCharT>(in_lhs, in_rhs);
	}
};

} // namespace jessilib
