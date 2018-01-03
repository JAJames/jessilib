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

namespace jessilib {

// thread_pool

thread_pool::thread_pool()
	: thread_pool{ std::thread::hardware_concurrency() } {
}

thread_pool::thread_pool(size_t in_threads)
	: m_threads{ in_threads } {
	assert(in_threads != 0);

	while (in_threads != 0) {
		thread& self = m_threads[--in_threads];
		self.m_thread = std::thread([this, &self]() {
			while (true) {
				// Run next pending task, if there is any
				self.m_task = pop_task();
				if (self.m_task != nullptr) {
					self.run_task();
					continue;
				}

				if (self.m_shutdown) {
					break;
				}

				// Push inactive thread
				{
					std::lock_guard<std::mutex> guard(m_inactive_threads_mutex);
					m_inactive_threads.push(&self);
				}

				// Wait for notification
				self.wait();

				// Run task
				self.run_task();
			}
		});
	}
}

template<typename T>
T lock_helper(T&& in_obj, std::mutex& in_mutex) {
	std::lock_guard<std::mutex> guard(in_mutex);
	return std::move(in_obj);
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

	// Join threads
	for (thread& thread : m_threads) {
		// Mark thread for shutdown
		thread.m_shutdown = true;
		thread.m_notifier.notify_one();

		// Wait for thread to complete
		thread.m_thread.join();
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

void thread_pool::thread::wait() {
	std::unique_lock<std::mutex> guard(m_notifier_mutex);
	m_notifier.wait(guard);
}

} // namespace jessilib
