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

#include <chrono>
#include <string>
#include <string_view>

namespace jessilib {

namespace impl {

constexpr const char* suffix_helper(const char* begin, const char* end, const char* ignored_suffix) {
	while (begin != end
		&& *begin == *ignored_suffix) {
		++begin;
		++ignored_suffix;
	}

	return begin;
}

} // namespace impl

// Base 10 only
// y = year (31556952 seconds == 365.2425 day)
// mo = month (2629746 seconds == year/12)
// w = weeks
// d[ay] = days
// h[r] = hours
// m[in] = minutes
// s = seconds
// ms = milliseconds
// us = microseconds
// ns = nanoseconds
// trailing: DurationT

template<typename DurationT = std::chrono::seconds>
struct duration_from_string_result {
	const char* itr{};
	DurationT duration{};
};

// At time of writing, days, weeks, months, years seem inconsistently available
using days = std::chrono::duration<std::chrono::hours::rep, std::ratio<86400>>;
using weeks = std::chrono::duration<std::chrono::hours::rep, std::ratio<604800>>;
using months = std::chrono::duration<std::chrono::hours::rep, std::ratio<2629746>>;
using years = std::chrono::duration<std::chrono::hours::rep, std::ratio<31556952>>;

template<typename DurationT = std::chrono::seconds>
constexpr duration_from_string_result<DurationT> duration_from_string(const char* begin, const char* end) {
	duration_from_string_result<DurationT> result{ begin, {} };
	auto& itr = result.itr;

	// Sanity check
	if (end <= itr) {
		return result;
	}

	while (itr != end) {
		// Advance through any whitespace preceding type specifier
		while (*itr == ' ') {
			++itr;

			if (itr == end) {
				// no text remains
				return result;
			}
		}

		typename DurationT::rep segment{};

		// Parse duration segment (i.e: [0-9]+[ms][]
		while (*itr >= '0' && *itr <= '9') {
			segment *= 10;
			segment += *itr - '0';
			++itr;

			if (itr == end) {
				// end of string; always treat ending digits as base unit (DurationT)
				result.duration += DurationT{ segment };
				return result;
			}
		}

		// Advance past any whitespace preceding type specifier
		while (*itr == ' ') {
			++itr;

			if (itr == end) {
				// end of string; always treat ending digits as base unit (DurationT)
				result.duration += DurationT{ segment };
				return result;
			}
		}

		// after switch, 'itr' shall point to first unparsed character, or end
		switch (*itr) {
			case 'y': // year
				++itr; // 'y'
				result.duration += std::chrono::duration_cast<DurationT>(years{ segment });

				// "yr"
				if (itr != end
					&& *itr == 'r') {
					// ignore trailing 'r'
					++itr;
					break;
				}

				// "year"
				itr = impl::suffix_helper(itr, end, "ear");
				break;

			case 'w': // week
				++itr; // 'w'
				result.duration += std::chrono::duration_cast<DurationT>(weeks{ segment });

				if (itr != end
					&& *itr == 'k') {
					// "wk"
					++itr;
					break;
				}

				// "week"
				itr = impl::suffix_helper(itr, end, "eek");
				break;

			case 'd': // day
				++itr; // 'd'
				result.duration += std::chrono::duration_cast<DurationT>(days{ segment });
				itr = impl::suffix_helper(itr, end, "ay");
				break;

			case 'h': // hour
				++itr; // 'h'
				result.duration += std::chrono::duration_cast<DurationT>(std::chrono::hours{ segment });

				if (itr != end
					&& *itr == 'r') {
					// "hr"
					++itr;
					break;
				}

				// "hour"
				itr = impl::suffix_helper(itr, end, "our");
				break;

			case 'm': // min/month/milli
				++itr; // 'm'

				if (itr == end) {
					// "m" end of string; treat as minutes
					result.duration += std::chrono::duration_cast<DurationT>(std::chrono::minutes{ segment });
					return result;
				}

				if (*itr == 'i') {
					// "minutes" or "milliseconds"
					++itr;

					if (itr != end
						&& *itr == 'n') {
						// minutes
						result.duration += std::chrono::duration_cast<DurationT>(std::chrono::minutes{ segment });
						itr = impl::suffix_helper(itr, end, "nutes");
						break;
					}

					// anything not starting with "min", assume milliseconds
					result.duration += std::chrono::duration_cast<DurationT>(std::chrono::milliseconds{ segment });
					itr = impl::suffix_helper(itr, end, "lliseconds");
					break;
				}

				if (*itr == 's') {
					// ms
					result.duration += std::chrono::duration_cast<DurationT>(std::chrono::milliseconds{ segment });
					++itr;
					break;
				}

				if (*itr == 'o') {
					// mo
					result.duration += std::chrono::duration_cast<DurationT>(months{ segment });
					++itr;

					// "month"
					itr = impl::suffix_helper(itr, end, "nth");
					break;
				}

				// Valid scenario for "m "; invalid specifier otherwise
				break;

			case 's':
				++itr; // 's'
				result.duration += std::chrono::duration_cast<DurationT>(std::chrono::seconds{ segment });
				itr = impl::suffix_helper(itr, end, "econd");
				break;

			case 'u': // micro
				++itr; // 'u'
				result.duration += std::chrono::duration_cast<DurationT>(std::chrono::microseconds{ segment });
				itr = impl::suffix_helper(itr, end, "s");
				break;

			case 'n': // nano
				++itr; // 'n'
				result.duration += std::chrono::duration_cast<DurationT>(std::chrono::nanoseconds{ segment });
				itr = impl::suffix_helper(itr, end, "s");
				break;

			default:
				// unexpected specifier; assume DurationT and cease parsing
				result.duration += DurationT{ segment };
				return result;
		}

		if (itr == end) {
			return result;
		}

		// Remove any trailing 's' at end of unit (i.e: years, months, etc)
		if (*itr == 's') {
			++itr;
		}
	}

	return result;
}

template<typename DurationT = std::chrono::seconds>
constexpr duration_from_string_result<DurationT> duration_from_string(const std::string_view& in_duration) {
	return duration_from_string<DurationT>(in_duration.data(), in_duration.data() + in_duration.size());
}

namespace literals {

constexpr std::chrono::nanoseconds operator ""_duration_ns(const char* in_str, size_t in_str_length) {
	return duration_from_string<std::chrono::nanoseconds>(in_str, in_str + in_str_length).duration;
}

constexpr std::chrono::microseconds operator ""_duration_us(const char* in_str, size_t in_str_length) {
	return duration_from_string<std::chrono::microseconds>(in_str, in_str + in_str_length).duration;
}

constexpr std::chrono::milliseconds operator ""_duration_ms(const char* in_str, size_t in_str_length) {
	return duration_from_string<std::chrono::milliseconds>(in_str, in_str + in_str_length).duration;
}

constexpr std::chrono::seconds operator ""_duration_s(const char* in_str, size_t in_str_length) {
	return duration_from_string<std::chrono::seconds>(in_str, in_str + in_str_length).duration;
}

constexpr std::chrono::minutes operator ""_duration_m(const char* in_str, size_t in_str_length) {
	return duration_from_string<std::chrono::minutes>(in_str, in_str + in_str_length).duration;
}

constexpr std::chrono::hours operator ""_duration_h(const char* in_str, size_t in_str_length) {
	return duration_from_string<std::chrono::hours>(in_str, in_str + in_str_length).duration;
}

constexpr days operator ""_duration_d(const char* in_str, size_t in_str_length) {
	return duration_from_string<days>(in_str, in_str + in_str_length).duration;
}

constexpr weeks operator ""_duration_w(const char* in_str, size_t in_str_length) {
	return duration_from_string<weeks>(in_str, in_str + in_str_length).duration;
}

constexpr months operator ""_duration_mo(const char* in_str, size_t in_str_length) {
	return duration_from_string<months>(in_str, in_str + in_str_length).duration;
}

constexpr years operator ""_duration_y(const char* in_str, size_t in_str_length) {
	return duration_from_string<years>(in_str, in_str + in_str_length).duration;
}

} // namespace literals
} // namespace jessilib
