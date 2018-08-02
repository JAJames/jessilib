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

#include <stdexcept>
#include <iostream>

/** Macros */

#define STRINGIFY(arg) \
	#arg
#define STRINGIFY_HELPER(line) \
	STRINGIFY(line)

#ifndef JESSILIB_ASSERT_MESSAGE
#define JESSILIB_ASSERT_MESSAGE(expression) \
	"Failed assertion: '" #expression "' at " __FILE__ ":" STRINGIFY_HELPER(__LINE__)
#endif // JESSILIB_ASSERT_MESSAGE

// Returns a boolean expression indicating assertion success/failure
#ifndef jessilib_assert
#define jessilib_assert(expression) \
	(::jessilib::impl::_assert_helper( expression, JESSILIB_ASSERT_MESSAGE(expression) ))
#endif // jessilib_assert

// Similar to jessilib_assert, except exceptions are thrown only when NDEBUG is undefined
#ifndef jessilib_debug_assert
#define jessilib_debug_assert(expression) \
	(::jessilib::impl::_debug_assert_helper( expression, JESSILIB_ASSERT_MESSAGE(expression) ))
#endif // jessilib_debug_assert

namespace jessilib {

/** Exception type */
class assertion_failed : public std::logic_error {
public:
	inline explicit assertion_failed(const char* expression)
		: std::logic_error{ expression } {
	}
};

namespace impl {

/** Macro helpers */
inline bool _assert_helper(bool value, const char* message) {
	if (!value) {
		// TODO: wrap std::cerr
		std::cerr << message << std::endl;

		throw assertion_failed{message};
	}

	return value;
}

inline bool _debug_assert_helper(bool value, const char* message) {
	if (!value) {
		// TODO: wrap std::cerr
		std::cerr << message << std::endl;

#ifndef NDEBUG
		throw assertion_failed{message};
#endif // NDEBUG
	}

	return value;
}

} // namespace impl
} // namespace jessilib
