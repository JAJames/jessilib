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

#pragma once

#include <vector>
#include <string>
#include <string_view>
#include <type_traits>
#include <fmt/format.h>
#include "color.hpp"

namespace jessilib {
namespace io {

class text {
public:
	/** Types */

	using property_backing_t = unsigned int;
	enum class property : property_backing_t {
		normal = 0x00,
		bold = 0x01,
		italic = 0x02,
		underline = 0x04,
		strikethrough = 0x08,
		colored = 0x10, // foreground
		colored_bg = 0x20, // background
		colord_fg_bg = 0x30 // foreground + background
	};

	/** Constructors */

	text() = default;

	template<typename StringT,
	    typename std::enable_if<!std::is_base_of<text, typename std::remove_reference<StringT>::type>::value>::type* = nullptr>
	text(StringT&& in_text)
		: m_string{ std::forward<StringT>(in_text) } {
		// Empty ctor body
	}

	template<typename StringT>
	text(StringT&& in_text, property in_properties)
		: m_string{ std::forward<StringT>(in_text) },
		m_properties{ in_properties } {
		// Empty ctor body
	}

	template<typename StringT>
	text(StringT&& in_text, color in_color)
		: m_string{ std::forward<StringT>(in_text) },
		m_properties{ property::colored },
		m_color{ in_color } {
		// Empty ctor body
	}

	template<typename StringT>
	text(StringT&& in_text, property in_properties, color in_color)
		: m_string{ std::forward<StringT>(in_text) },
		m_properties{ static_cast<property_backing_t>(in_properties) | static_cast<property_backing_t>(property::colored) },
		m_color{ in_color } {
		// Empty ctor body
	}

	template<typename StringT>
	text(StringT&& in_text, color in_color, color in_color_bg)
		: m_string{ std::forward<StringT>(in_text) },
		m_properties{ property::colord_fg_bg },
		m_color{ in_color },
		m_color_bg{ in_color_bg } {
		// Empty ctor body
	}

	template<typename StringT>
	text(StringT&& in_text, property in_properties, color in_color, color in_color_bg)
		: m_string{ std::forward<StringT>(in_text) },
		m_properties{ static_cast<property_backing_t>(in_properties) | static_cast<property_backing_t>(property::colord_fg_bg) },
		m_color{ in_color },
		m_color_bg{ in_color_bg } {
		// Empty ctor body
	}

	text(const text&) = default;
	text(text&&) = default;
	text& operator=(const text&) = default;
	text& operator=(text&&) = default;

	/** Properties */
	property properties() const;
	bool has_property(property in_property) const;
	void set_property(property in_property);
	void unset_property(property in_property);

	/** Colors */
	color get_color() const;
	void set_color(color in_color);
	color get_color_bg() const;
	void set_color_bg(color in_color);

	/** Text */
	const std::string& string() const;
	void set_string(std::string_view in_string);

private:
	std::string m_string;
	property m_properties{ property::normal };
	color m_color{};
	color m_color_bg{};
};

/**
 * message class consisting of a series of text segments that are then combined (or otherwise output in sequence).
 *
 * This class is necessary to facilitate colored messages and other properties that will be serialized differently for
 * differing outputs (i.e: IRC uses color codes, consoles are platform-specific, etc).
 */
class message {
public:
	template<typename... Args>
	message(Args&& ... args)
		: m_message{ std::forward<Args>(args)... } {
		// Empty ctor body
	}

	const std::vector<text>& get_message() const {
		return m_message;
	}

private:
	std::vector<text> m_message;
};

/**
 * formatted_message class similar to the message class, except that a format is specified to determine order of segments
 *
 * See fmtlib for details on the expected format syntax
 */
class formatted_message {
public:
	template<typename... Args>
	formatted_message(std::string in_format, Args&& ... args)
		: m_format{ std::move(in_format) },
		m_message{ std::forward<Args>(args)... } {
		// Empty ctor body
	}

	const std::string& format() const {
		return m_format;
	}

	const std::vector<text>& get_message() const {
		return m_message;
	}

private:
	std::string m_format;
	std::vector<text> m_message;
};

template<typename WrapperT>
std::string text_to_string(const WrapperT& in_text) {
	return in_text.string();
}

template<typename WrapperT,
    typename std::enable_if<std::is_base_of<text, WrapperT>::value && sizeof(text) == sizeof(WrapperT)>::type* = nullptr>
const WrapperT& wrap_text(const text& in_text) {
	// WrapperT extends text, which is non virtual (so the vtables must be the same) and the size is the same (so members must be the same)
	// Thus, a simple upcast should be safe
	return static_cast<const WrapperT&>(in_text);
}

template<typename WrapperT,
	typename std::enable_if<!std::is_base_of<text, WrapperT>::value || sizeof(text) != sizeof(WrapperT)>::type* = nullptr>
WrapperT wrap_text(const text& in_text) {
	// WrapperT either doesn't extend text, or adds additional members.
	// Attempt to construct WrapperT with in_text
	return { in_text };
}

template<typename WrapperT>
std::string process_message(const jessilib::io::message& msg) {
	std::string result;

	// Concatenate text portions together
	for (auto& text : msg.get_message()) {
		result += text_to_string(wrap_text<WrapperT>(text));
	}

	// Return result
	return result;
}

template<typename WrapperT>
std::string process_message(const jessilib::io::formatted_message& msg) {
	using format_arg = fmt::format_args::format_arg;
	std::vector<format_arg> args;

	// Populate args
	for (auto& text : msg.get_message()) {
		args.emplace_back(fmt::internal::make_arg<fmt::format_context>(wrap_text<WrapperT>(text)));
	}

	// Pass args into vformat
	fmt::format_args text_args{ args.data(), args.size() };
	return fmt::vformat(msg.format(), text_args);
}

} // namespace io
} // namespace jessilib
