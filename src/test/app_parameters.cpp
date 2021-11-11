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

class ArgWrapper {
public:
	template<typename... Args>
	ArgWrapper(Args... in_args)
		: ArgWrapper{ std::vector<std::string>{ in_args... } } {
		// Empty ctor body
	}

	ArgWrapper(std::vector<std::string> in_args)
		: m_args{ in_args },
		m_argv{ new const char*[in_args.size()] } {
		// Populate m_argv
		for (size_t index = 0; index != m_args.size(); ++index) {
			m_argv[index] = m_args[index].c_str();
		}
	}

	const char** argv() const {
		return m_argv.get();
	}

	int argc() const {
		return m_args.size();
	}

private:
	std::vector<std::string> m_args;
	std::unique_ptr<const char*[]> m_argv;
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

	EXPECT_EQ(parameters.path(), "/path/to/exe");
	EXPECT_TRUE(parameters.arguments().empty());
	EXPECT_TRUE(parameters.switches().empty());
	EXPECT_TRUE(parameters.switches_set().empty());
	EXPECT_TRUE(parameters.values().empty());

	auto obj = parameters.as_object();
	EXPECT_FALSE(obj.null());
	EXPECT_EQ(obj["Path"], "/path/to/exe");
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
	EXPECT_EQ(obj["Path"], "/path/to/exe");
	EXPECT_EQ(obj["Args"], object{ std::vector<std::string>{ "-switch" } });
	EXPECT_EQ(obj["Switches"], object{ std::vector<std::string>{ "switch" } });
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
	std::vector<std::string> expected_args{ "-switch1", "--switch2" };
	std::vector<std::string> expected_switches{ "switch1", "switch2" };
	EXPECT_FALSE(obj.null());
	EXPECT_EQ(obj["Path"], "/path/to/exe");
	EXPECT_EQ(obj["Args"], expected_args);
	EXPECT_EQ(obj["Switches"], expected_switches);
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
	std::vector<std::string> expected_args{ "-switch", "--switch" };
	std::vector<std::string> expected_switches{ "switch", "switch" };
	EXPECT_FALSE(obj.null());
	EXPECT_EQ(obj["Path"], "/path/to/exe");
	EXPECT_EQ(obj["Args"], expected_args);
	EXPECT_EQ(obj["Switches"], expected_switches);
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
	std::vector<std::string> expected_args{ "-key", "value" };
	std::map<std::string, object> expected_values{ { "key", "value" } };
	EXPECT_FALSE(obj.null());
	EXPECT_EQ(obj["Path"], "/path/to/exe");
	EXPECT_EQ(obj["Args"], expected_args);
	EXPECT_EQ(obj["Values"], expected_values);
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
	std::vector<std::string> expected_args{ "-key=value" };
	std::map<std::string, object> expected_values{ { "key", "value" } };
	EXPECT_FALSE(obj.null());
	EXPECT_EQ(obj["Path"], "/path/to/exe");
	EXPECT_EQ(obj["Args"], expected_args);
	EXPECT_EQ(obj["Values"], expected_values);
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
	std::vector<std::string> expected_args{ "-key", "valuePart1", "valuePart2" };
	std::map<std::string, object> expected_values{ { "key", "valuePart1 valuePart2" } };
	EXPECT_FALSE(obj.null());
	EXPECT_EQ(obj["Path"], "/path/to/exe");
	EXPECT_EQ(obj["Args"], expected_args);
	EXPECT_EQ(obj["Values"], expected_values);
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
	std::vector<std::string> expected_args{ "-key=valuePart1", "valuePart2" };
	std::map<std::string, object> expected_values{ { "key", "valuePart1 valuePart2" } };
	EXPECT_FALSE(obj.null());
	EXPECT_EQ(obj["Path"], "/path/to/exe");
	EXPECT_EQ(obj["Args"], expected_args);
	EXPECT_EQ(obj["Values"], expected_values);
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
	std::vector<std::string> expected_args{ "-key", "value", "--key2", "value2" };
	std::map<std::string, object> expected_values{ { "key", "value" }, { "key2", "value2" } };
	EXPECT_FALSE(obj.null());
	EXPECT_EQ(obj["Path"], "/path/to/exe");
	EXPECT_EQ(obj["Args"], expected_args);
	EXPECT_EQ(obj["Values"], expected_values);
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
	std::vector<std::string> expected_args{ "-key=value", "--key2=value2" };
	std::map<std::string, object> expected_values{ { "key", "value" }, { "key2", "value2" } };
	EXPECT_FALSE(obj.null());
	EXPECT_EQ(obj["Path"], "/path/to/exe");
	EXPECT_EQ(obj["Args"], expected_args);
	EXPECT_EQ(obj["Values"], expected_values);
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
	std::vector<std::string> expected_args{ "--switch", "-key", "value" };
	std::vector<std::string> expected_switches{ "switch" };
	std::map<std::string, object> expected_values{ { "key", "value" } };
	EXPECT_FALSE(obj.null());
	EXPECT_EQ(obj["Path"], "/path/to/exe");
	EXPECT_EQ(obj["Args"], expected_args);
	EXPECT_EQ(obj["Switches"], expected_switches);
	EXPECT_EQ(obj["Values"], expected_values);

	EXPECT_TRUE(parameters.hasSwitch("switch"));
	EXPECT_FALSE(parameters.hasSwitch("switch2"));
	EXPECT_EQ(parameters.getValue("key"), "value");
}
