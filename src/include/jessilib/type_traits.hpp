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

#include <cstddef>
#include <type_traits>

// Container types we're using, more or less purely because we can't forward declare these at all
#include <vector>
#include <list>
#include <forward_list>
#include <stack>
#include <queue>
#include <deque>
#include <set>
#include <unordered_set>
#include <map>
#include <unordered_map>
#include <string>
#include <string_view>

namespace jessilib {

/** is_basic_string */

template<typename T>
struct is_basic_string : std::false_type {};

template<typename T>
struct is_basic_string<std::basic_string<T>> {
	using type = T;
	static constexpr bool value{ true };
	constexpr operator bool() const noexcept { return true; }
	constexpr bool operator()() const noexcept { return true; }
};

/** is_basic_string_view */

template<typename T>
struct is_basic_string_view : std::false_type {};

template<typename T>
struct is_basic_string_view<std::basic_string_view<T>> {
	using type = T;
	static constexpr bool value{ true };
	constexpr operator bool() const noexcept { return true; }
	constexpr bool operator()() const noexcept { return true; }
};

/** is_pair */

template<typename T>
struct is_pair : std::false_type {};

template<typename LeftT, typename RightT>
struct is_pair<std::pair<LeftT, RightT>> {
	using first_type = LeftT;
	using second_type = RightT;
	static constexpr bool value{ true };
	constexpr operator bool() const noexcept { return true; }
	constexpr bool operator()() const noexcept { return true; }
};

/** is_vector */

template<typename T>
struct is_vector : std::false_type {};

template<typename T>
struct is_vector<std::vector<T>> {
	using type = T;
	static constexpr bool value{ true };
	constexpr operator bool() const noexcept { return true; }
	constexpr bool operator()() const noexcept { return true; }
};

/** is_list */

template<typename T>
struct is_list : std::false_type {};

template<typename T>
struct is_list<std::list<T>> {
	using type = T;
	static constexpr bool value{ true };
	constexpr operator bool() const noexcept { return true; }
	constexpr bool operator()() const noexcept { return true; }
};

/** is_forward_list */

template<typename T>
struct is_forward_list : std::false_type {};

template<typename T>
struct is_forward_list<std::forward_list<T>> {
	using type = T;
	static constexpr bool value{ true };
	constexpr operator bool() const noexcept { return true; }
	constexpr bool operator()() const noexcept { return true; }
};

/** is_stack */

template<typename T>
struct is_stack : std::false_type {};

template<typename T>
struct is_stack<std::stack<T>> {
	using type = T;
	static constexpr bool value{ true };
	constexpr operator bool() const noexcept { return true; }
	constexpr bool operator()() const noexcept { return true; }
};

/** is_queue */

template<typename T>
struct is_queue : std::false_type {};

template<typename T>
struct is_queue<std::queue<T>> {
	using type = T;
	static constexpr bool value{ true };
	constexpr operator bool() const noexcept { return true; }
	constexpr bool operator()() const noexcept { return true; }
};

/** is_deque */

template<typename T>
struct is_deque : std::false_type {};

template<typename T>
struct is_deque<std::deque<T>> {
	using type = T;
	static constexpr bool value{ true };
	constexpr operator bool() const noexcept { return true; }
	constexpr bool operator()() const noexcept { return true; }
};

/** is_set */

template<typename T>
struct is_set : std::false_type {};

template<typename T>
struct is_set<std::set<T>> {
	using type = T;
	static constexpr bool value{ true };
	constexpr operator bool() const noexcept { return true; }
	constexpr bool operator()() const noexcept { return true; }
};

/** is_multiset */

template<typename T>
struct is_multiset : std::false_type {};

template<typename T>
struct is_multiset<std::multiset<T>> {
	using type = T;
	static constexpr bool value{ true };
	constexpr operator bool() const noexcept { return true; }
	constexpr bool operator()() const noexcept { return true; }
};

/** is_unordered_set */

template<typename T>
struct is_unordered_set : std::false_type {};

template<typename T>
struct is_unordered_set<std::unordered_set<T>> {
	using type = T;
	static constexpr bool value{ true };
	constexpr operator bool() const noexcept { return true; }
	constexpr bool operator()() const noexcept { return true; }
};

/** is_unordered_multiset */

template<typename T>
struct is_unordered_multiset : std::false_type {};

template<typename T>
struct is_unordered_multiset<std::unordered_multiset<T>> {
	using type = T;
	static constexpr bool value{ true };
	constexpr operator bool() const noexcept { return true; }
	constexpr bool operator()() const noexcept { return true; }
};

/** is_map */

template<typename T>
struct is_map : std::false_type {};

template<typename KeyT, typename ValueT, typename CompareT, typename AllocatorT>
struct is_map<std::map<KeyT, ValueT, CompareT, AllocatorT>> {
	using key_type = KeyT;
	using value_type = ValueT;
	static constexpr bool value{ true };
	constexpr operator bool() const noexcept { return true; }
	constexpr bool operator()() const noexcept { return true; }
};

/** is_multimap */

template<typename T>
struct is_multimap : std::false_type {};

template<typename KeyT, typename ValueT, typename CompareT, typename AllocatorT>
struct is_multimap<std::multimap<KeyT, ValueT, CompareT, AllocatorT>> {
	using key_type = KeyT;
	using value_type = ValueT;
	static constexpr bool value{ true };
	constexpr operator bool() const noexcept { return true; }
	constexpr bool operator()() const noexcept { return true; }
};

/** is_unordered_map */

template<typename T>
struct is_unordered_map : std::false_type {};

template<typename KeyT, typename ValueT, typename HashT, typename KeyEqualT, typename AllocatorT>
struct is_unordered_map<std::unordered_map<KeyT, ValueT, HashT, KeyEqualT, AllocatorT>> {
	using key_type = KeyT;
	using value_type = ValueT;
	static constexpr bool value{ true };
	constexpr operator bool() const noexcept { return true; }
	constexpr bool operator()() const noexcept { return true; }
};

template<typename T>
struct is_unordered_multimap : std::false_type {};

template<typename KeyT, typename ValueT, typename HashT, typename KeyEqualT, typename AllocatorT>
struct is_unordered_multimap<std::unordered_multimap<KeyT, ValueT, HashT, KeyEqualT, AllocatorT>> {
	using key_type = KeyT;
	using value_type = ValueT;
	static constexpr bool value{ true };
	constexpr operator bool() const noexcept { return true; }
	constexpr bool operator()() const noexcept { return true; }
};

/** is_associative_container */

template<typename T>
struct is_associative_container : std::false_type {};

template<typename KeyT, typename ValueT, typename CompareT, typename AllocatorT>
struct is_associative_container<std::map<KeyT, ValueT, CompareT, AllocatorT>> {
	using key_type = KeyT;
	using value_type = ValueT;
	static constexpr bool value{ true };
	constexpr operator bool() const noexcept { return true; }
	constexpr bool operator()() const noexcept { return true; }
};

template<typename KeyT, typename ValueT, typename HashT, typename KeyEqualT, typename AllocatorT>
struct is_associative_container<std::unordered_map<KeyT, ValueT, HashT, KeyEqualT, AllocatorT>> {
	using key_type = KeyT;
	using value_type = ValueT;
	static constexpr bool value{ true };
	constexpr operator bool() const noexcept { return true; }
	constexpr bool operator()() const noexcept { return true; }
};

/** is_sequence_container */

template<typename T>
struct is_sequence_container : std::false_type {};

template<typename T>
struct is_sequence_container<std::vector<T>> {
	using type = T;
	static constexpr bool value{ true };
	constexpr operator bool() const noexcept { return true; }
	constexpr bool operator()() const noexcept { return true; }
};

template<typename T>
struct is_sequence_container<std::list<T>> {
	using type = T;
	static constexpr bool value{ true };
	constexpr operator bool() const noexcept { return true; }
	constexpr bool operator()() const noexcept { return true; }
};

template<typename T>
struct is_sequence_container<std::forward_list<T>> {
	using type = T;
	static constexpr bool value{ true };
	constexpr operator bool() const noexcept { return true; }
	constexpr bool operator()() const noexcept { return true; }
};

// Sets are really associative containers, not sequence...
template<typename T>
struct is_sequence_container<std::set<T>> {
	using type = T;
	static constexpr bool value{ true };
	constexpr operator bool() const noexcept { return true; }
	constexpr bool operator()() const noexcept { return true; }
};

template<typename T>
struct is_sequence_container<std::multiset<T>> {
	using type = T;
	static constexpr bool value{ true };
	constexpr operator bool() const noexcept { return true; }
	constexpr bool operator()() const noexcept { return true; }
};

template<typename T>
struct is_sequence_container<std::unordered_set<T>> {
	using type = T;
	static constexpr bool value{ true };
	constexpr operator bool() const noexcept { return true; }
	constexpr bool operator()() const noexcept { return true; }
};

template<typename T>
struct is_sequence_container<std::unordered_multiset<T>> {
	using type = T;
	static constexpr bool value{ true };
	constexpr operator bool() const noexcept { return true; }
	constexpr bool operator()() const noexcept { return true; }
};

/**
 * Push helper for pushing key/value pairs to arbitrary container types
 *
 * If ContainerT is associative: set key/value
 * If ContainerT is multi-associative: add key/value
 * If ContainerT is sequential: push key/value pair to back
 */
namespace container {
/** Pushing to associative containers */
template<typename ContainerT, typename LeftT, typename RightT,
	typename std::enable_if_t<is_map<ContainerT>::value || is_unordered_map<ContainerT>::value>* = nullptr>
constexpr void push(ContainerT& inout_container, LeftT&& in_key, RightT&& in_value) {
	inout_container[in_key] = in_value;
}

template<typename ContainerT, typename LeftT, typename RightT,
	typename std::enable_if_t<is_set<ContainerT>::value || is_unordered_set<ContainerT>::value>* = nullptr>
constexpr void push(ContainerT& inout_container, LeftT&& in_key, RightT&& in_value) {
	auto insert_result = inout_container.insert({in_key, in_value});
	if (!insert_result) {
		*insert_result.first = { in_key, in_value };
	}
}

template<typename ContainerT, typename LeftT, typename RightT,
	typename std::enable_if_t<is_multimap<ContainerT>::value || is_unordered_multimap<ContainerT>::value
	|| is_multiset<ContainerT>::value || is_unordered_multiset<ContainerT>::value>* = nullptr>
constexpr void push(ContainerT& inout_container, LeftT&& in_key, RightT&& in_value) {
	inout_container.insert({in_key, in_value});
}

/** Pushing to sequential containers */
template<typename ContainerT, typename LeftT, typename RightT,
	typename std::enable_if_t<is_vector<ContainerT>::value || is_list<ContainerT>::value || is_deque<ContainerT>::value>* = nullptr>
constexpr void push(ContainerT& inout_container, LeftT&& in_key, RightT&& in_value) {
	inout_container.push_back({in_key, in_value});
}

template<typename ContainerT, typename LeftT, typename RightT,
	typename std::enable_if_t<is_queue<ContainerT>::value || is_stack<ContainerT>::value>* = nullptr>
constexpr void push(ContainerT& inout_container, LeftT&& in_key, RightT&& in_value) {
	inout_container.push({in_key, in_value});
}

template<typename ContainerT, typename LeftT, typename RightT,
	typename std::enable_if_t<is_forward_list<ContainerT>::value>* = nullptr>
constexpr void push(ContainerT& inout_container, LeftT&& in_key, RightT&& in_value) {
	inout_container.push_front({in_key, in_value});
}

} // namespace container
} // namespace jessilib
