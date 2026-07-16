/**
 *
 *  @file config_source.cpp
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
#include <cgride/config/config_source.hpp>

#include <fstream>
#include <sstream>
#include <system_error>
#include <utility>

#include <cgride/core/error.hpp>

namespace cgride::config
{
  namespace
  {
    using cgride::core::Error;
    using cgride::core::ErrorCode;

  } // namespace

  ConfigSource::ConfigSource(std::string content)
      : content_(std::move(content))
  {
  }

  ConfigSource::ConfigSource(
      std::filesystem::path path,
      std::string content)
      : path_(std::move(path)),
        content_(std::move(content))
  {
  }

  ConfigSource ConfigSource::from_string(std::string content)
  {
    return ConfigSource(std::move(content));
  }

  ConfigSource ConfigSource::from_string(
      std::filesystem::path path,
      std::string content)
  {
    return ConfigSource(
        std::move(path),
        std::move(content));
  }

  cgride::core::Result<ConfigSource> ConfigSource::from_file(
      std::filesystem::path path)
  {
    if (path.empty())
    {
      return Error(
          ErrorCode::InvalidArgument,
          "Cannot read config source from an empty path.");
    }

    std::error_code error;

    const auto exists = std::filesystem::exists(path, error);

    if (error)
    {
      return Error(
          ErrorCode::IoError,
          "Failed to inspect config source path.",
          error.message());
    }

    if (!exists)
    {
      return Error(
          ErrorCode::NotFound,
          "Config source file does not exist.",
          path);
    }

    const auto regular_file = std::filesystem::is_regular_file(path, error);

    if (error)
    {
      return Error(
          ErrorCode::IoError,
          "Failed to inspect config source file type.",
          error.message());
    }

    if (!regular_file)
    {
      return Error(
          ErrorCode::InvalidArgument,
          "Config source path is not a regular file.",
          path);
    }

    std::ifstream file(path, std::ios::binary);

    if (!file)
    {
      return Error(
          ErrorCode::IoError,
          "Failed to open config source file.",
          path);
    }

    std::ostringstream buffer;
    buffer << file.rdbuf();

    if (file.bad())
    {
      return Error(
          ErrorCode::IoError,
          "Failed while reading config source file.",
          path);
    }

    return ConfigSource(
        std::move(path),
        buffer.str());
  }

  ConfigSource &ConfigSource::path(std::filesystem::path path)
  {
    path_ = std::move(path);
    return *this;
  }

  ConfigSource &ConfigSource::content(std::string content)
  {
    content_ = std::move(content);
    return *this;
  }

  ConfigSource &ConfigSource::clear_path() noexcept
  {
    path_.clear();
    return *this;
  }

  const std::filesystem::path &ConfigSource::path() const noexcept
  {
    return path_;
  }

  const std::string &ConfigSource::content() const noexcept
  {
    return content_;
  }

  bool ConfigSource::has_path() const noexcept
  {
    return !path_.empty();
  }

  bool ConfigSource::empty() const noexcept
  {
    return content_.empty();
  }

  std::size_t ConfigSource::size() const noexcept
  {
    return content_.size();
  }

  bool ConfigSource::valid() const noexcept
  {
    return !content_.empty();
  }

} // namespace cgride::config
