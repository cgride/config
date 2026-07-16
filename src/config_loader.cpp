/**
 *
 *  @file config_loader.cpp
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
#include <cgride/config/config_loader.hpp>

#include <system_error>
#include <utility>

#include <cgride/core/error.hpp>

namespace cgride::config
{
  namespace
  {
    using cgride::core::Error;
    using cgride::core::ErrorCode;

    [[nodiscard]] cgride::core::Result<ConfigDocument> load_with_options(
        const ConfigOptions &options)
    {
      if (!options.valid())
      {
        return Error(
            ErrorCode::InvalidArgument,
            "Cannot load config with invalid options.");
      }

      const auto path = options.resolved_config_path();

      std::error_code error;

      const auto exists = std::filesystem::exists(path, error);

      if (error)
      {
        return Error(
            ErrorCode::IoError,
            "Failed to inspect config file.",
            error.message());
      }

      if (!exists)
      {
        if (options.allow_missing())
        {
          return ConfigDocument::from_path(path);
        }

        return Error(
            ErrorCode::NotFound,
            "Config file does not exist.",
            path);
      }

      auto source = ConfigSource::from_file(path);

      if (!source)
      {
        return source.error();
      }

      ConfigParser parser(options.strict());

      auto document = parser.parse(source.value());

      if (!document)
      {
        return document.error();
      }

      return document.value();
    }

  } // namespace

  ConfigLoader::ConfigLoader(ConfigOptions options)
      : options_(std::move(options))
  {
  }

  const ConfigOptions &ConfigLoader::options() const noexcept
  {
    return options_;
  }

  ConfigLoader &ConfigLoader::options(ConfigOptions options)
  {
    options_ = std::move(options);
    return *this;
  }

  cgride::core::Result<ConfigDocument> ConfigLoader::load() const
  {
    return load_with_options(options_);
  }

  cgride::core::Result<ConfigDocument> ConfigLoader::load(
      std::filesystem::path path) const
  {
    ConfigOptions options = options_;

    options.config_path(std::move(path));

    return load_with_options(options);
  }

  cgride::core::Result<ConfigDocument> ConfigLoader::load(
      const ConfigOptions &options) const
  {
    return load_with_options(options);
  }

  cgride::core::Result<ConfigDocument> load_config()
  {
    ConfigLoader loader;

    return loader.load();
  }

  cgride::core::Result<ConfigDocument> load_config(
      std::filesystem::path path)
  {
    ConfigLoader loader;

    return loader.load(std::move(path));
  }

  cgride::core::Result<ConfigDocument> load_config(
      const ConfigOptions &options)
  {
    ConfigLoader loader(options);

    return loader.load();
  }

} // namespace cgride::config
