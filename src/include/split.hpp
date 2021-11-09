//
// Created by jessi on 11/9/2021.
//

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
