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

#include "app_parameters.hpp"
#include "jessilib/unicode.hpp"
#include "jessilib/split.hpp"

namespace jessilib {

template<typename CharT,
    std::enable_if_t<std::is_same_v<std::remove_cvref_t<CharT>, char>>* = nullptr>
std::vector<app_parameters::string_type> vectorize(CharT** in_ntarg_array) {
	std::vector<app_parameters::string_type> result;
	if (in_ntarg_array == nullptr) {
		return result;
	}

	for (auto argv = in_ntarg_array; *argv != nullptr; ++argv) {
		result.emplace_back(mbstring_to_ustring<char8_t>(*argv).second);
	}

	return result;
}

template<typename CharT,
	std::enable_if_t<std::is_same_v<std::remove_cvref_t<CharT>, wchar_t>>* = nullptr>
std::vector<app_parameters::string_type> vectorize(CharT** in_ntarg_array) {
	std::vector<app_parameters::string_type> result;
	if (in_ntarg_array == nullptr) {
		return result;
	}

	for (auto argv = in_ntarg_array; *argv != nullptr; ++argv) {
		result.emplace_back(jessilib::string_cast<char8_t>(std::wstring_view{ *argv }));
	}

	return result;
}

app_parameters::app_parameters(int, char** in_argv, char** in_envp)
	: app_parameters{ vectorize(in_argv), vectorize(in_envp) } {
	// Empty ctor body
}

app_parameters::app_parameters(int, const char** in_argv, const char** in_envp)
	: app_parameters{ vectorize(in_argv), vectorize(in_envp) } {
	// Empty ctor body
}

app_parameters::app_parameters(int, wchar_t** in_argv, wchar_t** in_envp)
	: app_parameters{ vectorize(in_argv), vectorize(in_envp) } {
	// Empty ctor body
}

app_parameters::app_parameters(int, const wchar_t** in_argv, const wchar_t** in_envp)
	: app_parameters{ vectorize(in_argv), vectorize(in_envp) } {
	// Empty ctor body
}

#ifdef __cpp_lib_generic_unordered_lookup
#define WRAP_MAP_KEY(in_key) in_key
#else // We can't use std::string_view for InKeyType until GCC 11 & clang 12, and I still want to support GCC 9
#define WRAP_MAP_KEY(in_key) static_cast<string_type>(in_key)
#endif // __cpp_lib_generic_unordered_lookup

bool app_parameters::has_switch(string_view_type in_switch) const {
	return m_switches_set.find(WRAP_MAP_KEY(in_switch)) != m_switches_set.end();
}

app_parameters::app_parameters(std::vector<string_type> in_args, std::vector<string_type> in_env) {
	// Parse in environment variables first to ensure they're parsed before the early-out
	for (const auto& env : in_env) {
		auto split = jessilib::split_once(env, u8'=');
		m_values[split.first] = split.second;
		m_env_values[split.first] = std::move(split.second);
	}

	// Sanity safety check; should never happen
	if (in_args.empty()) {
		return;
	}

	// Populate path
	m_path = in_args[0];

	// Process args
	string_view_type key;
	string_type value;
	auto flush_value = [&key, &value, this]() {
		// This is the start of a key; flush what we were previously processing
		if (!key.empty()) {
			if (value.empty()) {
				m_switches.emplace_back(key);
			}
			else {
				string_type key_str{ key };
				m_values[key_str] = value;
				m_arg_values[key_str] = std::move(value);
				value.clear();
			}
		}
	};

	for (size_t index = 1; index < in_args.size(); ++index) {
		const string_type& arg = in_args[index];
		if (!arg.empty()) {
			// Check if this is a key or value
			if (arg.front() == '-') {
				// Flush pending value (if any)
				flush_value();

				// Strip any leading '-' or '--' and set key
				key = arg;
				key.remove_prefix(1);
				if (key.front() == '-') {
					key.remove_prefix(1);
				}

				// Parse key for any value denominator ('=')
				size_t key_end = key.find('=');
				if (key_end != string_view_type::npos) {
					// arg contains start of a value
					value = key.substr(key_end + 1);
					key = key.substr(0, key_end);
				}
			}
			else {
				// This is part of a value; add it
				if (!value.empty()) {
					value += ' ';
				}

				value += arg;
			}
		}
		// else // empty string encountered

		// Push arg
		m_args.emplace_back(arg);
	}

	// Flush any pending switch/value
	flush_value();

	// Populate m_switches_set from m_switches
	m_switches_set = { m_switches.begin(), m_switches.end() };
}

app_parameters::string_view_type app_parameters::path() const {
	return m_path;
}

const std::vector<app_parameters::string_type>& app_parameters::arguments() const {
	return m_args;
}

const std::vector<app_parameters::string_type>& app_parameters::switches() const {
	return m_switches;
}

const app_parameters::set_type& app_parameters::switches_set() const {
	return m_switches_set;
}

const app_parameters::map_type& app_parameters::arg_values() const {
	return m_arg_values;
}

const app_parameters::map_type& app_parameters::env_values() const {
	return m_env_values;
}

const app_parameters::map_type& app_parameters::values() const {
	return m_values;
}

object app_parameters::as_object() const {
	using namespace std::literals;

	// Null check
	if (m_path.empty()
		&& m_args.empty()) {
		// app_parameters is null; return a null object
		return object{};
	}

	return std::map<string_type, object>{
		{ u8"Path"s, m_path },
		{ u8"Args"s, m_args },
		{ u8"Switches"s, m_switches },
		{ u8"ArgValues"s, m_arg_values },
		{ u8"EnvValues"s, m_env_values },
		{ u8"Values"s, m_values }
	};
}

app_parameters::string_view_type app_parameters::get_arg_value(string_view_type in_key, string_view_type in_default) const {
	auto result = m_arg_values.find(WRAP_MAP_KEY(in_key));

	// Safety check
	if (result == m_arg_values.end()) {
		return in_default;
	}

	return result->second;
}

app_parameters::string_view_type app_parameters::get_env_value(string_view_type in_key, string_view_type in_default) const {
	auto result = m_env_values.find(WRAP_MAP_KEY(in_key));

	// Safety check
	if (result == m_env_values.end()) {
		return in_default;
	}

	return result->second;
}

app_parameters::string_view_type app_parameters::get_value(string_view_type in_key, string_view_type in_default) const {
	auto result = m_values.find(WRAP_MAP_KEY(in_key));

	// Safety check
	if (result == m_values.end()) {
		return in_default;
	}

	return result->second;
}

} // namespace jessilib
