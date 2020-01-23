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
#include "test.hpp"
#include "object.hpp"

using namespace jessilib;
using namespace std::literals;

using signed_char_t = signed char;
using unsigned_char_t = unsigned char;
using long_long_t = long long;
using long_double_t = long double;

/** basic tests; these test function calls against null objects and heavily test for compilation errors */

TEST(ObjectTest, basic) {
	object obj;

	EXPECT_TRUE(obj.null());
	EXPECT_EQ(obj.size(), 0U);
}

/** basic_has tests */

TEST(ObjectTest, basic_has) {
	object obj;

	EXPECT_FALSE(obj.has<bool>());
	EXPECT_FALSE(obj.has<signed char>());
	EXPECT_FALSE(obj.has<unsigned char>());
	EXPECT_FALSE(obj.has<short>());
	EXPECT_FALSE(obj.has<int>());
	EXPECT_FALSE(obj.has<long>());
	EXPECT_FALSE(obj.has<long long>());
	EXPECT_FALSE(obj.has<intmax_t>());
	EXPECT_FALSE(obj.has<float>());
	EXPECT_FALSE(obj.has<double>());
	EXPECT_FALSE(obj.has<long double>());
	EXPECT_FALSE(obj.has<std::string>());
	EXPECT_FALSE(obj.has<object::array_t>());
	EXPECT_FALSE(obj.has<object::map_t>());
}

TEST(ObjectTest, basic_has_vector) {
	object obj;

	EXPECT_FALSE(obj.has<std::vector<bool>>());
	EXPECT_FALSE(obj.has<std::vector<signed char>>());
	EXPECT_FALSE(obj.has<std::vector<unsigned char>>());
	EXPECT_FALSE(obj.has<std::vector<short>>());
	EXPECT_FALSE(obj.has<std::vector<int>>());
	EXPECT_FALSE(obj.has<std::vector<long>>());
	EXPECT_FALSE(obj.has<std::vector<long long>>());
	EXPECT_FALSE(obj.has<std::vector<intmax_t>>());
	EXPECT_FALSE(obj.has<std::vector<float>>());
	EXPECT_FALSE(obj.has<std::vector<double>>());
	EXPECT_FALSE(obj.has<std::vector<long double>>());
	EXPECT_FALSE(obj.has<std::vector<std::string>>());
	EXPECT_FALSE(obj.has<std::vector<object>>());
}

TEST(ObjectTest, basic_has_list) {
	object obj;

	EXPECT_FALSE(obj.has<std::list<bool>>());
	EXPECT_FALSE(obj.has<std::list<signed char>>());
	EXPECT_FALSE(obj.has<std::list<unsigned char>>());
	EXPECT_FALSE(obj.has<std::list<short>>());
	EXPECT_FALSE(obj.has<std::list<int>>());
	EXPECT_FALSE(obj.has<std::list<long>>());
	EXPECT_FALSE(obj.has<std::list<long long>>());
	EXPECT_FALSE(obj.has<std::list<intmax_t>>());
	EXPECT_FALSE(obj.has<std::list<float>>());
	EXPECT_FALSE(obj.has<std::list<double>>());
	EXPECT_FALSE(obj.has<std::list<long double>>());
	EXPECT_FALSE(obj.has<std::list<std::string>>());
	EXPECT_FALSE(obj.has<std::list<object>>());
}

TEST(ObjectTest, basic_has_forward_list) {
	object obj;

	EXPECT_FALSE(obj.has<std::forward_list<bool>>());
	EXPECT_FALSE(obj.has<std::forward_list<signed char>>());
	EXPECT_FALSE(obj.has<std::forward_list<unsigned char>>());
	EXPECT_FALSE(obj.has<std::forward_list<short>>());
	EXPECT_FALSE(obj.has<std::forward_list<int>>());
	EXPECT_FALSE(obj.has<std::forward_list<long>>());
	EXPECT_FALSE(obj.has<std::forward_list<long long>>());
	EXPECT_FALSE(obj.has<std::forward_list<intmax_t>>());
	EXPECT_FALSE(obj.has<std::forward_list<float>>());
	EXPECT_FALSE(obj.has<std::forward_list<double>>());
	EXPECT_FALSE(obj.has<std::forward_list<long double>>());
	EXPECT_FALSE(obj.has<std::forward_list<std::string>>());
	EXPECT_FALSE(obj.has<std::forward_list<object>>());
}

TEST(ObjectTest, basic_has_set) {
	object obj;

	EXPECT_FALSE(obj.has<std::set<bool>>());
	EXPECT_FALSE(obj.has<std::set<signed char>>());
	EXPECT_FALSE(obj.has<std::set<unsigned char>>());
	EXPECT_FALSE(obj.has<std::set<short>>());
	EXPECT_FALSE(obj.has<std::set<int>>());
	EXPECT_FALSE(obj.has<std::set<long>>());
	EXPECT_FALSE(obj.has<std::set<long long>>());
	EXPECT_FALSE(obj.has<std::set<intmax_t>>());
	EXPECT_FALSE(obj.has<std::set<float>>());
	EXPECT_FALSE(obj.has<std::set<double>>());
	EXPECT_FALSE(obj.has<std::set<long double>>());
	EXPECT_FALSE(obj.has<std::set<std::string>>());
	EXPECT_FALSE(obj.has<std::set<object>>());
}

TEST(ObjectTest, basic_has_unordered_set) {
	object obj;

	EXPECT_FALSE(obj.has<std::unordered_set<bool>>());
	EXPECT_FALSE(obj.has<std::unordered_set<signed char>>());
	EXPECT_FALSE(obj.has<std::unordered_set<unsigned char>>());
	EXPECT_FALSE(obj.has<std::unordered_set<short>>());
	EXPECT_FALSE(obj.has<std::unordered_set<int>>());
	EXPECT_FALSE(obj.has<std::unordered_set<long>>());
	EXPECT_FALSE(obj.has<std::unordered_set<long long>>());
	EXPECT_FALSE(obj.has<std::unordered_set<intmax_t>>());
	EXPECT_FALSE(obj.has<std::unordered_set<float>>());
	EXPECT_FALSE(obj.has<std::unordered_set<double>>());
	EXPECT_FALSE(obj.has<std::unordered_set<long double>>());
	EXPECT_FALSE(obj.has<std::unordered_set<std::string>>());
	EXPECT_FALSE(obj.has<std::unordered_set<object>>());
}

using map_str_str = std::map<std::string, std::string>;
using map_str_strv = std::map<std::string, std::string_view>;
using map_str_obj = std::map<std::string, object>;

using unordered_map_str_str = std::unordered_map<std::string, std::string>;
using unordered_map_str_strv = std::unordered_map<std::string, std::string_view>;
using unordered_map_str_obj = std::unordered_map<std::string, object>;

TEST(ObjectTest, basic_has_map) {
	object obj;

	EXPECT_FALSE(obj.has<map_str_str>());
	EXPECT_FALSE(obj.has<map_str_strv>());
	EXPECT_FALSE(obj.has<map_str_obj>());
}

