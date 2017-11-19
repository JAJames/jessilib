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

#include <future>
#include "gtest/gtest.h"
#include "timer.hpp"

using namespace jessilib;
using namespace std::literals;

constexpr size_t total_iterations{ 4 };
constexpr std::chrono::steady_clock::duration period = 1ms;
constexpr std::chrono::steady_clock::duration timeout = 1s;

TEST(TimerTest, set) {
	size_t iterations{ 0 };
	std::promise<void> promise;

	timer::set(period, [&iterations, &promise](timer::node& in_timer) {
		if (++iterations == total_iterations) {
			in_timer.cancel();
			promise.set_value();
		}
	});

	EXPECT_EQ(promise.get_future().wait_for(timeout), std::future_status::ready);
}

TEST(TimerTest, scoped) {
	size_t iterations{ 0 };
	std::promise<void> promise;

	timer timer{ period, [&iterations, &promise](timer::node& in_timer) {
		if (++iterations == total_iterations) {
			in_timer.cancel();
			promise.set_value();
		}
	} };

	EXPECT_EQ(promise.get_future().wait_for(timeout), std::future_status::ready);
}

TEST(TimerTest, setWithIterations) {
	size_t iterations{ 0 };
	std::promise<void> promise;

	timer::set(period, total_iterations, [&iterations, &promise](timer::node& in_timer) {
		if (++iterations == total_iterations) {
			promise.set_value();
		}
	});

	EXPECT_EQ(promise.get_future().wait_for(timeout), std::future_status::ready);
}

TEST(TimerTest, scopedWithIterations) {
	size_t iterations{ 0 };
	std::promise<void> promise;

	timer timer{ period, total_iterations, [&iterations, &promise](timer::node& in_timer) {
		if (++iterations == total_iterations) {
			promise.set_value();
		}
	} };

	EXPECT_EQ(promise.get_future().wait_for(timeout), std::future_status::ready);
}

TEST(TimerTest, setWithIterationsCancel) {
	size_t iterations{ 0 };
	std::promise<void> promise;

	timer::set(period, total_iterations, [&iterations, &promise](timer::node& in_timer) {
		if (++iterations == total_iterations) {
			in_timer.cancel();
			promise.set_value();
		}
	});

	EXPECT_EQ(promise.get_future().wait_for(timeout), std::future_status::ready);
}

TEST(TimerTest, scopedWithIterationsCancel) {
	size_t iterations{ 0 };
	std::promise<void> promise;

	timer timer{ period, total_iterations, [&iterations, &promise](timer::node& in_timer) {
		if (++iterations == total_iterations) {
			in_timer.cancel();
			promise.set_value();
		}
	} };

	EXPECT_EQ(promise.get_future().wait_for(timeout), std::future_status::ready);
}
