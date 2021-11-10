/**
 * Copyright (C) 2017-2018 Jessica James.
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

#include "impl/timer_manager.hpp"
#include "impl/timer_context.hpp"

namespace jessilib {
namespace impl {

/** timer_manager */

timer_manager& timer_manager::instance() {
	static timer_manager s_manager;
	return s_manager;
}

timer_manager::~timer_manager() {
	m_thread_active = false;
	m_cvar.notify_one();
	m_thread.join();
	m_pool.join();
	m_detached_timers.clear();
}

void timer_manager::loop() {
	// loop
	while (m_thread_active) {
		std::unique_lock<std::mutex> lock(m_mutex);
		auto itr{ m_active_timers.begin() };
		if (itr != m_active_timers.end()) {
			// Wait until the next timer is ready to fire
			if (m_cvar.wait_until(lock, (*itr)->next()) == std::cv_status::timeout
				&& itr == m_active_timers.begin() && itr != m_active_timers.end()) {

				// Due to a race condition, we may still receive timeout when another thread has notified m_cvar too late
				// Notifying the thread before releasing the lock does not resolve this, because wait_until's return
				// status may be based entirely on the time of return and the input time (as is the case in GCC 7.2)
				if (!is_timeout) {
					// itr may be invalidated; restart wait
					is_timeout = true;
					continue;
				}

				timer_context* context = *itr;

				// Reset timings
				m_active_timers.erase(itr);
				context->m_next = context->calc_next();
				m_active_timers.insert(context);

				// Push timer to execute
				std::weak_ptr<timer_context> weak_context = context->weak_from_this();
				m_pool.push([weak_context]() {
					if (auto context = weak_context.lock()) {
						// Execute timer
						// Locking will only fail when the timer is in the process of being cancelled
						std::shared_lock<std::shared_mutex> context_guard{ context->m_mutex, std::try_to_lock };
						if (context_guard.owns_lock() && !context->null()) {
							timer& callback_timer{thread_callback_timer()};
							callback_timer.m_context = context;

							// Call callback
							context->m_callback(callback_timer);

							// Release timer context
							callback_timer.m_context = nullptr;
						}
					}
				});
			}
			// else // m_active_timers changed; itr may be invalid; itr may not be the next timer
		}
		else {
			// Wait until a timer is added
			m_cvar.wait(lock);
		}
	}
}

timer& timer_manager::thread_callback_timer() {
	static thread_local timer callback_timer;
	return callback_timer;
}

} // namespace impl
} // namespace jessilib