TEST(ObjectTest, basic_has_unordered_map) {
	object obj;

	EXPECT_FALSE(obj.has<unordered_map_str_str>());
	EXPECT_FALSE(obj.has<unordered_map_str_strv>());
	EXPECT_FALSE(obj.has<unordered_map_str_obj>());
}

/** basic_get tests */

TEST(ObjectTest, basic_get) {
	object obj;

	EXPECT_EQ(obj.get<bool>(), bool{});
	EXPECT_EQ(obj.get<signed char>(), signed_char_t{});
	EXPECT_EQ(obj.get<unsigned char>(), unsigned_char_t{});
	EXPECT_EQ(obj.get<short>(), short{});
	EXPECT_EQ(obj.get<int>(), int{});
	EXPECT_EQ(obj.get<long>(), long{});
	EXPECT_EQ(obj.get<long long>(), long_long_t{});
	EXPECT_EQ(obj.get<intmax_t>(), intmax_t{});
	EXPECT_EQ(obj.get<float>(), float{});
	EXPECT_EQ(obj.get<double>(), double{});
	EXPECT_EQ(obj.get<long double>(), long_double_t{});
	EXPECT_EQ(obj.get<std::string>(), std::string{});
	EXPECT_TRUE(obj.get<object::array_t>().empty());
	EXPECT_TRUE(obj.get<object::map_t>().empty());
}

TEST(ObjectTest, basic_get_vector) {
	object obj;

	EXPECT_TRUE(obj.get<std::vector<bool>>().empty());
	EXPECT_TRUE(obj.get<std::vector<signed char>>().empty());
	EXPECT_TRUE(obj.get<std::vector<unsigned char>>().empty());
	EXPECT_TRUE(obj.get<std::vector<short>>().empty());
	EXPECT_TRUE(obj.get<std::vector<int>>().empty());
	EXPECT_TRUE(obj.get<std::vector<long>>().empty());
	EXPECT_TRUE(obj.get<std::vector<long long>>().empty());
	EXPECT_TRUE(obj.get<std::vector<intmax_t>>().empty());
	EXPECT_TRUE(obj.get<std::vector<float>>().empty());
	EXPECT_TRUE(obj.get<std::vector<double>>().empty());
	EXPECT_TRUE(obj.get<std::vector<long double>>().empty());
	EXPECT_TRUE(obj.get<std::vector<std::string>>().empty());
	EXPECT_TRUE(obj.get<std::vector<object>>().empty());
}

TEST(ObjectTest, basic_get_list) {
	object obj;

	EXPECT_TRUE(obj.get<std::list<bool>>().empty());
	EXPECT_TRUE(obj.get<std::list<signed char>>().empty());
	EXPECT_TRUE(obj.get<std::list<unsigned char>>().empty());
	EXPECT_TRUE(obj.get<std::list<short>>().empty());
	EXPECT_TRUE(obj.get<std::list<int>>().empty());
	EXPECT_TRUE(obj.get<std::list<long>>().empty());
	EXPECT_TRUE(obj.get<std::list<long long>>().empty());
	EXPECT_TRUE(obj.get<std::list<intmax_t>>().empty());
	EXPECT_TRUE(obj.get<std::list<float>>().empty());
	EXPECT_TRUE(obj.get<std::list<double>>().empty());
	EXPECT_TRUE(obj.get<std::list<long double>>().empty());
	EXPECT_TRUE(obj.get<std::list<std::string>>().empty());
	EXPECT_TRUE(obj.get<std::list<object>>().empty());
}

TEST(ObjectTest, basic_get_forward_list) {
	object obj;

	EXPECT_TRUE(obj.get<std::forward_list<bool>>().empty());
	EXPECT_TRUE(obj.get<std::forward_list<signed char>>().empty());
	EXPECT_TRUE(obj.get<std::forward_list<unsigned char>>().empty());
	EXPECT_TRUE(obj.get<std::forward_list<short>>().empty());
	EXPECT_TRUE(obj.get<std::forward_list<int>>().empty());
	EXPECT_TRUE(obj.get<std::forward_list<long>>().empty());
	EXPECT_TRUE(obj.get<std::forward_list<long long>>().empty());
	EXPECT_TRUE(obj.get<std::forward_list<intmax_t>>().empty());
	EXPECT_TRUE(obj.get<std::forward_list<float>>().empty());
	EXPECT_TRUE(obj.get<std::forward_list<double>>().empty());
	EXPECT_TRUE(obj.get<std::forward_list<long double>>().empty());
	EXPECT_TRUE(obj.get<std::forward_list<std::string>>().empty());
	EXPECT_TRUE(obj.get<std::forward_list<object>>().empty());
}

TEST(ObjectTest, basic_get_set) {
	object obj;

	EXPECT_TRUE(obj.get<std::set<bool>>().empty());
	EXPECT_TRUE(obj.get<std::set<signed char>>().empty());
	EXPECT_TRUE(obj.get<std::set<unsigned char>>().empty());
	EXPECT_TRUE(obj.get<std::set<short>>().empty());
	EXPECT_TRUE(obj.get<std::set<int>>().empty());
	EXPECT_TRUE(obj.get<std::set<long>>().empty());
	EXPECT_TRUE(obj.get<std::set<long long>>().empty());
	EXPECT_TRUE(obj.get<std::set<intmax_t>>().empty());
	EXPECT_TRUE(obj.get<std::set<float>>().empty());
	EXPECT_TRUE(obj.get<std::set<double>>().empty());
	EXPECT_TRUE(obj.get<std::set<long double>>().empty());
	EXPECT_TRUE(obj.get<std::set<std::string>>().empty());
	EXPECT_TRUE(obj.get<std::set<object>>().empty());
}

TEST(ObjectTest, basic_get_multiset) {
	object obj;

	EXPECT_TRUE(obj.get<std::multiset<bool>>().empty());
	EXPECT_TRUE(obj.get<std::multiset<signed char>>().empty());
	EXPECT_TRUE(obj.get<std::multiset<unsigned char>>().empty());
	EXPECT_TRUE(obj.get<std::multiset<short>>().empty());
	EXPECT_TRUE(obj.get<std::multiset<int>>().empty());
	EXPECT_TRUE(obj.get<std::multiset<long>>().empty());
	EXPECT_TRUE(obj.get<std::multiset<long long>>().empty());
	EXPECT_TRUE(obj.get<std::multiset<intmax_t>>().empty());
	EXPECT_TRUE(obj.get<std::multiset<float>>().empty());
	EXPECT_TRUE(obj.get<std::multiset<double>>().empty());
	EXPECT_TRUE(obj.get<std::multiset<long double>>().empty());
	EXPECT_TRUE(obj.get<std::multiset<std::string>>().empty());
	EXPECT_TRUE(obj.get<std::multiset<object>>().empty());
}

