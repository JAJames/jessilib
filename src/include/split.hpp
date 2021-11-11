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
 * @file split.hpp
 * @author Jessica James
 *
 * Over-engineered and over-genericized versions of split, split_once, and split_n, with lots of syntactical sugar
 */

#pragma once

#include <string_view>
#include <vector>
#include "type_traits.hpp" // is_basic_string_view; remove when compilers don't suck

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

template<typename MemberT, typename ItrT, typename EndT, typename std::enable_if<is_basic_string_view<MemberT>::value>::type* = nullptr>
MemberT member_from_range(ItrT in_itr, EndT in_end) {
	// Workaround due to C++20 iterator constructor being inconsistently available
	if constexpr (std::is_pointer_v<MemberT>) {
		return { in_itr, static_cast<size_t>(in_end - in_itr) };
	}

	auto& element = *in_itr;
	return { &element, static_cast<size_t>(in_end - in_itr) };
}

template<typename MemberT, typename ItrT, typename EndT, typename std::enable_if<!is_basic_string_view<MemberT>::value>::type* = nullptr>
MemberT member_from_range(ItrT in_itr, EndT in_end) {
	// Workaround due to C++20 iterator constructor being inconsistently available
	return { in_itr, in_end };
}

} // namespace impl

/**
 * Splits an input string into substrings
 *
 * @tparam ContainerT Container type to store the results in
 * @tparam ContainerArgsT Optional template parameters for ContainerT
 * @param begin Start of range of elements to split
 * @param end End of range of elements to split
 * @param in_delim Delimiter to split upon
 * @return Container populated with
 */
template<template<typename...> typename ContainerT = std::vector, typename... ContainerArgsT, typename ItrT, typename EndT, typename ElementT>
constexpr auto split(ItrT begin, EndT end, ElementT in_delim) {
	using MemberT = typename impl::first_arg<ContainerArgsT..., std::basic_string<ElementT>>::first_type;
	using container_type = impl::split_container_helper_t<ContainerT, MemberT, ContainerArgsT...>;

	container_type result;
	if (begin >= end) {
		// Nothing to split
		return result;
	}

	for (auto itr = begin; itr != end; ++itr) {
		if (*itr == in_delim) {
			// Push token to result
			result.push_back(impl::member_from_range<MemberT>(begin, itr));
			begin = itr + 1;
		}
	}

	// Push final token to the end; may be empty
	result.push_back(impl::member_from_range<MemberT>(begin, end));

	return result;
}

/**
 * Splits an input string into substrings
 *
 * @tparam ContainerT Container type to store the results in
 * @tparam ContainerArgsT Optional template parameters for ContainerT
 * @param begin Start of range of elements to split
 * @param end End of range of elements to split
 * @param in_delim_begin Start of range containing the delimiter
 * @param in_delim_end End of range containing the delimiter
 * @return Container populated with
 */
template<template<typename...> typename ContainerT = std::vector, typename... ContainerArgsT, typename ItrT, typename EndT, typename DelimItrT, typename DelimEndT>
constexpr auto split(ItrT begin, EndT end, DelimItrT in_delim_begin, DelimEndT in_delim_end) {
	using ElementT = std::remove_cvref_t<decltype(*begin)>;
	using MemberT = typename impl::first_arg<ContainerArgsT..., std::basic_string<ElementT>>::first_type;
	using container_type = impl::split_container_helper_t<ContainerT, MemberT, ContainerArgsT...>;

	auto delim_length = std::distance(in_delim_begin, in_delim_end);
	if (delim_length == 1) {
		return split<ContainerT, ContainerArgsT...>(begin, end, *in_delim_begin);
	}

	container_type result{};
	if (begin >= end) {
		// Nothing to split
		return result;
	}

	if (in_delim_begin >= in_delim_end
		|| (end - begin) < delim_length) {
		// Absent or impossible to match delimiter, therefore no match, therefore return input as single token
		result.push_back(impl::member_from_range<MemberT>(begin, end));
		return result;
	}

	auto itr_end = end - (delim_length - 1);
	for (auto itr = begin; itr < itr_end;) {
		if (std::equal(in_delim_begin, in_delim_end, itr)) {
			// Push token to result
			result.push_back(impl::member_from_range<MemberT>(begin, itr));
			itr += delim_length;
			begin = itr;
			continue;
		}

		++itr;
	}

	// Push final token to the end; may be empty
	result.push_back(impl::member_from_range<MemberT>(begin, end));

	return result;
}

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
	return split<ContainerT, ContainerArgsT...>(in_string.begin(), in_string.end(), in_delim);
}

