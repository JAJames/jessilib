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

#include <string_view>
#include "jessilib/object.hpp"
#include "message.hpp"

namespace jessilib {
namespace io {

class command_context {
public:
	using string_type = std::u8string;
	command_context(string_type in_input);
	command_context(string_type in_input, string_type in_keyword, string_type in_parameter);

	/** User input */
	const string_type& input() const;
	const string_type& keyword() const;
	const string_type& parameter() const;
	std::vector<std::u8string_view> paramaters() const;

	/** Reply */
	virtual bool privateReply(const formatted_message& in_message) = 0; // Reply to invoker privately (i.e: PM)
	virtual bool publicReply(const formatted_message& in_message) = 0; // Reply to invoker publicly (i.e: channel)

	/** Additional contextual details */
	virtual object details() const = 0; // Additional details
	virtual std::u8string getText(std::u8string_view tag) const = 0; // Get localized text

private:
	string_type m_input;
	string_type m_keyword;
	string_type m_parameter;
}; // class command_context

} // namespace io
} // namespace jessilib