TEST(ObjectTest, basic_get_unordered_set) {
	object obj;

	EXPECT_TRUE(obj.get<std::unordered_set<bool>>().empty());
	EXPECT_TRUE(obj.get<std::unordered_set<signed char>>().empty());
	EXPECT_TRUE(obj.get<std::unordered_set<unsigned char>>().empty());
	EXPECT_TRUE(obj.get<std::unordered_set<short>>().empty());
	EXPECT_TRUE(obj.get<std::unordered_set<int>>().empty());
	EXPECT_TRUE(obj.get<std::unordered_set<long>>().empty());
	EXPECT_TRUE(obj.get<std::unordered_set<long long>>().empty());
	EXPECT_TRUE(obj.get<std::unordered_set<intmax_t>>().empty());
	EXPECT_TRUE(obj.get<std::unordered_set<float>>().empty());
	EXPECT_TRUE(obj.get<std::unordered_set<double>>().empty());
	EXPECT_TRUE(obj.get<std::unordered_set<long double>>().empty());
	EXPECT_TRUE(obj.get<std::unordered_set<std::string>>().empty());
	EXPECT_TRUE(obj.get<std::unordered_set<object>>().empty());
}

TEST(ObjectTest, basic_get_unordered_multiset) {
	object obj;

	EXPECT_TRUE(obj.get<std::unordered_multiset<bool>>().empty());
	EXPECT_TRUE(obj.get<std::unordered_multiset<signed char>>().empty());
	EXPECT_TRUE(obj.get<std::unordered_multiset<unsigned char>>().empty());
	EXPECT_TRUE(obj.get<std::unordered_multiset<short>>().empty());
	EXPECT_TRUE(obj.get<std::unordered_multiset<int>>().empty());
	EXPECT_TRUE(obj.get<std::unordered_multiset<long>>().empty());
	EXPECT_TRUE(obj.get<std::unordered_multiset<long long>>().empty());
	EXPECT_TRUE(obj.get<std::unordered_multiset<intmax_t>>().empty());
	EXPECT_TRUE(obj.get<std::unordered_multiset<float>>().empty());
	EXPECT_TRUE(obj.get<std::unordered_multiset<double>>().empty());
	EXPECT_TRUE(obj.get<std::unordered_multiset<long double>>().empty());
	EXPECT_TRUE(obj.get<std::unordered_multiset<std::string>>().empty());
	EXPECT_TRUE(obj.get<std::unordered_set<object>>().empty());
}

/** basic_value_constructor */

#define OBJECT_BASIC_VALUE_CONSTRUCTOR_TEST(in_type) \
	{ object test_object__{ in_type {} }; \
	EXPECT_TRUE(test_object__ .has< in_type >()); \
	EXPECT_EQ(test_object__.get< in_type >(), in_type {} ); }

// asdf

TEST(ObjectTest, basic_value_constructor) {
	OBJECT_BASIC_VALUE_CONSTRUCTOR_TEST(bool);
	OBJECT_BASIC_VALUE_CONSTRUCTOR_TEST(signed_char_t);
	OBJECT_BASIC_VALUE_CONSTRUCTOR_TEST(unsigned_char_t);
	OBJECT_BASIC_VALUE_CONSTRUCTOR_TEST(short);
	OBJECT_BASIC_VALUE_CONSTRUCTOR_TEST(int);
	OBJECT_BASIC_VALUE_CONSTRUCTOR_TEST(long);
	OBJECT_BASIC_VALUE_CONSTRUCTOR_TEST(long_long_t);
	OBJECT_BASIC_VALUE_CONSTRUCTOR_TEST(intmax_t);
	OBJECT_BASIC_VALUE_CONSTRUCTOR_TEST(float);
	OBJECT_BASIC_VALUE_CONSTRUCTOR_TEST(double);
	OBJECT_BASIC_VALUE_CONSTRUCTOR_TEST(long_double_t);
	OBJECT_BASIC_VALUE_CONSTRUCTOR_TEST(std::string);
	OBJECT_BASIC_VALUE_CONSTRUCTOR_TEST(object::array_t);

	// const char*
	{
		object obj{ "" };
		EXPECT_TRUE(obj.has<std::string>());
		EXPECT_EQ(obj.get<std::string>(), std::string{});
	}

	// std::string_view
	{
		object obj{ ""sv };
		EXPECT_TRUE(obj.has<std::string>());
		EXPECT_EQ(obj.get<std::string>(), std::string{});
	}
}

TEST(ObjectTest, basic_value_constructor_vector) {
	OBJECT_BASIC_VALUE_CONSTRUCTOR_TEST(std::vector<bool>);
	OBJECT_BASIC_VALUE_CONSTRUCTOR_TEST(std::vector<signed char>);
	OBJECT_BASIC_VALUE_CONSTRUCTOR_TEST(std::vector<unsigned char>);
	OBJECT_BASIC_VALUE_CONSTRUCTOR_TEST(std::vector<short>);
	OBJECT_BASIC_VALUE_CONSTRUCTOR_TEST(std::vector<int>);
	OBJECT_BASIC_VALUE_CONSTRUCTOR_TEST(std::vector<long>);
	OBJECT_BASIC_VALUE_CONSTRUCTOR_TEST(std::vector<long long>);
	OBJECT_BASIC_VALUE_CONSTRUCTOR_TEST(std::vector<intmax_t>);
	OBJECT_BASIC_VALUE_CONSTRUCTOR_TEST(std::vector<float>);
	OBJECT_BASIC_VALUE_CONSTRUCTOR_TEST(std::vector<double>);
	OBJECT_BASIC_VALUE_CONSTRUCTOR_TEST(std::vector<long double>);
	OBJECT_BASIC_VALUE_CONSTRUCTOR_TEST(std::vector<std::string>);
	OBJECT_BASIC_VALUE_CONSTRUCTOR_TEST(std::vector<object>);
}

TEST(ObjectTest, basic_value_constructor_list) {
	OBJECT_BASIC_VALUE_CONSTRUCTOR_TEST(std::list<bool>);
	OBJECT_BASIC_VALUE_CONSTRUCTOR_TEST(std::list<signed char>);
	OBJECT_BASIC_VALUE_CONSTRUCTOR_TEST(std::list<unsigned char>);
	OBJECT_BASIC_VALUE_CONSTRUCTOR_TEST(std::list<short>);
	OBJECT_BASIC_VALUE_CONSTRUCTOR_TEST(std::list<int>);
	OBJECT_BASIC_VALUE_CONSTRUCTOR_TEST(std::list<long>);
	OBJECT_BASIC_VALUE_CONSTRUCTOR_TEST(std::list<long long>);
	OBJECT_BASIC_VALUE_CONSTRUCTOR_TEST(std::list<intmax_t>);
	OBJECT_BASIC_VALUE_CONSTRUCTOR_TEST(std::list<float>);
	OBJECT_BASIC_VALUE_CONSTRUCTOR_TEST(std::list<double>);
	OBJECT_BASIC_VALUE_CONSTRUCTOR_TEST(std::list<long double>);
	OBJECT_BASIC_VALUE_CONSTRUCTOR_TEST(std::list<std::string>);
	OBJECT_BASIC_VALUE_CONSTRUCTOR_TEST(std::list<object>);
}

