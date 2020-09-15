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

#include <shared_mutex>
#include "command.hpp"

namespace jessilib {
namespace io {

class command_manager {
public:
	struct entry {
		basic_command* m_command;
	};

	void register_command(basic_command& in_command);
	void unregister_command(basic_command& in_command);
	bool execute_command(const command_context& in_context);

	// executes a predicate for each command, until the predicate returns false or no commands remain
	template<typename predicate_t> // TODO: add check for non-const
	void foreach(predicate_t&& in_predicate) {
		std::lock_guard<std::shared_mutex> guard{ m_commands_mutex };
		for (auto& command_entry : m_commands) {
			if (!in_predicate(command_entry)) {
				// Predicate returned false; we're done here
				return;
			}
		}
	}

	// executes a predicate for each command, until the predicate returns false or no commands remain
	template<typename predicate_t> // TODO: add check for const
	void foreach(predicate_t&& in_predicate) const {
		std::shared_lock<std::shared_mutex> guard{ m_commands_mutex };
		for (const auto& command_entry : m_commands) {
			if (!in_predicate(command_entry)) {
				// Predicate returned false; we're done here
				return;
			}
		}
	}

	static command_manager& instance();

private:
	std::vector<entry> m_commands;
	mutable std::shared_mutex m_commands_mutex;
};

} // namespace io
} // namespace jessilib