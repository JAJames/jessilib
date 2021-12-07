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

#include "object.hpp"
#include "unicode.hpp"

namespace jessilib {

class app_parameters {
public:
	using string_type = std::u8string;
	using string_view_type = std::u8string_view;
	using set_type = std::unordered_set<string_type, jessilib::text_hashi, jessilib::text_equali>;
	using map_type = std::unordered_map<string_type, string_type, jessilib::text_hashi, jessilib::text_equali>;

	app_parameters(int in_argc, char** in_argv, char** in_envp = nullptr);
	app_parameters(int in_argc, const char** in_argv, const char** in_envp = nullptr);
	app_parameters(int in_argc, wchar_t** in_argv, wchar_t** in_envp = nullptr);
	app_parameters(int in_argc, const wchar_t** in_argv, const wchar_t** in_envp = nullptr);
	app_parameters(std::vector<string_type> in_args, std::vector<string_type> in_env = {});

	[[nodiscard]] string_view_type path() const;
	[[nodiscard]] const std::vector<string_type>& arguments() const;

	[[nodiscard]] const std::vector<string_type>& switches() const;
	[[nodiscard]] const set_type& switches_set() const;
	[[nodiscard]] const map_type& arg_values() const;
	[[nodiscard]] const map_type& env_values() const;
	[[nodiscard]] const map_type& values() const;
	[[nodiscard]] jessilib::object as_object() const;

	[[nodiscard]] bool has_switch(string_view_type in_switch) const;
	[[nodiscard]] string_view_type get_arg_value(string_view_type in_key, string_view_type in_default = {}) const;
	[[nodiscard]] string_view_type get_env_value(string_view_type in_key, string_view_type in_default = {}) const;
	[[nodiscard]] string_view_type get_value(string_view_type in_key, string_view_type in_default = {}) const;

	[[nodiscard]] inline operator jessilib::object() const { return as_object(); }

private:
	string_type m_path;
	std::vector<string_type> m_args;
	std::vector<string_type> m_switches;
	set_type m_switches_set;
	map_type m_arg_values;
	map_type m_env_values;
	map_type m_values;
};

/**
 * Converts null-terminated argument array of null-terminated strings to a vector of unicode strings
 *
 * @tparam OutCharT Unicode character data type
 * @tparam InCharT Input character type (char for multi-byte string, or wchar_t for wide character strings)
 * @param in_ntarg_array Null-terminated argument array to vectorize
 * @return A vector of unicode strings recoded from the input
 */
template<typename OutCharT = char8_t, typename InCharT,
	std::enable_if_t<std::is_same_v<std::remove_cvref_t<InCharT>, char>>* = nullptr>
std::vector<std::basic_string<OutCharT>> vectorize_ntargs(InCharT** in_ntarg_array) {
	std::vector<std::basic_string<OutCharT>> result;
	if (in_ntarg_array == nullptr) {
		return result;
	}

	for (auto argv = in_ntarg_array; *argv != nullptr; ++argv) {
		result.emplace_back(mbstring_to_ustring<OutCharT>(*argv).second);
	}

	return result;
}

/**
 * Converts null-terminated argument array of null-terminated strings to a vector of unicode strings
 *
 * @tparam OutCharT Unicode character data type
 * @tparam InCharT Input character type (char for multi-byte string, or wchar_t for wide character strings)
 * @param in_ntarg_array Null-terminated argument array to vectorize
 * @return A vector of unicode strings recoded from the input
 */
template<typename OutCharT = char8_t, typename InCharT,
	std::enable_if_t<std::is_same_v<std::remove_cvref_t<InCharT>, wchar_t>>* = nullptr>
std::vector<std::basic_string<OutCharT>> vectorize_ntargs(InCharT** in_ntarg_array) {
	std::vector<std::basic_string<OutCharT>> result;
	if (in_ntarg_array == nullptr) {
		return result;
	}

	for (auto argv = in_ntarg_array; *argv != nullptr; ++argv) {
		result.emplace_back(jessilib::string_cast<OutCharT>(std::wstring_view{ *argv }));
	}

	return result;
}

} // namespace jessilib
