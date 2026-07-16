/**
 *
 *  @file config_loader_test.cpp
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

#include <cgride/config/config_loader.hpp>
#include <cgride/core/error.hpp>

namespace
{
  [[nodiscard]] std::filesystem::path make_test_directory()
  {
    const auto now = std::chrono::steady_clock::now().time_since_epoch().count();

    auto directory = std::filesystem::temp_directory_path() /
                     ("cgride_config_loader_test_" + std::to_string(now));

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
    cgride::config::ConfigLoader loader;

    assert(loader.options().project_root() == std::filesystem::path("."));
    assert(loader.options().config_path() == std::filesystem::path("cgride.config"));
    assert(loader.options().strict());
  }

  {
    cgride::config::ConfigOptions options;

    options
        .project_root("examples/app")
        .config_path("project.cgride")
        .strict(false)
        .allow_missing(true);

    cgride::config::ConfigLoader loader(options);

    assert(loader.options().project_root() == std::filesystem::path("examples/app"));
    assert(loader.options().config_path() == std::filesystem::path("project.cgride"));
    assert(!loader.options().strict());
    assert(loader.options().allow_missing());
  }

  {
    cgride::config::ConfigLoader loader;

    cgride::config::ConfigOptions options;

    options
        .project_root("root")
        .config_path("build.cgride")
        .strict(false);

    loader.options(options);

    assert(loader.options().project_root() == std::filesystem::path("root"));
    assert(loader.options().config_path() == std::filesystem::path("build.cgride"));
    assert(!loader.options().strict());
  }

  {
    cgride::config::ConfigOptions options;

    options.project_root({});

    cgride::config::ConfigLoader loader(options);

    auto result = loader.load();

    assert(!result);
    assert(result.error().code() == cgride::core::ErrorCode::InvalidArgument);
    assert(result.error().message() == "Cannot load config with invalid options.");
  }

  {
    auto directory = make_test_directory();

    cgride::config::ConfigOptions options;

    options
        .project_root(directory)
        .config_path("missing.config");

    cgride::config::ConfigLoader loader(options);

    auto result = loader.load();

    assert(!result);
    assert(result.error().code() == cgride::core::ErrorCode::NotFound);
    assert(result.error().message() == "Config file does not exist.");
    assert(result.error().path().has_value());
    assert(result.error().path().value() == directory / "missing.config");

    std::filesystem::remove_all(directory);
  }

  {
    auto directory = make_test_directory();

    cgride::config::ConfigOptions options;

    options
        .project_root(directory)
        .config_path("missing.config")
        .allow_missing(true);

    cgride::config::ConfigLoader loader(options);

    auto result = loader.load();

    assert(result);

    auto document = result.value();

    assert(document.has_path());
    assert(document.path() == directory / "missing.config");
    assert(document.empty());
    assert(!document.valid());

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

    cgride::config::ConfigOptions options;

    options
        .project_root(directory)
        .config_path("cgride.config");

    cgride::config::ConfigLoader loader(options);

    auto result = loader.load();

    assert(result);

    auto document = result.value();

    assert(document.has_path());
    assert(document.path() == path);
    assert(document.valid());
    assert(document.value("project", "name").value() == "app");
    assert(document.value("target.app", "kind").value() == "executable");
    assert(document.value("target.app", "sources").value() == "src/main.cpp");

    std::filesystem::remove_all(directory);
  }

  {
    auto directory = make_test_directory();
    auto path = directory / "custom.config";

    const std::string content =
        "[project]\n"
        "name = custom\n"
        "\n"
        "[target.custom]\n"
        "kind = static_library\n"
        "sources = src/custom.cpp\n";

    write_text_file(path, content);

    cgride::config::ConfigLoader loader;

    auto result = loader.load(path);

    assert(result);

    auto document = result.value();

    assert(document.has_path());
    assert(document.path() == path);
    assert(document.value("project", "name").value() == "custom");
    assert(document.value("target.custom", "kind").value() == "static_library");

    std::filesystem::remove_all(directory);
  }

  {
    auto directory = make_test_directory();
    auto path = directory / "explicit.config";

    const std::string content =
        "[project]\n"
        "name = explicit\n"
        "\n"
        "[target.explicit]\n"
        "kind = executable\n"
        "sources = src/main.cpp\n";

    write_text_file(path, content);

    cgride::config::ConfigOptions options;

    options
        .project_root(directory)
        .config_path("explicit.config");

    cgride::config::ConfigLoader loader;

    auto result = loader.load(options);

    assert(result);

    auto document = result.value();

    assert(document.path() == path);
    assert(document.value("project", "name").value() == "explicit");

    std::filesystem::remove_all(directory);
  }

  {
    auto directory = make_test_directory();
    auto path = directory / "bad.config";

    const std::string content =
        "[project]\n"
        "bad key = app\n";

    write_text_file(path, content);

    cgride::config::ConfigOptions options;

    options
        .project_root(directory)
        .config_path("bad.config")
        .strict(true);

    cgride::config::ConfigLoader loader(options);

    auto result = loader.load();

    assert(!result);
    assert(result.error().code() == cgride::core::ErrorCode::InvalidArgument);
    assert(result.error().message() == "Invalid config key.");

    std::filesystem::remove_all(directory);
  }

  {
    auto directory = make_test_directory();
    auto path = directory / "loose.config";

    const std::string content =
        "[project]\n"
        "bad key = ignored\n"
        "name = loose\n";

    write_text_file(path, content);

    cgride::config::ConfigOptions options;

    options
        .project_root(directory)
        .config_path("loose.config")
        .strict(false);

    cgride::config::ConfigLoader loader(options);

    auto result = loader.load();

    assert(result);

    auto document = result.value();

    assert(document.value("project", "name").value() == "loose");
    assert(!document.has("project", "bad key"));

    std::filesystem::remove_all(directory);
  }

  {
    auto directory = make_test_directory();
    auto path = directory / "empty.config";

    write_text_file(path, "");

    auto result = cgride::config::load_config(path);

    assert(!result);
    assert(result.error().code() == cgride::core::ErrorCode::InvalidArgument);
    assert(result.error().message() == "Cannot parse an empty config source.");

    std::filesystem::remove_all(directory);
  }

  {
    auto directory = make_test_directory();
    auto path = directory / "free.config";

    const std::string content =
        "[project]\n"
        "name = free\n"
        "\n"
        "[target.free]\n"
        "kind = executable\n"
        "sources = src/free.cpp\n";

    write_text_file(path, content);

    auto result = cgride::config::load_config(path);

    assert(result);
    assert(result.value().value("project", "name").value() == "free");

    std::filesystem::remove_all(directory);
  }

  {
    auto directory = make_test_directory();
    auto path = directory / "options.config";

    const std::string content =
        "[project]\n"
        "name = options\n"
        "\n"
        "[target.options]\n"
        "kind = executable\n"
        "sources = src/options.cpp\n";

    write_text_file(path, content);

    cgride::config::ConfigOptions options;

    options
        .project_root(directory)
        .config_path("options.config");

    auto result = cgride::config::load_config(options);

    assert(result);
    assert(result.value().path() == path);
    assert(result.value().value("project", "name").value() == "options");

    std::filesystem::remove_all(directory);
  }

  return 0;
}
