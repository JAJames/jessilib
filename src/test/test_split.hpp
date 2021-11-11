/**
 * Copyright (C) 2021 Jessica James.
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

#include "jessilib/split.hpp"
#include "test.hpp"
#include <random>

template<typename T>
constexpr T default_delim{};

template<typename T, typename ResultT = std::basic_string<T>, typename DelimT,
	typename std::enable_if_t<!std::is_scalar_v<DelimT>>* = nullptr>
ResultT make_word(DelimT delim, size_t length = 8) {
	ResultT result;

	if (length == 0) {
		return result;
	}

	if (delim.size() == 0) {
		delim.push_back(default_delim<T>);
	}

	// Add initial character
	{
		auto chr = delim.back() + 1;
		while (std::find(delim.begin(), delim.end(), chr) != delim.end()) {
			++chr;
		}
		result.push_back(chr);
	}

	while (result.size() < length) {
		auto chr = result.back() + 1;
		while (std::find(delim.begin(), delim.end(), chr) != delim.end()) {
			++chr;
		}
		result.push_back(chr);
	}

	if (result.size() != length) {
		std::string errmsg = std::to_string(result.size()) + " != " + std::to_string(length) + "; result.size() != length";
		throw std::runtime_error{ errmsg };
	}

	return result;
}

template<typename T, typename ResultT = std::basic_string<T>>
ResultT make_word(size_t length = 8, T delim = default_delim<T>) {
	return make_word<T, ResultT>(std::basic_string<T>{ delim }, length);
}

template<typename T, typename ResultT = std::basic_string<T>>
ResultT make_delim_long(size_t length = 8, T in_delim = default_delim<T>) {
	// in this context, in_delim should be whatever was previously passed to make_word
	// doing so will ensure the value returned by this method is distinctly different from others
	ResultT result;
	result += in_delim;
	result += make_word<T>(length, in_delim);
	result += in_delim;
	return result;
}

template<typename T>
std::basic_string_view<T> make_word_view(size_t length = 8, T delim = default_delim<T>) {
	static std::basic_string<T> s_result;
	s_result = make_word(length, delim);
	return s_result;
}

template<typename T, typename StringT = std::basic_string<T>>
struct RandomTestData {
	RandomTestData(T in_delim = default_delim<T>, size_t in_fixed_word_count = 0, size_t in_fixed_word_length = 0)
		: m_fixed_word_count{ in_fixed_word_count },
		m_fixed_word_length{ in_fixed_word_length } {
		m_delim.insert(m_delim.end(), in_delim);
		populate();
	}

	RandomTestData(StringT in_delim, size_t in_fixed_word_count = 0, size_t in_fixed_word_length = 0)
		: m_delim{ in_delim },
		m_fixed_word_count{ in_fixed_word_count },
		m_fixed_word_length{ in_fixed_word_length } {
		populate();
	}

	void populate() {
		m_tokens.clear();
		m_str.clear();

		std::mt19937 randgen(static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count()));
		std::uniform_int_distribution<uint32_t> word_count_distribution(5, 64);
		std::uniform_int_distribution<uint32_t> word_length_distribution(0, 16);

		auto word_count = m_fixed_word_count;
		if (word_count == 0) {
			word_count = word_count_distribution(randgen);
		}

		while (m_tokens.size() < word_count) {
			auto word_length = m_fixed_word_length;
			if (word_length == 0) {
				word_length = word_length_distribution(randgen);
			}
			m_tokens.push_back(make_word<T, StringT>(word_length, m_delim.back()));
			m_str.insert(m_str.end(), m_tokens.back().begin(), m_tokens.back().end());
			if (m_tokens.size() < word_count) {
				m_str.insert(m_str.end(), m_delim.begin(), m_delim.end());
			}
		}
	}

	StringT get_remainder(size_t in_times_split) {
		StringT result;
		while (in_times_split < m_tokens.size()) {
			auto& token = m_tokens[in_times_split];
			result.insert(result.end(), token.begin(), token.end());
			++in_times_split;
			if (in_times_split < m_tokens.size()) {
				result.insert(result.end(), m_delim.begin(), m_delim.end());
			}
		}

		return result;
	}

	// Inputs
	StringT m_delim;
	size_t m_fixed_word_count{};
	size_t m_fixed_word_length{};

	// Outputs
	StringT m_str;
	std::vector<StringT> m_tokens;
};

template<typename T, typename StringT = std::basic_string<T>>
struct RandomWordTestData {
	RandomWordTestData(T in_delim = default_delim<T>, size_t in_fixed_word_count = 0, size_t in_fixed_word_length = 0)
		: m_fixed_word_count{ in_fixed_word_count },
		m_fixed_word_length{ in_fixed_word_length } {
		m_delim.insert(m_delim.end(), in_delim);
		populate();
	}

	RandomWordTestData(StringT in_delim, size_t in_fixed_word_count = 0, size_t in_fixed_word_length = 0)
	: m_delim{ in_delim },
	m_fixed_word_count{ in_fixed_word_count },
	m_fixed_word_length{ in_fixed_word_length } {
		populate();
	}

	void populate() {
		m_tokens.clear();
		m_str.clear();

		std::mt19937 randgen(static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count()));
		std::uniform_int_distribution<uint32_t> word_count_distribution(5, 64);
		std::uniform_int_distribution<uint32_t> word_length_distribution(0, 16);

		auto word_count = m_fixed_word_count;
		if (word_count == 0) {
			word_count = word_count_distribution(randgen);
		}

		while (m_tokens.size() < word_count) {
			auto word_length = m_fixed_word_length;
			if (word_length == 0) {
				word_length = word_length_distribution(randgen);
			}
			m_tokens.push_back(make_word<T, StringT>(m_delim, word_length));
			if (m_tokens.back().empty()) {
				m_tokens.pop_back();
			}
			else {
				m_token_indexes.push_back(m_str.size());
				m_str.insert(m_str.end(), m_tokens.back().begin(), m_tokens.back().end());
			}
			m_str.insert(m_str.end(), m_delim.begin(), m_delim.end());
		}
	}

	StringT get_remainder(size_t in_times_split) {
		StringT result;
		if (in_times_split < m_tokens.size()) {
			auto index = m_token_indexes[in_times_split];
			result.insert(result.end(), m_str.begin() + index, m_str.end());
		}

		return result;
	}

	// Inputs
	StringT m_delim;
	size_t m_fixed_word_count{};
	size_t m_fixed_word_length{};

	// Outputs
	StringT m_str;
	std::vector<StringT> m_tokens;
	std::vector<size_t> m_token_indexes;
};
