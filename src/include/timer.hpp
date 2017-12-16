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

#pragma once

#include <chrono>
#include <list>
#include <memory>
#include <functional>

namespace jessilib {

/** forward delcarations */
namespace impl {
	class timer_manager;
} // namespace impl

/** timer */

class timer {
public:
	// Types
	using function_t = std::function<void(timer&)>; /** Function type called by the timer */
	using time_point_t = std::chrono::steady_clock::time_point; /** Type representing the point in time at which a timer will be called */
	using duration_t = std::chrono::steady_clock::duration;
	using iterations_t = size_t;

	enum class state {
		active, // this timer is still active
		processing, // this timer is processing right now
		null // this timer is inactive
	};

	// Constructors
	timer();
	timer(timer&& in_timer);
	timer(duration_t in_period, function_t in_callback);
	timer(duration_t in_period, iterations_t in_iterations, function_t in_callback);

	// Move operator
	timer& operator=(timer&& in_timer);

	// Explicitly deleted copy constructor and assignment operator
	timer(const timer&) = delete;
	timer& operator=(const timer&) = delete;

	// Destructor
	~timer();

	// Accessors
	time_point_t next() const;
	duration_t period() const;
	function_t function() const;
	bool null() const;

	// Mutators
	void detach();
	void cancel();

private:
	// possible states: null, active, processing, cancelled (still processing)

	// Internal helpers
	time_point_t calc_next();

	// Members
	duration_t m_period;
	function_t m_callback;
	time_point_t m_next;
	std::list<timer>::iterator m_self;

	// Friends
	friend impl::timer_manager;
}; // class timer

} // namespace jessilib