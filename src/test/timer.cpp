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

#include <future>
#include "test.hpp"
#include "jessilib/timer.hpp"

using namespace jessilib;
using namespace std::literals;

constexpr size_t total_iterations{ 4 };
constexpr std::chrono::steady_clock::duration period = 1ms;
constexpr std::chrono::steady_clock::duration timeout = period * total_iterations * 2 + 1s;

TEST(TimerTest, basic) {
	timer timer_obj;

	// Check default state
	EXPECT_EQ(timer_obj.next(), timer::time_point_t{});
	EXPECT_EQ(timer_obj.period(), timer::duration_t{ 0 });
	EXPECT_EQ(timer_obj.function(), nullptr);
	EXPECT_TRUE(timer_obj.null());
	EXPECT_FALSE(timer_obj.detached());

	// Should have no effect
	timer_obj.detach();
	timer_obj.cancel();

	// Verify state unchanged
	EXPECT_EQ(timer_obj.next(), timer::time_point_t{});
	EXPECT_EQ(timer_obj.period(), timer::duration_t{ 0 });
	EXPECT_EQ(timer_obj.function(), nullptr);
	EXPECT_TRUE(timer_obj.null());
	EXPECT_FALSE(timer_obj.detached());
}

TEST(TimerTest, scoped) {
	size_t iterations{ 0 };
	std::promise<void> promise;

	timer timer_obj{ period, [&iterations, &promise, &timer_obj](timer& in_timer) {
		EXPECT_EQ(timer_obj, in_timer);

		if (++iterations == total_iterations) {
			promise.set_value();
			in_timer.cancel();
		}
	} };

	EXPECT_EQ(promise.get_future().wait_for(timeout * 100000), std::future_status::ready);
}

TEST(TimerTest, detached) {
	size_t iterations{ 0 };
	std::promise<void> promise;

	{
		timer timer_obj{ period, [&iterations, &promise](timer& in_timer) {
			if (++iterations == total_iterations) {
				EXPECT_FALSE(in_timer.null());
				in_timer.cancel();
				promise.set_value();
			}
		} };

		EXPECT_FALSE(timer_obj.null());
		timer_obj.detach();
		EXPECT_FALSE(timer_obj.null());
	}

	EXPECT_EQ(promise.get_future().wait_for(timeout), std::future_status::ready);
}

TEST(TimerTest, scopedWithIterations) {
	size_t iterations{ 0 };
	std::promise<void> promise;

	timer timer_obj{ period, total_iterations, [&iterations, &promise]([[maybe_unused]] timer& in_timer) {
		if (++iterations == total_iterations) {
			promise.set_value();
		}
	} };

	EXPECT_EQ(promise.get_future().wait_for(timeout), std::future_status::ready);
}

TEST(TimerTest, detachedWithIterations) {
	size_t iterations{ 0 };
	std::promise<void> promise;

	{
		timer{period, total_iterations, [&iterations, &promise]([[maybe_unused]] timer& in_timer) {
			if (++iterations == total_iterations) {
				promise.set_value();
			}
		}}.detach();
	}

	EXPECT_EQ(promise.get_future().wait_for(timeout * 1000000), std::future_status::ready);
}

TEST(TimerTest, scopedWithIterationsCancel) {
	size_t iterations{ 0 };
	std::promise<void> promise;

	timer timer_obj{ period, total_iterations, [&iterations, &promise](timer& in_timer) {
		if (++iterations == total_iterations) {
			promise.set_value();
			in_timer.cancel();
		}
	} };

	EXPECT_EQ(promise.get_future().wait_for(timeout), std::future_status::ready);
}

TEST(TimerTest, detachedWithIterationsCancel) {
	size_t iterations{ 0 };
	std::promise<void> promise;

	{
		timer{period, total_iterations, [&iterations, &promise](timer& in_timer) {
			if (++iterations == total_iterations) {
				promise.set_value();
				in_timer.cancel();
			}
		}}.detach();
	}

	EXPECT_EQ(promise.get_future().wait_for(timeout), std::future_status::ready);
}

TEST(TimerTest, simultaneousTimers) {
	std::condition_variable notifier;
	std::mutex notifier_mutex;
	int timers_running = 0;
	bool test_finished = false;

	// Spin up timer
	timer timer_obj{period, [&notifier, &notifier_mutex, &timers_running, &test_finished]([[maybe_unused]] timer& in_timer) {
		std::unique_lock<std::mutex> lock(notifier_mutex);
		if (test_finished) {
			notifier.notify_all();
			return;
		}

		++timers_running;

		// Wait for a few more timer periods to pass before proceeding
		notifier.wait(lock);

		// We're done here; cancel
		--timers_running;
		notifier.notify_all();
	}};

	// Wait for some timers to fire
	std::this_thread::sleep_for(period + 100ms);
	EXPECT_GE(timers_running, 2);

	// Notify timers to close
	{
		std::unique_lock<std::mutex> lock(notifier_mutex);
		test_finished = true;
	}
	notifier.notify_all();

	// Wait for timers to complete
	timer_obj.cancel();
	std::unique_lock<std::mutex> lock(notifier_mutex);
	notifier.wait(lock, [&timers_running]() {
		return timers_running == 0;
	});
}

TEST(TimerTest, syncrhonizedTimers) {
	std::condition_variable notifier;
	std::mutex notifier_mutex;
	int timers_running = 0;

	// Spin up timer
	syncrhonized_timer timer_obj{period, [&notifier, &notifier_mutex, &timers_running](timer& in_timer) {
		std::unique_lock<std::mutex> lock(notifier_mutex);
		++timers_running;

		// Wait for a few more timer periods to pass before proceeding
		notifier.wait(lock);

		// We're done here; cancel
		--timers_running;
		notifier.notify_one();
		in_timer.cancel();
	}};

	// Wait for some timers to fire
	std::this_thread::sleep_for(period * 3);
	EXPECT_EQ(timers_running, 1);

	// Notify timers to close
	notifier.notify_all();

	// Wait for timers to complete
	std::unique_lock<std::mutex> lock(notifier_mutex);
	notifier.wait(lock, [&timers_running]() {
		return timers_running == 0;
	});
}
