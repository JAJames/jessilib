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

/**
 * Loosely structured application parameter parsing:
 * /your/app [precedent] [switches & arguments] [passthrough = [stop arg] ...]
 */
class app_parameters {
public:
	using string_type = std::u8string;
	using string_view_type = std::u8string_view;
	using set_type = std::unordered_set<string_view_type>;
	using map_type = std::unordered_map<string_view_type, string_type>;
	using light_map_type = std::unordered_map<string_view_type, string_view_type>;
	static set_type default_stop_args(); // "--" by default, but applications may wish to pass command names as well

	/** Value constructors */
	app_parameters(int in_argc, char** in_argv, char** in_envp = nullptr, const set_type& stop_args = default_stop_args());
	app_parameters(int in_argc, const char** in_argv, const char** in_envp = nullptr, const set_type& stop_args = default_stop_args());
	app_parameters(int in_argc, wchar_t** in_argv, wchar_t** in_envp = nullptr, const set_type& stop_args = default_stop_args());
	app_parameters(int in_argc, const wchar_t** in_argv, const wchar_t** in_envp = nullptr, const set_type& stop_args = default_stop_args());
	app_parameters(string_type in_name, std::vector<string_type> in_args, std::vector<string_type> in_env = {}, const set_type& stop_args = default_stop_args());

	/** Standard constructors & assignment operators */
	app_parameters() = default;

	// all owning copies are in vector or map_type, neither of which invalidate pointers/references/etc on move
	app_parameters(app_parameters&&) = default;
	app_parameters& operator=(app_parameters&&) = default;

	// Call app_parameters(in.name(), in.arguments(), in.environment(), in_stop_args) instead; we don't keep in_stop_args
	app_parameters(const app_parameters&) = delete;
	app_parameters& operator=(const app_parameters&) = delete;

	/** Input getters */
	[[nodiscard]] const string_type& name() const;
	[[nodiscard]] const std::vector<string_type>& arguments() const;
	[[nodiscard]] const std::vector<string_type>& environment() const;

	/** Parsed data object getters */
	[[nodiscard]] const string_type& precedent() const;
	[[nodiscard]] const string_type& passthrough() const;
	[[nodiscard]] const std::vector<string_view_type>& switches() const;
	[[nodiscard]] const set_type& switches_set() const;
	[[nodiscard]] const map_type& arg_values() const;
	[[nodiscard]] const light_map_type& env_values() const;
	[[nodiscard]] light_map_type values() const;
	[[nodiscard]] jessilib::object as_object() const;

	/** Parsed data value getters */
	[[nodiscard]] bool has_switch(string_view_type in_switch) const;
	[[nodiscard]] string_view_type get_arg_value(string_view_type in_key, string_view_type in_default = {}) const;
	[[nodiscard]] string_view_type get_env_value(string_view_type in_key, string_view_type in_default = {}) const;
	[[nodiscard]] string_view_type get_value(string_view_type in_key, string_view_type in_default = {}) const;

	// TODO: assertive accessors for aliases with exceptions, i.e: get_unique_arg_value(...), get_one_switch(...)

	/** Conversion */
	[[nodiscard]] inline operator jessilib::object() const { return as_object(); }

private:
	string_type m_name;
	std::vector<string_type> m_args, m_env; // owning vectors of arguments & environment variables passed to app_parameters
	string_type m_precedent; // non-owning vector of precedent arguments (i.e: text before switches/args)
	string_type m_passthrough; // non-owning vector of passthrough arguments
	std::vector<string_view_type> m_switches; // non-owning vector of switches
	set_type m_switches_set; // non-owning set of switches set
	map_type m_arg_values; // owning map of argument values
	light_map_type m_env_values; // non-owning map of environment variables
};

/**
 * Recodes a null-terminated multi-byte string to unicode
 *
 * @tparam OutCharT Unicode data unit type
 * @tparam InCharT Character type for input multi-byte null-terminated string (char or const char)
 * @param in_ntarg Multi-byte null-terminated string to recode (may be nullptr)
 * @return A unicode string equivalent to in_ntarg
 */
template<typename OutCharT = char8_t, typename InCharT,
	std::enable_if_t<std::is_same_v<std::remove_cvref_t<InCharT>, char>>* = nullptr>
std::basic_string<OutCharT> ntarg_cast(InCharT* in_ntarg) {
	if (in_ntarg == nullptr) {
		return {};
	}

	return mbstring_to_ustring<OutCharT>(in_ntarg).second;
}

/**
 * Recodes a null-terminated wide character string to unicode
 *
 * @tparam OutCharT Unicode data unit type
 * @tparam InCharT Character type for input wide character null-terminated string (wchar_t or const wchar_t)
 * @param in_ntarg Wide character null-terminated string to recode (may be nullptr)
 * @return A unicode string equivalent to in_ntarg
 */
template<typename OutCharT = char8_t, typename InCharT,
	std::enable_if_t<std::is_same_v<std::remove_cvref_t<InCharT>, wchar_t>>* = nullptr>
std::basic_string<OutCharT> ntarg_cast(InCharT* in_ntarg) {
	if (in_ntarg == nullptr) {
		return {};
	}

	return jessilib::string_cast<OutCharT>(std::wstring_view{ in_ntarg });
}

/**
 * Converts null-terminated argument array of null-terminated strings to a vector of unicode strings
 *
 * @tparam OutCharT Unicode data unit type
 * @tparam InCharT Input character type (char for multi-byte string, or wchar_t for wide character strings)
 * @param in_ntarg_array Null-terminated argument array to vectorize
 * @return A vector of unicode strings recoded from the input
 */
template<typename OutCharT = char8_t, typename InCharT>
std::vector<std::basic_string<OutCharT>> vectorize_ntargs(InCharT** in_ntarg_array) {
	std::vector<std::basic_string<OutCharT>> result;
	if (in_ntarg_array == nullptr) {
		return result;
	}

	for (auto argv = in_ntarg_array; *argv != nullptr; ++argv) {
		result.emplace_back(ntarg_cast<OutCharT, InCharT>(*argv));
	}

	return result;
}

} // namespace jessilib
