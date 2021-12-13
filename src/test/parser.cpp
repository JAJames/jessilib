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

#include <functional>
#include "test.hpp"
#include "jessilib/parser.hpp"
#include "jessilib/serialize.hpp"
#include "jessilib/unicode.hpp"

using namespace jessilib;
using namespace std::literals;

/** test_parser */

class test_parser : public parser {
public:
	/** deserialize/serialize overrides */
	virtual object deserialize(std::u8string_view in_data) override {
		return deserialize_impl(in_data);
	}

	virtual std::u8string serialize(const object& in_object) override {
		return serialize_impl(in_object);
	}

	/** helpers */
	static void reset() {
		serialize_impl = &serialize_default;
		deserialize_impl = &deserialize_default;
	}

	/** default serialize/deserialize implementations */
	static std::u8string serialize_default(const object& in_object) {
		if (in_object.has<std::u8string>()) {
			return in_object.get<std::u8string>();
		}

		return static_cast<std::u8string>(DEFAULT_SERIALIZE_RESULT);
	}

	static object deserialize_default(std::u8string_view in_data) {
		return object{ string_view_cast<char8_t>(in_data) };
	}

	/** static members */
	static constexpr std::u8string_view DEFAULT_SERIALIZE_RESULT = u8"serialize_result"sv;
	static std::function<std::u8string(const object&)> serialize_impl;
	static std::function<object(std::u8string_view)> deserialize_impl;
};

std::function<std::u8string(const object&)> test_parser::serialize_impl{ &serialize_default };
std::function<object(std::u8string_view)> test_parser::deserialize_impl{ &deserialize_default };

parser_registration<test_parser> test_parser_registration{ "test" };

/** ParserTest */

class ParserTest : public base_test {
	virtual void SetUp() override {
		test_parser::reset();
	}

	virtual void TearDown() override {
		test_parser::reset();
	}
};

/** Parser tests */

TEST_F(ParserTest, find_parser) {
	EXPECT_NO_THROW(serialize_object(u8"test_data", "test"));
	EXPECT_NO_THROW(deserialize_object(u8"test_data"sv, "test"));

	EXPECT_THROW(serialize_object(u8"test_data", "invalid_format_test"), format_not_available);
	EXPECT_THROW(deserialize_object(u8"test_data"sv, "invalid_format_test"), format_not_available);
}

TEST_F(ParserTest, temp_parser) {
	EXPECT_THROW(serialize_object(u8"test_data", "test_tmp"), format_not_available);
	EXPECT_THROW(deserialize_object(u8"test_data"sv, "test_tmp"), format_not_available);

	{
		parser_registration<test_parser> test_tmp_registration{ "test_tmp" };
		EXPECT_NO_THROW(serialize_object(u8"test_data", "test_tmp"));
		EXPECT_NO_THROW(deserialize_object(u8"test_data"sv, "test_tmp"));
	}

	EXPECT_THROW(serialize_object(u8"test_data", "test_tmp"), format_not_available);
	EXPECT_THROW(deserialize_object(u8"test_data"sv, "test_tmp"), format_not_available);
}

TEST_F(ParserTest, serialize) {
	EXPECT_EQ(serialize_object(u8"test_data", "test"), u8"test_data");
}

TEST_F(ParserTest, deserialize) {
	EXPECT_EQ(deserialize_object(u8"test_data"sv, "test").get<std::u8string>(), u8"test_data");
}
