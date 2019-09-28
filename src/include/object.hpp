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
	using array_t = std::vector<object>;
	using map_t = std::map<std::string, object>;

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
	struct is_backing<T, typename std::enable_if<std::is_same<T, std::string>::value>::type> {
		static constexpr bool value = true;
		using type = std::string;
	};

	template<typename T>
	struct is_backing<T, typename std::enable_if<is_sequence_container<T>::value>::type> {
		static constexpr bool value = true;
		using type = array_t;
	};

	template<typename T>
	struct is_backing<T, typename std::enable_if<std::is_same<T, map_t>::value>::type> {
		static constexpr bool value = true;
		using type = map_t;
	};

	/** type */

	enum class type : size_t {
		null = 0,
		boolean,
		integer,
		decimal,
		string, // TODO: consider separating into 'binary' (std::vector<std::byte>) and 'text' (std::string) types
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
		&& !is_sequence_container<typename std::decay<T>::type>::value>::type* = nullptr>
	object(T&& in_value)
		: m_value{ typename is_backing<typename std::decay<T>::type>::type{ std::forward<T>(in_value) } } {
		// Empty ctor body
	}

	template<typename T,
		typename std::enable_if<is_sequence_container<typename std::decay<T>::type>::value
		&& !std::is_same<typename std::decay<T>::type, std::vector<bool>>::value>::type* = nullptr>
	object(T&& in_value)
		: m_value{ array_t{ in_value.begin(), in_value.end() } } {
		// Empty ctor body
	}

	// std::vector<bool>
	template<typename T,
		typename std::enable_if<std::is_same<typename std::decay<T>::type, std::vector<bool>>::value>::type* = nullptr>
	object(T&& in_value)
		: m_value{ array_t{} } {
		auto& array = std::get<array_t>(m_value);
		array.reserve(in_value.size());

		for (const auto& item : in_value) {
			array.emplace_back(bool{item});
		}
	}

	object(const char* in_str);
	object(const std::string_view& in_str);

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

	const object& operator[](const std::string& in_key) const;
	object& operator[](const std::string& in_key);

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
		typename std::enable_if<std::is_same<T, std::string>::value && std::is_convertible<typename std::decay<DefaultT>::type, T>::value>::type* = nullptr>
	T get(DefaultT&& in_default_value = {}) const {
		const std::string* result = std::get_if<std::string>(&m_value);
		if (result != nullptr) {
			return *result;
		}

		return std::forward<DefaultT>(in_default_value);
	}

	// TODO: support other basic_string_view types
	template<typename T, typename DefaultT = T,
		typename std::enable_if<std::is_same<T, std::string>::value && std::is_same<typename std::decay<DefaultT>::type, std::string_view>::value>::type* = nullptr>
	T get(DefaultT&& in_default_value) const {
		const std::string* result = std::get_if<std::string>(&m_value);
		if (result != nullptr) {
			return *result;
		}

		return { in_default_value.begin(), in_default_value.end() };
	}

	/** arrays */

	// reference getter (array_t)
	template<typename T,
		typename std::enable_if<std::is_same<T, array_t>::value>::type* = nullptr>
	const T& get(const T& in_default_value) const {
		const array_t* result = std::get_if<array_t>(&m_value);
		if (result != nullptr) {
			return *result;
		}

		return in_default_value;
	}

	// copy getter (array_t)
	template<typename T,
		typename std::enable_if<std::is_same<T, array_t>::value>::type* = nullptr>
	T get(T&& in_default_value = {}) const {
		const array_t* result = std::get_if<array_t>(&m_value);
		if (result != nullptr) {
			return *result;
		}

		return std::move(in_default_value);
	}

	// conversion getter (non-array_t)
	template<typename T, typename DefaultT = T,
		typename std::enable_if<is_sequence_container<T>::value && !std::is_same<T, array_t>::value && std::is_same<typename std::decay<DefaultT>::type, T>::value>::type* = nullptr>
	T get(DefaultT&& in_default_value = {}) const {
		using backing_t = typename is_sequence_container<T>::type;

		const array_t* array = std::get_if<array_t>(&m_value);
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

	// TODO: implement in a way that does not require exposing map_t

	// reference getter (map_t)
	template<typename T,
		typename std::enable_if<std::is_same<T, map_t>::value>::type* = nullptr>
	const T& get(const T& in_default_value) const {
		const map_t* result = std::get_if<map_t>(&m_value);
		if (result != nullptr) {
			return *result;
		}

		return in_default_value;
	}

	// copy getter (map_t)
	template<typename T,
		typename std::enable_if<std::is_same<T, map_t>::value>::type* = nullptr>
	T get(T&& in_default_value = {}) const {
		const map_t* result = std::get_if<map_t>(&m_value);
		if (result != nullptr) {
			return *result;
		}

		return std::move(in_default_value);
	}

	// TODO: conversion getter (non-map_t, i.e: unordered_map)

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
		typename std::enable_if<std::is_convertible<typename std::decay<T>::type, std::string>::value>::type* = nullptr>
	void set(T&& in_value) {
		m_value.emplace<std::string>(std::forward<T>(in_value));
	}

	// string_view
	template<typename T,
		typename std::enable_if<std::is_same<T, std::string_view>::value>::type* = nullptr>
	void set(const T& in_value) {
		m_value.emplace<std::string>(in_value.begin(), in_value.end());
	}

	// array_t
	template<typename T,
		typename std::enable_if<std::is_same<typename std::decay<T>::type, array_t>::value>::type* = nullptr>
	void set(T&& in_value) {
		m_value.emplace<array_t>(std::forward<T>(in_value));
	}

	// is_sequence_container
	template<typename T,
		typename std::enable_if<is_sequence_container<typename std::decay<T>::type>::value
		&& !std::is_same<typename std::decay<T>::type, array_t>::value
		&& !std::is_same<typename std::decay<T>::type, std::vector<bool>>::value>::type* = nullptr>
	void set(T&& in_value) {
		m_value.emplace<array_t>(in_value.begin(), in_value.end());
	}

	// std::vector<bool>
	template<typename T,
		typename std::enable_if<std::is_same<typename std::decay<T>::type, std::vector<bool>>::value>::type* = nullptr>
	void set(T&& in_value) {
		auto& array = m_value.emplace<array_t>();
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
		return std::visit([this](auto&& value) -> size_t {
			using T = typename std::decay<decltype(value)>::type;

			if constexpr (std::is_same<T, null_variant_t>::value) {
				return 0;
			}
			else if constexpr (std::is_same<T, array_t>::value) {
				size_t result{};
				for (auto& obj : value) {
					result += obj.hash();
				}

				return result;
			}
			else if constexpr (std::is_same<T, map_t>::value) {
				size_t result{};
				for (auto& pair : value) {
					result += std::hash<std::string>{}(pair.first);
				}

				return result;
			}
			else {
				return std::hash<T>{}(std::forward<decltype(value)>(value));
			}
		}, m_value);
	}

private:
	using null_variant_t = void*;
	std::variant<null_variant_t, bool, intmax_t, long double, std::string, array_t, map_t> m_value;
}; // object

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
