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

#include "test.hpp"
#include "jessilib/parsers/json.hpp"

using namespace jessilib;
using namespace std::literals;

TEST(JsonParser, serialize_null) {
	json_parser parser;

	EXPECT_EQ(parser.serialize({}), "null");
}

TEST(JsonParser, serialize_boolean) {
	json_parser parser;

	EXPECT_EQ(parser.serialize(true), "true");
	EXPECT_EQ(parser.serialize(false), "false");
}

TEST(JsonParser, serialize_integer) {
	json_parser parser;

	EXPECT_EQ(parser.serialize(1234), "1234");
}

TEST(JsonParser, serialize_decimal) {
	json_parser parser;
	EXPECT_DOUBLE_EQ(std::atof(parser.serialize(12.34).c_str()), 12.34);
	EXPECT_DOUBLE_EQ(std::atof(parser.serialize(1234.0).c_str()), 1234.0);
}

// necessary due to some sort of bug with EXPECT_EQ on MSVC
template<typename LeftT, typename RightT>
void expect_eq(LeftT in_left, RightT in_right) {
	EXPECT_EQ(in_left, in_right);
}

TEST(JsonParser, serialize_string) {
	json_parser parser;

	EXPECT_EQ(parser.serialize(u8"text"), R"json("text")json");
	expect_eq(parser.serialize(u8"\"text\""), R"json("\"text\"")json");
	expect_eq(parser.serialize(u8"\"te\x01xt\""), R"json("\"te\u0001xt\"")json");
	expect_eq(parser.serialize(u8"\"te\x10xt\""), R"json("\"te\u0010xt\"")json");
}

TEST(JsonParser, serialize_array) {
	json_parser parser;
	std::vector<object> array {
		true,
		1234,
		u8"text",
		object{}
	};

	EXPECT_EQ(parser.serialize(array),
		R"json([true,1234,"text",null])json");
}

TEST(JsonParser, serialize_map) {
	json_parser parser;
	object obj;

	obj[u8"some_bool"] = true;
	obj[u8"some_int"] = 1234;
	obj[u8"some_string"] = u8"text";
	obj[u8"some_null"];

	EXPECT_EQ(parser.serialize(obj),
		R"json({"some_bool":true,"some_int":1234,"some_null":null,"some_string":"text"})json");
}

TEST(JsonParser, deserialize_null) {
	json_parser parser;

	EXPECT_EQ(parser.deserialize("null"sv), object{});
}

TEST(JsonParser, deserialize_boolean) {
	json_parser parser;

	EXPECT_EQ(parser.deserialize("true"sv), true);
	EXPECT_EQ(parser.deserialize("false"sv), false);
}

TEST(JsonParser, deserialize_integer) {
	json_parser parser;

	EXPECT_EQ(parser.deserialize("1234"sv), 1234);
	EXPECT_EQ(parser.deserialize("-1234"sv), -1234);
}

TEST(JsonParser, deserialize_decimal) {
	json_parser parser;

	EXPECT_DOUBLE_EQ(parser.deserialize("12.34"sv).get<double>(), 12.34);
	EXPECT_DOUBLE_EQ(parser.deserialize("1234."sv).get<double>(), 1234.0);
	EXPECT_DOUBLE_EQ(parser.deserialize("0.1234"sv).get<double>(), 0.1234);
	EXPECT_THROW(parser.deserialize(".1234"sv), std::invalid_argument);
	EXPECT_DOUBLE_EQ(parser.deserialize("-12.34"sv).get<double>(), -12.34);
}

TEST(JsonParser, deserialize_string) {
	json_parser parser;

	EXPECT_EQ(parser.deserialize(R"json("text")json"sv), u8"text");
}

TEST(JsonParser, deserialize_array) {
	json_parser parser;

	constexpr const char* json_data = R"json([
		true,
		false,
		1234,
		12.34,
		0.1234,
		"text"
	])json";

	auto array = parser.deserialize(json_data).get<std::vector<object>>();
	ASSERT_EQ(array.size(), 6U);
	EXPECT_EQ(array[0], true);
	EXPECT_EQ(array[1], false);
	EXPECT_EQ(array[2], 1234);
	EXPECT_DOUBLE_EQ(array[3].get<double>(), 12.34);
	EXPECT_DOUBLE_EQ(array[4].get<double>(), 0.1234);
	EXPECT_EQ(array[5], u8"text");
}

