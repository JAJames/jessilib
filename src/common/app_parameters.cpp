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
#include <cuchar>
#include "jessilib/unicode.hpp"
#include "jessilib/split.hpp"

namespace jessilib {

// Convert null-terminated multi-byte string to UTF-8
std::u8string ntmbs_to_u8string(const char* in_ntmbs) {
	std::u8string result;

	std::string_view ntmbs_view = in_ntmbs;
	std::mbstate_t mbstate{};
	char32_t codepoint{};
	while (!ntmbs_view.empty()) {
		size_t bytes_read = std::mbrtoc32(&codepoint, ntmbs_view.data(), ntmbs_view.size(), &mbstate);
		if (bytes_read > ntmbs_view.size()) {
			// Some sort of error; just break
			break;
		}
		// bytes_read will never be 0 except for null characters, which are excluded from our view; here for future reuse
		bytes_read = std::max(size_t{1}, bytes_read);
		ntmbs_view.remove_prefix(bytes_read);
		encode_codepoint(result, codepoint);
	}

	return result;
}

std::vector<std::u8string> vectorize(const char** in_argv) {
	std::vector<std::u8string> result;
	if (in_argv == nullptr) {
		return result;
	}

	for (auto argv = in_argv; *argv != nullptr; ++argv) {
		result.emplace_back(ntmbs_to_u8string(*argv));
	}
	return result;
}

std::vector<std::u8string> vectorize(const wchar_t** in_argv) {
	std::vector<std::u8string> result;
	if (in_argv == nullptr) {
		return result;
	}

	for (auto argv = in_argv; *argv != nullptr; ++argv) {
		result.emplace_back(jessilib::string_cast<char8_t>(std::wstring_view{ *argv }));
	}
	return result;
}

app_parameters::app_parameters(int in_argc, char** in_argv, char** in_envp)
	: app_parameters{ in_argc, const_cast<const char**>(in_argv), const_cast<const char**>(in_envp) } {
	// Empty ctor body
}

app_parameters::app_parameters(int, const char** in_argv, const char** in_envp)
	: app_parameters{ vectorize(in_argv), vectorize(in_envp) } {
}

app_parameters::app_parameters(int in_argc, wchar_t** in_argv, wchar_t** in_envp)
	: app_parameters{ in_argc, const_cast<const wchar_t**>(in_argv), const_cast<const wchar_t**>(in_envp) } {
}

app_parameters::app_parameters(int, const wchar_t** in_argv, const wchar_t** in_envp)
	: app_parameters{ vectorize(in_argv), vectorize(in_envp) } {
}

app_parameters::app_parameters(std::vector<std::u8string> in_args, std::vector<std::u8string> in_env) {
	// Parse in environment variables first to ensure they're parsed before the early-out
	for (const auto& env : in_env) {
		auto split = jessilib::split_once(env, u8'=');
		m_env_values[split.first] = split.second;
	}

	// Sanity safety check; should never happen
	if (in_args.empty()) {
		return;
	}

	// Populate path
	m_path = in_args[0];

	// Process args
	std::u8string_view key;
	std::u8string value;
	auto flush_value = [&key, &value, this]() {
		// This is the start of a key; flush what we were previously processing
		if (!key.empty()) {
			if (value.empty()) {
				m_switches.emplace_back(key);
			}
			else {
				m_arg_values.emplace(key, std::move(value));
				value.clear();
			}
		}
	};

	for (size_t index = 1; index < in_args.size(); ++index) {
		const std::u8string& arg = in_args[index];
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
				if (key_end != std::u8string_view::npos) {
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
	m_switches_set = std::unordered_set<std::u8string>{ m_switches.begin(), m_switches.end() };
}

std::u8string_view app_parameters::path() const {
	return m_path;
}

const std::vector<std::u8string>& app_parameters::arguments() const {
	return m_args;
}

const std::vector<std::u8string>& app_parameters::switches() const {
	return m_switches;
}

const std::unordered_set<std::u8string>& app_parameters::switches_set() const {
	return m_switches_set;
}

const std::unordered_map<std::u8string, std::u8string>& app_parameters::values() const {
	return m_arg_values;
}

object app_parameters::as_object() const {
	using namespace std::literals;

	// Null check
	if (m_path.empty()
		&& m_args.empty()) {
		// app_parameters is null; return a null object
		return object{};
	}

	return std::map<std::u8string, object>{
		{ u8"Path"s, m_path },
		{ u8"Args"s, m_args },
		{ u8"Switches"s, m_switches },
		{ u8"Values"s, m_arg_values }
	};
}

#ifdef __cpp_lib_generic_unordered_lookup
#define WRAP_MAP_KEY(in_key) in_key
#else // We can't use std::string_view for InKeyType until GCC 11 & clang 12, and I still want to support GCC 9
#define WRAP_MAP_KEY(in_key) static_cast<std::u8string>(in_key)
#endif // __cpp_lib_generic_unordered_lookup

bool app_parameters::has_switch(std::u8string_view in_switch) const {
	return m_switches_set.find(WRAP_MAP_KEY(in_switch)) != m_switches_set.end();
}

std::u8string_view app_parameters::get_arg_value(std::u8string_view in_key, std::u8string_view in_default) const {
	auto result = m_arg_values.find(WRAP_MAP_KEY(in_key));

	// Safety check
	if (result == m_arg_values.end()) {
		return in_default;
	}

	return result->second;
}

std::u8string_view app_parameters::get_env_value(std::u8string_view in_key, std::u8string_view in_default) const {
	auto result = m_env_values.find(WRAP_MAP_KEY(in_key));

	// Safety check
	if (result == m_env_values.end()) {
		return in_default;
	}

	return result->second;
}

std::u8string_view app_parameters::get_value(std::u8string_view in_key, std::u8string_view in_default) const {
	// Explicit args take priority
	auto result = m_arg_values.find(WRAP_MAP_KEY(in_key));
	if (result != m_arg_values.end()) {
		return result->second;
	}

	// Fallback to env
	return get_env_value(in_key, in_default);
}

} // namespace jessilib
