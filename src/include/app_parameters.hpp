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

#include "object.hpp"

namespace jessilib {

class app_parameters {
public:
	app_parameters(int in_argc, char** in_argv);
	app_parameters(int in_argc, const char** in_argv);

	std::string_view path() const;
	const std::vector<std::string_view>& arguments() const;
	const std::vector<std::string_view>& switches() const;
	std::unordered_set<std::string_view> switches_set() const;
	const std::unordered_map<std::string_view, std::string>& values() const;
	jessilib::object as_object() const;

	operator jessilib::object() const { return as_object(); }

private:
	std::string_view m_path;
	std::vector<std::string_view> m_args;
	std::vector<std::string_view> m_switches;
	std::unordered_map<std::string_view, std::string> m_values;
};

} // namespace jessilib