template<template<typename...> typename ContainerT = std::vector, typename... ContainerArgsT, typename InputT, typename DelimT,
	typename std::enable_if<!std::is_same<DelimT, typename InputT::value_type>::value>::type* = nullptr>
constexpr auto split(const InputT& in_string, const DelimT& in_delim) {
	return split<ContainerT, ContainerArgsT...>(in_string.begin(), in_string.end(), in_delim.begin(), in_delim.end());
}

/**
 * Splits an input string into 2 substrings at and omitting an input delimiter. Returns:
 * An empty pair if in_string is empty,
 * otherwise if the delimiter is not present, a pair who's `second` member is empty and `first` member is equal to `in_string`,
 * otherwise, a pair split at first instance of the delimiter
 *
 * @tparam ResultMemberT String type used to populate the result
 * @param begin Start of range of elements to split
 * @param end End of range of elements to split
 * @param in_delim delimiter to split on
 * @return A pair representing `in_string` split at a delimiter, with first half stored in `first` and second in `last`
 */
template<typename... OptionalMemberT, typename ItrT, typename EndT, typename ElementT>
constexpr auto split_once(ItrT begin, EndT end, ElementT in_delim) {
	static_assert(sizeof...(OptionalMemberT) <= 1, "Too many member types specified for OptionalMemberT");
	using MemberT = typename impl::first_arg<OptionalMemberT..., std::basic_string<ElementT>>::first_type;

	std::pair<MemberT, MemberT> result;
	if (begin >= end) {
		// Nothing to split
		return result;
	}

	for (auto itr = begin; itr != end; ++itr) {
		if (*itr == in_delim) {
			// in_delim found; split upon it
			result.first = impl::member_from_range<MemberT>(begin, itr);
			result.second = impl::member_from_range<MemberT>(itr + 1, end);
			return result;
		}
	}

	// in_delim not found
	result.first = impl::member_from_range<MemberT>(begin, end);
	return result;
}

/**
 * Splits an input string into 2 substrings at and omitting an input delimiter. Returns:
 * An empty pair if in_string is empty,
 * otherwise if the delimiter is not present, a pair who's `second` member is empty and `first` member is equal to `in_string`,
 * otherwise, a pair split at first instance of the delimiter
 *
 * @tparam ResultMemberT String type used to populate the result
 * @param begin Start of range of elements to split
 * @param end End of range of elements to split
 * @param in_delim_begin Start of range containing the delimiter
 * @param in_delim_end End of range containing the delimiter
 * @return A pair representing `in_string` split at a delimiter, with first half stored in `first` and second in `last`
 */
template<typename... OptionalMemberT, typename ItrT, typename EndT, typename DelimItrT, typename DelimEndT>
constexpr auto split_once(ItrT begin, EndT end, DelimItrT in_delim_begin, DelimEndT in_delim_end) {
	static_assert(sizeof...(OptionalMemberT) <= 1, "Too many member types specified for OptionalMemberT");
	using ElementT = std::remove_cvref_t<decltype(*begin)>;
	using MemberT = typename impl::first_arg<OptionalMemberT..., std::basic_string<ElementT>>::first_type;

	auto delim_length = std::distance(in_delim_begin, in_delim_end);
	if (delim_length == 1) {
		return split_once<OptionalMemberT...>(begin, end, *in_delim_begin);
	}

	std::pair<MemberT, MemberT> result;
	if (begin >= end) {
		// Nothing to split
		return result;
	}

	if (in_delim_begin >= in_delim_end
		|| (end - begin) < delim_length) {
		// Absent or impossible to match delimiter, therefore no match, therefore return input as single token
		result.first = impl::member_from_range<MemberT>(begin, end);
		return result;
	}

	auto itr_end = end - (delim_length - 1);
	for (auto itr = begin; itr < itr_end;) {
		if (std::equal(in_delim_begin, in_delim_end, itr)) {
			// in_delim found; split upon it
			result.first = impl::member_from_range<MemberT>(begin, itr);
			result.second = impl::member_from_range<MemberT>(itr + delim_length, end);
			return result;
		}
	}

	// in_delim not found
	result.first = impl::member_from_range<MemberT>(begin, end);
	return result;
}

