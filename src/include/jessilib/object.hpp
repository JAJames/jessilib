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

#include <cstdint>
#include <string>
#include <vector>
#include <map>
#include <variant>
#include <utility>
#include "type_traits.hpp"

namespace jessilib {

class object {
public:
	using array_type = std::vector<object>;
	using text_char_type = char8_t;
	using text_type = std::basic_string<text_char_type>;
	using text_view_type = std::basic_string_view<text_char_type>;
	using data_type = std::vector<unsigned char>;
	using string_type = text_type;
	using string_view_type = text_view_type;
	using map_type = std::map<string_type, object>;
	using index_type = std::size_t;

	/** is_backing */

	template<typename T, typename enable = void>
	struct is_backing {
		static constexpr bool value = false;
	};

	template<typename T>
	struct is_backing<T, typename std::enable_if<std::is_same<T, bool>::value>::type> {
		static constexpr bool value = true;
		using type = bool;
	};

	template<typename T>
	struct is_backing<T, typename std::enable_if<std::is_integral<T>::value && !std::is_same<T, bool>::value>::type> {
		static constexpr bool value = true;
		using type = intmax_t;
	};

	template<typename T>
	struct is_backing<T, typename std::enable_if<std::is_floating_point<T>::value>::type> {
		static constexpr bool value = true;
		using type = long double;
	};

	template<typename T>
	struct is_backing<T, typename std::enable_if<std::is_same<T, string_type>::value>::type> {
		static constexpr bool value = true;
		using type = string_type;
	};

	template<typename T>
	struct is_backing<T, typename std::enable_if<is_sequence_container<T>::value>::type> {
		static constexpr bool value = true;
		using type = array_type;
	};

	template<typename T>
	struct is_backing<T, typename std::enable_if<is_associative_container<T>::value>::type> {
		static constexpr bool value = std::is_same<typename is_associative_container<T>::key_type, string_type>::value;
		using type = map_type;
	};

	/** type */

	enum class type : size_t {
		null = 0,
		boolean,
		integer,
		decimal,
		text,
		data,
		array,
		map
	};

	// Standard constructors
	object() = default;
	object(const object& in_config);
	object(object&& in_config);
	~object() = default;

	// Value constructors
	template<typename T,
		typename std::enable_if<is_backing<typename std::decay<T>::type>::value
		&& !is_sequence_container<typename std::decay<T>::type>::value
		&& (!is_associative_container<typename std::decay<T>::type>::value || std::is_same<typename std::remove_cvref<T>::type, map_type>::value)>::type* = nullptr>
	object(T&& in_value)
		: m_value{ typename is_backing<typename std::decay<T>::type>::type{ std::forward<T>(in_value) } } {
		// Empty ctor body
	}

	template<typename T,
		typename std::enable_if<is_sequence_container<typename std::decay<T>::type>::value
		&& !std::is_same<typename std::decay<T>::type, std::vector<bool>>::value>::type* = nullptr>
	object(T&& in_value)
		: m_value{ array_type{ in_value.begin(), in_value.end() } } {
		// Empty ctor body
	}

	// std::vector<bool>
	template<typename T,
		typename std::enable_if<std::is_same<typename std::decay<T>::type, std::vector<bool>>::value>::type* = nullptr>
	object(T&& in_value)
		: m_value{ array_type{} } {
		auto& array = std::get<array_type>(m_value);
		array.reserve(in_value.size());

		for (const auto& item : in_value) {
			array.emplace_back(bool{item});
		}
	}

	// std::unordered_map<string_type, object>
	template<typename T,
		typename std::enable_if<is_unordered_map<typename std::decay<T>::type>::value
			&& std::is_same<typename is_unordered_map<typename std::decay<T>::type>::key_type, string_type>::value
			&& std::is_same<typename is_unordered_map<typename std::decay<T>::type>::value_type, object>::value>::type* = nullptr>
	object(T&& in_value)
		: m_value{ map_type{ in_value.begin(), in_value.end() } } {
		// Empty ctor body
	}

