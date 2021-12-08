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

#pragma once

#include <chrono>
#include <list>
#include <memory>
#include <functional>

namespace jessilib {

/** forward delcarations */
namespace impl {
	class timer_manager;
	class timer_context;
	struct cancel_token_context;
} // namespace impl

/** timer */

class timer {
public:
	// Types
	using function_t = std::function<void(timer&)>; /** Function type called by the timer */
	using time_point_t = std::chrono::steady_clock::time_point; /** Type representing the point in time at which a timer will be called */
	using duration_t = std::chrono::steady_clock::duration; /** Type representing the time between calls */
	using iterations_t = size_t; /** Type representing iterations */

	// Constructors
	timer() = default;
	timer(const timer& in_timer) = default;
	timer(timer&& in_timer) = default;
	timer(duration_t in_period, function_t in_callback);
	timer(duration_t in_period, iterations_t in_iterations, function_t in_callback);

	// Assignment/move operators
	timer& operator=(const timer&) = default;
	timer& operator=(timer&& in_timer) = default;

	// Comparison operators
	bool operator==(const timer& rhs) const;
	bool operator!=(const timer& rhs) const;

	// Accessors
	time_point_t next() const;
	duration_t period() const;
	const function_t& function() const;
	bool null() const;
	bool detached() const;

	// Mutators
	void detach();
	void cancel();

private:
	// Timer context block
	std::shared_ptr<impl::timer_context> m_context;

	// Friends
	friend impl::timer_manager;
	friend impl::timer_context;
}; // class timer

class syncrhonized_timer : public timer {
public:
	// Constructors
	syncrhonized_timer() = default;
	syncrhonized_timer(const syncrhonized_timer& in_timer) = default;
	syncrhonized_timer(syncrhonized_timer&& in_timer) = default;
	syncrhonized_timer(duration_t in_period, function_t in_callback);
	syncrhonized_timer(duration_t in_period, iterations_t in_iterations, function_t in_callback);

	// Assignment/move operators
	syncrhonized_timer& operator=(const syncrhonized_timer&) = default;
	syncrhonized_timer& operator=(syncrhonized_timer&& in_timer) = default;
};

/** Useful when performing actions within a timer which may destroy the timer's callback */

class cancel_token {
public:
	cancel_token();
	cancel_token(cancel_token&& in_token);
	~cancel_token();

private:
	friend class cancel_detector;
	impl::cancel_token_context* m_context;
};

class cancel_detector {
public:
	cancel_detector(const cancel_token& in_token);
	~cancel_detector();
	bool expired() const;

private:
	impl::cancel_token_context* m_context;
};

} // namespace jessilib