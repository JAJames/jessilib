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

#include <string>
#include <string_view>
#include <ostream>
#include "unicode_base.hpp"

namespace jessilib {

/** Utilities */

namespace impl_unicode {

// Add a narrower version in type_traits.hpp if this is needed elsewhere
template<typename T>
struct is_string : std::false_type {};

template<typename T>
struct is_string<std::basic_string<T>> {
	using type = T;
	static constexpr bool is_fixed_array{ false };
	static constexpr bool is_container{ true };
	static constexpr bool value{ true };
	constexpr operator bool() const noexcept { return true; }
	constexpr bool operator()() const noexcept { return true; }
};

template<typename T>
struct is_string<std::basic_string_view<T>> {
	using type = T;
	static constexpr bool is_fixed_array{ false };
	static constexpr bool is_container{ true };
	static constexpr bool value{ true };
	constexpr operator bool() const noexcept { return true; }
	constexpr bool operator()() const noexcept { return true; }
};

template<typename T>
struct is_string<T*> {
	using type = T;
	static constexpr bool is_fixed_array{ false };
	static constexpr bool is_container{ false };
	static constexpr bool value{ true };
	constexpr operator bool() const noexcept { return true; }
	constexpr bool operator()() const noexcept { return true; }
};

template<typename T>
struct is_string<T[]> {
	using type = T;
	static constexpr bool is_fixed_array{ true };
	static constexpr bool is_container{ false };
	static constexpr bool value{ true };
	constexpr operator bool() const noexcept { return true; }
	constexpr bool operator()() const noexcept { return true; }
};

template<typename T, size_t N>
struct is_string<T[N]> {
	using type = T;
	static constexpr bool is_fixed_array{ true };
	static constexpr bool is_container{ false };
	static constexpr bool value{ true };
	constexpr operator bool() const noexcept { return true; }
	constexpr bool operator()() const noexcept { return true; }
};

} // namespace impl_unicode

template<typename InT>
bool is_valid(const InT& in_string) {
	using InCharT = typename impl_unicode::is_string<InT>::type;
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
	using InCharT = typename impl_unicode::is_string<InT>::type;
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
	return { data_begin, out_string_units };
}

template<typename OutCharT, typename InT>
std::basic_string<OutCharT> string_cast(const InT& in_string) {
	static_assert(impl_unicode::is_string<InT>::value == true);
	using InCharT = typename impl_unicode::is_string<InT>::type;
	using InEquivalentT = typename unicode_traits<InCharT>::equivalent_type;
	using InViewT = std::basic_string_view<InCharT>;
	using OutT = std::basic_string<OutCharT>;

	if constexpr (std::is_same_v<InT, OutT>) {
		// This does nothing at all; consider static_assert against this?
		return in_string;
	}
	else if constexpr (std::is_same_v<OutCharT, InCharT>
	    || std::is_same_v<OutCharT, InEquivalentT>) {
		// Just do a dumb copy when same or equivalent char types; should be faster than re-encoding
		if constexpr (impl_unicode::is_string<InT>::is_container) {
			return { reinterpret_cast<const OutCharT*>(in_string.data()), in_string.size() };
		}
		else if constexpr (impl_unicode::is_string<InT>::is_fixed_array) {
			return { reinterpret_cast<const OutCharT*>(in_string), std::size(in_string) - 1 }; // strip null term
		}
		else {
			return { reinterpret_cast<const OutCharT*>(in_string) };
		}
	}
	else {
		// Last resort: reencode the string
		std::basic_string<OutCharT> result;
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
}

/**
 * Checks if two codepoints are equal to each-other (case insensitive)
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

using find_if_predicate_type = bool(*)(char32_t, char*, size_t);
inline void find_if(std::basic_string<char>& in_string, find_if_predicate_type in_predicate) {
	using CharT = char;
	CharT* ptr = in_string.data();
	std::basic_string_view<CharT> in_string_view = in_string;
	for (auto decode = decode_codepoint(in_string_view); decode.units != 0; decode = decode_codepoint(in_string_view)) {
		if (in_predicate(decode.codepoint, ptr, decode.units)) {
			// predicate indicates it's found what it's looking for, cool
			return;
		}

		in_string_view.remove_prefix(decode.units);
		ptr += decode.units;
	}
}

using find_if_view_predicate_type = bool(*)(char32_t, const char*, size_t);
inline void find_if(std::basic_string_view<char>& in_string, find_if_view_predicate_type in_predicate) {
	using CharT = char;
	const CharT* ptr = in_string.data();
	std::basic_string_view<CharT> in_string_view = in_string;
	for (auto decode = decode_codepoint(in_string_view); decode.units != 0; decode = decode_codepoint(in_string_view)) {
		if (in_predicate(decode.codepoint, ptr, decode.units)) {
			// predicate indicates it's found what it's looking for, cool
			return;
		}

		in_string_view.remove_prefix(decode.units);
		ptr += decode.units;
	}
}

namespace impl_join {

constexpr size_t join_sizes() {
	return 0;
}

// Returns maximum number of bytes needed to represent the joined
template<typename FirstArgT, typename... ArgsT>
constexpr size_t join_sizes(const FirstArgT& in_arg, const ArgsT&... in_args) {
	return in_arg.size() + join_sizes(in_args...);
}

template<typename T>
constexpr void join_append(T&){}; // noop

template<typename OutT, typename InT, typename... ArgsT>
constexpr void join_append(OutT& out_string, InT&& in_string, ArgsT&&... in_args) {
	using InCharT = typename std::remove_cvref_t<InT>::value_type;
	if constexpr (std::is_same_v<typename std::remove_cvref_t<OutT>::value_type, InCharT>) {
		// Join these straight together
		out_string += std::forward<InT>(in_string);
	}
	else {
		// Append over all the codepoints
		get_endpoint_result decode;
		std::basic_string_view<InCharT> in_view = in_string;
		while ((decode = decode_codepoint(in_view)).units != 0) {
			encode_codepoint(out_string, decode.codepoint);
			in_view.remove_prefix(decode.units);
		}
	}

	join_append(out_string, std::forward<ArgsT>(in_args)...);
}

} // impl_join

// Join any number of strings of any type
template<typename OutT, typename... ArgsT>
OutT join(ArgsT&&... args) {
	OutT result;
	result.reserve(impl_join::join_sizes(args...));
	impl_join::join_append<OutT, ArgsT...>(result, std::forward<ArgsT>(args)...);
	return result;
}

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

		get_endpoint_result decode;
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

		get_endpoint_result decode;
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

/** to_lower / to_upper */
//char32_t to_lower(char32_t in_chr); // TODO: implement
//char32_t to_upper(char32_t in_chr); // TODO: implement

} // namespace jessilib
