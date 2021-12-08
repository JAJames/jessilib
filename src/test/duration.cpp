/**
 * Copyright (C) 2018-2021 Jessica James.
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

#include "jessilib/duration.hpp"
#include "test.hpp"

using namespace jessilib;
using namespace std::literals;
using namespace jessilib::literals;

/*
 * ns
 * us
 * ms
 * s
 * m
 * h
 * d
 * w
 * mo
 * y
 */

// Basic compile-time tests
static_assert("1234ns"_duration_ns.count() == 1234, "1234ns Test Failed");
static_assert("1234us"_duration_us.count() == 1234, "1234us Test Failed");
static_assert("1234ms"_duration_ms.count() == 1234, "1234ms Test Failed");
static_assert("1234s"_duration_s.count() == 1234, "1234s Test Failed");
static_assert("1234m"_duration_m.count() == 1234, "1234m Test Failed");
static_assert("1234h"_duration_h.count() == 1234, "1234h Test Failed");
static_assert("1234d"_duration_d.count() == 1234, "1234d Test Failed");
static_assert("1234w"_duration_w.count() == 1234, "1234w Test Failed");
static_assert("1234mo"_duration_mo.count() == 1234, "1234mo Test Failed");
static_assert("1234y"_duration_y.count() == 1234, "1234y Test Failed");

// Ensure string_view variant compiles
static_assert(duration_from_string("1234s"sv).duration.count() == 1234, "1234s string_view test failed");

TEST(duration, as_seconds) {
	EXPECT_EQ(duration_from_string("1234"sv).duration.count(), 1234);
	EXPECT_EQ(duration_from_string("0s"sv).duration.count(), 0);
	EXPECT_EQ(duration_from_string("1s"sv).duration.count(), 1);
	EXPECT_EQ(duration_from_string("5000ms"sv).duration.count(), 5);
	EXPECT_EQ(duration_from_string("5000000us"sv).duration.count(), 5);
	EXPECT_EQ(duration_from_string("5000000000ns"sv).duration.count(), 5);
	EXPECT_EQ(duration_from_string("1h"sv).duration.count(), 3600);
	EXPECT_EQ(duration_from_string("1hr"sv).duration.count(), 3600);
	EXPECT_EQ(duration_from_string("1hour"sv).duration.count(), 3600);
	EXPECT_EQ(duration_from_string("1hours"sv).duration.count(), 3600);
	EXPECT_EQ(duration_from_string("1hrs"sv).duration.count(), 3600);
	EXPECT_EQ(duration_from_string("1d"sv).duration.count(), 86400);
	EXPECT_EQ(duration_from_string("1day"sv).duration.count(), 86400);
	EXPECT_EQ(duration_from_string("1days"sv).duration.count(), 86400);
	EXPECT_EQ(duration_from_string("1w"sv).duration.count(), 604800);
	EXPECT_EQ(duration_from_string("1week"sv).duration.count(), 604800);
	EXPECT_EQ(duration_from_string("1weeks"sv).duration.count(), 604800);
	EXPECT_EQ(duration_from_string("1mo"sv).duration.count(), 2629746);
	EXPECT_EQ(duration_from_string("1mon"sv).duration.count(), 2629746);
	EXPECT_EQ(duration_from_string("1month"sv).duration.count(), 2629746);
	EXPECT_EQ(duration_from_string("1months"sv).duration.count(), 2629746);
	EXPECT_EQ(duration_from_string("1y"sv).duration.count(), 31556952);
	EXPECT_EQ(duration_from_string("1year"sv).duration.count(), 31556952);
	EXPECT_EQ(duration_from_string("1years"sv).duration.count(), 31556952);
}