TEST(ObjectTest, basic_value_constructor_forward_list) {
	OBJECT_BASIC_VALUE_CONSTRUCTOR_TEST(std::forward_list<bool>);
	OBJECT_BASIC_VALUE_CONSTRUCTOR_TEST(std::forward_list<signed char>);
	OBJECT_BASIC_VALUE_CONSTRUCTOR_TEST(std::forward_list<unsigned char>);
	OBJECT_BASIC_VALUE_CONSTRUCTOR_TEST(std::forward_list<short>);
	OBJECT_BASIC_VALUE_CONSTRUCTOR_TEST(std::forward_list<int>);
	OBJECT_BASIC_VALUE_CONSTRUCTOR_TEST(std::forward_list<long>);
	OBJECT_BASIC_VALUE_CONSTRUCTOR_TEST(std::forward_list<long long>);
	OBJECT_BASIC_VALUE_CONSTRUCTOR_TEST(std::forward_list<intmax_t>);
	OBJECT_BASIC_VALUE_CONSTRUCTOR_TEST(std::forward_list<float>);
	OBJECT_BASIC_VALUE_CONSTRUCTOR_TEST(std::forward_list<double>);
	OBJECT_BASIC_VALUE_CONSTRUCTOR_TEST(std::forward_list<long double>);
	OBJECT_BASIC_VALUE_CONSTRUCTOR_TEST(std::forward_list<std::string>);
	OBJECT_BASIC_VALUE_CONSTRUCTOR_TEST(std::forward_list<object>);
}

TEST(ObjectTest, basic_value_constructor_set) {
	OBJECT_BASIC_VALUE_CONSTRUCTOR_TEST(std::set<bool>);
	OBJECT_BASIC_VALUE_CONSTRUCTOR_TEST(std::set<signed char>);
	OBJECT_BASIC_VALUE_CONSTRUCTOR_TEST(std::set<unsigned char>);
	OBJECT_BASIC_VALUE_CONSTRUCTOR_TEST(std::set<short>);
	OBJECT_BASIC_VALUE_CONSTRUCTOR_TEST(std::set<int>);
	OBJECT_BASIC_VALUE_CONSTRUCTOR_TEST(std::set<long>);
	OBJECT_BASIC_VALUE_CONSTRUCTOR_TEST(std::set<long long>);
	OBJECT_BASIC_VALUE_CONSTRUCTOR_TEST(std::set<intmax_t>);
	OBJECT_BASIC_VALUE_CONSTRUCTOR_TEST(std::set<float>);
	OBJECT_BASIC_VALUE_CONSTRUCTOR_TEST(std::set<double>);
	OBJECT_BASIC_VALUE_CONSTRUCTOR_TEST(std::set<long double>);
	OBJECT_BASIC_VALUE_CONSTRUCTOR_TEST(std::set<std::string>);
	OBJECT_BASIC_VALUE_CONSTRUCTOR_TEST(std::set<object>);
}

TEST(ObjectTest, basic_value_constructor_multiset) {
	OBJECT_BASIC_VALUE_CONSTRUCTOR_TEST(std::multiset<bool>);
	OBJECT_BASIC_VALUE_CONSTRUCTOR_TEST(std::multiset<signed char>);
	OBJECT_BASIC_VALUE_CONSTRUCTOR_TEST(std::multiset<unsigned char>);
	OBJECT_BASIC_VALUE_CONSTRUCTOR_TEST(std::multiset<short>);
	OBJECT_BASIC_VALUE_CONSTRUCTOR_TEST(std::multiset<int>);
	OBJECT_BASIC_VALUE_CONSTRUCTOR_TEST(std::multiset<long>);
	OBJECT_BASIC_VALUE_CONSTRUCTOR_TEST(std::multiset<long long>);
	OBJECT_BASIC_VALUE_CONSTRUCTOR_TEST(std::multiset<intmax_t>);
	OBJECT_BASIC_VALUE_CONSTRUCTOR_TEST(std::multiset<float>);
	OBJECT_BASIC_VALUE_CONSTRUCTOR_TEST(std::multiset<double>);
	OBJECT_BASIC_VALUE_CONSTRUCTOR_TEST(std::multiset<long double>);
	OBJECT_BASIC_VALUE_CONSTRUCTOR_TEST(std::multiset<std::string>);
	OBJECT_BASIC_VALUE_CONSTRUCTOR_TEST(std::multiset<object>);
}

TEST(ObjectTest, basic_value_constructor_unordered_set) {
	OBJECT_BASIC_VALUE_CONSTRUCTOR_TEST(std::unordered_set<bool>);
	OBJECT_BASIC_VALUE_CONSTRUCTOR_TEST(std::unordered_set<signed char>);
	OBJECT_BASIC_VALUE_CONSTRUCTOR_TEST(std::unordered_set<unsigned char>);
	OBJECT_BASIC_VALUE_CONSTRUCTOR_TEST(std::unordered_set<short>);
	OBJECT_BASIC_VALUE_CONSTRUCTOR_TEST(std::unordered_set<int>);
	OBJECT_BASIC_VALUE_CONSTRUCTOR_TEST(std::unordered_set<long>);
	OBJECT_BASIC_VALUE_CONSTRUCTOR_TEST(std::unordered_set<long long>);
	OBJECT_BASIC_VALUE_CONSTRUCTOR_TEST(std::unordered_set<intmax_t>);
	OBJECT_BASIC_VALUE_CONSTRUCTOR_TEST(std::unordered_set<float>);
	OBJECT_BASIC_VALUE_CONSTRUCTOR_TEST(std::unordered_set<double>);
	OBJECT_BASIC_VALUE_CONSTRUCTOR_TEST(std::unordered_set<long double>);
	OBJECT_BASIC_VALUE_CONSTRUCTOR_TEST(std::unordered_set<std::string>);
	OBJECT_BASIC_VALUE_CONSTRUCTOR_TEST(std::unordered_set<object>);
}

TEST(ObjectTest, basic_value_constructor_unordered_multiset) {
	OBJECT_BASIC_VALUE_CONSTRUCTOR_TEST(std::unordered_multiset<bool>);
	OBJECT_BASIC_VALUE_CONSTRUCTOR_TEST(std::unordered_multiset<signed char>);
	OBJECT_BASIC_VALUE_CONSTRUCTOR_TEST(std::unordered_multiset<unsigned char>);
	OBJECT_BASIC_VALUE_CONSTRUCTOR_TEST(std::unordered_multiset<short>);
	OBJECT_BASIC_VALUE_CONSTRUCTOR_TEST(std::unordered_multiset<int>);
	OBJECT_BASIC_VALUE_CONSTRUCTOR_TEST(std::unordered_multiset<long>);
	OBJECT_BASIC_VALUE_CONSTRUCTOR_TEST(std::unordered_multiset<long long>);
	OBJECT_BASIC_VALUE_CONSTRUCTOR_TEST(std::unordered_multiset<intmax_t>);
	OBJECT_BASIC_VALUE_CONSTRUCTOR_TEST(std::unordered_multiset<float>);
	OBJECT_BASIC_VALUE_CONSTRUCTOR_TEST(std::unordered_multiset<double>);
	OBJECT_BASIC_VALUE_CONSTRUCTOR_TEST(std::unordered_multiset<long double>);
	OBJECT_BASIC_VALUE_CONSTRUCTOR_TEST(std::unordered_multiset<std::string>);
	OBJECT_BASIC_VALUE_CONSTRUCTOR_TEST(std::unordered_set<object>);
}

