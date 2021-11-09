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

#pragma once

#include <string_view>
#include <vector>

namespace jessilib {

/** Splits an input string into substrings */
template<typename InStringT, typename ResultT = std::vector<InStringT>>
constexpr ResultT split(const InStringT& in_string, typename InStringT::value_type in_delim) {
	ResultT result;
	// Nothing to return
	if (in_string.empty()) {
		return result;
	}

	// If only MSVC didn't suck, we could use begin() and end()...
	auto begin = in_string.data();
	auto end = in_string.data() + in_string.size();
	for (auto itr = begin; itr != end; ++itr) {
		if (*itr == in_delim) {
			// Push token to result
			result.emplace_back(begin, itr - begin);
			begin = itr + 1;
		}
	}

	// begin == end only if last character was in_delim
	if (begin == end) {
		result.emplace_back();
	}
	else {
		result.emplace_back(begin, end - begin);
	}

	return result;
}

/** Splits an input string into view substrings (same as split, but different default return type) */
template<typename InStringT, typename ResultT = std::vector<std::basic_string_view<typename InStringT::value_type>>>
constexpr ResultT split_view(const InStringT& in_string, typename InStringT::value_type in_delim) {
	return split<InStringT, ResultT>(in_string, in_delim);
}

} // namespace jessilib