TEST(duration, as_seconds_spaced) {
	EXPECT_EQ(duration_from_string("0 s"sv).duration.count(), 0);
	EXPECT_EQ(duration_from_string("1 s"sv).duration.count(), 1);
	EXPECT_EQ(duration_from_string("5000 ms"sv).duration.count(), 5);
	EXPECT_EQ(duration_from_string("5000000 us"sv).duration.count(), 5);
	EXPECT_EQ(duration_from_string("5000000000 ns"sv).duration.count(), 5);
	EXPECT_EQ(duration_from_string("1 h"sv).duration.count(), 3600);
	EXPECT_EQ(duration_from_string("1 hr"sv).duration.count(), 3600);
	EXPECT_EQ(duration_from_string("1 hour"sv).duration.count(), 3600);
	EXPECT_EQ(duration_from_string("1 hours"sv).duration.count(), 3600);
	EXPECT_EQ(duration_from_string("1 hrs"sv).duration.count(), 3600);
	EXPECT_EQ(duration_from_string("1 d"sv).duration.count(), 86400);
	EXPECT_EQ(duration_from_string("1 day"sv).duration.count(), 86400);
	EXPECT_EQ(duration_from_string("1 days"sv).duration.count(), 86400);
	EXPECT_EQ(duration_from_string("1 w"sv).duration.count(), 604800);
	EXPECT_EQ(duration_from_string("1 week"sv).duration.count(), 604800);
	EXPECT_EQ(duration_from_string("1 weeks"sv).duration.count(), 604800);
	EXPECT_EQ(duration_from_string("1 mo"sv).duration.count(), 2629746);
	EXPECT_EQ(duration_from_string("1 mon"sv).duration.count(), 2629746);
	EXPECT_EQ(duration_from_string("1 month"sv).duration.count(), 2629746);
	EXPECT_EQ(duration_from_string("1 months"sv).duration.count(), 2629746);
	EXPECT_EQ(duration_from_string("1 y"sv).duration.count(), 31556952);
	EXPECT_EQ(duration_from_string("1 year"sv).duration.count(), 31556952);
	EXPECT_EQ(duration_from_string("1 years"sv).duration.count(), 31556952);
}

TEST(duration, as_seconds_front_spaced) {
	EXPECT_EQ(duration_from_string(" 1234"sv).duration.count(), 1234);
	EXPECT_EQ(duration_from_string(" 0s"sv).duration.count(), 0);
	EXPECT_EQ(duration_from_string(" 1s"sv).duration.count(), 1);
	EXPECT_EQ(duration_from_string(" 5000ms"sv).duration.count(), 5);
	EXPECT_EQ(duration_from_string(" 5000000us"sv).duration.count(), 5);
	EXPECT_EQ(duration_from_string(" 5000000000ns"sv).duration.count(), 5);
	EXPECT_EQ(duration_from_string(" 1h"sv).duration.count(), 3600);
	EXPECT_EQ(duration_from_string(" 1hr"sv).duration.count(), 3600);
	EXPECT_EQ(duration_from_string(" 1hour"sv).duration.count(), 3600);
	EXPECT_EQ(duration_from_string(" 1hours"sv).duration.count(), 3600);
	EXPECT_EQ(duration_from_string(" 1hrs"sv).duration.count(), 3600);
	EXPECT_EQ(duration_from_string(" 1d"sv).duration.count(), 86400);
	EXPECT_EQ(duration_from_string(" 1day"sv).duration.count(), 86400);
	EXPECT_EQ(duration_from_string(" 1days"sv).duration.count(), 86400);
	EXPECT_EQ(duration_from_string(" 1w"sv).duration.count(), 604800);
	EXPECT_EQ(duration_from_string(" 1week"sv).duration.count(), 604800);
	EXPECT_EQ(duration_from_string(" 1weeks"sv).duration.count(), 604800);
	EXPECT_EQ(duration_from_string(" 1mo"sv).duration.count(), 2629746);
	EXPECT_EQ(duration_from_string(" 1mon"sv).duration.count(), 2629746);
	EXPECT_EQ(duration_from_string(" 1month"sv).duration.count(), 2629746);
	EXPECT_EQ(duration_from_string(" 1months"sv).duration.count(), 2629746);
	EXPECT_EQ(duration_from_string(" 1y"sv).duration.count(), 31556952);
	EXPECT_EQ(duration_from_string(" 1year"sv).duration.count(), 31556952);
	EXPECT_EQ(duration_from_string(" 1years"sv).duration.count(), 31556952);
}

