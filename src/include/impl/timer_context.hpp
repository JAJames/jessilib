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

#pragma once

#include <atomic>
#include <shared_mutex>
#include "timer.hpp"

namespace jessilib {
namespace impl {

class timer_context : public std::enable_shared_from_this<timer_context> {
public:
	// Destructor (needs to be public for shared_ptr)
	~timer_context();

private:
	// Types
	using function_t = timer::function_t; /** Function type called by the timer */
	using time_point_t = timer::time_point_t; /** Type representing the point in time at which a timer will be called */
	using duration_t = timer::duration_t; /** Type representing the time between calls */

	// Constructor
	timer_context(duration_t in_period, function_t in_callback);

	// Explicitly delete all implicit constructors/assignment operators
	timer_context() = delete;
	timer_context(const timer_context&) = delete;
	timer_context(timer_context&&) = delete;
	timer_context& operator=(const timer_context&) = delete;
	timer_context& operator=(timer_context&&) = delete;

	// Accessors
	time_point_t next() const;
	duration_t period() const;
	const function_t& function() const;
	bool null() const;
	bool detached() const;
	time_point_t calc_next();

	// Mutators
	void detach();
	void cancel();

	// Members
	duration_t m_period;
	function_t m_callback;
	time_point_t m_next;
	std::list<std::shared_ptr<timer_context>>::iterator m_self; // Necessary to cancel detached timers, and implement detached()
	std::shared_mutex m_mutex;
	std::chrono::steady_clock::time_point m_last_fire_finish; // Set and accessed only from timer_manager

	// Friends
	friend timer;
	friend timer_manager;
};

}
}