/** basic_set */

#define OBJECT_BASIC_SET_TEST(in_object, in_type) \
	in_object .set< in_type >( in_type {} ); \
	EXPECT_TRUE(in_object .has< in_type >()); \
	EXPECT_EQ(in_object .get< in_type >(), in_type {} )

TEST(ObjectTest, basic_set) {
	object obj;

	OBJECT_BASIC_SET_TEST(obj, bool);
	OBJECT_BASIC_SET_TEST(obj, signed_char_t);
	OBJECT_BASIC_SET_TEST(obj, unsigned_char_t);
	OBJECT_BASIC_SET_TEST(obj, short);
	OBJECT_BASIC_SET_TEST(obj, int);
	OBJECT_BASIC_SET_TEST(obj, long);
	OBJECT_BASIC_SET_TEST(obj, long_long_t);
	OBJECT_BASIC_SET_TEST(obj, intmax_t);
	OBJECT_BASIC_SET_TEST(obj, float);
	OBJECT_BASIC_SET_TEST(obj, double);
	OBJECT_BASIC_SET_TEST(obj, long_double_t);
	OBJECT_BASIC_SET_TEST(obj, std::string);
	OBJECT_BASIC_SET_TEST(obj, object::array_t);

	// const char*
	obj.set("");
	EXPECT_TRUE(obj.has<std::string>());
	EXPECT_EQ(obj.get<std::string>(), std::string{});

	// std::string_view
	obj.set(""sv);
	EXPECT_TRUE(obj.has<std::string>());
	EXPECT_EQ(obj.get<std::string>(), std::string{});
}

TEST(ObjectTest, basic_set_vector) {
	object obj;

	OBJECT_BASIC_SET_TEST(obj, std::vector<bool>);
	OBJECT_BASIC_SET_TEST(obj, std::vector<signed char>);
	OBJECT_BASIC_SET_TEST(obj, std::vector<unsigned char>);
	OBJECT_BASIC_SET_TEST(obj, std::vector<short>);
	OBJECT_BASIC_SET_TEST(obj, std::vector<int>);
	OBJECT_BASIC_SET_TEST(obj, std::vector<long>);
	OBJECT_BASIC_SET_TEST(obj, std::vector<long long>);
	OBJECT_BASIC_SET_TEST(obj, std::vector<intmax_t>);
	OBJECT_BASIC_SET_TEST(obj, std::vector<float>);
	OBJECT_BASIC_SET_TEST(obj, std::vector<double>);
	OBJECT_BASIC_SET_TEST(obj, std::vector<long double>);
	OBJECT_BASIC_SET_TEST(obj, std::vector<std::string>);
	OBJECT_BASIC_SET_TEST(obj, std::vector<object>);
}

TEST(ObjectTest, basic_set_list) {
	object obj;

	OBJECT_BASIC_SET_TEST(obj, std::list<bool>);
	OBJECT_BASIC_SET_TEST(obj, std::list<signed char>);
	OBJECT_BASIC_SET_TEST(obj, std::list<unsigned char>);
	OBJECT_BASIC_SET_TEST(obj, std::list<short>);
	OBJECT_BASIC_SET_TEST(obj, std::list<int>);
	OBJECT_BASIC_SET_TEST(obj, std::list<long>);
	OBJECT_BASIC_SET_TEST(obj, std::list<long long>);
	OBJECT_BASIC_SET_TEST(obj, std::list<intmax_t>);
	OBJECT_BASIC_SET_TEST(obj, std::list<float>);
	OBJECT_BASIC_SET_TEST(obj, std::list<double>);
	OBJECT_BASIC_SET_TEST(obj, std::list<long double>);
	OBJECT_BASIC_SET_TEST(obj, std::list<std::string>);
	OBJECT_BASIC_SET_TEST(obj, std::list<object>);
}

TEST(ObjectTest, basic_set_forward_list) {
	object obj;

	OBJECT_BASIC_SET_TEST(obj, std::forward_list<bool>);
	OBJECT_BASIC_SET_TEST(obj, std::forward_list<signed char>);
	OBJECT_BASIC_SET_TEST(obj, std::forward_list<unsigned char>);
	OBJECT_BASIC_SET_TEST(obj, std::forward_list<short>);
	OBJECT_BASIC_SET_TEST(obj, std::forward_list<int>);
	OBJECT_BASIC_SET_TEST(obj, std::forward_list<long>);
	OBJECT_BASIC_SET_TEST(obj, std::forward_list<long long>);
	OBJECT_BASIC_SET_TEST(obj, std::forward_list<intmax_t>);
	OBJECT_BASIC_SET_TEST(obj, std::forward_list<float>);
	OBJECT_BASIC_SET_TEST(obj, std::forward_list<double>);
	OBJECT_BASIC_SET_TEST(obj, std::forward_list<long double>);
	OBJECT_BASIC_SET_TEST(obj, std::forward_list<std::string>);
	OBJECT_BASIC_SET_TEST(obj, std::forward_list<object>);
}

TEST(ObjectTest, basic_set_set) {
	object obj;

	OBJECT_BASIC_SET_TEST(obj, std::set<bool>);
	OBJECT_BASIC_SET_TEST(obj, std::set<signed char>);
	OBJECT_BASIC_SET_TEST(obj, std::set<unsigned char>);
	OBJECT_BASIC_SET_TEST(obj, std::set<short>);
	OBJECT_BASIC_SET_TEST(obj, std::set<int>);
	OBJECT_BASIC_SET_TEST(obj, std::set<long>);
	OBJECT_BASIC_SET_TEST(obj, std::set<long long>);
	OBJECT_BASIC_SET_TEST(obj, std::set<intmax_t>);
	OBJECT_BASIC_SET_TEST(obj, std::set<float>);
	OBJECT_BASIC_SET_TEST(obj, std::set<double>);
	OBJECT_BASIC_SET_TEST(obj, std::set<long double>);
	OBJECT_BASIC_SET_TEST(obj, std::set<std::string>);
	OBJECT_BASIC_SET_TEST(obj, std::set<object>);
}

TEST(ObjectTest, basic_set_multiset) {
	object obj;

	OBJECT_BASIC_SET_TEST(obj, std::multiset<bool>);
	OBJECT_BASIC_SET_TEST(obj, std::multiset<signed char>);
	OBJECT_BASIC_SET_TEST(obj, std::multiset<unsigned char>);
	OBJECT_BASIC_SET_TEST(obj, std::multiset<short>);
	OBJECT_BASIC_SET_TEST(obj, std::multiset<int>);
	OBJECT_BASIC_SET_TEST(obj, std::multiset<long>);
	OBJECT_BASIC_SET_TEST(obj, std::multiset<long long>);
	OBJECT_BASIC_SET_TEST(obj, std::multiset<intmax_t>);
	OBJECT_BASIC_SET_TEST(obj, std::multiset<float>);
	OBJECT_BASIC_SET_TEST(obj, std::multiset<double>);
	OBJECT_BASIC_SET_TEST(obj, std::multiset<long double>);
	OBJECT_BASIC_SET_TEST(obj, std::multiset<std::string>);
	OBJECT_BASIC_SET_TEST(obj, std::multiset<object>);
}

