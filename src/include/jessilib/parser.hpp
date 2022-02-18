/**
 * Copyright (C) 2018-2021 Jessica James.
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

#include <memory>
#include "object.hpp"
#include "text_encoding.hpp"
#include "impl/parser_manager.hpp"

namespace jessilib {

class parser {
public:
	virtual ~parser() = default;
	using byte_type = char;
	using bytes_view_type = std::basic_string_view<byte_type>;

	/** Interface methods */

	/**
	 * Deserializes an object directly from a stream of bytes
	 * May throw: invalid_argument
	 *
	 * @param in_stream Stream to deserialize object from
	 * @return A valid (possibly null) object
	 */
	virtual object deserialize_bytes(std::istream& in_stream, text_encoding in_read_encoding);
	virtual object deserialize_bytes(bytes_view_type in_data, text_encoding in_read_encoding) = 0;
	virtual void serialize_bytes(std::ostream& in_stream, const object& in_object, text_encoding in_write_encoding);
	virtual std::string serialize_bytes(const object& in_object, text_encoding in_write_encoding) = 0;

	template<typename CharT>
	object deserialize(std::basic_string_view<CharT> in_text) {
		bytes_view_type byte_view{ reinterpret_cast<const byte_type*>(in_text.data()), in_text.size() * sizeof(CharT) };
		return deserialize_bytes(byte_view, default_encoding_info<CharT>::encoding);
	}

	// Perhaps this could be condensed down to a simple method such that: serialize(out_variant, in_object, encoding)?
	virtual std::u8string serialize_u8(const object& in_object) = 0;
	virtual std::u16string serialize_u16(const object& in_object) = 0;
	virtual std::u32string serialize_u32(const object& in_object) = 0;
	virtual std::wstring serialize_w(const object& in_object) = 0;

	template<typename CharT>
	std::basic_string<CharT> serialize(const object& in_object) {
		if constexpr (std::is_same_v<CharT, char8_t>) {
			return serialize_u8(in_object);
		}
		else if constexpr (std::is_same_v<CharT, char16_t>) {
			return serialize_u16(in_object);
		}
		else if constexpr (std::is_same_v<CharT, char32_t>) {
			return serialize_u32(in_object);
		}
		else if constexpr (std::is_same_v<CharT, wchar_t>) {
			return serialize_w(in_object);
		}
	}
}; // parser

template<typename T>
class parser_registration {
public:
	parser_registration(std::string in_format, bool in_force = false) {
		m_id = impl::parser_manager::instance().register_parser(std::make_shared<T>(), in_format, in_force);
	}

	~parser_registration() {
		impl::parser_manager::instance().unregister_parser(m_id);
	}

	impl::parser_manager::id m_id;
}; // parser_registration

template<typename OutCharT, typename ResultCharT = OutCharT>
std::basic_string<ResultCharT> simple_copy(std::u8string_view in_string) {
	if constexpr (sizeof(OutCharT) == sizeof(ResultCharT)) {
		return { in_string.begin(), in_string.end() };
	}
	else if constexpr (std::is_same_v<ResultCharT, char>) {
		// Copy in_string into result _as if_ result were of OutCharT
		for (OutCharT codepoint : in_string) {
			// TODO: Assuming native for now, but we need to account for endianness later
			return { reinterpret_cast<const char*>(&codepoint), sizeof(codepoint) };
		}
	}
	// else // Invalid use of simple_copy
}

template<typename OutCharT, typename ResultCharT>
void simple_append(std::basic_string<ResultCharT>& out_string, std::u8string_view in_string) {
	if constexpr (sizeof(OutCharT) == sizeof(ResultCharT)) {
		out_string.append(in_string.begin(), in_string.end());
	}
	else if constexpr (std::is_same_v<ResultCharT, char>) {
		// Copy in_string into result _as if_ result were of OutCharT
		for (OutCharT codepoint : in_string) {
			// TODO: Assuming native for now, but we need to account for endianness later
			out_string.append(reinterpret_cast<const char*>(&codepoint), sizeof(codepoint));
		}
	}
	// else // Invalid use of simple_append
}

template<typename OutCharT, typename ResultCharT>
void simple_append(std::basic_string<ResultCharT>& out_string, char8_t in_character) {
	if constexpr (sizeof(OutCharT) == sizeof(ResultCharT)) {
		out_string += in_character;
	}
	else if constexpr (std::is_same_v<ResultCharT, char>) {
		// Copy in_character into result _as if_ result were of OutCharT
		OutCharT codepoint = in_character;
		// TODO: Assuming native for now, but we need to account for endianness later
		out_string.append(reinterpret_cast<const char*>(&codepoint), sizeof(codepoint));
	}
}

} // namespace jessilib
