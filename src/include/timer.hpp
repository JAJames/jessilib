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

#include <cassert>
#include <mutex>
#include <chrono>
#include <set>
#include <condition_variable>
#include <atomic>
#include <memory>

// TODO: add 'timer* m_parent' to timer::node; rename timer::node to timer::internals?

namespace jessilib {

/** timer */

class timer {
public:
	class node;

	// Types
	using callback_t = std::function<void(node&)>; /** Function type called by the timer */
	using time_point_t = std::chrono::steady_clock::time_point; /** Type representing the point in time at which a timer will be called */
	using duration_t = std::chrono::steady_clock::duration;

	// Static functions
	static void set(duration_t period, callback_t callback);
	static void set(duration_t period, size_t iterations, callback_t callback);

	// Constructors
	timer(duration_t period, callback_t callback);
	timer(duration_t period, size_t iterations, callback_t callback);

	// Explicitly deleted constructors
	timer() = delete;
	timer(const timer&) = delete;
	timer(timer&& in_timer) = delete;

	// Destructor
	~timer();

	// Functions
	time_point_t next() const;
	duration_t period() const;
	callback_t function() const;
	void cancel();
	bool active() const;

private:
	// Weak pointer to referenced timer
	std::weak_ptr<node> m_timer;

	// Timer manager
	class manager;
	static manager s_manager;

	// Helper
	static callback_t callback_with_iterations(size_t iterations, callback_t callback);
}; // timer

/** timer::node */

class timer::node : public std::enable_shared_from_this<node> {
public:
	time_point_t next() const;
	duration_t period() const;
	callback_t function() const;
	void cancel();

private:
	// Internal constructor
	node(duration_t period, callback_t callback);

	// Explicitly deleted constructors
	node() = delete;
	node(const node&) = delete;
	node(node&& in_timer) = delete;

	// Internal helpers
	time_point_t calc_next();

	// Members
	duration_t m_period;
	callback_t m_callback;
	time_point_t m_next;

	// Friends
	friend class timer;
}; // class timer::node


/** timer::manager */

class timer::manager {
	// Destructor
	~manager();

	struct timer_sort {
		bool operator()(const std::shared_ptr<node>& lhs, const std::shared_ptr<node>& rhs) const {
			return lhs->next() < rhs->next();
		}
	};

	// Loop
	void loop();

	// Members
	std::multiset<std::shared_ptr<node>, timer_sort> m_timers;
	std::mutex m_mutex;
	std::condition_variable m_cvar;
	std::atomic<node*> m_active{ nullptr };
	std::atomic<bool> m_thread_active{ true };
	std::thread m_thread{ [this]() {
		loop();
	}};

	// Friends
	friend class timer;
}; // class timer::manager

}; // namespace jessilib