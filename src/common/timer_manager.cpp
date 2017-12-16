/**
 * Copyright (C) 2017 Jessica James.
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
}

void timer_manager::loop() {
	// loop
	while (m_thread_active) {
		std::unique_lock<std::mutex> lock(m_mutex);
		auto itr{m_active_timers.begin()};
		if (itr != m_active_timers.end()) {
			// Wait until the next timer is ready to fire
			if (m_cvar.wait_until(lock, (*itr)->next()) == std::cv_status::timeout && itr != m_active_timers.end()) {
				// Set active
				timer* target = *itr;
				m_current_timer = target;
				bool detached = is_detached(*target);

				// Execute timer
				target->m_callback(*target);

				// Check if timer still exists
				itr = m_active_timers.begin();
				if (itr != m_active_timers.end() && *itr == target) {
					// Timer still exists; extract
					m_active_timers.erase(itr);

					// reset timings
					target->m_next = target->calc_next();

					// push
					m_active_timers.insert(target);
				}
				else if (detached) {
					// Timer has been canceled; remove it from the detached timers list
					std::lock_guard<std::mutex> lock(m_detached_timers_mutex);
					m_detached_timers.erase(target->m_self);
				}

				// Reset active
				m_current_timer = nullptr;
			}
			// else // m_active_timers changed; itr may be invalid; itr may not be the next timer
		}
		else {
			// Wait until a timer is added
			m_cvar.wait(lock);
		}
	}
}

bool timer_manager::is_current(timer& in_timer) {
	return m_current_timer == &in_timer && m_thread.get_id() == std::this_thread::get_id();
}

bool timer_manager::is_detached(timer& in_timer) {
	std::lock_guard<std::mutex> lock(m_detached_timers_mutex);
	return in_timer.m_self != m_detached_timers.end();
}

} // namespace impl
} // namespace jessilib