TEST(duration, as_seconds_back_spaced) {
	EXPECT_EQ(duration_from_string("1234 "sv).duration.count(), 1234);
	EXPECT_EQ(duration_from_string("0s "sv).duration.count(), 0);
	EXPECT_EQ(duration_from_string("1s "sv).duration.count(), 1);
	EXPECT_EQ(duration_from_string("5000ms "sv).duration.count(), 5);
	EXPECT_EQ(duration_from_string("5000000us "sv).duration.count(), 5);
	EXPECT_EQ(duration_from_string("5000000000ns "sv).duration.count(), 5);
	EXPECT_EQ(duration_from_string("1h "sv).duration.count(), 3600);
	EXPECT_EQ(duration_from_string("1hr "sv).duration.count(), 3600);
	EXPECT_EQ(duration_from_string("1hour "sv).duration.count(), 3600);
	EXPECT_EQ(duration_from_string("1hours "sv).duration.count(), 3600);
	EXPECT_EQ(duration_from_string("1hrs "sv).duration.count(), 3600);
	EXPECT_EQ(duration_from_string("1d "sv).duration.count(), 86400);
	EXPECT_EQ(duration_from_string("1day "sv).duration.count(), 86400);
	EXPECT_EQ(duration_from_string("1days "sv).duration.count(), 86400);
	EXPECT_EQ(duration_from_string("1w "sv).duration.count(), 604800);
	EXPECT_EQ(duration_from_string("1week "sv).duration.count(), 604800);
	EXPECT_EQ(duration_from_string("1weeks "sv).duration.count(), 604800);
	EXPECT_EQ(duration_from_string("1mo "sv).duration.count(), 2629746);
	EXPECT_EQ(duration_from_string("1mon "sv).duration.count(), 2629746);
	EXPECT_EQ(duration_from_string("1month "sv).duration.count(), 2629746);
	EXPECT_EQ(duration_from_string("1months "sv).duration.count(), 2629746);
	EXPECT_EQ(duration_from_string("1y "sv).duration.count(), 31556952);
	EXPECT_EQ(duration_from_string("1year "sv).duration.count(), 31556952);
	EXPECT_EQ(duration_from_string("1years "sv).duration.count(), 31556952);
}

TEST(duration, as_seconds_all_spaced) {
	EXPECT_EQ(duration_from_string(" 1234 "sv).duration.count(), 1234);
	EXPECT_EQ(duration_from_string(" 0 s "sv).duration.count(), 0);
	EXPECT_EQ(duration_from_string(" 1 s "sv).duration.count(), 1);
	EXPECT_EQ(duration_from_string(" 5000 ms "sv).duration.count(), 5);
	EXPECT_EQ(duration_from_string(" 5000000 us "sv).duration.count(), 5);
	EXPECT_EQ(duration_from_string(" 5000000000 ns "sv).duration.count(), 5);
	EXPECT_EQ(duration_from_string(" 1 h "sv).duration.count(), 3600);
	EXPECT_EQ(duration_from_string(" 1 hr "sv).duration.count(), 3600);
	EXPECT_EQ(duration_from_string(" 1 hour "sv).duration.count(), 3600);
	EXPECT_EQ(duration_from_string(" 1 hours "sv).duration.count(), 3600);
	EXPECT_EQ(duration_from_string(" 1 hrs "sv).duration.count(), 3600);
	EXPECT_EQ(duration_from_string(" 1 d "sv).duration.count(), 86400);
	EXPECT_EQ(duration_from_string(" 1 day "sv).duration.count(), 86400);
	EXPECT_EQ(duration_from_string(" 1 days "sv).duration.count(), 86400);
	EXPECT_EQ(duration_from_string(" 1 w "sv).duration.count(), 604800);
	EXPECT_EQ(duration_from_string(" 1 week "sv).duration.count(), 604800);
	EXPECT_EQ(duration_from_string(" 1 weeks "sv).duration.count(), 604800);
	EXPECT_EQ(duration_from_string(" 1 mo "sv).duration.count(), 2629746);
	EXPECT_EQ(duration_from_string(" 1 mon "sv).duration.count(), 2629746);
	EXPECT_EQ(duration_from_string(" 1 month "sv).duration.count(), 2629746);
	EXPECT_EQ(duration_from_string(" 1 months "sv).duration.count(), 2629746);
	EXPECT_EQ(duration_from_string(" 1 y "sv).duration.count(), 31556952);
	EXPECT_EQ(duration_from_string(" 1 year "sv).duration.count(), 31556952);
	EXPECT_EQ(duration_from_string(" 1 years "sv).duration.count(), 31556952);
}

