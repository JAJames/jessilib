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

#include "io/command_context.hpp"

namespace jessilib {
namespace io {

command_context::command_context(string_type in_input)
	: m_input{ std::move(in_input) } {
	std::u8string_view input_view = m_input;

	// Strip leading whitespace
	std::size_t pos = input_view.find_first_not_of(u8' ');
	if (pos != std::string_view::npos) {
		input_view.remove_prefix(pos);

		// Whitespace is stripped; parse keyword from input
		pos = input_view.find(u8' ');
		if (pos == std::string_view::npos) {
			pos = input_view.size();
		}
		m_keyword = input_view.substr(0, pos);
		input_view.remove_prefix(pos);

		// Strip leading whitespace and parse parameter from remaining input
		pos = input_view.find_first_not_of(u8' ');
		if (pos != std::string_view::npos) {
			input_view.remove_prefix(pos);
			m_parameter = input_view;
		}
	}
}

command_context::command_context(string_type in_input, string_type in_keyword, string_type in_parameter)
	: m_input{ std::move(in_input) },
	m_keyword{ std::move(in_keyword) },
	m_parameter{ std::move(in_parameter) } {
	// Empty ctor body
}

const command_context::string_type& command_context::input() const {
	return m_input;
}

const command_context::string_type& command_context::keyword() const {
	return m_keyword;
}

const command_context::string_type& command_context::parameter() const {
	return m_parameter;
}

std::vector<std::u8string_view> command_context::paramaters() const {
	std::vector<std::u8string_view> result;
	std::u8string_view parameter = m_parameter;

	// Strip leading whitespace
	std::size_t pos = parameter.find_first_not_of(' ');
	if (pos == std::string_view::npos) {
		// parameter is empty; return empty vector
		return result;
	}
	parameter.remove_prefix(pos);

	// Parse parameter into result
	while (!parameter.empty()) {
		// Parse word from parameter into result
		pos = parameter.find(' ');
		if (pos == std::string_view::npos) {
			pos = parameter.size();
		}
		result.push_back(parameter.substr(0, pos));

		// Strip word and trailing whitespace
		parameter.remove_prefix(pos);
		parameter.remove_prefix(std::min(parameter.find_first_not_of(' '), parameter.size()));
	}

	return result;
};

} // namespace io
} // namespace jessilib
