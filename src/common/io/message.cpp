/**
 * Copyright (C) 2019 Jessica James.
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

#include "io/message.hpp"

namespace jessilib {
namespace io {

text::property text::properties() const {
	return m_properties;
}

bool text::has_property(property in_property) const {
	if (in_property == property::normal) {
		return m_properties == property::normal;
	}

	return static_cast<property_backing_t>(m_properties) & static_cast<property_backing_t>(in_property);
}

void text:: set_property(property in_property) {
	m_properties = static_cast<property>(static_cast<property_backing_t>(m_properties)
		| static_cast<property_backing_t>(in_property));
}

void text::unset_property(property in_property) {
	m_properties = static_cast<property>(static_cast<property_backing_t>(m_properties)
		& ~static_cast<property_backing_t>(in_property));
}

/** Colors */

color text::get_color() const {
	return m_color;
}

void text::set_color(color in_color) {
	m_color = in_color;
	set_property(property::colored);
}

color text::get_color_bg() const {
	return m_color_bg;
}

void text::set_color_bg(color in_color) {
	m_color_bg = in_color;
	set_property(property::colored_bg);
}

/** Text */

const std::string& text::string() const {
	return m_string;
}

void text::set_string(std::string_view in_string) {
	m_string = in_string;
}

} // namespace io
} // namespace jessilib
