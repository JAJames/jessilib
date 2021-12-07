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
	app_parameters(int in_argc, char** in_argv, char** in_envp = nullptr);
	app_parameters(int in_argc, const char** in_argv, const char** in_envp = nullptr);
	app_parameters(int in_argc, wchar_t** in_argv, wchar_t** in_envp = nullptr);
	app_parameters(int in_argc, const wchar_t** in_argv, const wchar_t** in_envp = nullptr);
	app_parameters(std::vector<std::u8string> in_args, std::vector<std::u8string> in_env = {});

	std::u8string_view path() const;
	const std::vector<std::u8string>& arguments() const;

	const std::vector<std::u8string>& switches() const;
	const std::unordered_set<std::u8string>& switches_set() const;
	const std::unordered_map<std::u8string, std::u8string>& values() const;
	jessilib::object as_object() const;

	bool has_switch(std::u8string_view in_switch) const;
	std::u8string_view get_arg_value(std::u8string_view in_key, std::u8string_view in_default = {}) const;
	std::u8string_view get_env_value(std::u8string_view in_key, std::u8string_view in_default = {}) const;
	std::u8string_view get_value(std::u8string_view in_key, std::u8string_view in_default = {}) const;

	operator jessilib::object() const { return as_object(); }

private:
	std::u8string m_path;
	std::vector<std::u8string> m_args;
	std::vector<std::u8string> m_switches;
	std::unordered_set<std::u8string> m_switches_set;
	std::unordered_map<std::u8string, std::u8string> m_arg_values;
	std::unordered_map<std::u8string, std::u8string> m_env_values;
};

} // namespace jessilib
