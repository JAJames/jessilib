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
#include <filesystem>
#include <fstream>
#include "test.hpp"
#include "jessilib/config.hpp"
#include "jessilib/serialize.hpp" // format_not_available

using namespace jessilib;
using namespace std::literals;

// NOTE: see test/parser.cpp for test_parser implementation and details for format "test"

std::filesystem::path make_tmp_file(std::filesystem::path in_filename, const std::string& in_data) {
	// Build path
	std::filesystem::path path = std::filesystem::temp_directory_path();
	path /= in_filename;

	// Delete any previously existing file
	std::filesystem::remove(path);

	// Populate file
	{
		std::ofstream file{ path, std::ios::binary | std::ios::out };
		file << in_data << std::flush;
	}

	// Return fstream to file
	return path;
	//return std::fstream{ path, std::ios::binary | std::ios::out | std::ios::in };
}

TEST(ConfigTest, get_format) {
	EXPECT_EQ(config::get_format("example.json"), "json");
	EXPECT_EQ(config::get_format("example.json", "xml"), "xml");
	EXPECT_EQ(config::get_format("example", "json"), "json");
}

TEST(ConfigTest, read_object) {
	std::filesystem::path file_path = make_tmp_file("read_object.test", "some_data");
	EXPECT_EQ(config::read_object(file_path).get<std::u8string>(), u8"some_data");
}

TEST(ConfigTest, write_object) {
	std::filesystem::path file_path = make_tmp_file("write_object.test", "some_data");
	config::write_object(object{}, file_path);

	EXPECT_EQ(config::read_object(file_path).get<std::u8string>(), u8"serialize_result");
}

TEST(ConfigTest, load) {
	config l_config;

	// Write temp file out
	std::filesystem::path file_path = make_tmp_file("load.test", "some_data");

	// Load data from disk
	l_config.load(file_path);

	// Verify
	EXPECT_EQ(l_config.data().get<std::u8string>(), u8"some_data");
	EXPECT_EQ(l_config.filename(), file_path);
	EXPECT_EQ(l_config.format(), "test");
}

TEST(ConfigTest, reload) {
	config l_config;

	// Write temp file out
	std::filesystem::path file_path = make_tmp_file("reload.test", "some_data");

	// Load data from disk
	l_config.load(file_path);

	// Write new data to disk
	make_tmp_file("reload.test", "some_other_data");

	// Reload data from disk and compare
	l_config.reload();
	EXPECT_EQ(l_config.data().get<std::u8string>(), u8"some_other_data");
}

TEST(ConfigTest, set_data) {
	config l_config;

	l_config.set_data(u8"some_data");
	EXPECT_EQ(l_config.data().get<std::u8string>(), u8"some_data");
}

TEST(ConfigTest, write) {
	config l_config;
	std::filesystem::path file_path = make_tmp_file("write.test", "");

	l_config.set_data(u8"some_data");
	l_config.write(file_path);

	l_config.reload();
	EXPECT_EQ(l_config.data().get<std::u8string>(), u8"some_data");
}

TEST(ConfigTest, rewrite) {
	config l_config;

	// Write temp file out
	std::filesystem::path file_path = make_tmp_file("rewrite.test", "some_data");

	// Load data from disk
	l_config.load(file_path);

	// Set some other data
	l_config.set_data(u8"some_other_data");

	// Write data to disk
	l_config.write();

	// Reload from disk and verify
	l_config.reload();
	EXPECT_EQ(l_config.data().get<std::u8string>(), u8"some_other_data");
}
