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
namespace impl {

template<typename FirstArgT, typename...>
struct first_arg {
	using first_type = FirstArgT;
};

// Using a function's return type because both `using` and classes require parameter packs be at the end
template<template<typename...> typename ContainerT, typename DefaultMemberT, typename... ArgsT,
	typename std::enable_if<sizeof...(ArgsT) == 0>::type* = nullptr>
constexpr auto split_container_helper_f() -> ContainerT<DefaultMemberT> {
	return {};
};

template<template<typename...> typename ContainerT, typename DefaultMemberT, typename... ArgsT,
	typename std::enable_if<sizeof...(ArgsT) != 0>::type* = nullptr>
constexpr auto split_container_helper_f() -> ContainerT<ArgsT...> {
	return {};
};

template<template<typename...> typename ContainerT, typename... ArgsT>
using split_container_helper_t = decltype(split_container_helper_f<ContainerT, ArgsT...>());

} // namespace impl

/**
 * Splits an input string into substrings
 *
 * @tparam ContainerT Container type to store the results in
 * @tparam ContainerArgsT Optional template parameters for ContainerT
 * @param in_string String to split
 * @param in_delim Delimiter to split upon
 * @return Container populated with
 */
template<template<typename...> typename ContainerT = std::vector, typename... ContainerArgsT, typename InputT>
constexpr auto split(const InputT& in_string, typename InputT::value_type in_delim) {
	using MemberT = typename impl::first_arg<ContainerArgsT..., std::basic_string<typename InputT::value_type>>::first_type;
	using container_type = impl::split_container_helper_t<ContainerT, MemberT, ContainerArgsT...>;
	container_type result;
	// Nothing to return
	if (in_string.empty()) {
		return result;
	}

	auto begin = in_string.data();
	auto end = in_string.data() + in_string.size();
	for (auto itr = begin; itr != end; ++itr) {
		if (*itr == in_delim) {
			// Push token to result
			result.emplace_back(begin, itr);
			begin = itr + 1;
		}
	}

	// Push final token to the end; may be empty
	result.emplace_back(begin, end);

	return result;
}

/**
 * Splits an input string into 2 substrings at and omitting an input delimiter. Returns:
 * An empty pair if in_string is empty,
 * otherwise if the delimiter is not present, a pair who's `second` member is empty and `first` member is equal to `in_string`,
 * otherwise, a pair split at first instance of the delimiter
 *
 * @tparam InStringT String type being passed into split_once
 * @tparam ResultMemberT String type used to populate the result
 * @param in_string string to split
 * @param in_delim delimiter to split on
 * @return A pair representing `in_string` split at a delimiter, with first half stored in `first` and second in `last`
 */
template<typename InStringT, typename ResultMemberT = InStringT>
constexpr std::pair<ResultMemberT, ResultMemberT> split_once(const InStringT& in_string, typename InStringT::value_type in_delim) {
	// Nothing in, nothing out
	std::pair<ResultMemberT, ResultMemberT> result;
	if (in_string.empty()) {
		return result;
	}

	auto begin = in_string.data();
	auto end = in_string.data() + in_string.size();
	for (auto itr = begin; itr != end; ++itr) {
		if (*itr == in_delim) {
			// in_delim found; split upon it
			result.first = ResultMemberT{ begin, itr };
			result.second = ResultMemberT{ itr + 1, end };
			return result;
		}
	}

	// in_delim not found
	result.first = in_string;
	return result;
}

/**
 * Splits a string up to a specified number of times and returns the result
 *
 * @tparam ContainerT Container type to store the results in
 * @tparam ContainerArgsT Optional template parameters for ContainerT
 * @param in_string String to split
 * @param in_delim Delimiter to split upon
 * @param in_limit Maximum number of times to split
 * @return Container containing to up `in_limit` + 1 substrings; result[in_limit] is the unprocessed remainder
 */
template<template<typename...> typename ContainerT = std::vector, typename... ContainerArgsT, typename InputT>
constexpr auto split_n(const InputT& in_string, typename InputT::value_type in_delim, size_t in_limit) {
	using MemberT = typename impl::first_arg<ContainerArgsT..., std::basic_string<typename InputT::value_type>>::first_type;
	using container_type = impl::split_container_helper_t<ContainerT, MemberT, ContainerArgsT...>;

	container_type result;
	if (in_string.empty()) {
		return result;
	}

	auto begin = in_string.data();
	auto end = in_string.data() + in_string.size();
	for (auto itr = begin; itr != end && in_limit != 0; ++itr) {
		if (*itr == in_delim) {
			// Push token to result
			result.emplace_back(begin, itr);
			begin = itr + 1;
			--in_limit;
		}
	}

	// Push final token to the end; may be empty
	result.emplace_back(begin, end);

	return result;
}

/** Splits an input string into view substrings; cannot specify element return type */
template<template<typename...> typename ContainerT = std::vector, typename... ContainerArgsT, typename InStringT, typename MemberT = std::basic_string_view<typename InStringT::value_type>>
constexpr ContainerT<MemberT, ContainerArgsT...> split_view(const InStringT& in_string, typename InStringT::value_type in_delim) {
	return split<ContainerT, MemberT, ContainerArgsT...>(in_string, in_delim);
}

/** Splits an input string into view substrings (same as split, but different default return type) */
template<typename InStringT, typename ResultT = std::basic_string_view<typename InStringT::value_type>>
constexpr std::pair<ResultT, ResultT> split_once_view(const InStringT& in_string, typename InStringT::value_type in_delim) {
	return split_once<InStringT, ResultT>(in_string, in_delim);
}

/** Splits an input string into a specified number of view substrings */
template<template<typename...> typename ContainerT = std::vector, typename... ContainerArgsT, typename InStringT, typename MemberT = std::basic_string_view<typename InStringT::value_type>>
constexpr auto split_n_view(const InStringT& in_string, typename InStringT::value_type in_delim, size_t in_limit) {
	return split_n<ContainerT, MemberT, ContainerArgsT...>(in_string, in_delim, in_limit);
}

} // namespace jessilib