TEST(ObjectTest, basic_set_unordered_set) {
	object obj;

	OBJECT_BASIC_SET_TEST(obj, std::unordered_set<bool>);
	OBJECT_BASIC_SET_TEST(obj, std::unordered_set<signed char>);
	OBJECT_BASIC_SET_TEST(obj, std::unordered_set<unsigned char>);
	OBJECT_BASIC_SET_TEST(obj, std::unordered_set<short>);
	OBJECT_BASIC_SET_TEST(obj, std::unordered_set<int>);
	OBJECT_BASIC_SET_TEST(obj, std::unordered_set<long>);
	OBJECT_BASIC_SET_TEST(obj, std::unordered_set<long long>);
	OBJECT_BASIC_SET_TEST(obj, std::unordered_set<intmax_t>);
	OBJECT_BASIC_SET_TEST(obj, std::unordered_set<float>);
	OBJECT_BASIC_SET_TEST(obj, std::unordered_set<double>);
	OBJECT_BASIC_SET_TEST(obj, std::unordered_set<long double>);
	OBJECT_BASIC_SET_TEST(obj, std::unordered_set<std::string>);
	OBJECT_BASIC_SET_TEST(obj, std::unordered_set<object>);
}

TEST(ObjectTest, basic_set_unordered_multiset) {
	object obj;

	OBJECT_BASIC_SET_TEST(obj, std::unordered_multiset<bool>);
	OBJECT_BASIC_SET_TEST(obj, std::unordered_multiset<signed char>);
	OBJECT_BASIC_SET_TEST(obj, std::unordered_multiset<unsigned char>);
	OBJECT_BASIC_SET_TEST(obj, std::unordered_multiset<short>);
	OBJECT_BASIC_SET_TEST(obj, std::unordered_multiset<int>);
	OBJECT_BASIC_SET_TEST(obj, std::unordered_multiset<long>);
	OBJECT_BASIC_SET_TEST(obj, std::unordered_multiset<long long>);
	OBJECT_BASIC_SET_TEST(obj, std::unordered_multiset<intmax_t>);
	OBJECT_BASIC_SET_TEST(obj, std::unordered_multiset<float>);
	OBJECT_BASIC_SET_TEST(obj, std::unordered_multiset<double>);
	OBJECT_BASIC_SET_TEST(obj, std::unordered_multiset<long double>);
	OBJECT_BASIC_SET_TEST(obj, std::unordered_multiset<std::string>);
	OBJECT_BASIC_SET_TEST(obj, std::unordered_set<object>);
}

/** basic_assignment_operator */

#define OBJECT_BASIC_ASSIGNMENT_OPERATOR_TEST(in_object, in_type) \
	in_object = in_type {}; \
	EXPECT_TRUE(in_object .has< in_type >()); \
	EXPECT_EQ(in_object .get< in_type >(), in_type {} )

TEST(ObjectTest, basic_assignment_operator) {
	object obj;

	OBJECT_BASIC_ASSIGNMENT_OPERATOR_TEST(obj, bool);
	OBJECT_BASIC_ASSIGNMENT_OPERATOR_TEST(obj, signed_char_t);
	OBJECT_BASIC_ASSIGNMENT_OPERATOR_TEST(obj, unsigned_char_t);
	OBJECT_BASIC_ASSIGNMENT_OPERATOR_TEST(obj, short);
	OBJECT_BASIC_ASSIGNMENT_OPERATOR_TEST(obj, int);
	OBJECT_BASIC_ASSIGNMENT_OPERATOR_TEST(obj, long);
	OBJECT_BASIC_ASSIGNMENT_OPERATOR_TEST(obj, long_long_t);
	OBJECT_BASIC_ASSIGNMENT_OPERATOR_TEST(obj, intmax_t);
	OBJECT_BASIC_ASSIGNMENT_OPERATOR_TEST(obj, float);
	OBJECT_BASIC_ASSIGNMENT_OPERATOR_TEST(obj, double);
	OBJECT_BASIC_ASSIGNMENT_OPERATOR_TEST(obj, long_double_t);
	OBJECT_BASIC_ASSIGNMENT_OPERATOR_TEST(obj, std::string);
	OBJECT_BASIC_ASSIGNMENT_OPERATOR_TEST(obj, object::array_t);

	// const char*
	obj = "";
	EXPECT_TRUE(obj.has<std::string>());
	EXPECT_EQ(obj.get<std::string>(), std::string{});

	// std::string_view
	obj = ""sv;
	EXPECT_TRUE(obj.has<std::string>());
	EXPECT_EQ(obj.get<std::string>(), std::string{});
}

TEST(ObjectTest, basic_assignment_operator_vector) {
	object obj;

	OBJECT_BASIC_ASSIGNMENT_OPERATOR_TEST(obj, std::vector<bool>);
	OBJECT_BASIC_ASSIGNMENT_OPERATOR_TEST(obj, std::vector<signed char>);
	OBJECT_BASIC_ASSIGNMENT_OPERATOR_TEST(obj, std::vector<unsigned char>);
	OBJECT_BASIC_ASSIGNMENT_OPERATOR_TEST(obj, std::vector<short>);
	OBJECT_BASIC_ASSIGNMENT_OPERATOR_TEST(obj, std::vector<int>);
	OBJECT_BASIC_ASSIGNMENT_OPERATOR_TEST(obj, std::vector<long>);
	OBJECT_BASIC_ASSIGNMENT_OPERATOR_TEST(obj, std::vector<long long>);
	OBJECT_BASIC_ASSIGNMENT_OPERATOR_TEST(obj, std::vector<intmax_t>);
	OBJECT_BASIC_ASSIGNMENT_OPERATOR_TEST(obj, std::vector<float>);
	OBJECT_BASIC_ASSIGNMENT_OPERATOR_TEST(obj, std::vector<double>);
	OBJECT_BASIC_ASSIGNMENT_OPERATOR_TEST(obj, std::vector<long double>);
	OBJECT_BASIC_ASSIGNMENT_OPERATOR_TEST(obj, std::vector<std::string>);
	OBJECT_BASIC_ASSIGNMENT_OPERATOR_TEST(obj, std::vector<object>);
}

TEST(ObjectTest, basic_assignment_operator_list) {
	object obj;

	OBJECT_BASIC_ASSIGNMENT_OPERATOR_TEST(obj, std::list<bool>);
	OBJECT_BASIC_ASSIGNMENT_OPERATOR_TEST(obj, std::list<signed char>);
	OBJECT_BASIC_ASSIGNMENT_OPERATOR_TEST(obj, std::list<unsigned char>);
	OBJECT_BASIC_ASSIGNMENT_OPERATOR_TEST(obj, std::list<short>);
	OBJECT_BASIC_ASSIGNMENT_OPERATOR_TEST(obj, std::list<int>);
	OBJECT_BASIC_ASSIGNMENT_OPERATOR_TEST(obj, std::list<long>);
	OBJECT_BASIC_ASSIGNMENT_OPERATOR_TEST(obj, std::list<long long>);
	OBJECT_BASIC_ASSIGNMENT_OPERATOR_TEST(obj, std::list<intmax_t>);
	OBJECT_BASIC_ASSIGNMENT_OPERATOR_TEST(obj, std::list<float>);
	OBJECT_BASIC_ASSIGNMENT_OPERATOR_TEST(obj, std::list<double>);
	OBJECT_BASIC_ASSIGNMENT_OPERATOR_TEST(obj, std::list<long double>);
	OBJECT_BASIC_ASSIGNMENT_OPERATOR_TEST(obj, std::list<std::string>);
	OBJECT_BASIC_ASSIGNMENT_OPERATOR_TEST(obj, std::list<object>);
}

