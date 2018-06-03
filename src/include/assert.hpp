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

/** Macros */

#define STRINGIFY(arg) \
	#arg
#define STRINGIFY_HELPER(line) \
	STRINGIFY(line)

// Returns a boolean expression indicating assertion success/failure
#define jessilib_assert(expression) \
	(::jessilib::impl::_assert_helper( expression, "Failed assertion: '" #expression "' at " __FILE__ ":" STRINGIFY_HELPER(__LINE__) ))

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
inline bool _assert_helper(bool value, [[maybe_unused]] const char* message) {
#ifndef NDEBUG
	if (!value) {
		throw assertion_failed{message};
	}
#endif // NDEBUG

	return value;
}

} // namespace impl
} // namespace jessilib

// Provides for disabling of assertions; will likely produce warnings
#ifdef DISABLE_ASSERTIONS

// Disable jessilib_assert
#undef jessilib_assert
#define jessilib_assert(expression) \
	(true)

#endif // DISABLE_ASSERTIONS