	// Non-map_type associative containers (container<string_type, T>)
	template<typename T,
		typename std::enable_if<is_associative_container<typename std::remove_cvref<T>::type>::value
			&& (std::is_convertible<typename is_associative_container<typename std::remove_cvref<T>::type>::key_type, string_type>::value
			|| std::is_convertible<typename is_associative_container<typename std::remove_cvref<T>::type>::key_type, string_view_type>::value)
			&& !std::is_same<typename is_associative_container<typename std::remove_cvref<T>::type>::value_type, object>::value>::type* = nullptr>
	object(T&& in_value)
		: m_value{ map_type{} } {
		auto& map = std::get<map_type>(m_value);
		for (auto& pair : in_value) {
			map.emplace(pair.first, pair.second);
		}
	}

	object(const text_char_type* in_str);
	object(const string_view_type& in_str);

	// Comparison operators
	bool operator==(const object& rhs) const {
		return m_value == rhs.m_value;
	}

	bool operator!=(const object& rhs) const {
		return m_value != rhs.m_value;
	}

	bool operator<(const object& rhs) const {
		return m_value < rhs.m_value;
	}

	bool operator>(const object& rhs) const {
		return m_value > rhs.m_value;
	}

	bool operator<=(const object& rhs) const {
		return m_value <= rhs.m_value;
	}

	bool operator>=(const object& rhs) const {
		return m_value >= rhs.m_value;
	}

	// Assignment operators
	object& operator=(const object& in_config) = default;
	object& operator=(object&& in_config) = default;

	// Non-copy/move assignment operator; forwards to set()
	template<typename T,
		typename std::enable_if<!std::is_same<typename std::decay<T>::type, object>::value>::type* = nullptr>
	object& operator=(T&& in) {
		set(std::forward<T>(in));
		return *this;
	}

	const object& operator[](const string_type& in_key) const;
	object& operator[](const string_type& in_key);
	const object& operator[](index_type in_index) const;
	object& operator[](index_type in_index);

	/** Accessors */

	bool null() const;
	size_t size() const;

	template<typename T>
	bool has() const {
		using backing_t = typename is_backing<T>::type;
		return std::holds_alternative<backing_t>(m_value);
	}

	type type() const {
		return static_cast<decltype(type::null)>(m_value.index());
	}

	/** arithmetic types (numbers, bool) */

	template<typename T,
		typename std::enable_if<std::is_arithmetic<T>::value>::type* = nullptr>
	T get(T in_default_value = {}) const {
		using backing_t = typename is_backing<T>::type;

		const backing_t* result = std::get_if<backing_t>(&m_value);
		if (result != nullptr) {
			return static_cast<T>(*result);
		}

		return in_default_value;
	}

	/** strings */

	// TODO: support other basic_string types
	template<typename T, typename DefaultT = T,
		typename std::enable_if<std::is_same<T, string_type>::value && std::is_convertible<typename std::decay<DefaultT>::type, T>::value>::type* = nullptr>
	T get(DefaultT&& in_default_value = {}) const {
		const string_type* result = std::get_if<string_type>(&m_value);
		if (result != nullptr) {
			return *result;
		}

		return std::forward<DefaultT>(in_default_value);
	}

	// TODO: support other basic_string_view types
	template<typename T, typename DefaultT = T,
		typename std::enable_if<std::is_same<T, string_type>::value && std::is_same<typename std::decay<DefaultT>::type, string_view_type>::value>::type* = nullptr>
	T get(DefaultT&& in_default_value) const {
		const string_type* result = std::get_if<string_type>(&m_value);
		if (result != nullptr) {
			return *result;
		}

		return { in_default_value.begin(), in_default_value.end() };
	}

	/** arrays */

	// reference getter (array_type)
	template<typename T,
		typename std::enable_if<std::is_same<T, array_type>::value>::type* = nullptr>
	const T& get(const T& in_default_value) const {
		const array_type* result = std::get_if<array_type>(&m_value);
		if (result != nullptr) {
			return *result;
		}

		return in_default_value;
	}

