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

#pragma once

#include <cstdint>
#include <memory>
#include <unordered_map>
#include <set>
#include <shared_mutex>

namespace jessilib {

class parser;

namespace impl {

class parser_manager {
public:
	/** Types */
	using id = size_t;

	/** Constants */
	static constexpr id bad_id = 0;

	/** Methods */
	id register_parser(std::shared_ptr<parser> in_parser, const std::string& in_format, bool in_force = false);
	void unregister_parser(id in_id);
	std::shared_ptr<parser> find_parser(const std::string& in_format);
	void clear();

	/** Singleton */
	static parser_manager& instance();

private:
	class registration {
	public:
		registration() = default;
		registration(const registration&) = default;
		registration(registration&&) = default;

		explicit registration(id in_id);
		explicit registration(const std::string& in_format);
		registration(id in_id, const std::string& in_format);

		registration& operator=(const registration&) = default;
		registration& operator=(registration&&) = default;

		bool operator<(const registration& rhs) const;

		id m_id{};
		std::string m_format;
	};

	std::shared_mutex m_mutex;
	id m_last_id{};
	std::set<registration> m_registrations; // This set and map could be condensed into a bimap
	std::unordered_map<std::string, std::shared_ptr<parser>> m_parsers;
}; // parser_manager

} // namespace impl
} // namespace jessilib