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

/** synchronized_timer */

class synchonrized_callback {
public:
	synchonrized_callback(timer::function_t in_callback)
		: m_callback{ std::move(in_callback) },
		m_calls{ 0 } {
		// Empty ctor body
	}

	synchonrized_callback(const synchonrized_callback& in_callback)
		: m_callback{ in_callback.m_callback } {
		// Empty ctor body
	}

	synchonrized_callback(synchonrized_callback&& in_callback)
		: m_callback{ std::move(in_callback.m_callback) } {
		// Empty ctor body
	}

	void operator()(timer& in_timer) {
		cancel_detector detector = m_cancel_token;
		// Iterate calls
		if (++m_calls == 1) {
			// No other calls were queued; this should be safe.
			do {
				m_callback(in_timer);

				// callback may have cancelled the timer (and thus destructed this callback); check
				if (detector.expired()) {
					// We cannot access any members of this struct outside of the stack; return immediately
					return;
				}
			}
			while (--m_calls != 0);
		}
	}

private:
	timer::function_t m_callback;
	std::atomic<unsigned int> m_calls{ 0 };
	cancel_token m_cancel_token;
};

syncrhonized_timer::syncrhonized_timer(duration_t in_period, function_t in_callback)
	: timer{ in_period, synchonrized_callback{ std::move(in_callback) } } {
	// Empty ctor body
}

syncrhonized_timer::syncrhonized_timer(duration_t in_period, iterations_t in_iterations, function_t in_callback)
	: timer{ in_period, in_iterations, synchonrized_callback{ std::move(in_callback) } } {
	// Empty ctor body
}

} // namespace jessilib