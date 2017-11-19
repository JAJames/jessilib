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

#include "timer.hpp"
#include <iostream>

namespace jessilib {

/** timer::manager */

timer::manager timer::s_manager;

timer::manager::~manager() {
	m_thread_active = false;
	m_cvar.notify_one();
	m_thread.join();
}

void timer::manager::loop() {
	// loop
	while (m_thread_active) {
		std::unique_lock<std::mutex> lock(m_mutex);
		auto itr{ m_timers.begin() };
		if (itr != m_timers.end()) {
			// Wait until the next timer is ready to fire
			if (m_cvar.wait_until(lock, (*itr)->next()) == std::cv_status::timeout && itr != m_timers.end()) {
				// Set active
				std::shared_ptr<timer::node> target = *itr;
				m_active = target.get();

				// Execute timer
				target->m_callback(*target);
				itr = m_timers.begin();
				if (itr != m_timers.end() && *itr == target) {
					// Timer still exists; pop
					m_timers.erase(itr);

					// reset timings
					target->m_next = target->calc_next();

					// push
					m_timers.insert(std::move(target));
				}

				// Reset active
				m_active = nullptr;
			}
		}
		else {
			// Wait until a timer is added
			m_cvar.wait(lock);
		}
	}
}

/** timer::node */
timer::node::node(duration_t period, callback_t callback)
	: m_period{ period },
	m_next{ calc_next() },
	m_callback{ callback } {
	// Empty ctor body
}

timer::time_point_t timer::node::next() const {
	return m_next;
}

timer::duration_t timer::node::period() const {
	return m_period;
}

timer::callback_t timer::node::function() const {
	return m_callback;
}

void timer::node::cancel() {
	// Race condition: cancel() being called externally while active
	if (s_manager.m_active == this && s_manager.m_thread.get_id() == std::this_thread::get_id()) {
		s_manager.m_timers.erase(shared_from_this());
		return;
	}

	{
		std::lock_guard<std::mutex> lock(s_manager.m_mutex);
		s_manager.m_timers.erase(shared_from_this());
	}
	s_manager.m_cvar.notify_one();
}

// Internals

timer::time_point_t timer::node::calc_next() {
	return std::chrono::steady_clock::now() + m_period;
}

/** timer */

// Static functions
void timer::set(duration_t period, callback_t callback) {
	// Emplace timer
	{
		std::lock_guard<std::mutex> lock(s_manager.m_mutex);
		s_manager.m_timers.emplace(new node(period, callback));
	}

	// Notify
	s_manager.m_cvar.notify_one();
}

void timer::set(duration_t period, size_t iterations, callback_t callback) {
	// Expand callback and forward to set
	set(period, callback_with_iterations(iterations, callback));
}

// Constructors
timer::timer(duration_t period, callback_t callback) {
	// Sanity check
	assert(callback != nullptr);

	// Initialize timer
	{
		std::lock_guard<std::mutex> lock(s_manager.m_mutex);
		m_timer = *s_manager.m_timers.emplace(new node(period, callback));
	}

	// Notify
	s_manager.m_cvar.notify_one();
}

timer::timer(duration_t period, size_t iterations, callback_t callback)
	: timer{ period, callback_with_iterations(iterations, callback) } {
	// Empty ctor body
}

// Destructor
timer::~timer() {
	if (auto my_timer = m_timer.lock()) {
		my_timer->cancel();
	}
}

// Functions
timer::time_point_t timer::next() const {
	if (auto my_timer = m_timer.lock()) {
		my_timer->next();
	}

	return {};
}


timer::duration_t timer::period() const {
	if (auto my_timer = m_timer.lock()) {
		return my_timer->period();
	}

	return duration_t{ 0 };
}

timer::callback_t timer::function() const {
	if (auto my_timer = m_timer.lock()) {
		return my_timer->function();
	}

	return nullptr;
}

void timer::cancel() {
	if (auto my_timer = m_timer.lock()) {
		my_timer->cancel();
	}
}

bool timer::active() const {
	return m_timer.lock() != nullptr;
}

timer::callback_t timer::callback_with_iterations(size_t iterations, callback_t callback) {
	assert(iterations > 0);

	return [iterations, callback](node& timer) mutable {
		callback(timer);

		if (--iterations == 0) {
			timer.cancel();
		}
	};
}

} // namespace jessilib