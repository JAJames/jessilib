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

#include <atomic>
#include "timer.hpp"

namespace jessilib {

/** cancel_token_context */

namespace impl {
struct cancel_token_context {
	bool m_expired{ false };
	std::atomic<size_t> m_weak_reference_count{ 0 };
};
} // namespace impl

/** cancel_token */

cancel_token::cancel_token()
	: m_context{ new impl::cancel_token_context } {
	// Empty ctor body
};

cancel_token::cancel_token(cancel_token&& in_token)
	: m_context{ in_token.m_context } {
	in_token.m_context = nullptr;
}

cancel_token::~cancel_token() {
	if (m_context != nullptr) {
		m_context->m_expired = true;

		if (m_context->m_weak_reference_count == 0) {
			delete m_context;
		}
	}
}

/** cancel_detector */

cancel_detector::cancel_detector(const cancel_token& in_token)
	: m_context{ in_token.m_context } {
	// Increment reference count
	++m_context->m_weak_reference_count;
}

cancel_detector::~cancel_detector() {
	// Decrement reference count
	if (--m_context->m_weak_reference_count == 0 && m_context->m_expired) {
		// No other references exist to the context block; delete it
		delete m_context;
	}
}

bool cancel_detector::expired() const {
	return m_context->m_expired;
};

} // namespace jessilib