/**
 *
 *  @file config_options_test.cpp
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
#include <filesystem>
#include <string_view>

#include <cgride/config/config_options.hpp>

int main()
{
  {
    assert(cgride::config::to_string(cgride::config::ConfigFormat::Auto) == std::string_view("auto"));
    assert(cgride::config::to_string(cgride::config::ConfigFormat::Cgride) == std::string_view("cgride"));
  }

  {
    cgride::config::ConfigOptions options;

    assert(options.project_root() == std::filesystem::path("."));
    assert(options.config_path() == std::filesystem::path("cgride.config"));
    assert(options.resolved_config_path() == std::filesystem::path(".") / "cgride.config");
    assert(options.format() == cgride::config::ConfigFormat::Auto);
    assert(!options.allow_missing());
    assert(options.strict());
    assert(options.valid());
  }

  {
    cgride::config::ConfigOptions options("project.cgride");

    assert(options.config_path() == std::filesystem::path("project.cgride"));
    assert(options.project_root() == std::filesystem::path("."));
    assert(options.resolved_config_path() == std::filesystem::path(".") / "project.cgride");
    assert(options.valid());
  }

  {
    auto options = cgride::config::ConfigOptions::defaults();

    assert(options.project_root() == std::filesystem::path("."));
    assert(options.config_path() == std::filesystem::path("cgride.config"));
    assert(options.format() == cgride::config::ConfigFormat::Auto);
    assert(!options.allow_missing());
    assert(options.strict());
    assert(options.valid());
  }

  {
    cgride::config::ConfigOptions options;

    options
        .project_root("examples/app")
        .config_path("build.cgride")
        .format(cgride::config::ConfigFormat::Cgride)
        .allow_missing(true)
        .strict(false);

    assert(options.project_root() == std::filesystem::path("examples/app"));
    assert(options.config_path() == std::filesystem::path("build.cgride"));
    assert(options.resolved_config_path() == std::filesystem::path("examples/app") / "build.cgride");
    assert(options.format() == cgride::config::ConfigFormat::Cgride);
    assert(options.allow_missing());
    assert(!options.strict());
    assert(options.valid());
  }

  {
    cgride::config::ConfigOptions options;

    auto absolute = std::filesystem::temp_directory_path() / "cgride.config";

    options
        .project_root("ignored-root")
        .config_path(absolute);

    assert(options.config_path() == absolute);
    assert(options.resolved_config_path() == absolute);
    assert(options.valid());
  }

  {
    cgride::config::ConfigOptions options;

    options.project_root({});

    assert(options.project_root().empty());
    assert(!options.valid());

    options.project_root(".");

    assert(options.valid());

    options.config_path({});

    assert(options.config_path().empty());
    assert(!options.valid());

    options.config_path("cgride.config");

    assert(options.valid());
  }

  {
    cgride::config::ConfigOptions options;

    options
        .project_root("root")
        .config_path("cgride.config")
        .format(cgride::config::ConfigFormat::Auto)
        .allow_missing(false)
        .strict(true);

    assert(options.project_root() == std::filesystem::path("root"));
    assert(options.config_path() == std::filesystem::path("cgride.config"));
    assert(options.resolved_config_path() == std::filesystem::path("root") / "cgride.config");
    assert(options.format() == cgride::config::ConfigFormat::Auto);
    assert(!options.allow_missing());
    assert(options.strict());
    assert(options.valid());
  }

  return 0;
}
