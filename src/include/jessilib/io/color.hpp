/**
 * Copyright (C) 2020-2021 Jessica James.
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

#include <cmath>
#include <vector>
#include <string>
#include <string_view>
#include <fmt/format.h>
#include "jessilib/util.hpp"

namespace jessilib {
namespace io {

class color {
public:
	// Constructors
	constexpr color() {
		// Empty ctor body
	}

	constexpr color(uint32_t in_value)
		: m_value{ in_value } {
		// Empty ctor body
	}

	constexpr color(uint8_t in_red, uint8_t in_green, uint8_t in_blue)
		: m_value{ static_cast<uint32_t>(in_red << 16) | in_green << 8 | in_blue } {
		// Empty ctor body
	}

	constexpr color(const color&) = default;
	constexpr color(color&&) = default;

	// Methods

	constexpr uint8_t red() const {
		return (m_value & 0xFF0000) >> 16;
	}

	constexpr uint8_t green() const {
		return (m_value & 0xFF00) >> 8;
	}

	constexpr uint8_t blue() const {
		return m_value & 0xFF;
	}

	constexpr uint32_t value() const {
		return m_value;
	}

	constexpr operator uint32_t() const {
		return m_value;
	}

	double distance(const color& in_color) const {
		return std::sqrt(distance_sq(in_color));
	}

	constexpr uint32_t distance_sq(const color& in_color) const {
		return square(red() - in_color.red()) + square(green() - in_color.green()) + square(blue() - in_color.blue());
	}

	// Assignment operators
	constexpr color& operator=(const color&) = default;
	constexpr color& operator=(color&&) = default;

	// Comparison operators
	bool operator==(const color& rhs) const {
		return m_value == rhs.m_value;
	}

	bool operator!=(const color& rhs) const {
		return m_value != rhs.m_value;
	}

	bool operator<(const color& rhs) const {
		return m_value < rhs.m_value;
	}

	bool operator>(const color& rhs) const {
		return m_value >= rhs.m_value;
	}

	bool operator<=(const color& rhs) const {
		return m_value <= rhs.m_value;
	}

	bool operator>=(const color& rhs) const {
		return m_value >= rhs.m_value;
	}

private:
	uint32_t m_value{};
};

} // namespace io
} // namespace jessilib