TEST(JsonParser, deserialize_array_nested) {
	json_parser parser;

	constexpr const char* json_data = R"json([
		true,
		false,
		1234
	,	[
				1,2,3,
				null,
				"text",
				[5,6,7],
		]		, [ ] , [	" text " ],
		12.34,
		0.1234,
		"text"
	])json";

	auto array = parser.deserialize(json_data).get<std::vector<object>>();
	ASSERT_EQ(array.size(), 9U);
	EXPECT_EQ(array[0], true);
	EXPECT_EQ(array[1], false);
	EXPECT_EQ(array[2], 1234);
	EXPECT_FALSE(array[3].null());
	EXPECT_EQ(array[3].size(), 6U);
	EXPECT_FALSE(array[4].null());
	EXPECT_EQ(array[4].size(), 0U);
	EXPECT_FALSE(array[5].null());
	ASSERT_EQ(array[5].size(), 1U);
	EXPECT_EQ(array[5], std::vector<object>{ u8" text " });
	EXPECT_EQ(array[5], std::vector<std::u8string>{ u8" text " });
	EXPECT_DOUBLE_EQ(array[6].get<double>(), 12.34);
	EXPECT_DOUBLE_EQ(array[7].get<double>(), 0.1234);
	EXPECT_EQ(array[8], u8"text");

	auto nested_array = array[3].get<std::vector<object>>();
	ASSERT_EQ(nested_array.size(), 6U);
	EXPECT_EQ(nested_array[0], 1);
	EXPECT_EQ(nested_array[1], 2);
	EXPECT_EQ(nested_array[2], 3);
	EXPECT_TRUE(nested_array[3].null());
	EXPECT_EQ(nested_array[4],u8"text");
	std::vector<int> expected{ 5, 6, 7 };
	EXPECT_EQ(nested_array[5], expected);
}

TEST(JsonParser, deserialize_map) {
	json_parser parser;

	constexpr const char* json_data = R"json({
		"some_true":true,
		"some_false"	:  false,
		"some_int": 1234,
		"some_double" : 12.34,
		"some_other_double" :0.1234,
		"some_text" : "text"
	})json";

	object obj = parser.deserialize(json_data);
	EXPECT_EQ(obj.size(), 6U);
	EXPECT_EQ(obj[u8"some_true"], true);
	EXPECT_EQ(obj[u8"some_false"], false);
	EXPECT_EQ(obj[u8"some_int"], 1234);
	EXPECT_DOUBLE_EQ(obj[u8"some_double"].get<double>(), 12.34);
	EXPECT_DOUBLE_EQ(obj[u8"some_other_double"].get<double>(), 0.1234);
	EXPECT_EQ(obj[u8"some_text"], u8"text");
}

TEST(JsonParser, deserialize_map_nested) {
	json_parser parser;

	constexpr const char* json_data = R"json({
		"some_text" : "text",
		"some_object" : {
			"some_null_object": {}

		}	,
		"some_other_object" : {
			"beans": {
				"fruit": true
	,			"magical": true	,
				"makes toot": true
			}},
		"some other text":" asdf "
	})json";

	object obj = parser.deserialize(json_data);
	EXPECT_EQ(obj.size(), 4U);
	EXPECT_EQ(obj[u8"some_text"], u8"text");
	EXPECT_EQ(obj[u8"some other text"], u8" asdf ");

	// some_object
	EXPECT_FALSE(obj[u8"some_object"].null());
	EXPECT_EQ(obj[u8"some_object"].size(), 1U);
	EXPECT_FALSE(obj[u8"some_object"][u8"some_null_object"].null());
	EXPECT_EQ(obj[u8"some_object"][u8"some_null_object"].size(), 0U);

	// some_other_object
	EXPECT_FALSE(obj[u8"some_other_object"].null());
	EXPECT_EQ(obj[u8"some_other_object"].size(), 1U);
	EXPECT_FALSE(obj[u8"some_other_object"][u8"beans"].null());
	EXPECT_EQ(obj[u8"some_other_object"][u8"beans"].size(), 3U);
	EXPECT_EQ(obj[u8"some_other_object"][u8"beans"][u8"fruit"], true);
	EXPECT_EQ(obj[u8"some_other_object"][u8"beans"][u8"magical"], true);
	EXPECT_EQ(obj[u8"some_other_object"][u8"beans"][u8"makes toot"], true);
}
