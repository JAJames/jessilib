/**
 * Copyright (C) 2020 Jessica James.
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
#include "jessilib/app_parameters.hpp"

using namespace jessilib;
using namespace std::literals;

template<typename CharT = char>
class ArgWrapper {
public:
	template<typename... Args>
	ArgWrapper(Args... in_args)
		: ArgWrapper{ std::vector<std::basic_string<CharT>>{ in_args... } } {
		// Empty ctor body
	}

	ArgWrapper(std::vector<std::basic_string<CharT>> in_args)
		: m_args{ in_args },
		m_argv{ new const CharT*[in_args.size() + 1] } {
		// Populate m_argv
		for (size_t index = 0; index != m_args.size(); ++index) {
			m_argv[index] = m_args[index].c_str();
		}
		m_argv[in_args.size()] = nullptr; // last arg is always nullptr
	}

	const CharT** argv() const {
		return m_argv.get();
	}

	int argc() const {
		return static_cast<int>(m_args.size());
	}

private:
	std::vector<std::basic_string<CharT>> m_args;
	std::unique_ptr<const CharT*[]> m_argv;
};

TEST(AppParametersTest, null) {
	app_parameters parameters{ 0, static_cast<const char**>(nullptr) };
	app_parameters parameters2{ 1234, static_cast<const char**>(nullptr) };

	EXPECT_TRUE(parameters.path().empty());
	EXPECT_TRUE(parameters.arguments().empty());
	EXPECT_TRUE(parameters.switches().empty());
	EXPECT_TRUE(parameters.switches_set().empty());
	EXPECT_TRUE(parameters.values().empty());
	EXPECT_TRUE(parameters.as_object().null());

	EXPECT_TRUE(parameters2.path().empty());
	EXPECT_TRUE(parameters2.arguments().empty());
	EXPECT_TRUE(parameters2.switches().empty());
	EXPECT_TRUE(parameters2.switches_set().empty());
	EXPECT_TRUE(parameters2.values().empty());
	EXPECT_TRUE(parameters2.as_object().null());
}

TEST(AppParametersTest, path_only) {
	ArgWrapper args{ "/path/to/exe" };
	app_parameters parameters{ args.argc(), args.argv() };

	EXPECT_EQ(parameters.path(), u8"/path/to/exe");
	EXPECT_TRUE(parameters.arguments().empty());
	EXPECT_TRUE(parameters.switches().empty());
	EXPECT_TRUE(parameters.switches_set().empty());
	EXPECT_TRUE(parameters.values().empty());

	auto obj = parameters.as_object();
	EXPECT_FALSE(obj.null());
	EXPECT_EQ(obj[u8"Path"], u8"/path/to/exe");
}

TEST(AppParametersTest, path_only_w) {
	ArgWrapper<wchar_t> args{ L"/path/to/exe" };
	app_parameters parameters{ args.argc(), args.argv() };

	EXPECT_EQ(parameters.path(), u8"/path/to/exe");
	EXPECT_TRUE(parameters.arguments().empty());
	EXPECT_TRUE(parameters.switches().empty());
	EXPECT_TRUE(parameters.switches_set().empty());
	EXPECT_TRUE(parameters.values().empty());

	auto obj = parameters.as_object();
	EXPECT_FALSE(obj.null());
	EXPECT_EQ(obj[u8"Path"], u8"/path/to/exe");
}

TEST(AppParametersTest, single_switch) {
	ArgWrapper args{ "/path/to/exe", "-switch" };
	app_parameters parameters{ args.argc(), args.argv() };

	EXPECT_FALSE(parameters.path().empty());
	EXPECT_EQ(parameters.arguments().size(), 1U);
	EXPECT_EQ(parameters.switches().size(), 1U);
	EXPECT_EQ(parameters.switches_set().size(), 1U);
	EXPECT_TRUE(parameters.values().empty());

	auto obj = parameters.as_object();
	EXPECT_FALSE(obj.null());
	EXPECT_EQ(obj[u8"Path"], u8"/path/to/exe");
	EXPECT_EQ(obj[u8"Args"], object{ std::vector<std::u8string>{ u8"-switch" } });
	EXPECT_EQ(obj[u8"Switches"], object{ std::vector<std::u8string>{ u8"switch" } });
}

TEST(AppParametersTest, double_switch) {
	ArgWrapper args{ "/path/to/exe", "-switch1", "--switch2" };
	app_parameters parameters{ args.argc(), args.argv() };

	EXPECT_FALSE(parameters.path().empty());
	EXPECT_EQ(parameters.arguments().size(), 2U);
	EXPECT_EQ(parameters.switches().size(), 2U);
	EXPECT_EQ(parameters.switches_set().size(), 2U);
	EXPECT_TRUE(parameters.values().empty());

	auto obj = parameters.as_object();
	std::vector<std::u8string> expected_args{ u8"-switch1", u8"--switch2" };
	std::vector<std::u8string> expected_switches{ u8"switch1", u8"switch2" };
	EXPECT_FALSE(obj.null());
	EXPECT_EQ(obj[u8"Path"], u8"/path/to/exe");
	EXPECT_EQ(obj[u8"Args"], expected_args);
	EXPECT_EQ(obj[u8"Switches"], expected_switches);
}

TEST(AppParametersTest, duplicate_switch) {
	ArgWrapper args{ "/path/to/exe", "-switch", "--switch" };
	app_parameters parameters{ args.argc(), args.argv() };

	EXPECT_FALSE(parameters.path().empty());
	EXPECT_EQ(parameters.arguments().size(), 2U);
	EXPECT_EQ(parameters.switches().size(), 2U);
	EXPECT_EQ(parameters.switches_set().size(), 1U);
	EXPECT_TRUE(parameters.values().empty());

	auto obj = parameters.as_object();
	std::vector<std::u8string> expected_args{ u8"-switch", u8"--switch" };
	std::vector<std::u8string> expected_switches{ u8"switch", u8"switch" };
	EXPECT_FALSE(obj.null());
	EXPECT_EQ(obj[u8"Path"], u8"/path/to/exe");
	EXPECT_EQ(obj[u8"Args"], expected_args);
	EXPECT_EQ(obj[u8"Switches"], expected_switches);
}

TEST(AppParametersTest, single_value) {
	ArgWrapper args{ "/path/to/exe", "-key", "value" };
	app_parameters parameters{ args.argc(), args.argv() };

	EXPECT_FALSE(parameters.path().empty());
	EXPECT_EQ(parameters.arguments().size(), 2U);
	EXPECT_EQ(parameters.switches().size(), 0U);
	EXPECT_EQ(parameters.switches_set().size(), 0U);
	EXPECT_EQ(parameters.values().size(), 1U);

	auto obj = parameters.as_object();
	std::vector<std::u8string> expected_args{ u8"-key", u8"value" };
	std::map<std::u8string, object> expected_values{ { u8"key", u8"value" } };
	EXPECT_FALSE(obj.null());
	EXPECT_EQ(obj[u8"Path"], u8"/path/to/exe");
	EXPECT_EQ(obj[u8"Args"], expected_args);
	EXPECT_EQ(obj[u8"Values"], expected_values);
}

TEST(AppParametersTest, single_value_eq) {
	ArgWrapper args{ "/path/to/exe", "-key=value" };
	app_parameters parameters{ args.argc(), args.argv() };

	EXPECT_FALSE(parameters.path().empty());
	EXPECT_EQ(parameters.arguments().size(), 1U);
	EXPECT_EQ(parameters.switches().size(), 0U);
	EXPECT_EQ(parameters.switches_set().size(), 0U);
	EXPECT_EQ(parameters.values().size(), 1U);

	auto obj = parameters.as_object();
	std::vector<std::u8string> expected_args{ u8"-key=value" };
	std::map<std::u8string, object> expected_values{ { u8"key", u8"value" } };
	EXPECT_FALSE(obj.null());
	EXPECT_EQ(obj[u8"Path"], u8"/path/to/exe");
	EXPECT_EQ(obj[u8"Args"], expected_args);
	EXPECT_EQ(obj[u8"Values"], expected_values);
}

TEST(AppParametersTest, multiword_value) {
	ArgWrapper args{ "/path/to/exe", "-key", "valuePart1", "valuePart2" };
	app_parameters parameters{ args.argc(), args.argv() };

	EXPECT_FALSE(parameters.path().empty());
	EXPECT_EQ(parameters.arguments().size(), 3U);
	EXPECT_EQ(parameters.switches().size(), 0U);
	EXPECT_EQ(parameters.switches_set().size(), 0U);
	EXPECT_EQ(parameters.values().size(), 1U);

	auto obj = parameters.as_object();
	std::vector<std::u8string> expected_args{ u8"-key", u8"valuePart1", u8"valuePart2" };
	std::map<std::u8string, object> expected_values{ { u8"key", u8"valuePart1 valuePart2" } };
	EXPECT_FALSE(obj.null());
	EXPECT_EQ(obj[u8"Path"], u8"/path/to/exe");
	EXPECT_EQ(obj[u8"Args"], expected_args);
	EXPECT_EQ(obj[u8"Values"], expected_values);
}

TEST(AppParametersTest, multiword_value_eq) {
	ArgWrapper args{ "/path/to/exe", "-key=valuePart1", "valuePart2" };
	app_parameters parameters{ args.argc(), args.argv() };

	EXPECT_FALSE(parameters.path().empty());
	EXPECT_EQ(parameters.arguments().size(), 2U);
	EXPECT_EQ(parameters.switches().size(), 0U);
	EXPECT_EQ(parameters.switches_set().size(), 0U);
	EXPECT_EQ(parameters.values().size(), 1U);

	auto obj = parameters.as_object();
	std::vector<std::u8string> expected_args{ u8"-key=valuePart1", u8"valuePart2" };
	std::map<std::u8string, object> expected_values{ { u8"key", u8"valuePart1 valuePart2" } };
	EXPECT_FALSE(obj.null());
	EXPECT_EQ(obj[u8"Path"], u8"/path/to/exe");
	EXPECT_EQ(obj[u8"Args"], expected_args);
	EXPECT_EQ(obj[u8"Values"], expected_values);
}

TEST(AppParametersTest, double_value) {
	ArgWrapper args{ "/path/to/exe", "-key", "value", "--key2", "value2" };
	app_parameters parameters{ args.argc(), args.argv() };

	EXPECT_FALSE(parameters.path().empty());
	EXPECT_EQ(parameters.arguments().size(), 4U);
	EXPECT_EQ(parameters.switches().size(), 0U);
	EXPECT_EQ(parameters.switches_set().size(), 0U);
	EXPECT_EQ(parameters.values().size(), 2U);

	auto obj = parameters.as_object();
	std::vector<std::u8string> expected_args{ u8"-key", u8"value", u8"--key2", u8"value2" };
	std::map<std::u8string, object> expected_values{ { u8"key", u8"value" }, { u8"key2", u8"value2" } };
	EXPECT_FALSE(obj.null());
	EXPECT_EQ(obj[u8"Path"], u8"/path/to/exe");
	EXPECT_EQ(obj[u8"Args"], expected_args);
	EXPECT_EQ(obj[u8"Values"], expected_values);
}

TEST(AppParametersTest, double_value_eq) {
	ArgWrapper args{ "/path/to/exe", "-key=value", "--key2=value2" };
	app_parameters parameters{ args.argc(), args.argv() };

	EXPECT_FALSE(parameters.path().empty());
	EXPECT_EQ(parameters.arguments().size(), 2U);
	EXPECT_EQ(parameters.switches().size(), 0U);
	EXPECT_EQ(parameters.switches_set().size(), 0U);
	EXPECT_EQ(parameters.values().size(), 2U);

	auto obj = parameters.as_object();
	std::vector<std::u8string> expected_args{ u8"-key=value", u8"--key2=value2" };
	std::map<std::u8string, object> expected_values{ { u8"key", u8"value" }, { u8"key2", u8"value2" } };
	EXPECT_FALSE(obj.null());
	EXPECT_EQ(obj[u8"Path"], u8"/path/to/exe");
	EXPECT_EQ(obj[u8"Args"], expected_args);
	EXPECT_EQ(obj[u8"Values"], expected_values);
}

TEST(AppParametersTest, switch_and_value) {
	ArgWrapper args{ "/path/to/exe", "--switch", "-key", "value" };
	app_parameters parameters{ args.argc(), args.argv() };

	EXPECT_FALSE(parameters.path().empty());
	EXPECT_EQ(parameters.arguments().size(), 3U);
	EXPECT_EQ(parameters.switches().size(), 1U);
	EXPECT_EQ(parameters.switches_set().size(), 1U);
	EXPECT_EQ(parameters.values().size(), 1U);

	auto obj = parameters.as_object();
	std::vector<std::u8string> expected_args{ u8"--switch", u8"-key", u8"value" };
	std::vector<std::u8string> expected_switches{ u8"switch" };
	std::map<std::u8string, object> expected_values{ { u8"key", u8"value" } };
	EXPECT_FALSE(obj.null());
	EXPECT_EQ(obj[u8"Path"], u8"/path/to/exe");
	EXPECT_EQ(obj[u8"Args"], expected_args);
	EXPECT_EQ(obj[u8"Switches"], expected_switches);
	EXPECT_EQ(obj[u8"Values"], expected_values);

	EXPECT_TRUE(parameters.has_switch(u8"switch"));
	EXPECT_FALSE(parameters.has_switch(u8"switch2"));
	EXPECT_EQ(parameters.get_value(u8"key"), u8"value");
}

TEST(AppParametersTest, switch_and_value_w) {
	ArgWrapper<wchar_t> args{ L"/path/to/exe", L"--switch", L"-key", L"value" };
	app_parameters parameters{ args.argc(), args.argv() };

	EXPECT_FALSE(parameters.path().empty());
	EXPECT_EQ(parameters.arguments().size(), 3U);
	EXPECT_EQ(parameters.switches().size(), 1U);
	EXPECT_EQ(parameters.switches_set().size(), 1U);
	EXPECT_EQ(parameters.values().size(), 1U);

	auto obj = parameters.as_object();
	std::vector<std::u8string> expected_args{ u8"--switch", u8"-key", u8"value" };
	std::vector<std::u8string> expected_switches{ u8"switch" };
	std::map<std::u8string, object> expected_values{ { u8"key", u8"value" } };
	EXPECT_FALSE(obj.null());
	EXPECT_EQ(obj[u8"Path"], u8"/path/to/exe");
	EXPECT_EQ(obj[u8"Args"], expected_args);
	EXPECT_EQ(obj[u8"Switches"], expected_switches);
	EXPECT_EQ(obj[u8"Values"], expected_values);

	EXPECT_TRUE(parameters.has_switch(u8"switch"));
	EXPECT_FALSE(parameters.has_switch(u8"switch2"));
	EXPECT_EQ(parameters.get_value(u8"key"), u8"value");
}
