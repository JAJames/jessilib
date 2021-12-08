/**
 * Copyright (C) 2020 Jessica James.
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

#include "console_command_context.hpp"

#include <iostream>
#include "jessilib/io/ansi/ansi_text.hpp"

namespace jessibot {
namespace io {

/** Reply */
bool console_command_context::privateReply(const jessilib::io::formatted_message& in_message) {
	auto result = jessilib::io::process_message<jessilib::io::ansi::text_wrapper>(in_message);
	// TODO check locale before printing to see if console is using UTF-8; if so we can just chuck this straight to cout
	// instead of leveraging to wchar_t
	std::wcout << jessilib::string_cast<wchar_t>(result) << std::endl;
	return true;
}

bool console_command_context::publicReply(const jessilib::io::formatted_message& in_message) {
	// Consoles only have 1 output mechanism
	return privateReply(in_message);
}

/** Additional contextual details */
jessilib::object console_command_context::details() const {
	static jessilib::object s_details {
		jessilib::object::map_type{ { u8"table", u8"console" } }
	};

	return s_details;
}

std::u8string console_command_context::getText(std::u8string_view tag) const {
	return { tag.begin(), tag.end() }; // TODO: implement properly
}

} // namespace io
} // namespace jessilib
