/**
 * Copyright (C) 2017-2021 Jessica James.
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

#include "timer.hpp"
#include <atomic>
#include "assert.hpp"
#include "impl/timer_manager.hpp"
#include "impl/timer_context.hpp"

namespace jessilib {

/** callback_with_iterations */

class callback_with_iterations {
public:
	callback_with_iterations(timer::iterations_t in_iterations, timer::function_t in_callback)
		: m_callback{ std::move(in_callback) },
		m_iterations{ in_iterations } {
		// Empty ctor body
	}

	callback_with_iterations(const callback_with_iterations& in_callback)
		: m_callback{ in_callback.m_callback },
		m_iterations{ in_callback.m_iterations.load() } {
		// Empty ctor body
	}

	callback_with_iterations(callback_with_iterations&& in_callback)
		: m_callback{ std::move(in_callback.m_callback) },
		m_iterations{ in_callback.m_iterations.load() } {
		// Empty ctor body
	}

	void operator()(timer& in_timer) {
		cancel_detector detector{ m_cancel_token };

		auto iterations = m_iterations.load();
		do {
			// Ensure timer is not already expired
			if (iterations == 0) {
				return;
			}

			// Decrement iterations and cancel if necessary
		} while (!m_iterations.compare_exchange_weak(iterations, iterations - 1));

		// Call callback
		m_callback(in_timer);

		if (!detector.expired()) {
			if (m_iterations == 0) {
				// Cancel the timer
				in_timer.cancel();
			}
		}
	}

private:
	timer::function_t m_callback;
	std::atomic<timer::iterations_t> m_iterations;
	cancel_token m_cancel_token;
};

/** timer */

timer::timer(duration_t in_period, function_t in_callback)
	: m_context{ new impl::timer_context{ in_period, std::move(in_callback) } } {
	// Add timer_context to timer_manager
	// Note: this logic must be here (rather than timer_context) to ensure timer_context is wrapped in a shared_ptr
	if (jessilib_debug_assert(in_period.count() != 0)
		&& jessilib_debug_assert(!m_context->null())) {
		// Add timer
		impl::timer_manager& manager = impl::timer_manager::instance();
		{
			std::lock_guard<std::mutex> lock(manager.m_mutex);
			manager.m_active_timers.insert(m_context.get());
		}

		// Notify
		manager.m_cvar.notify_one();
	}
}

timer::timer(duration_t in_period, iterations_t in_iterations, function_t in_callback)
	: timer{ in_period, callback_with_iterations(in_iterations, std::move(in_callback)) } {
	// Empty ctor body
}

bool timer::operator==(const timer& rhs) const {
	return m_context == rhs.m_context;
}

bool timer::operator!=(const timer& rhs) const {
	return !(rhs == *this);
}

timer::time_point_t timer::next() const {
	if (m_context != nullptr) {
		return m_context->next();
	}

	return {};
}

timer::duration_t timer::period() const {
	if (m_context != nullptr) {
		return m_context->period();
	}

	return {};
}

const timer::function_t& timer::function() const {
	if (m_context != nullptr) {
		return m_context->function();
	}

	// No context; return nullptr
	static timer::function_t s_null_function_t { nullptr };
	return s_null_function_t;
}

bool timer::null() const {
	return m_context == nullptr || m_context->null();
}

bool timer::detached() const {
	if (m_context != nullptr) {
		return m_context->detached();
	}

	return false;
}

void timer::detach() {
	if (m_context != nullptr) {
		m_context->detach();
	}
}

void timer::cancel() {
	if (m_context != nullptr) {
		m_context->cancel();
	}
}

} // namespace jessilib
