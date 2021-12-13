/**
 * Copyright (C) 2021 Jessica James.
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

#include "jessilib/http_query.hpp"
#include <charconv>
#include "test.hpp"

using namespace std::literals;

// Compile-time tests for constexpr on compilers which support C++20 constexpr std::u8string
#if defined(__cpp_lib_constexpr_string) && (__GNUC__ >= 12 || _MSC_VER >= 1929)
constexpr std::u8string query_constexpr(std::u8string_view in_expression) {
	std::u8string result{ static_cast<std::u8string>(in_expression) };
	jessilib::deserialize_http_query(result);
	return result;
}
ASSERT_COMPILES_CONSTEXPR(return query_constexpr(u8"test"s) == u8"test"s);
ASSERT_COMPILES_CONSTEXPR(return query_constexpr(u8"first+second"s) == u8"first second"s);
ASSERT_COMPILES_CONSTEXPR(return query_constexpr(u8"first%20second"s) == u8"first second"s);
#endif // __cpp_lib_constexpr_string

using char_types = ::testing::Types</*char,*/ char8_t, char16_t, char32_t>;
using utf8_char_types = ::testing::Types</*char,*/ char8_t>;

template<typename T>
class QuerySequenceTest : public ::testing::Test {
public:
};
TYPED_TEST_SUITE(QuerySequenceTest, utf8_char_types);

constexpr char32_t MAX_LOOP_CODEPOINT = 0x100FF; // use 0x10FFFF for full testing

TYPED_TEST(QuerySequenceTest, single_chars) {
	// [U+0000, U+100FF)
	for (char32_t codepoint = 0; codepoint < MAX_LOOP_CODEPOINT; ++codepoint) {
		std::basic_string<TypeParam> expected;
		size_t units = jessilib::encode_codepoint(expected, codepoint);
		EXPECT_NE(units, 0);
		EXPECT_EQ(units, expected.size());

		// Construct the query string
		std::basic_string<TypeParam> query_string;
		for (auto& unit : expected) {
			char encoded[3] { '%', 0, 0 };
			char* encoded_end = encoded + sizeof(encoded);
			auto to_chars_result = std::to_chars(encoded + 1, encoded_end, static_cast<unsigned char>(unit), 16);
			ASSERT_EQ(to_chars_result.ec, std::errc{}) // assertion will fail when `unit` is signed type
							<< "For unit " << static_cast<int>(unit) << " in codepoint " << static_cast<int>(codepoint) << std::endl;

			if (to_chars_result.ptr != encoded_end) {
				// Only wrote one hex; shift it
				encoded[2] = encoded[1];
				encoded[1] = '0';
			}

			EXPECT_EQ(encoded[0], '%');
			EXPECT_NE(encoded[1], 0);
			EXPECT_NE(encoded[2], 0);
			query_string.insert(query_string.end(), encoded, encoded_end);
		}
		EXPECT_EQ(query_string.size(), expected.size() * 3);

		// Decode & check the query string
		jessilib::deserialize_http_query(query_string);
		EXPECT_EQ(query_string, expected);
	}
}

TYPED_TEST(QuerySequenceTest, invalids) {
	std::basic_string<TypeParam> query_string, long_query_string;
	for (size_t unit = 0; unit <= 0xFF; ++unit) {
		TypeParam encoded[2] { '%', static_cast<TypeParam>(unit) };
		TypeParam* encoded_end = encoded + sizeof(encoded);
		query_string.insert(query_string.end(), encoded, encoded_end);

		long_query_string += query_string;
		jessilib::deserialize_http_query(query_string);
		EXPECT_TRUE(query_string.empty())
						<< "in unit: " << unit << std::endl;
	}

	jessilib::deserialize_http_query(long_query_string);
	EXPECT_TRUE(long_query_string.empty());
}

TYPED_TEST(QuerySequenceTest, invalids_2len) {
	std::basic_string<TypeParam> query_string, long_query_string;
	for (size_t unit = 0; unit <= 0xFFFF; ++unit) {
		TypeParam first = static_cast<TypeParam>(unit >> 8); // order of these two doesn't matter
		TypeParam second = static_cast<TypeParam>(unit & 0xFF);
		if (jessilib::as_base(first, 16) >= 0
			&& jessilib::as_base(second, 16) >= 0) {
			continue;
		}
		TypeParam encoded[3] { '%', static_cast<TypeParam>(first), static_cast<TypeParam>(second) };
		TypeParam* encoded_end = encoded + sizeof(encoded);
		query_string.insert(query_string.end(), encoded, encoded_end);

		long_query_string += query_string;
		jessilib::deserialize_http_query(query_string);
		EXPECT_TRUE(query_string.empty())
						<< "in unit: " << unit << std::endl;
	}

	jessilib::deserialize_http_query(long_query_string);
	EXPECT_TRUE(long_query_string.empty());
}

TYPED_TEST(QuerySequenceTest, invalids_trailing) {
	std::basic_string<TypeParam> query_string, long_query_string;
	for (size_t unit = 0; unit <= 0xFF; ++unit) {
		TypeParam encoded[3] { '%', static_cast<TypeParam>(unit), '%' };
		TypeParam* encoded_end = encoded + sizeof(encoded);
		query_string.insert(query_string.end(), encoded, encoded_end);

		long_query_string += query_string;
		jessilib::deserialize_http_query(query_string);
		EXPECT_TRUE(query_string.empty())
						<< "in unit: " << unit << std::endl;
	}

	jessilib::deserialize_http_query(long_query_string);
	EXPECT_TRUE(long_query_string.empty());
}

