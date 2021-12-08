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

#include "console.hpp"

#include <iostream>
#include "jessilib/io/command_manager.hpp"
#include "jessilib/io/ansi/ansi_text.hpp"
#include "shutdown.hpp"
#include "console/console_command_context.hpp"

namespace jessibot {
namespace io {

void console_input_loop() {
	using namespace jessilib::io;

	std::wstring input;
	auto shutdown_future = get_shutdown_future();
	while (shutdown_future.wait_for(std::chrono::milliseconds(10)) != std::future_status::ready) {
		std::getline(std::wcin, input); // TODO: use a non-bloicking call and poll running periodically?
		jessibot::io::console_command_context context{ jessilib::string_cast<char8_t>(input) };
		if (!command_manager::instance().execute_command(context)) {
			text error_text{ u8"ERROR", text::property::bold, color{ 0xFF0000 }};
			text keyword_text{ context.keyword(), text::property::bold, color{ 0x0000FF }};
			auto result = process_message<ansi::text_wrapper>(formatted_message{u8"{} Command \"{}\" not found", error_text, keyword_text});
			std::wcout << jessilib::string_cast<wchar_t>(result) << std::endl;
		}
	}
}

} // namespace io
} // namespace jessibot
