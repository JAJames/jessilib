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

#include "parser.hpp"
#include <istream>

namespace jessilib {

object parser::deserialize(std::istream& in_stream) {
	std::vector<char> data;

	// Read entire stream into data
	char buffer[1024];
	while(!in_stream.eof()) {
		in_stream.read(buffer, sizeof(buffer));
		data.insert(data.end(), buffer, buffer + in_stream.gcount());
	}

	// Pass data to deserialize
	return deserialize(std::string_view{ &data.front(), data.size() });
}

void parser::serialize(std::ostream& in_stream, const object& in_object) {
	in_stream << serialize(in_object);
}

} // namespace jessilib
