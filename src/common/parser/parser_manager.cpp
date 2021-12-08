/**
 * Copyright (C) 2018-2021 Jessica James.
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

#include "impl/parser_manager.hpp"
#include "parser.hpp"
#include "assert.hpp"

namespace jessilib {
namespace impl {

parser_manager::registration::registration(id in_id)
	: m_id{ in_id } {
	// Empty ctor body
}

parser_manager::registration::registration(const std::string& in_format)
	: m_format{ in_format } {
	// Empty ctor body
}

parser_manager::registration::registration(id in_id, const std::string& in_format)
	: m_id{ in_id },
	m_format{ in_format } {
	// Empty ctor body
}

bool parser_manager::registration::operator<(const registration& rhs) const {
	if (m_id != 0 && rhs.m_id) {
		// Both registrations have an ID; this is a search by ID
		return m_id < rhs.m_id;
	}

	// One of the registrations lacked an ID; this must be a search by format
	return m_format < rhs.m_format;
}

parser_manager::id parser_manager::register_parser(std::shared_ptr<parser> in_parser, const std::string& in_format, bool in_force) {
	std::lock_guard<std::shared_mutex> guard{ m_mutex };

	// Check for any existing parser
	auto itr = m_parsers.find(in_format);
	if (itr != m_parsers.end()) {
		// A parser already exists; return or replace
		if (!in_force) {
			return bad_id;
		}

		// Remove existing parser and registration
		m_parsers.erase(itr);
		m_registrations.erase(registration{ in_format });
	}

	// Register our new parser
	id parser_id = ++m_last_id;
	m_parsers.emplace(in_format, in_parser);
	m_registrations.emplace(parser_id, in_format);

	// Our parser is registered; return the parser ID we just registered
	return parser_id;
}

void parser_manager::unregister_parser(id in_id) {
	std::lock_guard<std::shared_mutex> guard{ m_mutex };

	// Search for registration
	auto itr = m_registrations.find(registration{ in_id });
	if (itr == m_registrations.end()) {
		// Parser already unregistered; do nothing
		return;
	}

	// Unregister
	registration removed_registration = *itr;
	m_registrations.erase(itr);
	m_parsers.erase(removed_registration.m_format);
}

std::shared_ptr<parser> parser_manager::find_parser(const std::string& in_format) {
	std::shared_lock<std::shared_mutex> guard{ m_mutex };

	// Search for the parser by key
	auto itr = m_parsers.find(in_format);
	if (itr != m_parsers.end()) {
		// Found our parser; return
		return itr->second;
	}

	// No parser exists for the given format
	return nullptr;
}

void parser_manager::clear() {
	std::lock_guard<std::shared_mutex> guard{ m_mutex };

	m_parsers.clear();
	m_registrations.clear();
}

/** Singleton */
parser_manager& parser_manager::instance() {
	static parser_manager s_parser_manager{};
	return s_parser_manager;
}

} // namespace jessilib
} // namespace jessilib
