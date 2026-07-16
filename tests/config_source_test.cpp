/**
 *
 *  @file config_source_test.cpp
 *  @author Gaspard Kirira
 *
 *  Copyright 2026, Gaspard Kirira.
 *  All rights reserved.
 *  https://github.com/cgride/config
 *
 *  Use of this source code is governed by an MIT license
 *  that can be found in the LICENSE file.
 *
 *  Cgride
 *
 */
#include <cassert>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <string>

#include <cgride/config/config_source.hpp>
#include <cgride/core/error.hpp>

namespace
{
  [[nodiscard]] std::filesystem::path make_test_directory()
  {
    const auto now = std::chrono::steady_clock::now().time_since_epoch().count();

    auto directory = std::filesystem::temp_directory_path() /
                     ("cgride_config_source_test_" + std::to_string(now));

    std::filesystem::remove_all(directory);
    std::filesystem::create_directories(directory);

    return directory;
  }

  void write_text_file(const std::filesystem::path &path, const std::string &content)
  {
    std::filesystem::create_directories(path.parent_path());

    std::ofstream file(path, std::ios::binary | std::ios::trunc);
    file << content;
  }

} // namespace

int main()
{
  {
    cgride::config::ConfigSource source;

    assert(source.path().empty());
    assert(source.content().empty());
    assert(!source.has_path());
    assert(source.empty());
    assert(source.size() == 0);
    assert(!source.valid());
  }

  {
    cgride::config::ConfigSource source(std::string("[project]\nname = app\n"));

    assert(source.path().empty());
    assert(!source.has_path());
    assert(!source.empty());
    assert(source.size() == std::string("[project]\nname = app\n").size());
    assert(source.content() == "[project]\nname = app\n");
    assert(source.valid());
  }

  {
    cgride::config::ConfigSource source(
        std::filesystem::path("cgride.config"),
        std::string("[project]\nname = app\n"));

    assert(source.has_path());
    assert(source.path() == std::filesystem::path("cgride.config"));
    assert(source.content() == "[project]\nname = app\n");
    assert(source.valid());
  }

  {
    auto source = cgride::config::ConfigSource::from_string(
        std::string("[project]\nname = app\n"));

    assert(!source.has_path());
    assert(source.content() == "[project]\nname = app\n");
    assert(source.valid());
  }

  {
    auto source = cgride::config::ConfigSource::from_string(
        std::filesystem::path("project.cgride"),
        std::string("[project]\nname = app\n"));

    assert(source.has_path());
    assert(source.path() == std::filesystem::path("project.cgride"));
    assert(source.content() == "[project]\nname = app\n");
    assert(source.valid());
  }

  {
    cgride::config::ConfigSource source;

    source
        .path("cgride.config")
        .content("[project]\nname = demo\n");

    assert(source.has_path());
    assert(source.path() == std::filesystem::path("cgride.config"));
    assert(source.content() == "[project]\nname = demo\n");
    assert(source.valid());

    source.clear_path();

    assert(!source.has_path());
    assert(source.path().empty());
    assert(source.valid());
  }

  {
    cgride::config::ConfigSource source;

    source.content("");

    assert(source.empty());
    assert(source.size() == 0);
    assert(!source.valid());

    source.content("x = y\n");

    assert(!source.empty());
    assert(source.size() == std::string("x = y\n").size());
    assert(source.valid());
  }

  {
    auto result = cgride::config::ConfigSource::from_file({});

    assert(!result);
    assert(result.error().code() == cgride::core::ErrorCode::InvalidArgument);
    assert(result.error().message() == "Cannot read config source from an empty path.");
  }

  {
    auto directory = make_test_directory();
    auto path = directory / "missing.cgride";

    auto result = cgride::config::ConfigSource::from_file(path);

    assert(!result);
    assert(result.error().code() == cgride::core::ErrorCode::NotFound);
    assert(result.error().message() == "Config source file does not exist.");
    assert(result.error().path().has_value());
    assert(result.error().path().value() == path);

    std::filesystem::remove_all(directory);
  }

  {
    auto directory = make_test_directory();

    auto result = cgride::config::ConfigSource::from_file(directory);

    assert(!result);
    assert(result.error().code() == cgride::core::ErrorCode::InvalidArgument);
    assert(result.error().message() == "Config source path is not a regular file.");
    assert(result.error().path().has_value());
    assert(result.error().path().value() == directory);

    std::filesystem::remove_all(directory);
  }

  {
    auto directory = make_test_directory();
    auto path = directory / "cgride.config";

    const std::string content =
        "[project]\n"
        "name = app\n"
        "\n"
        "[target.app]\n"
        "kind = executable\n"
        "sources = src/main.cpp\n";

    write_text_file(path, content);

    auto result = cgride::config::ConfigSource::from_file(path);

    assert(result);

    auto source = result.value();

    assert(source.has_path());
    assert(source.path() == path);
    assert(source.content() == content);
    assert(source.size() == content.size());
    assert(!source.empty());
    assert(source.valid());

    std::filesystem::remove_all(directory);
  }

  {
    auto directory = make_test_directory();
    auto path = directory / "empty.config";

    write_text_file(path, "");

    auto result = cgride::config::ConfigSource::from_file(path);

    assert(result);

    auto source = result.value();

    assert(source.has_path());
    assert(source.path() == path);
    assert(source.content().empty());
    assert(source.empty());
    assert(source.size() == 0);
    assert(!source.valid());

    std::filesystem::remove_all(directory);
  }

  return 0;
}