	// copy getter (array_type)
	template<typename T,
		typename std::enable_if<std::is_same<T, array_type>::value>::type* = nullptr>
	T get(T&& in_default_value = {}) const {
		const array_type* result = std::get_if<array_type>(&m_value);
		if (result != nullptr) {
			return *result;
		}

		return std::move(in_default_value);
	}

	// conversion getter (non-array_type)
	template<typename T, typename DefaultT = T,
		typename std::enable_if<is_sequence_container<T>::value && !std::is_same<T, array_type>::value && std::is_same<typename std::decay<DefaultT>::type, T>::value>::type* = nullptr>
	T get(DefaultT&& in_default_value = {}) const {
		using backing_t = typename is_sequence_container<T>::type;

		const array_type* array = std::get_if<array_type>(&m_value);
		if (array != nullptr) {
			T result;
			// Expand capacity to fit values (if possible)
			if constexpr (is_vector<T>::value) {
				result.reserve(array->size());
			}

			// Populate result from array
			if constexpr (is_forward_list<T>::value) {
				// forward_list
				if constexpr (std::is_same<backing_t, class object>::value) {
					// forward_list<object>
					result.insert_after(result.begin(), array->begin(), array->end());
				}
				else {
					for (auto itr = array->rend(); itr != array->rbegin(); ++itr) {
						if (itr->has<backing_t>()) {
							result.push_front(itr->get<backing_t>());
						}
					}
				}
			}
			else if constexpr (is_set<T>::value || is_multiset<T>::value || is_unordered_set<T>::value || is_unordered_multiset<T>::value) {
				// set, unordered_set, multiset, unordered_multiset
				if constexpr (std::is_same<backing_t, class object>::value) {
					// <object>
					result.insert(array->begin(), array->end());
				}
				else {
					for (auto& object : *array) {
						if (object.has<backing_t>()) {
							result.insert(object.get<backing_t>());
						}
					}
				}
			}
			else {
				// vector, list, etc
				for (auto& object : *array) {
					if constexpr (std::is_same<backing_t, class object>::value) {
						// <object>
						result.push_back(object);
					}
					else if (object.has<backing_t>()) {
						result.push_back(object.get<backing_t>());
					}
				}
			}

			return result;
		}

		return std::forward<DefaultT>(in_default_value);
	}

	/** maps */

	// TODO: implement in a way that does not require exposing map_type

	// reference getter (map_type)
	template<typename T,
		typename std::enable_if<std::is_same<T, map_type>::value>::type* = nullptr>
	const T& get(const T& in_default_value) const {
		const map_type* result = std::get_if<map_type>(&m_value);
		if (result != nullptr) {
			return *result;
		}

		return in_default_value;
	}

	// copy getter (map_type)
	template<typename T,
		typename std::enable_if<std::is_same<T, map_type>::value>::type* = nullptr>
	T get(T&& in_default_value = {}) const {
		const map_type* result = std::get_if<map_type>(&m_value);
		if (result != nullptr) {
			return *result;
		}

		return std::move(in_default_value);
	}

	// TODO: conversion getter (non-map_type, i.e: unordered_map)

	/** set */

	// arithmetic types
	template<typename T,
		typename std::enable_if<std::is_arithmetic<T>::value>::type* = nullptr>
	void set(T in_value) {
		using backing_t = typename is_backing<T>::type;

		m_value.emplace<backing_t>(in_value);
	}

	// string
	template<typename T,
		typename std::enable_if<std::is_convertible<typename std::decay<T>::type, string_type>::value>::type* = nullptr>
	void set(T&& in_value) {
		m_value.emplace<string_type>(std::forward<T>(in_value));
	}

	// string_view
	template<typename T,
		typename std::enable_if<std::is_same<T, string_view_type>::value>::type* = nullptr>
	void set(const T& in_value) {
		m_value.emplace<string_type>(in_value.begin(), in_value.end());
	}

