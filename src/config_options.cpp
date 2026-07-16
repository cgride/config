/**
 *
 *  @file config_options.cpp
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
#include <cgride/config/config_options.hpp>

#include <utility>

namespace cgride::config
{
  std::string_view to_string(ConfigFormat format) noexcept
  {
    switch (format)
    {
    case ConfigFormat::Auto:
      return "auto";

    case ConfigFormat::Cgride:
      return "cgride";
    }

    return "auto";
  }

  ConfigOptions::ConfigOptions(std::filesystem::path path)
      : config_path_(std::move(path))
  {
  }

  ConfigOptions ConfigOptions::defaults()
  {
    return ConfigOptions{};
  }

  ConfigOptions &ConfigOptions::project_root(std::filesystem::path path)
  {
    project_root_ = std::move(path);
    return *this;
  }

  ConfigOptions &ConfigOptions::config_path(std::filesystem::path path)
  {
    config_path_ = std::move(path);
    return *this;
  }

  ConfigOptions &ConfigOptions::format(ConfigFormat format) noexcept
  {
    format_ = format;
    return *this;
  }

  ConfigOptions &ConfigOptions::allow_missing(bool value) noexcept
  {
    allow_missing_ = value;
    return *this;
  }

  ConfigOptions &ConfigOptions::strict(bool value) noexcept
  {
    strict_ = value;
    return *this;
  }

  const std::filesystem::path &ConfigOptions::project_root() const noexcept
  {
    return project_root_;
  }

  const std::filesystem::path &ConfigOptions::config_path() const noexcept
  {
    return config_path_;
  }

  std::filesystem::path ConfigOptions::resolved_config_path() const
  {
    if (config_path_.is_absolute())
    {
      return config_path_;
    }

    return project_root_ / config_path_;
  }

  ConfigFormat ConfigOptions::format() const noexcept
  {
    return format_;
  }

  bool ConfigOptions::allow_missing() const noexcept
  {
    return allow_missing_;
  }

  bool ConfigOptions::strict() const noexcept
  {
    return strict_;
  }

  bool ConfigOptions::valid() const noexcept
  {
    return !project_root_.empty() &&
           !config_path_.empty();
  }

} // namespace cgride::config