template<typename DurationT>
constexpr std::chrono::seconds to_seconds(DurationT in_duration) {
	return std::chrono::duration_cast<std::chrono::seconds>(in_duration);
}

TEST(duration, combinations) {
	EXPECT_EQ(duration_from_string("1d 4 weeks"sv).duration.count(),
		to_seconds(days{1} + weeks{4}).count());
	EXPECT_EQ(duration_from_string("1d 4 weeks 1 mon"sv).duration.count(),
		to_seconds(days{1} + weeks{4} + months{1}).count());
	EXPECT_EQ(duration_from_string("1d 4 weeks 1 mon 4 min"sv).duration.count(),
		to_seconds(days{1} + weeks{4} + months{1} + 4min).count());
	EXPECT_EQ(duration_from_string("1d 4 weeks 1 mon 4 min 36 sec"sv).duration.count(),
		to_seconds(days{1} + weeks{4} + months{1} + 4min + 36s).count());
	EXPECT_EQ(duration_from_string("1d 4 weeks 1 mon 4 min 36 sec 12hr"sv).duration.count(),
		to_seconds(days{1} + weeks{4} + months{1} + 4min + 36s + 12h).count());
	EXPECT_EQ(duration_from_string("1d 4 weeks 1 mon 4 min 36 sec 12hr 5s"sv).duration.count(),
		to_seconds(days{1} + weeks{4} + months{1} + 4min + 36s + 12h + 5s).count());

	// Consider refactoring such that the expressions are equal without usage of to_seconds for types smaller than seconds
	EXPECT_EQ(duration_from_string("1d 4 weeks 1 mon 4 min 36 sec 12hr 5s 4900ms"sv).duration.count(),
		to_seconds(days{1} + weeks{4} + months{1} + 4min + 36s + 12h + 5s + to_seconds(4900ms)).count());
	EXPECT_EQ(duration_from_string("1d 4 weeks 1 mon 4 min 36 sec 12hr 5s 4900ms 6900000us"sv).duration.count(),
		to_seconds(days{1} + weeks{4} + months{1} + 4min + 36s + 12h + 5s + to_seconds(4900ms) + to_seconds(6900000us)).count());
	EXPECT_EQ(duration_from_string("1d 4 weeks 1 mon 4 min 36 sec 12hr 5s 4900ms 6900000us 900000000ns"sv).duration.count(),
		to_seconds(days{1} + weeks{4} + months{1} + 4min + 36s + 12h + 5s + to_seconds(4900ms) + to_seconds(6900000us) + to_seconds(900000000ns)).count());
	EXPECT_EQ(duration_from_string("1d 4 weeks 1 mon 4 min 36 sec 12hr 5s 4900ms 6900000us 900000000ns 7days 6years"sv).duration.count(),
		to_seconds(days{1} + weeks{4} + months{1} + 4min + 36s + 12h + 5s + to_seconds(4900ms) + to_seconds(6900000us) + to_seconds(900000000ns) + days{7} + years {6}).count());
}

TEST(duration, invalid) {
	EXPECT_EQ(duration_from_string("Jessica was here"sv).duration.count(),0);
	EXPECT_EQ(duration_from_string("and here"sv).duration.count(),0);
	EXPECT_EQ(duration_from_string("second guessing code"sv).duration.count(),0);
	EXPECT_EQ(duration_from_string("year round"sv).duration.count(),0);

	EXPECT_EQ(duration_from_string("1yr 3 da 4 mi invalid 77 months"sv).duration.count(),
		to_seconds(years{1} + days{3}).count()); // "4 mi" is actually valid, but is rounded down to zero on conversion
}
