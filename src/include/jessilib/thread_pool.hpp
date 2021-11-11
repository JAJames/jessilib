/**
 * Copyright (C) 2018 Jessica James.
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

#include <cstdint>
#include <functional>
#include <vector>
#include <thread>
#include <queue>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <algorithm>

namespace jessilib {

class thread_pool {
public:
	// Types
	using task_t = std::function<void(void)>;

	thread_pool();
	thread_pool(size_t in_threads);
	thread_pool(const thread_pool&) = delete;
	thread_pool(thread_pool&&) = delete;
	~thread_pool();

	void push(task_t in_task); // push a task to the pool
	void join(); // join all threads
	size_t threads() const; // how many threads are in the pool
	size_t active() const; // how many threads are running tasks

	static unsigned int default_threads();

private:
	struct thread {
		void run_task();

		std::atomic<bool> m_active{ false };
		bool m_shutdown{ false };
		task_t m_task;
		std::condition_variable m_notifier;
		std::mutex m_notifier_mutex;
		std::thread m_thread;
	};

	thread* inactive_thread();
	task_t pop_task();

	std::vector<thread> m_threads;
	std::queue<thread*> m_inactive_threads;
	std::queue<task_t> m_tasks;

	mutable std::mutex m_threads_mutex;
	mutable std::mutex m_inactive_threads_mutex;
	mutable std::mutex m_tasks_mutex;
}; // class thread_pool

} // namespace jessilib
