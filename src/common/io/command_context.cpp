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

#include "io/command_context.hpp"

namespace jessilib {
namespace io {

command_context::command_context(std::string_view in_input)
	: m_input{ in_input } {
	// Strip leading whitespace
	std::size_t pos = in_input.find_first_not_of(' ');
	if (pos != std::string_view::npos) {
		in_input.remove_prefix(pos);

		// Whitespace is stripped; parse keyword from input
		pos = in_input.find(' ');
		if (pos == std::string_view::npos) {
			pos = in_input.size();
		}
		m_keyword = in_input.substr(0, pos);
		in_input.remove_prefix(pos);

		// Strip leading whitespace and parse parameter from remaining input
		pos = in_input.find_first_not_of(' ');
		if (pos != std::string_view::npos) {
			in_input.remove_prefix(pos);
			m_parameter = in_input;
		}
	}
}

command_context::command_context(std::string_view in_input, std::string_view in_keyword, std::string_view in_parameter)
	: m_input{ std::move(in_input) },
	m_keyword{ std::move(in_keyword) },
	m_parameter{ std::move(in_parameter) } {
	// Empty ctor body
}

std::string_view command_context::input() const {
	return m_input;
}

std::string_view command_context::keyword() const {
	return m_keyword;
}

std::string_view command_context::parameter() const {
	return m_parameter;
}

std::vector<std::string_view> command_context::paramaters() const {
	std::vector<std::string_view> result;
	std::string_view parameter = m_parameter;

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
