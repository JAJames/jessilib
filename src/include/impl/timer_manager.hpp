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

#pragma once

#include <list>
#include <set>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <thread>
#include "timer.hpp"
#include "thread_pool.hpp"
#include "timer_context.hpp"

namespace jessilib {
namespace impl {

/** timer::manager */

class timer_manager {
    // Singleton accessor
    static timer_manager& instance();

    // Destructor
    ~timer_manager();

    struct timer_sort {
        inline bool operator()(const timer_context* lhs, const timer_context* rhs) const {
            return lhs->next() < rhs->next();
        }
    };

    class worker_thread {
    public:
    	void operator()();
    };

    // Loop
    void loop();

    // Attempt to execute timer
	//bool try_fire(timer* in_timer);
	static timer& thread_callback_timer();

    // Members
	thread_pool m_pool{ thread_pool::default_threads() * 2 };
    std::list<std::shared_ptr<timer_context>> m_detached_timers;
    std::multiset<timer_context*, timer_sort> m_active_timers; // timer? weak_ptr<timer_context>?
    std::mutex m_mutex, m_detached_timers_mutex;
    std::condition_variable m_cvar;
    bool is_timeout{ true };
    std::atomic<bool> m_thread_active{true};
    std::thread m_thread{[this]() {
        loop();
    }};

    // Friends
    friend timer;
    friend timer_context;
}; // class timer_manager

} // namespace impl
} // namespace jessilib
