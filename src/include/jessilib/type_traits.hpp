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
#include <vector>
#include <list>
#include <forward_list>
#include <set>
#include <unordered_set>
#include <map>
#include <unordered_map>
#include <string>
#include <string_view>

namespace jessilib {

/** remove_cvref (can be replaced with C++20) */

template<class T>
struct remove_cvref {
	typedef std::remove_cv_t<std::remove_reference_t<T>> type;
};

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

template<typename KeyT, typename ValueT>
struct is_map<std::map<KeyT, ValueT>> {
	using key_type = KeyT;
	using value_type = ValueT;
	static constexpr bool value{ true };
	constexpr operator bool() const noexcept { return true; }
	constexpr bool operator()() const noexcept { return true; }
};

/** is_unordered_map */

template<typename T>
struct is_unordered_map : std::false_type {};

template<typename KeyT, typename ValueT>
struct is_unordered_map<std::unordered_map<KeyT, ValueT>> {
	using key_type = KeyT;
	using value_type = ValueT;
	static constexpr bool value{ true };
	constexpr operator bool() const noexcept { return true; }
	constexpr bool operator()() const noexcept { return true; }
};

/** is_associative_container */

template<typename T>
struct is_associative_container : std::false_type {};

template<typename KeyT, typename ValueT>
struct is_associative_container<std::map<KeyT, ValueT>> {
	using key_type = KeyT;
	using value_type = ValueT;
	static constexpr bool value{ true };
	constexpr operator bool() const noexcept { return true; }
	constexpr bool operator()() const noexcept { return true; }
};

template<typename KeyT, typename ValueT>
struct is_associative_container<std::unordered_map<KeyT, ValueT>> {
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

} // namespace jessilib
