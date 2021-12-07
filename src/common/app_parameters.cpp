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

#include "app_parameters.hpp"
#include "split.hpp"
#include "assert.hpp"

namespace jessilib {

app_parameters::set_type app_parameters::default_stop_args() {
	using namespace std::literals;
	return { u8"--"sv };
}

template<typename CharT>
app_parameters::string_type get_first_arg(CharT** in_argv) {
	if (in_argv == nullptr) {
		return {};
	}

	return ntarg_cast<char8_t, CharT>(*in_argv);
}

template<typename CharT>
CharT** get_next_args(CharT** in_argv) {
	if (in_argv == nullptr) {
		return nullptr;
	}

	if (*in_argv == nullptr) {
		return nullptr;
	}

	return in_argv + 1;
}

app_parameters::app_parameters(int, char** in_argv, char** in_envp, const set_type& in_stop_args)
	: app_parameters{ get_first_arg(in_argv), vectorize_ntargs(get_next_args(in_argv)), vectorize_ntargs(in_envp), in_stop_args } {
	// Empty ctor body
}

app_parameters::app_parameters(int, const char** in_argv, const char** in_envp, const set_type& in_stop_args)
	: app_parameters{ get_first_arg(in_argv), vectorize_ntargs(get_next_args(in_argv)), vectorize_ntargs(in_envp), in_stop_args } {
	// Empty ctor body
}

app_parameters::app_parameters(int, wchar_t** in_argv, wchar_t** in_envp, const set_type& in_stop_args)
	: app_parameters{ get_first_arg(in_argv), vectorize_ntargs(get_next_args(in_argv)), vectorize_ntargs(in_envp), in_stop_args } {
	// Empty ctor body
}

app_parameters::app_parameters(int, const wchar_t** in_argv, const wchar_t** in_envp, const set_type& in_stop_args)
	: app_parameters{ get_first_arg(in_argv), vectorize_ntargs(get_next_args(in_argv)), vectorize_ntargs(in_envp), in_stop_args } {
	// Empty ctor body
}

bool app_parameters::has_switch(string_view_type in_switch) const {
	return m_switches_set.find(in_switch) != m_switches_set.end();
}

app_parameters::app_parameters(string_type in_name, std::vector<string_type> in_args, std::vector<string_type> in_env, const set_type& in_stop_args)
	: m_name{ std::move(in_name) }, m_args{ std::move(in_args) }, m_env{ std::move(in_env) } {
	m_env_values.reserve(m_env.size());

	// Parse in environment variables first to ensure they're parsed before the early-out
	for (const auto& env : m_env) {
		auto split = jessilib::split_once_view(env, u8'=');
		m_env_values[split.first] = split.second;
	}

	// Sanity safety check; should never happen
	if (m_args.empty()) {
		return;
	}

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
				m_arg_values[key] = std::move(value);
				value.clear();
			}
		}
		else {
			jessilib_debug_assert(m_precedent.empty()); // m_precedent should always be empty when key is empty
			// This key start marks the end of the precedent
			m_precedent = std::move(value);
			value.clear();
		}
	};

	for (auto itr = m_args.begin(); itr != m_args.end(); ++itr) {
		const string_type& arg = *itr;
		if (arg.empty()) {
			continue;
		}

		// Check if argument is a parse stopper
		if (in_stop_args.find(arg) != in_stop_args.end()) {
			// Copy remaining args to m_passthrough
			m_passthrough = arg;
			while (++itr != m_args.end()) {
				m_passthrough += u8' ';
				m_passthrough += *itr;
			}
			break;
		}

		// Check if this is a key or value
		if (arg.front() == u8'-') {
			// Flush pending value (if any)
			flush_value();

			// Strip any leading '-' or '--' and set key
			key = arg;
			key.remove_prefix(1);
			if (key.front() == u8'-') {
				key.remove_prefix(1);
			}

			// Parse key for any value delimiter ('=')
			size_t key_end = key.find(u8'=');
			if (key_end != string_view_type::npos) {
				// arg contains start of a value
				value = key.substr(key_end + 1);
				key = key.substr(0, key_end);
			}
		}
		else {
			// This is part of a value; add it
			if (!value.empty()) {
				value += u8' ';
			}

			value += arg;
		}
	}

	// Flush any pending switch/value
	flush_value();

	// Populate m_switches_set from m_switches
	m_switches_set = { m_switches.begin(), m_switches.end() };
}

const app_parameters::string_type& app_parameters::name() const {
	return m_name;
}

const std::vector<app_parameters::string_type>& app_parameters::arguments() const {
	return m_args;
}

const std::vector<app_parameters::string_type>& app_parameters::environment() const {
	return m_env;
}

const app_parameters::string_type& app_parameters::precedent() const {
	return m_precedent;
}

const app_parameters::string_type& app_parameters::passthrough() const {
	return m_passthrough;
}

const std::vector<app_parameters::string_view_type>& app_parameters::switches() const {
	return m_switches;
}

const app_parameters::set_type& app_parameters::switches_set() const {
	return m_switches_set;
}

const app_parameters::map_type& app_parameters::arg_values() const {
	return m_arg_values;
}

const app_parameters::light_map_type& app_parameters::env_values() const {
	return m_env_values;
}

app_parameters::light_map_type app_parameters::values() const {
	app_parameters::light_map_type result;

	for (const auto& key_value : m_env_values) {
		result[key_value.first] = key_value.second;
	}

	for (const auto& key_value : m_arg_values) {
		result[key_value.first] = key_value.second;
	}

	return result;
}

object app_parameters::as_object() const {
	using namespace std::literals;

	// Null check
	if (m_name.empty()
		&& m_args.empty()) {
		// app_parameters is null; return a null object
		return object{};
	}

	return std::map<string_type, object>{
		{ u8"Name"s, m_name },
		{ u8"Env"s, m_env },
		{ u8"Args"s, m_args },
		{ u8"Switches"s, m_switches },
		{ u8"ArgValues"s, m_arg_values },
		{ u8"EnvValues"s, m_env_values },
		{ u8"Values"s, values() }
	};
}

app_parameters::string_view_type app_parameters::get_arg_value(string_view_type in_key, string_view_type in_default) const {
	auto result = m_arg_values.find(in_key);

	// Safety check
	if (result == m_arg_values.end()) {
		return in_default;
	}

	return result->second;
}

app_parameters::string_view_type app_parameters::get_env_value(string_view_type in_key, string_view_type in_default) const {
	auto result = m_env_values.find(in_key);

	// Safety check
	if (result == m_env_values.end()) {
		return in_default;
	}

	return result->second;
}

app_parameters::string_view_type app_parameters::get_value(string_view_type in_key, string_view_type in_default) const {
	// Try args first
	auto result = m_arg_values.find(in_key);
	if (result != m_arg_values.end()) {
		return result->second;
	}

	// Fallback to trying env
	return get_env_value(in_key, in_default);
}

} // namespace jessilib
