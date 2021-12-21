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

#include "serialize.hpp"
#include "impl/parser_manager.hpp"
#include "parser.hpp"

namespace jessilib {

/** Exception types */

format_not_available::format_not_available(const std::string& in_format)
	: std::runtime_error{ "Format \"" + in_format + "\"not recognized" } {
	// Empty ctor body
};

/** Helpers */

std::shared_ptr<parser> get_parser(const std::string& in_format) {
	auto parser = impl::parser_manager::instance().find_parser(in_format);
	if (parser == nullptr) {
		throw format_not_available{ in_format };
	}

	return parser;
}

/** Deserialization */
object deserialize_object(const std::u8string& in_data, const std::string& in_format) {
	return deserialize_object(std::u8string_view{ &in_data.front(), in_data.size() }, in_format);
}

object deserialize_object(const std::vector<char8_t>& in_data, const std::string& in_format) {
	return deserialize_object(std::u8string_view{ &in_data.front(), in_data.size() }, in_format);
}

object deserialize_object(std::u8string_view in_data, const std::string& in_format) {
	return get_parser(in_format)->deserialize(in_data);
}

object deserialize_object(std::istream& in_stream, const std::string& in_format, text_encoding in_encoding) {
	return get_parser(in_format)->deserialize_bytes(in_stream, in_encoding);
}

/** Serialization */
std::u8string serialize_object(const object& in_object, const std::string& in_format) {
	return get_parser(in_format)->serialize<char8_t>(in_object);
}

void serialize_object(std::ostream& in_stream, const object& in_object, const std::string& in_format, text_encoding in_encoding) {
	in_object.get<object::string_view_type>(object::string_view_type{});

	get_parser(in_format)->serialize_bytes(in_stream, in_object, in_encoding);
}

} // namespace jessilib
