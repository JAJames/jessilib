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

namespace jessilib {

app_parameters::app_parameters(int in_argc, char** in_argv)
	: app_parameters{ in_argc, const_cast<const char**>(in_argv) } {
	// Empty ctor body
}

app_parameters::app_parameters(int in_argc, const char** in_argv) {
	// TODO: discard argc/argv and use GetCommandLineW on Windows
	// TODO: not assume argv is utf-8; it often will not be
	// Sanity safety check; should never happen
	if (in_argc <= 0 || in_argv == nullptr) {
		return;
	}

	// Populate path
	m_path = reinterpret_cast<const char8_t*>(in_argv[0]);

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
				m_values.emplace(key, std::move(value));
				value.clear();
			}
		}
	};

	for (int index = 1; index < in_argc; ++index) {
		const char8_t* arg = reinterpret_cast<const char8_t*>(in_argv[index]);
		if (arg != nullptr && *arg != '\0') {
			// Check if this is a key or value
			if (*arg == '-') {
				// Flush pending value (if any)
				flush_value();

				// Strip any leading '-' or '--' and set key
				key = arg + 1;
				if (key[0] == '-') {
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
	m_switches_set = std::unordered_set<std::u8string_view>{ m_switches.begin(), m_switches.end() };
}

std::u8string_view app_parameters::path() const {
	return m_path;
}

const std::vector<std::u8string_view>& app_parameters::arguments() const {
	return m_args;
}

const std::vector<std::u8string_view>& app_parameters::switches() const {
	return m_switches;
}

const std::unordered_set<std::u8string_view>& app_parameters::switches_set() const {
	return m_switches_set;
}

const std::unordered_map<std::u8string_view, std::u8string>& app_parameters::values() const {
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

	return std::map<std::u8string, object>{
		{ u8"Path"s, m_path },
		{ u8"Args"s, m_args },
		{ u8"Switches"s, m_switches },
		{ u8"Values"s, m_values }
	};
}

bool app_parameters::has_switch(std::u8string_view in_switch) const {
	return m_switches_set.find(in_switch) != m_switches_set.end();
}

std::u8string_view app_parameters::get_value(std::u8string_view in_key, std::u8string_view in_default) const {
	auto result = m_values.find(in_key);

	// Safety check
	if (result == m_values.end()) {
		return in_default;
	}

	return result->second;
}

} // namespace jessilib
