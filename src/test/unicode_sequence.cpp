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

#include "jessilib/unicode_sequence.hpp"
#include <charconv>
#include "test.hpp"

using char_types = ::testing::Types<char, char8_t, char16_t, char32_t>;
using utf8_char_types = ::testing::Types<char, char8_t>;
using char_type_combos = ::testing::Types<
	std::pair<char, char>, std::pair<char, char8_t>, std::pair<char, char16_t>, std::pair<char, char32_t>,
	std::pair<char8_t, char>, std::pair<char8_t, char8_t>, std::pair<char8_t, char16_t>, std::pair<char8_t, char32_t>,
	std::pair<char16_t, char>, std::pair<char16_t, char8_t>, std::pair<char16_t, char16_t>, std::pair<char16_t, char32_t>,
	std::pair<char32_t, char>, std::pair<char32_t, char8_t>, std::pair<char32_t, char16_t>, std::pair<char32_t, char32_t>>;

template<typename T>
class UnicodeSequenceTest : public ::testing::Test {
public:
};
TYPED_TEST_SUITE(UnicodeSequenceTest, char_types);

template<typename T>
class UnicodeUTF8SequenceTest : public ::testing::Test {
public:
};
TYPED_TEST_SUITE(UnicodeUTF8SequenceTest, utf8_char_types);

constexpr char32_t MAX_LOOP_CODEPOINT = 0x100FF; // use 0x10FFFF for full testing