/**
 * Splits an input string into 2 substrings at and omitting an input delimiter. Returns:
 * An empty pair if in_string is empty,
 * otherwise if the delimiter is not present, a pair who's `second` member is empty and `first` member is equal to `in_string`,
 * otherwise, a pair split at first instance of the delimiter
 *
 * @tparam InputT String type being passed into split_once
 * @tparam ResultMemberT String type used to populate the result
 * @param in_string string to split
 * @param in_delim delimiter to split on
 * @return A pair representing `in_string` split at a delimiter, with first half stored in `first` and second in `last`
 */
template<typename... OptionalMemberT, typename InputT>
constexpr auto split_once(const InputT& in_string, typename InputT::value_type in_delim) {
	static_assert(sizeof...(OptionalMemberT) <= 1, "Too many member types specified for OptionalMemberT");
	using ElementT = typename InputT::value_type;
	using MemberT = typename impl::first_arg<OptionalMemberT..., std::basic_string<ElementT>>::first_type;

	return split_once<MemberT>(in_string.begin(), in_string.end(), in_delim);
}

template<typename... OptionalMemberT, typename InputT, typename DelimT,
	typename std::enable_if<!std::is_same<DelimT, typename InputT::value_type>::value>::type* = nullptr>
constexpr auto split_once(const InputT& in_string, const DelimT& in_delim) {
	static_assert(sizeof...(OptionalMemberT) <= 1, "Too many member types specified for OptionalMemberT");
	return split_once<OptionalMemberT...>(in_string.begin(), in_string.end(), in_delim.begin(), in_delim.end());
}

/**
 * Splits a range of elements up to a specified number of times and returns the result
 *
 * @tparam ContainerT Container type to store the results in
 * @tparam ContainerArgsT Optional template parameters for ContainerT
 * @param begin Start of range of elements to split
 * @param end End of range of elements to split
 * @param in_delim Delimiter to split upon
 * @param in_limit Maximum number of times to split
 * @return Container containing to up `in_limit` + 1 substrings; result[in_limit] is the unprocessed remainder
 */
template<template<typename...> typename ContainerT = std::vector, typename... ContainerArgsT, typename ItrT, typename EndT, typename ElementT>
constexpr auto split_n(ItrT begin, EndT end, ElementT in_delim, size_t in_limit) {
	using MemberT = typename impl::first_arg<ContainerArgsT..., std::basic_string<ElementT>>::first_type;
	using container_type = impl::split_container_helper_t<ContainerT, MemberT, ContainerArgsT...>;

	container_type result;
	if (begin >= end) {
		// Nothing to split
		return result;
	}

	for (auto itr = begin; itr != end && in_limit != 0; ++itr) {
		if (*itr == in_delim) {
			// Push token to result
			result.push_back(impl::member_from_range<MemberT>(begin, itr));
			begin = itr + 1;
			--in_limit;
		}
	}

	// Push final token to the end; may be empty
	result.push_back(impl::member_from_range<MemberT>(begin, end));

	return result;
}

/**
 * Splits a range of elements up to a specified number of times and returns the result
 *
 * @tparam ContainerT Container type to store the results in
 * @tparam ContainerArgsT Optional template parameters for ContainerT
 * @param begin Start of range of elements to split
 * @param end End of range of elements to split
 * @param in_delim Delimiter to split upon
 * @param in_delim_begin Start of range containing the delimiter
 * @param in_delim_end End of range containing the delimiter
 * @return Container containing to up `in_limit` + 1 substrings; result[in_limit] is the unprocessed remainder
 */
