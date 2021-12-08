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
#include "impl/parser_manager.hpp"

namespace jessilib {

class parser {
public:
	virtual ~parser() = default;

	/** Interface methods */

	/**
	 * Deserializes an object directly from a stream
	 * May throw: invalid_argument
	 *
	 * @param in_stream Stream to deserialize object from
	 * @return A valid (possibly null) object
	 */
	virtual object deserialize(std::istream& in_stream);
	virtual object deserialize(std::string_view in_data) = 0; // TODO: serialize from arbitrary unicode strings
	virtual void serialize(std::ostream& in_stream, const object& in_object);
	virtual std::string serialize(const object& in_object) = 0; // TODO: serialize to arbitrary unicode strings
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

} // namespace jessilib
