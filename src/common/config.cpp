/**
 * Copyright (C) 2018 Jessica James.
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

#include "config.hpp"
#include <cstring>
#include <fstream>
#include "assert.hpp"
#include "serialize.hpp"

namespace jessilib {

/** Error types */

config::file_error::file_error()
	: std::runtime_error{ std::strerror(errno) } {
	// Empty ctor body
}

config::file_error::file_error(const std::filesystem::path& in_filename)
	: std::runtime_error{ "Error openening file \"" + std::string{ in_filename } + "\". Error: " + std::strerror(errno) } {
	// Empty ctor body
};

/** config */

object config::data() const {
	std::shared_lock<std::shared_mutex> guard{ m_mutex };
	return m_data;
}

std::filesystem::path config::filename() const {
	std::shared_lock<std::shared_mutex> guard{ m_mutex };
	return m_filename;
}

std::string config::format() const {
	std::shared_lock<std::shared_mutex> guard{ m_mutex };
	return m_format;
}

/** Modifiers */
void config::set_data(const object& in_data) {
	std::lock_guard<std::shared_mutex> guard{ m_mutex };
	m_data = in_data;
}

/** File I/O */
void config::load(const std::filesystem::path& in_filename, const std::string& in_format) {
	jessilib_assert(!in_filename.empty());
	std::lock_guard<std::shared_mutex> guard{ m_mutex };

	// Determine format
	m_filename = in_filename;
	m_format = get_format(m_filename, in_format);

	// Load
	m_data = read_object(m_filename, m_format);
}

void config::reload() {
	std::lock_guard<std::shared_mutex> guard{ m_mutex };

	if (jessilib_debug_assert(!m_filename.empty())
		&& jessilib_debug_assert(!m_format.empty())) {
		// Load data from disk
		m_data = read_object(m_filename, m_format);
	}
}

void config::write() const {
	std::shared_lock<std::shared_mutex> guard{ m_mutex };

	if (jessilib_debug_assert(!m_filename.empty())
		&& jessilib_debug_assert(!m_format.empty())) {
		// Write data to disk
		write_object(m_data, m_filename, m_format);
	}
}

void config::write(const std::filesystem::path& in_filename , const std::string& in_format) {
	jessilib_assert(!in_filename.empty());
	std::lock_guard<std::shared_mutex> guard{ m_mutex };

	// Setup
	m_filename = in_filename;
	m_format = get_format(m_filename, in_format);

	// Write
	write_object(m_data, m_filename, m_format);
}

/** Static File I/O */
object config::read_object(const std::filesystem::path& in_filename, const std::string& in_format) {
	// Open up file for reading
	std::ifstream file{ in_filename, std::ios::in | std::ios::binary };
	if (!file) {
		// Failed to open the file; throw file_error
		throw file_error( in_filename );
	}

	// Deserialize1
	return deserialize_object(file, in_format);
}

void config::write_object(const object& in_object, const std::filesystem::path& in_filename, const std::string& in_format) {
	// Open up file for writing
	std::ofstream file{ in_filename, std::ios::out | std::ios::binary };
	if (!file) {
		// Failed to open the file; throw file_error
		throw file_error( in_filename );
	}

	// Deserialize1
	return serialize_object(file, in_object, in_format);
}

std::string get_format(const std::filesystem::path& in_filename, const std::string& in_format) {
	if (!in_format.empty()) {
		// Use specified format
		return in_format;
	}

	// Try to determine format from filename
	std::string extension = in_filename.extension();
	if jessilib_assert(!extension.empty()) {
		return extension.substr(1);
	}

	return extension;
}

} // namespace jessilib
