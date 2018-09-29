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

#pragma once

#include <vector>
#include <string>
#include <istream>
#include <filesystem>
#include <shared_mutex>
#include "object.hpp"

namespace jessilib {

class config {
public:
	/** Types */

	class file_error : public std::runtime_error {
	public:
		file_error();
		file_error(const std::filesystem::path& in_filename);
	};

	/** Accessors */
	object data() const;
	std::filesystem::path filename() const;
	std::string format() const;

	/** Modifiers */
	void set_data(const object& in_data);

	/** File I/O */
	void load(const std::filesystem::path& in_filename, const std::string& in_format = {});
	void reload();
	void write() const;
	void write(const std::filesystem::path& in_filename , const std::string& in_format = {});

	/** Static File I/O */
	static object read_object(const std::filesystem::path& in_filename, const std::string& in_format = {});
	static void write_object(const object& in_object, const std::filesystem::path& in_filename, const std::string& in_format = {});

	/** Static helpers */
	static std::string get_format(const std::filesystem::path& in_filename, const std::string& in_format = {});

private:
	mutable std::shared_mutex m_mutex;
	object m_data;
	std::string m_format;
	std::filesystem::path m_filename;
};

} // namespace jessilib