template<template<typename...> typename ContainerT = std::vector, typename... ContainerArgsT, typename ItrT, typename EndT, typename DelimItrT, typename DelimEndT>
constexpr auto split_n(ItrT begin, EndT end, DelimItrT in_delim_begin, DelimEndT in_delim_end, size_t in_limit) {
	using ElementT = std::remove_cvref_t<decltype(*begin)>;
	using MemberT = typename impl::first_arg<ContainerArgsT..., std::basic_string<ElementT>>::first_type;
	using container_type = impl::split_container_helper_t<ContainerT, MemberT, ContainerArgsT...>;

	auto delim_length = std::distance(in_delim_begin, in_delim_end);
	if (delim_length == 1) {
		return split_n<ContainerT, ContainerArgsT...>(begin, end, *in_delim_begin, in_limit);
	}

	container_type result{};
	if (begin >= end) {
		// Nothing to split
		return result;
	}

	if (in_delim_begin >= in_delim_end
		|| (end - begin) < delim_length) {
		// Absent or impossible to match delimiter, therefore no match, therefore return input as single token
		result.push_back(impl::member_from_range<MemberT>(begin, end));
		return result;
	}

	auto itr_end = end - (delim_length - 1);
	for (auto itr = begin; itr < itr_end && in_limit != 0;) {
		if (std::equal(in_delim_begin, in_delim_end, itr)) {
			// Push token to result
			result.push_back(impl::member_from_range<MemberT>(begin, itr));
			itr += delim_length;
			begin = itr;
			--in_limit;
			continue;
		}

		++itr;
	}

	// Push final token to the end; may be empty
	result.push_back(impl::member_from_range<MemberT>(begin, end));

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
	return split_n<ContainerT, ContainerArgsT...>(in_string.begin(), in_string.end(), in_delim, in_limit);
}

template<template<typename...> typename ContainerT = std::vector, typename... ContainerArgsT, typename InputT, typename DelimT,
	typename std::enable_if<!std::is_same<DelimT, typename InputT::value_type>::value>::type* = nullptr>
constexpr auto split_n(const InputT& in_string, const DelimT& in_delim, size_t in_limit) {
	return split_n<ContainerT, ContainerArgsT...>(in_string.begin(), in_string.end(), in_delim.begin(), in_delim.end(), in_limit);
}

/** Splits an input string into view substrings; cannot specify element return type */
template<template<typename...> typename ContainerT = std::vector, typename... ContainerArgsT, typename InputT, typename DelimT>
constexpr auto split_view(const InputT& in_string, const DelimT& in_delim) {
	using MemberT = std::basic_string_view<typename InputT::value_type>;
	return split<ContainerT, MemberT, ContainerArgsT...>(in_string, in_delim);
}

/** Splits an input string into view substring pair */
template<typename... NothingT, typename InputT, typename DelimT>
constexpr auto split_once_view(const InputT& in_string, const DelimT& in_delim) {
	// Using a static assertion instead of `auto` for inputs, because this is the only place it would be used, and it'd
	// require -fconcepts which isn't currently used. Replace inputs with `auto` later and remove template parameters
	// if -fconcepts is ever added.
	static_assert(sizeof...(NothingT) == 0, "split_once_view does not accept template parameters");
	using MemberT = std::basic_string_view<typename InputT::value_type>;
	return split_once<MemberT>(in_string, in_delim);
}

/** Splits an input string into a specified number of view substrings */
template<template<typename...> typename ContainerT = std::vector, typename... ContainerArgsT,
	typename InputT, typename DelimT>
constexpr auto split_n_view(const InputT& in_string, const DelimT& in_delim, size_t in_limit) {
	using MemberT = std::basic_string_view<typename InputT::value_type>;
	return split_n<ContainerT, MemberT, ContainerArgsT...>(in_string, in_delim, in_limit);
}

} // namespace jessilib
