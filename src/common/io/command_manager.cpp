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

#include "io/command_manager.hpp"

namespace jessilib {
namespace io {

void command_manager::register_command(basic_command& in_command) {
	std::lock_guard<std::shared_mutex> guard{ m_commands_mutex };
	m_commands.push_back(&in_command);
}

void command_manager::unregister_command(basic_command& in_command) {
	std::lock_guard<std::shared_mutex> guard{ m_commands_mutex };
	std::remove(m_commands.begin(), m_commands.end(), &in_command);
}

bool command_manager::execute_command(command_context& in_context) {
	std::shared_lock<std::shared_mutex> lock{ m_commands_mutex };
	for (auto& command : m_commands) {
		if (in_context.keyword() == command->label()) {
			command->execute(in_context);
			return true;
		}
	}

	return false;
}

command_manager& command_manager::instance() {
	static command_manager s_command_manager;
	return s_command_manager;
}

} // namespace io
} // namespace jessilib
