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
#include <functional>
#include "test.hpp"
#include "parser.hpp"
#include "serialize.hpp"

using namespace jessilib;
using namespace std::literals;

/** test_parser */

class test_parser : public parser {
public:
	/** deserialize/serialize overrides */
	virtual object deserialize(std::string_view in_data) override {
		return deserialize_impl(in_data);
	}

	virtual std::string serialize(const object& in_object) override {
		return serialize_impl(in_object);
	}

	/** helpers */
	static void reset() {
		serialize_impl = &serialize_default;
		deserialize_impl = &deserialize_default;
	}

	/** default serialize/deserialize implementations */
	static std::string serialize_default(const object& in_object) {
		if (in_object.has<std::string>()) {
			return in_object.get<std::string>();
		}

		return DEFAULT_SERIALIZE_RESULT;
	}

	static object deserialize_default(std::string_view in_data) {
		return object{ in_data };
	}

	/** static members */
	static constexpr const char DEFAULT_SERIALIZE_RESULT[]{ "serialize_result" };
	static std::function<std::string(const object&)> serialize_impl;
	static std::function<object(std::string_view)> deserialize_impl;
};

std::function<std::string(const object&)> test_parser::serialize_impl{ &serialize_default };
std::function<object(std::string_view)> test_parser::deserialize_impl{ &deserialize_default };

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
	EXPECT_NO_THROW(serialize_object("test_data", "test"));
	EXPECT_NO_THROW(deserialize_object("test_data"sv, "test"));

	EXPECT_THROW(serialize_object("test_data", "invalid_format_test"), format_not_available);
	EXPECT_THROW(deserialize_object("test_data"sv, "invalid_format_test"), format_not_available);
}

TEST_F(ParserTest, temp_parser) {
	EXPECT_THROW(serialize_object("test_data", "test_tmp"), format_not_available);
	EXPECT_THROW(deserialize_object("test_data"sv, "test_tmp"), format_not_available);

	{
		parser_registration<test_parser> test_tmp_registration{ "test_tmp" };
		EXPECT_NO_THROW(serialize_object("test_data", "test_tmp"));
		EXPECT_NO_THROW(deserialize_object("test_data"sv, "test_tmp"));
	}

	EXPECT_THROW(serialize_object("test_data", "test_tmp"), format_not_available);
	EXPECT_THROW(deserialize_object("test_data"sv, "test_tmp"), format_not_available);
}

TEST_F(ParserTest, serialize) {
	EXPECT_EQ(serialize_object("test_data", "test"), "test_data");
}

TEST_F(ParserTest, deserialize) {
	EXPECT_EQ(deserialize_object("test_data"sv, "test").get<std::string>(), "test_data");
}