TEST(ObjectTest, basic_assignment_operator_forward_list) {
	object obj;

	OBJECT_BASIC_ASSIGNMENT_OPERATOR_TEST(obj, std::forward_list<bool>);
	OBJECT_BASIC_ASSIGNMENT_OPERATOR_TEST(obj, std::forward_list<signed char>);
	OBJECT_BASIC_ASSIGNMENT_OPERATOR_TEST(obj, std::forward_list<unsigned char>);
	OBJECT_BASIC_ASSIGNMENT_OPERATOR_TEST(obj, std::forward_list<short>);
	OBJECT_BASIC_ASSIGNMENT_OPERATOR_TEST(obj, std::forward_list<int>);
	OBJECT_BASIC_ASSIGNMENT_OPERATOR_TEST(obj, std::forward_list<long>);
	OBJECT_BASIC_ASSIGNMENT_OPERATOR_TEST(obj, std::forward_list<long long>);
	OBJECT_BASIC_ASSIGNMENT_OPERATOR_TEST(obj, std::forward_list<intmax_t>);
	OBJECT_BASIC_ASSIGNMENT_OPERATOR_TEST(obj, std::forward_list<float>);
	OBJECT_BASIC_ASSIGNMENT_OPERATOR_TEST(obj, std::forward_list<double>);
	OBJECT_BASIC_ASSIGNMENT_OPERATOR_TEST(obj, std::forward_list<long double>);
	OBJECT_BASIC_ASSIGNMENT_OPERATOR_TEST(obj, std::forward_list<std::string>);
	OBJECT_BASIC_ASSIGNMENT_OPERATOR_TEST(obj, std::forward_list<object>);
}

TEST(ObjectTest, basic_assignment_operator_set) {
	object obj;

	OBJECT_BASIC_ASSIGNMENT_OPERATOR_TEST(obj, std::set<bool>);
	OBJECT_BASIC_ASSIGNMENT_OPERATOR_TEST(obj, std::set<signed char>);
	OBJECT_BASIC_ASSIGNMENT_OPERATOR_TEST(obj, std::set<unsigned char>);
	OBJECT_BASIC_ASSIGNMENT_OPERATOR_TEST(obj, std::set<short>);
	OBJECT_BASIC_ASSIGNMENT_OPERATOR_TEST(obj, std::set<int>);
	OBJECT_BASIC_ASSIGNMENT_OPERATOR_TEST(obj, std::set<long>);
	OBJECT_BASIC_ASSIGNMENT_OPERATOR_TEST(obj, std::set<long long>);
	OBJECT_BASIC_ASSIGNMENT_OPERATOR_TEST(obj, std::set<intmax_t>);
	OBJECT_BASIC_ASSIGNMENT_OPERATOR_TEST(obj, std::set<float>);
	OBJECT_BASIC_ASSIGNMENT_OPERATOR_TEST(obj, std::set<double>);
	OBJECT_BASIC_ASSIGNMENT_OPERATOR_TEST(obj, std::set<long double>);
	OBJECT_BASIC_ASSIGNMENT_OPERATOR_TEST(obj, std::set<std::string>);
	OBJECT_BASIC_ASSIGNMENT_OPERATOR_TEST(obj, std::set<object>);
}

TEST(ObjectTest, basic_assignment_operator_multiset) {
	object obj;

	OBJECT_BASIC_ASSIGNMENT_OPERATOR_TEST(obj, std::multiset<bool>);
	OBJECT_BASIC_ASSIGNMENT_OPERATOR_TEST(obj, std::multiset<signed char>);
	OBJECT_BASIC_ASSIGNMENT_OPERATOR_TEST(obj, std::multiset<unsigned char>);
	OBJECT_BASIC_ASSIGNMENT_OPERATOR_TEST(obj, std::multiset<short>);
	OBJECT_BASIC_ASSIGNMENT_OPERATOR_TEST(obj, std::multiset<int>);
	OBJECT_BASIC_ASSIGNMENT_OPERATOR_TEST(obj, std::multiset<long>);
	OBJECT_BASIC_ASSIGNMENT_OPERATOR_TEST(obj, std::multiset<long long>);
	OBJECT_BASIC_ASSIGNMENT_OPERATOR_TEST(obj, std::multiset<intmax_t>);
	OBJECT_BASIC_ASSIGNMENT_OPERATOR_TEST(obj, std::multiset<float>);
	OBJECT_BASIC_ASSIGNMENT_OPERATOR_TEST(obj, std::multiset<double>);
	OBJECT_BASIC_ASSIGNMENT_OPERATOR_TEST(obj, std::multiset<long double>);
	OBJECT_BASIC_ASSIGNMENT_OPERATOR_TEST(obj, std::multiset<std::string>);
	OBJECT_BASIC_ASSIGNMENT_OPERATOR_TEST(obj, std::multiset<object>);
}

TEST(ObjectTest, basic_assignment_operator_unordered_set) {
	object obj;

	OBJECT_BASIC_ASSIGNMENT_OPERATOR_TEST(obj, std::unordered_set<bool>);
	OBJECT_BASIC_ASSIGNMENT_OPERATOR_TEST(obj, std::unordered_set<signed char>);
	OBJECT_BASIC_ASSIGNMENT_OPERATOR_TEST(obj, std::unordered_set<unsigned char>);
	OBJECT_BASIC_ASSIGNMENT_OPERATOR_TEST(obj, std::unordered_set<short>);
	OBJECT_BASIC_ASSIGNMENT_OPERATOR_TEST(obj, std::unordered_set<int>);
	OBJECT_BASIC_ASSIGNMENT_OPERATOR_TEST(obj, std::unordered_set<long>);
	OBJECT_BASIC_ASSIGNMENT_OPERATOR_TEST(obj, std::unordered_set<long long>);
	OBJECT_BASIC_ASSIGNMENT_OPERATOR_TEST(obj, std::unordered_set<intmax_t>);
	OBJECT_BASIC_ASSIGNMENT_OPERATOR_TEST(obj, std::unordered_set<float>);
	OBJECT_BASIC_ASSIGNMENT_OPERATOR_TEST(obj, std::unordered_set<double>);
	OBJECT_BASIC_ASSIGNMENT_OPERATOR_TEST(obj, std::unordered_set<long double>);
	OBJECT_BASIC_ASSIGNMENT_OPERATOR_TEST(obj, std::unordered_set<std::string>);
	OBJECT_BASIC_ASSIGNMENT_OPERATOR_TEST(obj, std::unordered_set<object>);
}

