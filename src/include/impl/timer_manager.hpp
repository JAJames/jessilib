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

#include <list>
#include <set>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <thread>
#include "../timer.hpp"

namespace jessilib {
namespace impl {

/** timer::manager */

class timer_manager {
    // Singleton accessor
    static timer_manager& instance();

    // Destructor
    ~timer_manager();

    struct timer_sort {
        bool operator()(const timer* lhs, const timer* rhs) const {
            return lhs->next() < rhs->next();
        }
    };

    // Loop
    void loop();

	// helpers
	bool is_current(const timer& in_timer);
	bool is_detached(const timer& in_timer);

    // Members
    std::list<timer> m_detached_timers;
    std::multiset<timer*, timer_sort> m_active_timers;
    std::mutex m_mutex, m_detached_timers_mutex;
    std::condition_variable m_cvar;
    std::atomic<timer*> m_current_timer{nullptr};
    std::atomic<bool> m_thread_active{true};
    std::thread m_thread{[this]() {
        loop();
    }};

    // Friends
    friend timer;
}; // class timer_manager

} // namespace impl
} // namespace jessilib