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

#include <chrono>
#include "test.hpp"
#include "thread_pool.hpp"

using namespace jessilib;
using namespace std::literals;

constexpr size_t total_iterations{ 100 };

TEST(ThreadPoolTest, initialDefault) {
	thread_pool pool;
	std::this_thread::sleep_for(10ms);

	EXPECT_EQ(pool.threads(), std::thread::hardware_concurrency());
	std::this_thread::sleep_for(10ms);
	EXPECT_EQ(pool.active(), 0U);

	pool.join();

	EXPECT_EQ(pool.active(), 0U);
	EXPECT_EQ(pool.threads(), 0U);
}

TEST(ThreadPoolTest, initialSizeDefined) {
	thread_pool pool{ 7 };

	EXPECT_EQ(pool.threads(), 7U);
	std::this_thread::sleep_for(10ms);
	EXPECT_EQ(pool.active(), 0U);

	pool.join();

	EXPECT_EQ(pool.active(), 0U);
	EXPECT_EQ(pool.threads(), 0U);
}

TEST(ThreadPoolTest, push) {
	std::atomic<size_t> iterations{0};
	thread_pool pool;

	repeat (total_iterations) {
		pool.push([&iterations]() {
			++iterations;
		});
	}

	pool.join();
	EXPECT_EQ(iterations, total_iterations);
}

TEST(ThreadPoolTest, deadlockSingleThread) {
	std::atomic<size_t> iterations{ 0 };
	thread_pool pool{ 1 };

	repeat (total_iterations) {
		pool.push([&iterations, &pool]() {
			++iterations;

			// Neither of the below should cause a deadlock
			EXPECT_EQ(pool.threads(), 1U);
			EXPECT_EQ(pool.active(), 1U);
		});
	}

	pool.join();
	EXPECT_EQ(iterations, total_iterations);
}

TEST(ThreadPoolTest, deadlockMultiThread) {
	std::atomic<size_t> iterations{ 0 };
	thread_pool pool{ 8 };

	repeat (total_iterations) {
		pool.push([&iterations, &pool]() {
			++iterations;

			// Neither of the below should cause a deadlock
			pool.threads();
			pool.active();
		});
	}

	pool.join();
	EXPECT_EQ(iterations, total_iterations);
}
