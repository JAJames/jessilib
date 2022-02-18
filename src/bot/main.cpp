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

#include <iostream>
#include "jessilib/app_parameters.hpp"
#include "jessilib/parsers/json.hpp"
#include "console/console.hpp"

int main(int argc, char** argv) {
	jessilib::app_parameters parameters{ argc, argv };

	if (parameters.has_switch(u8"echoParameters")) {
		// TODO: Write pretty JSON serializer based on JSON serializer
		std::wcout << std::endl << jessilib::json_parser{}.serialize<wchar_t>(parameters) << std::endl;
	}

	jessibot::io::console_input_loop();
	return 0;
}
