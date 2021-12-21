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

#include "parser.hpp"
#include <istream>

namespace jessilib {

object parser::deserialize_bytes(std::istream& in_stream, text_encoding in_read_encoding) {
	std::vector<byte_type> data;

	// Read entire stream into data
	char buffer[1024];
	while(!in_stream.eof()) {
		in_stream.read(buffer, sizeof(buffer));
		data.insert(data.end(), buffer, buffer + in_stream.gcount());
	}

	// Pass data to deserialize
	return deserialize_bytes(bytes_view_type{ &data.front(), data.size() }, in_read_encoding);
}

void parser::serialize_bytes(std::ostream& in_stream, const object& in_object, text_encoding in_write_encoding) {
	// TODO: replace this method
	auto bytes = serialize_bytes(in_object, in_write_encoding);
	in_stream.write(bytes.data(), bytes.size());
}

} // namespace jessilib