#define TEST_CPP_SEQUENCE(expr) \
	{ auto parsed_string = jessilib::string_cast<TypeParam>(#expr); \
    auto normal_string = jessilib::string_cast<TypeParam>(expr); \
	parsed_string = parsed_string.substr(1, parsed_string.size() - 2); \
	jessilib::apply_cpp_escape_sequences(parsed_string); \
	EXPECT_EQ(parsed_string, normal_string); }

TYPED_TEST(UnicodeSequenceTest, cpp_simple) {
	// Most basic of tests
	TEST_CPP_SEQUENCE("test")
	TEST_CPP_SEQUENCE("\"test\"")

	// Do each character once
	TEST_CPP_SEQUENCE("\'")
	TEST_CPP_SEQUENCE("\"")
	TEST_CPP_SEQUENCE("\?")
	TEST_CPP_SEQUENCE("\\")
	TEST_CPP_SEQUENCE("\a")
	TEST_CPP_SEQUENCE("\b")
	TEST_CPP_SEQUENCE("\f")
	TEST_CPP_SEQUENCE("\n")
	TEST_CPP_SEQUENCE("\r")
	TEST_CPP_SEQUENCE("\t")
	TEST_CPP_SEQUENCE("\v")
}

TYPED_TEST(UnicodeSequenceTest, cpp_octal) {
	// "\0" -> "\177" with & without leading zeroes
	std::basic_string<TypeParam> parsed_string;
	for (unsigned int codepoint = 0; codepoint <= 0377; ++codepoint) {
		uint8_t front = (codepoint >> 6);
		uint8_t middle = (codepoint & 0b00'111'000) >> 3;
		uint8_t last = (codepoint & 0b00'000'111);

		// "\000" -> "\177"
		parsed_string = static_cast<TypeParam>('\\');
		parsed_string += static_cast<TypeParam>('0' + front);
		parsed_string += static_cast<TypeParam>('0' + middle);
		parsed_string += static_cast<TypeParam>('0' + last);
		jessilib::apply_cpp_escape_sequences(parsed_string);
		EXPECT_EQ(parsed_string.front(), static_cast<TypeParam>(codepoint));

		if (front == 0) {
			// "\00" -> "\77"
			parsed_string = static_cast<TypeParam>('\\');
			parsed_string += static_cast<TypeParam>('0' + middle);
			parsed_string += static_cast<TypeParam>('0' + last);
			jessilib::apply_cpp_escape_sequences(parsed_string);
			EXPECT_EQ(parsed_string.front(), static_cast<TypeParam>(codepoint));

			if (middle == 0) {
				// "\0" -> "\7"
				parsed_string = static_cast<TypeParam>('\\');
				parsed_string += static_cast<TypeParam>('0' + last);
				jessilib::apply_cpp_escape_sequences(parsed_string);
				EXPECT_EQ(parsed_string.front(), static_cast<TypeParam>(codepoint));
			}
		}
	}
}

template<typename CharT, typename IntegerT>
std::basic_string<CharT> make_hex_string(IntegerT in_integer, size_t min_length = 0) {
	char buffer[32];
	auto buffer_end = std::to_chars(buffer, std::end(buffer), in_integer, 16).ptr;
	std::basic_string<CharT> result{ buffer, buffer_end };

	if (min_length > result.size()) {
		result.insert(0, min_length - result.size(), static_cast<CharT>('0'));
	}

	return result;
}

TYPED_TEST(UnicodeSequenceTest, cpp_hex) {
	// "x0" -> "xff" with & without leading zeroes
	if constexpr (sizeof(TypeParam) == 1) {
		for (unsigned int codepoint = 0; codepoint <= 0xFF; ++codepoint) {
			std::basic_string<TypeParam> parsed_string;
			for (size_t min_length = 0; min_length <= 2; ++min_length) {
				parsed_string = jessilib::string_cast<TypeParam>("\\x");
				parsed_string += make_hex_string<TypeParam>(codepoint, min_length);
				jessilib::apply_cpp_escape_sequences(parsed_string);
				EXPECT_EQ(parsed_string.front(), static_cast<TypeParam>(codepoint));
			}
		}
		return;
	}

	// "x0" -> "xffff" with & without leading zeroes
	if constexpr (sizeof(TypeParam) == 2) {
		for (unsigned int codepoint = 0; codepoint <= 0xFFFF; ++codepoint) {
			std::basic_string<TypeParam> parsed_string;
			for (size_t min_length = 0; min_length <= 4; ++min_length) {
				// "\x0" -> "\xffff"
				parsed_string = jessilib::string_cast<TypeParam>("\\x");
				parsed_string += make_hex_string<TypeParam>(codepoint, min_length);
				jessilib::apply_cpp_escape_sequences(parsed_string);
				EXPECT_EQ(parsed_string.front(), static_cast<TypeParam>(codepoint));
			}
		}

		return;
	}

	// "x0" -> "x100ff" with & without leading zeroes
	if constexpr (sizeof(TypeParam) == 4) {
		for (unsigned int codepoint = 0; codepoint <= MAX_LOOP_CODEPOINT; ++codepoint) {
			std::basic_string<TypeParam> parsed_string;
			for (size_t min_length = 0; min_length <= 8; ++min_length) {
				// "\x0" -> "\x0010ffff"
				parsed_string = jessilib::string_cast<TypeParam>("\\x");
				parsed_string += make_hex_string<TypeParam>(codepoint, min_length);
				jessilib::apply_cpp_escape_sequences(parsed_string);
				EXPECT_EQ(parsed_string.front(), static_cast<TypeParam>(codepoint));
			}
		}

		return;
	}
}

TYPED_TEST(UnicodeSequenceTest, cpp_u16) {
	// "u0000" -> "uffff" with & without leading zeroes
	for (unsigned int codepoint = 0; codepoint <= 0xFFFF; ++codepoint) {
		std::basic_string<TypeParam> parsed_string = jessilib::string_cast<TypeParam>("\\u");
		parsed_string += make_hex_string<TypeParam>(codepoint, 4);
		jessilib::apply_cpp_escape_sequences(parsed_string);

		auto decode = jessilib::decode_codepoint(parsed_string);
		EXPECT_NE(decode.units, 0);
		EXPECT_EQ(decode.codepoint, static_cast<char32_t>(codepoint));
	}
}

TYPED_TEST(UnicodeSequenceTest, cpp_u32) {
	// "U00000000" -> "U000100FF" with & without leading zeroes
	for (unsigned int codepoint = 0; codepoint <= MAX_LOOP_CODEPOINT; ++codepoint) {
		std::basic_string<TypeParam> parsed_string = jessilib::string_cast<TypeParam>("\\U");
		parsed_string += make_hex_string<TypeParam>(codepoint, 8);
		jessilib::apply_cpp_escape_sequences(parsed_string);

		auto decode = jessilib::decode_codepoint(parsed_string);
		EXPECT_NE(decode.units, 0);
		EXPECT_EQ(decode.codepoint, static_cast<char32_t>(codepoint));
	}
}

/**
 * Query strings
 */

TYPED_TEST(UnicodeUTF8SequenceTest, single_chars) {
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

TYPED_TEST(UnicodeUTF8SequenceTest, invalids) {
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

TYPED_TEST(UnicodeUTF8SequenceTest, invalids_2len) {
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

TYPED_TEST(UnicodeUTF8SequenceTest, invalids_trailing) {
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

TYPED_TEST(UnicodeUTF8SequenceTest, invalids_2len_trailing) {
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
