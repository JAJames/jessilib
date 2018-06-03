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

#include "thread_pool.hpp"
#include <cassert>
#include <algorithm>

namespace jessilib {

// thread_pool

unsigned int thread_pool::default_threads() {
	static constexpr unsigned int MIN_THREADS{ 1 };
	static const unsigned int DEFAULT_THREADS{ std::max(std::thread::hardware_concurrency(), MIN_THREADS) };
	return DEFAULT_THREADS;
};

thread_pool::thread_pool()
	: thread_pool{ default_threads() } {
}

thread_pool::thread_pool(size_t in_threads)
	: m_threads{ in_threads } {
	assert(in_threads != 0);

	while (in_threads != 0) {
		thread& worker = m_threads[--in_threads];
		worker.m_thread = std::thread([this, &worker]() {
			while (true) {
				// Run next pending task, if there is any
				worker.m_task = pop_task();
				if (worker.m_task != nullptr) {
					worker.run_task();
					continue;
				}

				{
					// Check if we're shutting down
					std::unique_lock<std::mutex> notifier_guard(worker.m_notifier_mutex);
					if (worker.m_shutdown) {
						break;
					}

					// Push inactive thread
					{
						std::lock_guard<std::mutex> inactive_threads_guard(m_inactive_threads_mutex);
						m_inactive_threads.push(&worker);
					}

					// Wait for notification
					worker.m_notifier.wait(notifier_guard);
				}

				// Run task
				worker.run_task();
			}
		});
	}
}

thread_pool::~thread_pool() {
	join();
}

void thread_pool::push(task_t in_task) {
	thread* target_thread = inactive_thread();

	if (target_thread != nullptr) {
		target_thread->m_task = in_task;
		target_thread->m_notifier.notify_one();
	}
	else {
		std::lock_guard<std::mutex> guard(m_tasks_mutex);
		m_tasks.push(in_task);
	}
}

void thread_pool::join() {
	std::lock_guard<std::mutex> guard(m_threads_mutex);

	// Shutdown threads
	for (thread& worker : m_threads) {
		// Mark thread for shutdown
		{
			std::unique_lock<std::mutex> guard(worker.m_notifier_mutex);
			worker.m_shutdown = true;
		}

		// Notify thread to shutdown
		worker.m_notifier.notify_one();
	}

	// Join threads
	for (thread& worker : m_threads) {
		worker.m_thread.join();
	}

	// Cleanup threads
	m_threads.clear();
	while (!m_inactive_threads.empty()) {
		m_inactive_threads.pop();
	}
}

size_t thread_pool::threads() const {
	//std::lock_guard<std::mutex> guard(m_threads_mutex);
	return m_threads.size();
}

size_t thread_pool::active() const {
	std::lock_guard<std::mutex> guard(m_inactive_threads_mutex);
	return threads() - m_inactive_threads.size();
}

// thread_pool private functions

thread_pool::thread* thread_pool::inactive_thread() {
	std::lock_guard<std::mutex> guard(m_inactive_threads_mutex);

	if (!m_inactive_threads.empty()) {
		thread* result = m_inactive_threads.front();
		m_inactive_threads.pop();
		return result;
	}

	return nullptr;
}

thread_pool::task_t thread_pool::pop_task() {
	std::lock_guard<std::mutex> guard(m_tasks_mutex);
	if (!m_tasks.empty()) {
		task_t task = m_tasks.front();
		m_tasks.pop();
		return task;
	}

	return nullptr;
}

// thread_pool::thread

void thread_pool::thread::run_task() {
	if (m_task) {
		m_active = true;
		m_task();
		m_task = nullptr;
		m_active = false;
	}
}

} // namespace jessilib
