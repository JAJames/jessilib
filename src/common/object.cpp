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

#include "object.hpp"

namespace jessilib {

object::object(const object& in_object) {
	m_value = in_object.m_value;
}

object::object(object&& in_object) {
	m_value = std::move(in_object.m_value);
}

object::object(const char* in_str)
	: m_value{ string_type{ in_str } } {
	// Empty ctor body
}

object::object(const string_view_type& in_str)
	: m_value{ string_type{ in_str.begin(), in_str.end() } } {
	// Empty ctor body
}

/** Accessors */

bool object::null() const {
	return std::holds_alternative<null_variant_t>(m_value);
}

size_t object::size() const {
	// If we're null, we don't have any members
	if (null()) {
		return 0;
	}

	// Try array
	{
		const array_type* array = std::get_if<array_type>(&m_value);
		if (array != nullptr) {
			return array->size();
		}
	}

	// Try map
	{
		const map_type* map = std::get_if<map_type>(&m_value);
		if (map != nullptr) {
			return map->size();
		}
	}

	// We're a single value
	return 1;
}

const object& object::operator[](const string_type& in_key) const {
	auto map_ptr = std::get_if<map_type>(&m_value);
	if (map_ptr != nullptr) {
		auto itr = map_ptr->find(in_key);
		if (itr != map_ptr->end()) {
			return itr->second;
		}
	}

	static const object s_null_object;
	return s_null_object;
}

object& object::operator[](const string_type& in_key) {
	if (null()) {
		return m_value.emplace<map_type>()[in_key];
	}

	auto map_ptr = std::get_if<map_type>(&m_value);
	if (map_ptr != nullptr) {
		return map_ptr->operator[](in_key);
	}

	static thread_local object s_null_object;
	s_null_object.m_value.emplace<null_variant_t>();
	return s_null_object;
}

const object& object::operator[](index_type in_index) const {
	auto array_ptr = std::get_if<array_type>(&m_value);
	if (array_ptr != nullptr
		&& in_index < array_ptr->size()) {
		return array_ptr->at(in_index);
	}

	static const object s_null_object;
	return s_null_object;
}

object& object::operator[](index_type in_index) {
	if (null()) {
		m_value.emplace<array_type>();
	}

	auto array_ptr = std::get_if<array_type>(&m_value);
	if (array_ptr != nullptr) {
		while (array_ptr->size() <= in_index) {
			array_ptr->emplace_back();
		}

		return array_ptr->at(in_index);
	}

	static thread_local object s_null_object;
	s_null_object.m_value.emplace<null_variant_t>();
	return s_null_object;
}

} // namespace jessilib
