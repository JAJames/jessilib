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

#include "parsers/json.hpp"

namespace jessilib {

object json_parser::deserialize_bytes(bytes_view_type in_data, text_encoding in_write_encoding) {
	object result;

	if (in_write_encoding == text_encoding::utf_8) {
		std::u8string_view data_view = jessilib::string_view_cast<char8_t>(in_data);
		deserialize_json<char8_t, true>(result, data_view);
	}
	else if (in_write_encoding == text_encoding::utf_16) {
		std::u16string_view data_view = jessilib::string_view_cast<char16_t>(in_data);
		deserialize_json<char16_t, true>(result, data_view);
	}
	else if (in_write_encoding == text_encoding::utf_32) {
		std::u32string_view data_view = jessilib::string_view_cast<char32_t>(in_data);
		deserialize_json<char32_t, true>(result, data_view);
	}
	else if (in_write_encoding == text_encoding::wchar) {
		std::wstring_view data_view = jessilib::string_view_cast<wchar_t>(in_data);
		deserialize_json<wchar_t, true>(result, data_view);
	}
	else if (in_write_encoding == text_encoding::multibyte) {
		// TODO: support without copying
		auto u8_data = mbstring_to_ustring<char8_t>(jessilib::string_view_cast<char>(in_data));
		std::u8string_view data_view = u8_data.second;
		deserialize_json<char8_t, true>(result, data_view);
	}
	else if (in_write_encoding == text_encoding::utf_16_foreign) {
		// TODO: support without copying
		std::u16string u16_data{ jessilib::string_view_cast<char16_t>(in_data) };
		array_byteswap(u16_data.data(), u16_data.data() + u16_data.size());
		std::u16string_view data_view = u16_data;

		deserialize_json<char16_t, true>(result, data_view);
	}
	else if (in_write_encoding == text_encoding::utf_32_foreign) {
		// TODO: support without copying
		std::u32string u32_data{ jessilib::string_view_cast<char32_t>(in_data) };
		array_byteswap(u32_data.data(), u32_data.data() + u32_data.size());
		std::u32string_view data_view = u32_data;

		deserialize_json<char32_t, true>(result, data_view);
	}

	return result;
}

std::string json_parser::serialize_bytes(const object& in_object, text_encoding in_write_encoding) {
	switch (in_write_encoding) {
		case text_encoding::utf_8:
			return serialize_impl<char8_t, char>(in_object);
		case text_encoding::utf_16:
			return serialize_impl<char16_t, char>(in_object);
		case text_encoding::utf_32:
			return serialize_impl<char32_t, char>(in_object);
		case text_encoding::wchar:
			return serialize_impl<wchar_t, char>(in_object);
		case text_encoding::multibyte:
			return ustring_to_mbstring(serialize_impl<char8_t>(in_object)).second;

			// Other-endianness
		case text_encoding::utf_16_foreign: {
			std::string result = serialize_impl<char16_t, char>(in_object);
			string_byteswap<char16_t>(result);
			return result;
		}
		case text_encoding::utf_32_foreign: {
			std::string result = serialize_impl<char32_t, char>(in_object);
			string_byteswap<char32_t>(result);
			return result;
		}
		default:
			break;
	}

	return {};
}

} // namespace jessilib
