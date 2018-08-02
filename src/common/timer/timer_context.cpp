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

#include "impl/timer_context.hpp"
#include "impl/timer_manager.hpp"
#include "assert.hpp"

namespace jessilib {
namespace impl {

timer_context::timer_context(duration_t in_period, function_t in_callback)
	: m_period{ in_period },
	// m_next{ calc_next() },
	m_callback{ std::move(in_callback) },
	m_self{ timer_manager::instance().m_detached_timers.end() },
	m_last_fire_finish{ std::chrono::steady_clock::now() } {
	// Empty ctor body
}

timer_context::~timer_context() {
	// All references to the context have been destroyed
	if (!null()) {
		cancel();
	}
}

timer_context::time_point_t timer_context::next() const {
	return m_next;
}

timer_context::duration_t timer_context::period() const {
	return m_period;
}

const timer::function_t& timer_context::function() const {
	return m_callback;
}

bool timer_context::null() const {
	return m_callback == nullptr;
}

bool timer_context::detached() const {
	impl::timer_manager& manager = impl::timer_manager::instance();
	return m_self != manager.m_detached_timers.end();
}

void timer_context::detach() {
	if (!null()) {
		impl::timer_manager& manager = impl::timer_manager::instance();
		std::list<std::shared_ptr<timer_context>>& detached_timers = manager.m_detached_timers;

		jessilib_debug_assert(!detached()); // you cannot detach a timer that is already detached
		std::lock_guard<std::mutex> lock(manager.m_detached_timers_mutex);
		// We need to attach this new shared_ptr to the callback
		m_self = detached_timers.emplace(detached_timers.end(), shared_from_this());
	}
}

void timer_context::cancel() {
	impl::timer_manager& manager = impl::timer_manager::instance();

	// Unlock mutex if it's currently being held by this thread
	bool needs_shared_lock = false;
	if (manager.thread_callback_timer().m_context.get() == this) {
		needs_shared_lock = true;
		m_mutex.unlock_shared();
	}

	{
		std::lock_guard<std::mutex> manager_guard(manager.m_mutex);
		std::lock_guard<std::shared_mutex> context_guard(m_mutex);

		if (!null()) {
			// Remove from active timers
			manager.m_active_timers.erase(this);

			// Nullify timer
			m_callback = nullptr;

			// Remove from detached timers (if it's detached)
			if (detached()) {
				std::lock_guard<std::mutex> detached_timers_lock(manager.m_detached_timers_mutex);
				manager.m_detached_timers.erase(m_self);
			}
		}

		manager.is_timeout = false;
	}

	// Re-lock mutex
	if (needs_shared_lock) {
		m_mutex.lock_shared();
	}

	manager.m_cvar.notify_one();
}

timer_context::time_point_t timer_context::calc_next() {
	return std::chrono::steady_clock::now() + m_period;
}

} // namespace impl
} // namespace jessilib