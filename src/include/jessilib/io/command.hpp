/**
 * Copyright (C) 2019-2021 Jessica James.
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
#include <functional>
#include "command_context.hpp"

namespace jessilib {
namespace io {

class basic_command {
public:
	virtual ~basic_command() = default;
	virtual std::u8string_view label() const = 0;
	virtual void execute(command_context& in_context) const = 0;
}; // class basic_command

class command : public basic_command {
public:
	// Types
	using callback_t = std::function<void(command_context&)>;

	// Delete all implicit constructors and assignment operators
	command() = delete;
	command(const command&) = delete;
	command(command&&) = delete;
	command& operator=(const command&) = delete;
	command& operator=(command&&) = delete;

	// Instantiates and self-registers a command
	command(callback_t in_callback, std::u8string_view in_label);

	// Cleans up and unregisters the command
	virtual ~command();

	// Unique label associated with command
	virtual std::u8string_view label() const override;

	// Executes the command
	virtual void execute(command_context& in_context) const override;

private:
	callback_t m_callback;
	std::u8string_view m_label;
}; // class command

} // namespace io
} // namespace jessilib
