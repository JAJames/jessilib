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

#pragma once

// Common includes
#include "gtest/gtest.h"

// Helper macros
#define ASSERT_COMPILES(expr) static_assert([]() { expr; } != nullptr, "Your compiler is whack yo");
#define ASSERT_COMPILES_CONSTEXPR(expr) static_assert([]() constexpr { expr; } != nullptr, "Your compiler is whack yo");
#define UNIQUE_LABEL( LABEL ) LABEL ## __LINE__ ## __
#define repeat( ITERATIONS ) for (size_t UNIQUE_LABEL(iteration_) = 0; UNIQUE_LABEL(iteration_) != (ITERATIONS); ++ UNIQUE_LABEL(iteration_) )
#define JESSITEST_CONCAT(lhs, rhs) JESSITEST_CONCAT_HELPER(lhs, rhs)
#define JESSITEST_CONCAT_HELPER(lhs, rhs) lhs ## rhs

using base_test = ::testing::Test;
