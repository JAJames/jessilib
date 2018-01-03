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
#include <cassert>
#include "impl/timer_manager.hpp"

namespace jessilib {

/** helper */
timer::function_t callback_with_iterations(timer::iterations_t iterations, timer::function_t callback) {
	assert(iterations > 0);

	return [iterations, callback](timer& timer) mutable {
		callback(timer);

		if (--iterations == 0) {
			timer.cancel();
		}
	};
}

/** timer */

timer::timer()
	: m_callback{ nullptr },
	m_self{ impl::timer_manager::instance().m_detached_timers.end() } {
	// Empty ctor body
}

timer::timer(timer&& in_timer)
	: m_period{ in_timer.m_period },
	m_next{ in_timer.m_next },
	m_callback{ std::move(in_timer.m_callback) },
	m_self{ impl::timer_manager::instance().m_detached_timers.end() } {

	// Cancel in_timer
	in_timer.m_callback = nullptr;

	// Replace active timer
	impl::timer_manager& manager = impl::timer_manager::instance();
	{
		std::lock_guard<std::mutex> lock(manager.m_mutex);
		manager.m_active_timers.erase(&in_timer); // remove in_timer
		manager.m_active_timers.insert(this); // insert this
	}

	// Notify
	manager.m_cvar.notify_one();
}

timer::timer(duration_t in_period, function_t in_callback)
	: m_period{ in_period },
	m_next{ calc_next() },
	m_callback{ in_callback },
	m_self{ impl::timer_manager::instance().m_detached_timers.end() } {
	// PROBLEM: timer may be executing while moving data????

	// Assertion checks
	assert(m_callback != nullptr);
	assert(m_period.count() != 0);

	// Add timer
	impl::timer_manager& manager = impl::timer_manager::instance();
	{
		std::lock_guard<std::mutex> lock(manager.m_mutex);
		manager.m_active_timers.insert(this);
	}

	// Notify
	manager.m_cvar.notify_one();
}

timer::timer(duration_t in_period, iterations_t in_iterations, function_t in_callback)
	: timer{ in_period, callback_with_iterations(in_iterations, in_callback) } {
	// Empty ctor body
}

timer& timer::operator=(timer&& in_timer) {
	impl::timer_manager& manager = impl::timer_manager::instance();

	m_period = in_timer.m_period;
	m_next = in_timer.m_next;
	m_callback = std::move(in_timer.m_callback);
	m_self = in_timer.m_self;
}

timer::~timer() {
	// If it's null, then it was either never added (default constructed) or already removed (as part of move)
	if (!null()) {
		cancel();
	}
}

timer::time_point_t timer::next() const {
	return m_next;
}

timer::duration_t timer::period() const {
	return m_period;
}

timer::function_t timer::function() const {
	return m_callback;
}

bool timer::null() const {
	return m_callback == nullptr;
}

bool timer::current() const {
	impl::timer_manager& manager = impl::timer_manager::instance();

	return manager.m_current_timer == this && manager.m_thread.get_id() == std::this_thread::get_id();
}

bool timer::detached() const {
	impl::timer_manager& manager = impl::timer_manager::instance();

	std::lock_guard<std::mutex> lock(manager.m_detached_timers_mutex);
	return m_self != manager.m_detached_timers.end();
}

void timer::detach() {
	impl::timer_manager& manager = impl::timer_manager::instance();

	assert(!current()); // you cannot detach a timer from within itself, because that would destroy the callback you're currently executing
	assert(!detached()); // you cannot detach a timer that is already detached
	std::lock_guard<std::mutex> lock(manager.m_detached_timers_mutex);
	manager.m_detached_timers.emplace_back(std::move(*this));
	--manager.m_detached_timers.back().m_self; // Is this a race condition?
}

void timer::cancel() {
	impl::timer_manager& manager = impl::timer_manager::instance();

	if (current()) {
		manager.m_active_timers.erase(this);
		return;
	}

	{
		std::lock_guard<std::mutex> lock(manager.m_mutex);
		manager.m_active_timers.erase(this);
	}
	manager.m_cvar.notify_one();
}

// Internals

timer::time_point_t timer::calc_next() {
	return std::chrono::steady_clock::now() + m_period;
}

} // namespace jessilib