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
 * @file word_split.hpp
 * @author Jessica James
 *
 * Over-engineered and over-genericized versions of word_split, word_split_once, and word_split_n, with lots of syntactical sugar
 */

#pragma once

#include <string_view>
#include <vector>

namespace jessilib {

template<template<typename...> typename ContainerT, typename ElementT, typename...>
struct word_split_defaults {
	using member_type = std::conditional_t<std::is_trivial_v<ElementT>, std::basic_string<ElementT>, std::vector<ElementT>>;
	using container_type = ContainerT<member_type>;
};

template<template<typename...> typename ContainerT, typename ElementT, typename FirstOptional, typename... ContainerArgsT>
struct word_split_defaults<ContainerT, ElementT, FirstOptional, ContainerArgsT...> {
	using member_type = FirstOptional;
	using container_type = ContainerT<FirstOptional, ContainerArgsT...>;
};

// Can probably be specialized for types which don't take in iterators _or_
template<typename MemberT, typename ItrT, typename EndT, typename std::enable_if<!std::is_constructible<MemberT, ItrT, EndT>::value>::type* = nullptr>
constexpr MemberT make_word_split_member(ItrT in_itr, EndT in_end) {
	// Intended for string_view
	if constexpr (std::is_pointer_v<MemberT>) {
		return { in_itr, static_cast<size_t>(in_end - in_itr) };
	}

	if (in_itr == in_end) {
		return {};
	}

	return { &*in_itr, static_cast<size_t>(in_end - in_itr) };
}

template<typename MemberT, typename ItrT, typename EndT, typename std::enable_if<std::is_constructible<MemberT, ItrT, EndT>::value>::type* = nullptr>
constexpr MemberT make_word_split_member(ItrT in_itr, EndT in_end) {
	// Can construct with iterators, so construct with iterators
	return { in_itr, in_end };
}

/**
 * Splits an input string into substrings based on words
 *
 * @tparam ContainerT Container type to store the results in
 * @tparam ContainerArgsT Optional template parameters for ContainerT
 * @param begin Start of range of elements to split
 * @param end End of range of elements to split
 * @param in_whitespace Whitespace to split upon
 * @return Container populated with
 */
template<template<typename...> typename ContainerT = std::vector, typename... ContainerArgsT, typename ItrT, typename EndT, typename ElementT>
constexpr auto word_split(ItrT begin, EndT end, ElementT in_whitespace) {
	using word_split_defaults_type = word_split_defaults<ContainerT, ElementT, ContainerArgsT...>;
	using member_type = typename word_split_defaults_type::member_type;
	using container_type = typename word_split_defaults_type::container_type;

	container_type result;
	if (begin >= end) {
		// Nothing to word_split
		return result;
	}

	// Skip over any preceeding whitespace
	while (begin != end
		&& *begin == in_whitespace) {
		++begin;
	}

	for (auto itr = begin; itr != end;) {
		if (*itr == in_whitespace) {
			// End of word reached; push token to result and skip over any whitespace
			result.push_back(make_word_split_member<member_type>(begin, itr));

			++itr;
			while (itr != end
				&& *itr == in_whitespace) {
				++itr;
			}

			begin = itr;
			continue;
		}

		++itr;
	}

	// Push final token to the end if not empty
	if (begin != end) {
		result.push_back(make_word_split_member<member_type>(begin, end));
	}

	return result;
}

/**
 * Splits an input string into substrings
 *
 * @tparam ContainerT Container type to store the results in
 * @tparam ContainerArgsT Optional template parameters for ContainerT
 * @param begin Start of range of elements to split
 * @param end End of range of elements to split
 * @param in_whitespace_begin Start of range containing the whitespace values
 * @param in_whitespace_end End of range containing the whitespace values
 * @return Container populated with
 */
template<template<typename...> typename ContainerT = std::vector, typename... ContainerArgsT, typename ItrT, typename EndT, typename SpaceItrT, typename SpaceEndT>
constexpr auto word_split(ItrT begin, EndT end, SpaceItrT in_whitespace_begin, SpaceEndT in_whitespace_end) {
	using ElementT = std::remove_cvref_t<decltype(*begin)>;
	using word_split_defaults_type = word_split_defaults<ContainerT, ElementT, ContainerArgsT...>;
	using member_type = typename word_split_defaults_type::member_type;
	using container_type = typename word_split_defaults_type::container_type;

	if (std::distance(in_whitespace_begin, in_whitespace_end) == 1) {
		return word_split<ContainerT, ContainerArgsT...>(begin, end, *in_whitespace_begin);
	}

	container_type result{};
	if (begin >= end) {
		// Nothing to word_split
		return result;
	}

	if (in_whitespace_begin >= in_whitespace_end) {
		// Absent whitespace, therefore no match, therefore return input as single token
		result.push_back(make_word_split_member<member_type>(begin, end));
		return result;
	}

	auto is_whitespace = [in_whitespace_begin, in_whitespace_end](ElementT in_element) {
		return std::find(in_whitespace_begin, in_whitespace_end, in_element) != in_whitespace_end;
	};

	while (begin != end
		&& is_whitespace(*begin)) {
		++begin;
	}

	for (auto itr = begin; itr < end;) {
		if (is_whitespace(*itr)) {
			// Push token to result
			result.push_back(make_word_split_member<member_type>(begin, itr));

			++itr;
			while (itr != end
				&& is_whitespace(*itr)) {
				++itr;
			}

			begin = itr;
			continue;
		}

		++itr;
	}

	// Push final token to the end if not empty
	if (begin != end) {
		result.push_back(make_word_split_member<member_type>(begin, end));
	}

	return result;
}

/**
 * Splits an input string into substrings
 *
 * @tparam ContainerT Container type to store the results in
 * @tparam ContainerArgsT Optional template parameters for ContainerT
 * @param in_string String to split
 * @param in_whitespace Whitespace to split upon
 * @return Container populated with
 */
template<template<typename...> typename ContainerT = std::vector, typename... ContainerArgsT, typename InputT>
constexpr auto word_split(const InputT& in_string, typename InputT::value_type in_whitespace) {
	return word_split<ContainerT, ContainerArgsT...>(in_string.begin(), in_string.end(), in_whitespace);
}

template<template<typename...> typename ContainerT = std::vector, typename... ContainerArgsT, typename InputT, typename SpaceT,
	typename std::enable_if<!std::is_same<SpaceT, typename InputT::value_type>::value>::type* = nullptr>
constexpr auto word_split(const InputT& in_string, const SpaceT& in_whitespace) {
	return word_split<ContainerT, ContainerArgsT...>(in_string.begin(), in_string.end(), in_whitespace.begin(), in_whitespace.end());
}

/**
 * Splits an input string into 2 substrings at and omitting any input whitespace. Returns:
 * An empty pair if in_string is empty,
 * otherwise if whitespace is not present, a pair who's `second` member is empty and `first` member is equal to `in_string`,
 * otherwise, a pair split at first instance of a string of whitespace
 *
 * @tparam ResultMemberT String type used to populate the result
 * @param begin Start of range of elements to split
 * @param end End of range of elements to split
 * @param in_whitespace Whitespace to split on
 * @return A pair representing `in_string` split at some whitespace, with first word stored in `first` and rest of sentence in `last`
 */
template<typename... OptionalMemberT, typename ItrT, typename EndT, typename ElementT>
constexpr auto word_split_once(ItrT begin, EndT end, ElementT in_whitespace) {
	static_assert(sizeof...(OptionalMemberT) <= 1, "Too many member types specified for OptionalMemberT");
	using MemberT = std::tuple_element_t<0, std::tuple<OptionalMemberT..., std::basic_string<ElementT>>>;

	std::pair<MemberT, MemberT> result;
	if (begin >= end) {
		// Nothing to word_split
		return result;
	}

	while (begin != end
		&& *begin == in_whitespace) {
		++begin;
	}

	for (auto itr = begin; itr != end; ++itr) {
		if (*itr == in_whitespace) {
			// in_whitespace found; word_split upon it
			result.first = make_word_split_member<MemberT>(begin, itr);

			++itr;
			while (itr != end
				&& *itr == in_whitespace) {
				++itr;
			}

			result.second = make_word_split_member<MemberT>(itr, end);
			return result;
		}
	}

	// in_whitespace not found
	result.first = make_word_split_member<MemberT>(begin, end);
	return result;
}

/**
 * Splits an input string into 2 substrings at and omitting any input whitespace. Returns:
 * An empty pair if in_string is empty,
 * otherwise if whitespace is not present, a pair who's `second` member is empty and `first` member is equal to `in_string`,
 * otherwise, a pair split at first instance of any whitespace
 *
 * @tparam ResultMemberT String type used to populate the result
 * @param begin Start of range of elements to split
 * @param end End of range of elements to split
 * @param in_whitespace_begin Start of range containing the whitespace values
 * @param in_whitespace_end End of range containing the whitespace values
 * @return A pair representing `in_string` split at some whitespace, with first word stored in `first` and rest of sentence in `last`
 */
template<typename... OptionalMemberT, typename ItrT, typename EndT, typename SpaceItrT, typename SpaceEndT>
constexpr auto word_split_once(ItrT begin, EndT end, SpaceItrT in_whitespace_begin, SpaceEndT in_whitespace_end) {
	static_assert(sizeof...(OptionalMemberT) <= 1, "Too many member types specified for OptionalMemberT");
	using ElementT = std::remove_cvref_t<decltype(*begin)>;
	using MemberT = std::tuple_element_t<0, std::tuple<OptionalMemberT..., std::basic_string<ElementT>>>;

	if (std::distance(in_whitespace_begin, in_whitespace_end) == 1) {
		return word_split_once<OptionalMemberT...>(begin, end, *in_whitespace_begin);
	}

	std::pair<MemberT, MemberT> result;
	if (begin >= end) {
		// Nothing to word_split
		return result;
	}

	if (in_whitespace_begin >= in_whitespace_end) {
		// Absent whitespace, therefore no match, therefore return input as single token
		result.first = make_word_split_member<MemberT>(begin, end);
		return result;
	}

	auto is_whitespace = [in_whitespace_begin, in_whitespace_end](ElementT in_element) {
		return std::find(in_whitespace_begin, in_whitespace_end, in_element) != in_whitespace_end;
	};

	// Skip over preceeding whitespace
	while (begin != end
		&& is_whitespace(*begin)) {
		++begin;
	}

	for (auto itr = begin; itr < end; ++itr) {
		if (is_whitespace(*itr)) {
			// in_whitespace found; word_split upon it
			result.first = make_word_split_member<MemberT>(begin, itr);

			++itr;
			while (itr != end
				&& is_whitespace(*itr)) {
				++itr;
			}

			result.second = make_word_split_member<MemberT>(itr, end);
			return result;
		}
	}

	// in_whitespace not found
	result.first = make_word_split_member<MemberT>(begin, end);
	return result;
}

/**
 * Splits an input string into 2 substrings at and omitting any input whitespace. Returns:
 * An empty pair if in_string is empty,
 * otherwise if whitespace is not present, a pair who's `second` member is empty and `first` member is equal to `in_string`,
 * otherwise, a pair split at first instance of any whitespace
 *
 * @tparam InputT String type being passed into split_once
 * @tparam ResultMemberT String type used to populate the result
 * @param in_string string to split
 * @param in_whitespace Whitespace to split on
 * @return A pair representing `in_string` split at some whitespace, with first word stored in `first` and rest of sentence in `last`
 */
template<typename... OptionalMemberT, typename InputT>
constexpr auto word_split_once(const InputT& in_string, typename InputT::value_type in_whitespace) {
	static_assert(sizeof...(OptionalMemberT) <= 1, "Too many member types specified for OptionalMemberT");
	using ElementT = typename InputT::value_type;
	using MemberT = std::tuple_element_t<0, std::tuple<OptionalMemberT..., std::basic_string<ElementT>>>;

	return word_split_once<MemberT>(in_string.begin(), in_string.end(), in_whitespace);
}

template<typename... OptionalMemberT, typename InputT, typename SpaceT,
	typename std::enable_if<!std::is_same<SpaceT, typename InputT::value_type>::value>::type* = nullptr>
constexpr auto word_split_once(const InputT& in_string, const SpaceT& in_whitespace) {
	static_assert(sizeof...(OptionalMemberT) <= 1, "Too many member types specified for OptionalMemberT");
	return word_split_once<OptionalMemberT...>(in_string.begin(), in_string.end(), in_whitespace.begin(), in_whitespace.end());
}

/**
 * Splits a range of elements up to a specified number of times and returns the result
 *
 * @tparam ContainerT Container type to store the results in
 * @tparam ContainerArgsT Optional template parameters for ContainerT
 * @param begin Start of range of elements to split
 * @param end End of range of elements to split
 * @param in_whitespace Whitespace to split upon
 * @param in_limit Maximum number of times to split
 * @return Container containing to up `in_limit` + 1 substrings; result[in_limit] is the unprocessed remainder
 */
template<template<typename...> typename ContainerT = std::vector, typename... ContainerArgsT, typename ItrT, typename EndT, typename ElementT>
constexpr auto word_split_n(ItrT begin, EndT end, ElementT in_whitespace, size_t in_limit) {
	using word_split_defaults_type = word_split_defaults<ContainerT, ElementT, ContainerArgsT...>;
	using member_type = typename word_split_defaults_type::member_type;
	using container_type = typename word_split_defaults_type::container_type;

	container_type result;
	if (begin >= end) {
		// Nothing to word_split
		return result;
	}

	while (begin != end
		&& *begin == in_whitespace) {
		++begin;
	}

	for (auto itr = begin; itr != end && in_limit != 0; ++itr) {
		if (*itr == in_whitespace) {
			// Push token to result
			result.push_back(make_word_split_member<member_type>(begin, itr));

			++itr;
			while (itr != end
				&& *itr == in_whitespace) {
				++itr;
			}

			begin = itr;
			--in_limit;
		}
	}

	// Push final token to the end if not empty
	if (begin != end) {
		result.push_back(make_word_split_member<member_type>(begin, end));
	}

	return result;
}

/**
 * Splits a range of elements up to a specified number of times and returns the result
 *
 * @tparam ContainerT Container type to store the results in
 * @tparam ContainerArgsT Optional template parameters for ContainerT
 * @param begin Start of range of elements to split
 * @param end End of range of elements to split
 * @param in_whitespace_begin Start of range containing the whitespace values
 * @param in_whitespace_end End of range containing the whitespace values
 * @return Container containing to up `in_limit` + 1 substrings; result[in_limit] is the unprocessed remainder
 */
template<template<typename...> typename ContainerT = std::vector, typename... ContainerArgsT, typename ItrT, typename EndT, typename SpaceItrT, typename SpaceEndT>
constexpr auto word_split_n(ItrT begin, EndT end, SpaceItrT in_whitespace_begin, SpaceEndT in_whitespace_end, size_t in_limit) {
	using ElementT = std::remove_cvref_t<decltype(*begin)>;
	using word_split_defaults_type = word_split_defaults<ContainerT, ElementT, ContainerArgsT...>;
	using member_type = typename word_split_defaults_type::member_type;
	using container_type = typename word_split_defaults_type::container_type;

	if (std::distance(in_whitespace_begin, in_whitespace_end) == 1) {
		return word_split_n<ContainerT, ContainerArgsT...>(begin, end, *in_whitespace_begin, in_limit);
	}

	container_type result{};
	if (begin >= end) {
		// Nothing to word_split
		return result;
	}

	if (in_whitespace_begin >= in_whitespace_end) {
		// Absent whitespace, therefore no match, therefore return input as single token
		result.push_back(make_word_split_member<member_type>(begin, end));
		return result;
	}

	auto is_whitespace = [in_whitespace_begin, in_whitespace_end](ElementT in_element) {
		return std::find(in_whitespace_begin, in_whitespace_end, in_element) != in_whitespace_end;
	};

	while (begin != end
		&& is_whitespace(*begin)) {
		++begin;
	}

	for (auto itr = begin; itr != end && in_limit != 0;) {
		if (std::equal(in_whitespace_begin, in_whitespace_end, itr)) {
			// Push token to result
			result.push_back(make_word_split_member<member_type>(begin, itr));

			++itr;
			while (itr != end
				&& is_whitespace(*itr)) {
				++itr;
			}

			begin = itr;
			--in_limit;
			continue;
		}

		++itr;
	}

	// Push final token to the end; may be empty
	result.push_back(make_word_split_member<member_type>(begin, end));

	return result;
}

/**
 * Splits a string up to a specified number of times and returns the result
 *
 * @tparam ContainerT Container type to store the results in
 * @tparam ContainerArgsT Optional template parameters for ContainerT
 * @param in_string String to split
 * @param in_whitespace Whitespace to split upon
 * @param in_limit Maximum number of times to split
 * @return Container containing to up `in_limit` + 1 substrings; result[in_limit] is the unprocessed remainder
 */
template<template<typename...> typename ContainerT = std::vector, typename... ContainerArgsT, typename InputT>
constexpr auto word_split_n(const InputT& in_string, typename InputT::value_type in_whitespace, size_t in_limit) {
	return word_split_n<ContainerT, ContainerArgsT...>(in_string.begin(), in_string.end(), in_whitespace, in_limit);
}

template<template<typename...> typename ContainerT = std::vector, typename... ContainerArgsT, typename InputT, typename SpaceT,
	typename std::enable_if<!std::is_same<SpaceT, typename InputT::value_type>::value>::type* = nullptr>
constexpr auto word_split_n(const InputT& in_string, const SpaceT& in_whitespace, size_t in_limit) {
	return word_split_n<ContainerT, ContainerArgsT...>(in_string.begin(), in_string.end(), in_whitespace.begin(), in_whitespace.end(), in_limit);
}

/** Splits an input string into view substrings; cannot specify element return type */
template<template<typename...> typename ContainerT = std::vector, typename... ContainerArgsT, typename InputT, typename SpaceT>
constexpr auto word_split_view(const InputT& in_string, const SpaceT& in_whitespace) {
	using MemberT = std::basic_string_view<typename InputT::value_type>;
	return word_split<ContainerT, MemberT, ContainerArgsT...>(in_string, in_whitespace);
}

/** Splits an input string into view substring pair */
template<typename... NothingT, typename InputT, typename SpaceT>
constexpr auto word_split_once_view(const InputT& in_string, const SpaceT& in_whitespace) {
	// Using a static assertion instead of `auto` for inputs, because this is the only place it would be used, and it'd
	// require -fconcepts which isn't currently used. Replace inputs with `auto` later and remove template parameters
	// if -fconcepts is ever added.
	static_assert(sizeof...(NothingT) == 0, "word_split_once_view does not accept template parameters");
	using MemberT = std::basic_string_view<typename InputT::value_type>;
	return word_split_once<MemberT>(in_string, in_whitespace);
}

/** Splits an input string into a specified number of view substrings */
template<template<typename...> typename ContainerT = std::vector, typename... ContainerArgsT,
	typename InputT, typename SpaceT>
constexpr auto word_split_n_view(const InputT& in_string, const SpaceT& in_whitespace, size_t in_limit) {
	using MemberT = std::basic_string_view<typename InputT::value_type>;
	return word_split_n<ContainerT, MemberT, ContainerArgsT...>(in_string, in_whitespace, in_limit);
}

} // namespace jessilib
