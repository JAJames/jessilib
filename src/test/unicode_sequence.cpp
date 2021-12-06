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
#include "jessilib/unicode.hpp" // string_cast
#include "test.hpp"

using namespace std::literals;

// Compile-time tests for constexpr on compilers which support C++20 constexpr std::string
#ifdef __cpp_lib_constexpr_string
constexpr std::string cpp_constexpr(std::string_view in_expression) {
	std::string result{ static_cast<std::string>(in_expression) };
	jessilib::apply_cpp_escape_sequences(result);
	return result;
}
static_assert(cpp_constexpr("test"s) == "test"s);
static_assert(cpp_constexpr("\\r\\n"s) == "\r\n"s);
#endif // __cpp_lib_constexpr_string

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

		auto decode = jessilib::decode_codepoint(parsed_string.data(), parsed_string.size());
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

		auto decode = jessilib::decode_codepoint(parsed_string.data(), parsed_string.size());
		EXPECT_NE(decode.units, 0);
		EXPECT_EQ(decode.codepoint, static_cast<char32_t>(codepoint));
	}
}