	// array_type
	template<typename T,
		typename std::enable_if<std::is_same<typename std::decay<T>::type, array_type>::value>::type* = nullptr>
	void set(T&& in_value) {
		m_value.emplace<array_type>(std::forward<T>(in_value));
	}

	// is_sequence_container
	template<typename T,
		typename std::enable_if<is_sequence_container<typename std::decay<T>::type>::value
		&& !std::is_same<typename std::decay<T>::type, array_type>::value
		&& !std::is_same<typename std::decay<T>::type, std::vector<bool>>::value>::type* = nullptr>
	void set(T&& in_value) {
		m_value.emplace<array_type>(in_value.begin(), in_value.end());
	}

	// std::vector<bool>
	template<typename T,
		typename std::enable_if<std::is_same<typename std::decay<T>::type, std::vector<bool>>::value>::type* = nullptr>
	void set(T&& in_value) {
		auto& array = m_value.emplace<array_type>();
		array.reserve(in_value.size());

		for (const auto& item : in_value) {
			array.emplace_back(bool{item});
		}
	}

	// object
	template<typename T,
		typename std::enable_if<std::is_same<typename std::decay<T>::type, object>::value>::type* = nullptr>
	void set(T&& in_value) {
		operator=(std::forward<T>(in_value));
	}

	size_t hash() const {
		return std::visit([](auto&& value) -> size_t {
			using T = typename std::decay<decltype(value)>::type;

			if constexpr (std::is_same<T, null_variant_t>::value) {
				return 0;
			}
			else if constexpr (std::is_same<T, array_type>::value) {
				size_t result{};
				for (auto& obj : value) {
					result += obj.hash();
				}

				return result;
			}
			else if constexpr (std::is_same<T, map_type>::value) {
				size_t result{};
				for (auto& pair : value) {
					result += std::hash<string_type>{}(pair.first);
				}

				return result;
			}
			else if constexpr (std::is_same<T, data_type>::value) {
				// TODO: pull this into a separate method
				uint64_t hash = 14695981039346656037ULL;

				for (auto byte : value) {
					hash = hash ^ byte;
					hash = hash * 1099511628211ULL;
				}

				return hash;
			}
			else {
				return std::hash<T>{}(std::forward<decltype(value)>(value));
			}
		}, m_value);
	}

private:
	using null_variant_t = void*;
	// TODO: consider replacing std::string with std::u8string (for strings) & std::vector<unsigned char> (for data)
	// TODO: consider some more generic mechanism for underlying string type, to support utf-16 & utf-32 strings
	std::variant<null_variant_t, bool, intmax_t, long double, text_type, data_type, array_type, map_type> m_value;

	// TODO: note for future self, just use either first or last element in array_type to hold XML attributes
	// OR, have every XML tag objects be a map, with all subobjects being in a "__values" array subobject or such
	// For extra syntactical sugar, could have xml_object class extend object w/o additional members to allow arbitrary
	// static_cast usage
	// This may be a good justification for separate 'xml_config' and 'xml' parsers, as config files are easier to
	// represent as a map, whereas an actual xml document is sequenced
}; // object

namespace container {

template<typename ContainerT, typename LeftT, typename RightT,
	typename std::enable_if_t<std::is_same_v<ContainerT, object>>* = nullptr>
constexpr void push(ContainerT& inout_container, LeftT&& in_key, RightT&& in_value) {
	auto object_type = inout_container.type();
	if (object_type == object::type::null || object_type == object::type::map) {
		// Push to map if null or map type
		inout_container[in_key] = in_value;
	}
	else if (object_type == object::type::array) {
		// Push to back of array if array type
		inout_container[inout_container.size()][in_key] = in_value;
	}
	// else // do nothing; pushing a key/value pair isn't valid here
}

} // namespace container

} // namespace jessilib


namespace std {

template<>
struct hash<jessilib::object> {
	using argument_type = jessilib::object;
	using result_type = size_t;

	result_type operator()(const argument_type& in_object) const noexcept {
		return in_object.hash();
	}
};

} // namepsace std
