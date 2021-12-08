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

#include "io/command.hpp"
#include "io/command_manager.hpp"

namespace jessilib {
namespace io {

command::command(callback_t in_callback, std::u8string_view in_label)
	: m_callback{ std::move(in_callback) },
	m_label{ std::move(in_label) } {
	// Register command
	command_manager::instance().register_command(*this);
}

command::~command() {
	// Unregister command
	command_manager::instance().unregister_command(*this);
}

std::u8string_view command::label() const {
	return m_label;
}

void command::execute(command_context& in_context) const {
	m_callback(in_context);
}

command test_command{ [](command_context&) {
	// do stuff
}, u8"test" };

} // namespace io
} // namespace jessilib
