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
#include "unicode_compare.hpp"

namespace jessilib {

class app_parameters {
public:
	using set_type = std::unordered_set<std::u8string, jessilib::text_hashi, jessilib::text_equali>;
	using map_type = std::unordered_map<std::u8string, std::u8string, jessilib::text_hashi, jessilib::text_equali>;

	app_parameters(int in_argc, char** in_argv, char** in_envp = nullptr);
	app_parameters(int in_argc, const char** in_argv, const char** in_envp = nullptr);
	app_parameters(int in_argc, wchar_t** in_argv, wchar_t** in_envp = nullptr);
	app_parameters(int in_argc, const wchar_t** in_argv, const wchar_t** in_envp = nullptr);
	app_parameters(std::vector<std::u8string> in_args, std::vector<std::u8string> in_env = {});

	std::u8string_view path() const;
	const std::vector<std::u8string>& arguments() const;

	const std::vector<std::u8string>& switches() const;
	const set_type& switches_set() const;
	const map_type& values() const;
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
	set_type m_switches_set;
	map_type m_arg_values;
	map_type m_env_values;
};

} // namespace jessilib
