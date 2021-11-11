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

#include "jessilib/io/command.hpp"
#include "jessilib/io/command_manager.hpp"
#include "shutdown.hpp"

namespace jessibot {
namespace io {

using namespace jessilib::io;

command quit_command{ [](command_context& context) {
	using namespace jessilib::io;
	text quit_text{ "Closing jessibot", text::property::bold, color{ 0xFF0000 } }; // TODO: localize
	context.publicReply(formatted_message{ "{}", quit_text });
	notify_shutdown();
}, "quit" };

// ISSUE: help command has no way to know what commands exist for the given context
command help_command{ [](command_context& context) {
	auto details = context.details();
	auto table_name = details["table"].get<std::string>();

	if (table_name.empty()) {
		text error_text{ "ERROR", text::property::bold, color{ 0xFF0000 } }; // TODO: localize
		context.publicReply(formatted_message{ "{} command context is missing permission table name", error_text });
		return;
	}

	// table examples: "console", "irc", "irc+", "irc%", "irc@"; should table instead be 'tables'?

	text table_text{ table_name, text::property::bold, color{ 0x0000FF } };
	context.publicReply(formatted_message{ "Commands for table '{}':", table_text });

	// TODO: read some permission table and filter commands based upon permission and context information
	const command_manager& manager = command_manager::instance();
	manager.foreach([&context](basic_command* in_command) {
		context.publicReply(formatted_message{ "{}", in_command->label() });
		return true;
	});
}, "help" };

} // namespace io
} // namespace jessibot