TEST(ObjectTest, basic_assignment_operator_unordered_multiset) {
	object obj;

	OBJECT_BASIC_ASSIGNMENT_OPERATOR_TEST(obj, std::unordered_multiset<bool>);
	OBJECT_BASIC_ASSIGNMENT_OPERATOR_TEST(obj, std::unordered_multiset<signed char>);
	OBJECT_BASIC_ASSIGNMENT_OPERATOR_TEST(obj, std::unordered_multiset<unsigned char>);
	OBJECT_BASIC_ASSIGNMENT_OPERATOR_TEST(obj, std::unordered_multiset<short>);
	OBJECT_BASIC_ASSIGNMENT_OPERATOR_TEST(obj, std::unordered_multiset<int>);
	OBJECT_BASIC_ASSIGNMENT_OPERATOR_TEST(obj, std::unordered_multiset<long>);
	OBJECT_BASIC_ASSIGNMENT_OPERATOR_TEST(obj, std::unordered_multiset<long long>);
	OBJECT_BASIC_ASSIGNMENT_OPERATOR_TEST(obj, std::unordered_multiset<intmax_t>);
	OBJECT_BASIC_ASSIGNMENT_OPERATOR_TEST(obj, std::unordered_multiset<float>);
	OBJECT_BASIC_ASSIGNMENT_OPERATOR_TEST(obj, std::unordered_multiset<double>);
	OBJECT_BASIC_ASSIGNMENT_OPERATOR_TEST(obj, std::unordered_multiset<long double>);
	OBJECT_BASIC_ASSIGNMENT_OPERATOR_TEST(obj, std::unordered_multiset<std::string>);
	OBJECT_BASIC_ASSIGNMENT_OPERATOR_TEST(obj, std::unordered_set<object>);
}

/** basic_access_operator */

TEST(ObjectTest, basic_access_operator) {
	object obj;

	obj["test"] = 1234;
	EXPECT_EQ(obj["test"].get<int>(), 1234);
	EXPECT_EQ(obj["test2"].get<int>(), 0);

	obj["test"] = 4567;
	EXPECT_EQ(obj["test"].get<int>(), 4567);
	EXPECT_EQ(obj["test2"].get<int>(), 0);

	obj["test2"] = 1234;
	EXPECT_EQ(obj["test"].get<int>(), 4567);
	EXPECT_EQ(obj["test2"].get<int>(), 1234);
}

/** end basic tests */

TEST(ObjectTest, set_bool) {
	object obj;

	obj.set(true);

	EXPECT_TRUE(obj.has<bool>());
	EXPECT_TRUE(obj.get<bool>());
	EXPECT_FALSE(obj.has<int>());
	EXPECT_EQ(obj.get<int>(), 0);

	obj.set(false);

	EXPECT_TRUE(obj.has<bool>());
	EXPECT_FALSE(obj.get<bool>());
	EXPECT_FALSE(obj.has<int>());
	EXPECT_EQ(obj.get<int>(), 0);
}

TEST(ObjectTest, set_int) {
	object obj;

	obj.set(1337);

	EXPECT_TRUE(obj.has<int>());
	EXPECT_EQ(obj.get<int>(), 1337);
	EXPECT_FALSE(obj.has<bool>());
	EXPECT_FALSE(obj.get<bool>());

	obj.set(7331);

	EXPECT_TRUE(obj.has<int>());
	EXPECT_EQ(obj.get<int>(), 7331);
	EXPECT_FALSE(obj.has<bool>());
	EXPECT_FALSE(obj.get<bool>());

	obj.set(0);

	EXPECT_TRUE(obj.has<int>());
	EXPECT_EQ(obj.get<int>(), 0);
	EXPECT_FALSE(obj.has<bool>());
	EXPECT_FALSE(obj.get<bool>());
}

TEST(ObjectTest, set_float) {
	object obj;

	obj.set(13.37);

	EXPECT_TRUE(obj.has<double>());
	EXPECT_EQ(obj.get<double>(), 13.37);
	EXPECT_FALSE(obj.has<bool>());
	EXPECT_FALSE(obj.get<bool>());

	obj.set(73.31);

	EXPECT_TRUE(obj.has<double>());
	EXPECT_EQ(obj.get<double>(), 73.31);
	EXPECT_FALSE(obj.has<bool>());
	EXPECT_FALSE(obj.get<bool>());

	obj.set(0.0);

	EXPECT_TRUE(obj.has<double>());
	EXPECT_EQ(obj.get<double>(), 0.0);
	EXPECT_FALSE(obj.has<bool>());
	EXPECT_FALSE(obj.get<bool>());
}

TEST(ObjectTest, set_string) {
	object obj;

	obj.set("Jessica");

	EXPECT_TRUE(obj.has<std::string>());
	EXPECT_EQ(obj.get<std::string>(), "Jessica");
	EXPECT_FALSE(obj.has<bool>());
	EXPECT_FALSE(obj.get<bool>());

	obj.set("was"s);

	EXPECT_TRUE(obj.has<std::string>());
	EXPECT_EQ(obj.get<std::string>(), "was");
	EXPECT_FALSE(obj.has<bool>());
	EXPECT_FALSE(obj.get<bool>());

	obj.set("here"sv);

	EXPECT_TRUE(obj.has<std::string>());
	EXPECT_EQ(obj.get<std::string>(), "here");
	EXPECT_FALSE(obj.has<bool>());
	EXPECT_FALSE(obj.get<bool>());

	obj.set("");

	EXPECT_TRUE(obj.has<std::string>());
	EXPECT_EQ(obj.get<std::string>(), "");
	EXPECT_FALSE(obj.has<bool>());
	EXPECT_FALSE(obj.get<bool>());
}

TEST(ObjectTest, set_vector) {
	object obj;

	obj.set(std::vector<bool>{ true });

	EXPECT_TRUE(obj.has<std::vector<bool>>());
	EXPECT_EQ(obj.get<std::vector<bool>>(), std::vector<bool>{ true });
	EXPECT_TRUE(obj.has<std::vector<object>>());
	EXPECT_EQ(obj.get<std::vector<object>>().size(), 1U);
	EXPECT_TRUE(obj.has<std::vector<int>>());
	EXPECT_EQ(obj.get<std::vector<int>>().size(), 0U);
	EXPECT_FALSE(obj.has<bool>());
	EXPECT_FALSE(obj.get<bool>());

	obj.set(std::vector<bool>{});

	EXPECT_TRUE(obj.has<std::vector<bool>>());
	EXPECT_EQ(obj.get<std::vector<bool>>().size(), 0U);
	EXPECT_TRUE(obj.has<std::vector<object>>());
	EXPECT_EQ(obj.get<std::vector<object>>().size(), 0U);
	EXPECT_TRUE(obj.has<std::vector<int>>());
	EXPECT_EQ(obj.get<std::vector<int>>().size(), 0U);
	EXPECT_FALSE(obj.has<bool>());
	EXPECT_FALSE(obj.get<bool>());
}

TEST(ObjectTest, set_object) {
	object obj1, obj2;

	obj1.set(true);
	obj2.set(obj1);

	EXPECT_EQ(obj1, obj2);
}