TYPED_TEST(QuerySequenceTest, invalids_2len_trailing) {
	std::basic_string<TypeParam> query_string, long_query_string;
	for (size_t unit = 0; unit <= 0xFFFF; ++unit) {
		TypeParam first = static_cast<TypeParam>(unit >> 8); // order of these two doesn't matter
		TypeParam second = static_cast<TypeParam>(unit & 0xFF);
		if (jessilib::as_base(first, 16) >= 0
			&& jessilib::as_base(second, 16) >= 0) {
			continue;
		}
		TypeParam encoded[4] { '%', static_cast<TypeParam>(first), static_cast<TypeParam>(second), '%' };
		TypeParam* encoded_end = encoded + sizeof(encoded);
		query_string.insert(query_string.end(), encoded, encoded_end);

		long_query_string += query_string;
		jessilib::deserialize_http_query(query_string);
		EXPECT_TRUE(query_string.empty())
						<< "in unit: " << unit << std::endl;
	}

	jessilib::deserialize_http_query(long_query_string);
	EXPECT_TRUE(long_query_string.empty());
}

TEST(HtmlFormParser, empty) {
	std::vector<std::pair<std::u8string_view, std::u8string_view>> parsed_result;
	std::u8string query_text;
	EXPECT_TRUE(jessilib::deserialize_html_form(parsed_result, query_text));
	EXPECT_TRUE(query_text.empty());
	EXPECT_TRUE(parsed_result.empty());
}

TEST(HtmlFormParser, one_key) {
	std::vector<std::pair<std::u8string_view, std::u8string_view>> parsed_result;
	std::u8string query_text = u8"key";
	EXPECT_TRUE(jessilib::deserialize_html_form(parsed_result, query_text));
	EXPECT_EQ(query_text, u8"key");
	EXPECT_EQ(parsed_result.size(), 1);
	EXPECT_EQ(parsed_result[0].first, query_text);
	EXPECT_TRUE(parsed_result[0].second.empty());
}

TEST(HtmlFormParser, one_key_and_value) {
	std::vector<std::pair<std::u8string_view, std::u8string_view>> parsed_result;
	std::u8string query_text = u8"key=value";
	EXPECT_TRUE(jessilib::deserialize_html_form(parsed_result, query_text));
	EXPECT_TRUE(query_text.starts_with(u8"keyvalue"));
	EXPECT_EQ(parsed_result.size(), 1);
	EXPECT_EQ(parsed_result[0].first, u8"key");
	EXPECT_EQ(parsed_result[0].second, u8"value");
}

TEST(HtmlFormParser, one_key_and_value_trailing) {
	std::vector<std::pair<std::u8string_view, std::u8string_view>> parsed_result;
	std::u8string query_text = u8"key=value&";
	EXPECT_TRUE(jessilib::deserialize_html_form(parsed_result, query_text));
	EXPECT_TRUE(query_text.starts_with(u8"keyvalue"));
	EXPECT_EQ(parsed_result.size(), 2);
	EXPECT_EQ(parsed_result[0].first, u8"key");
	EXPECT_EQ(parsed_result[0].second, u8"value");
	EXPECT_TRUE(parsed_result[1].first.empty());
	EXPECT_TRUE(parsed_result[1].second.empty());
}

TEST(HtmlFormParser, two_key_one_value) {
	std::vector<std::pair<std::u8string_view, std::u8string_view>> parsed_result;
	std::u8string query_text = u8"key=value&second_key";
	EXPECT_TRUE(jessilib::deserialize_html_form(parsed_result, query_text));
	EXPECT_TRUE(query_text.starts_with(u8"keyvaluesecond_key"));
	EXPECT_EQ(parsed_result.size(), 2);
	EXPECT_EQ(parsed_result[0].first, u8"key");
	EXPECT_EQ(parsed_result[0].second, u8"value");
	EXPECT_EQ(parsed_result[1].first, u8"second_key");
	EXPECT_TRUE(parsed_result[1].second.empty());
}

TEST(HtmlFormParser, two_key_two_value) {
	std::vector<std::pair<std::u8string_view, std::u8string_view>> parsed_result;
	std::u8string query_text = u8"key=value&second_key=second=value";
	EXPECT_TRUE(jessilib::deserialize_html_form(parsed_result, query_text));
	EXPECT_TRUE(query_text.starts_with(u8"keyvaluesecond_keysecond=value"));
	EXPECT_EQ(parsed_result.size(), 2);
	EXPECT_EQ(parsed_result[0].first, u8"key");
	EXPECT_EQ(parsed_result[0].second, u8"value");
	EXPECT_EQ(parsed_result[1].first, u8"second_key");
	EXPECT_EQ(parsed_result[1].second, u8"second=value");
}

TEST(HtmlFormParser, some_sequences) {
	std::vector<std::pair<std::u8string_view, std::u8string_view>> parsed_result;
	std::u8string query_text = u8"k+y=va+u%20&%73econd%5Fke%79=second_valu%65";
	EXPECT_TRUE(jessilib::deserialize_html_form(parsed_result, query_text));
	EXPECT_TRUE(query_text.starts_with(u8"k yva u second_keysecond_value"));
	EXPECT_EQ(parsed_result.size(), 2);
	EXPECT_EQ(parsed_result[0].first, u8"k y");
	EXPECT_EQ(parsed_result[0].second, u8"va u ");
	EXPECT_EQ(parsed_result[1].first, u8"second_key");
	EXPECT_EQ(parsed_result[1].second, u8"second_value");
}
