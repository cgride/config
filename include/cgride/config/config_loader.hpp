/**
 *
 *  @file config_loader.hpp
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
#ifndef CGRIDE_CONFIG_CONFIG_LOADER_HPP
#define CGRIDE_CONFIG_CONFIG_LOADER_HPP

#include <filesystem>

#include <cgride/config/config_document.hpp>
#include <cgride/config/config_options.hpp>
#include <cgride/config/config_parser.hpp>
#include <cgride/core/result.hpp>

namespace cgride::config
{
  /**
   * @class ConfigLoader
   * @brief Loads a config file from disk and parses it.
   *
   * ConfigLoader connects ConfigOptions, ConfigSource and ConfigParser.
   * It does not create project targets and it does not invoke the build engine.
   */
  class ConfigLoader
  {
  public:
    /**
     * @brief Construct a config loader with default options.
     */
    ConfigLoader() = default;

    /**
     * @brief Construct a config loader with explicit options.
     *
     * @param options Loader options.
     */
    explicit ConfigLoader(ConfigOptions options);

    /**
     * @brief Access loader options.
     */
    [[nodiscard]] const ConfigOptions &options() const noexcept;

    /**
     * @brief Set loader options.
     *
     * @param options Loader options.
     * @return Reference to this loader.
     */
    ConfigLoader &options(ConfigOptions options);

    /**
     * @brief Load and parse the config file from current options.
     *
     * @return Parsed document or load/parse error.
     */
    [[nodiscard]] cgride::core::Result<ConfigDocument> load() const;

    /**
     * @brief Load and parse a config file path.
     *
     * @param path Config file path.
     * @return Parsed document or load/parse error.
     */
    [[nodiscard]] cgride::core::Result<ConfigDocument> load(
        std::filesystem::path path) const;

    /**
     * @brief Load and parse a config file with explicit options.
     *
     * @param options Loader options.
     * @return Parsed document or load/parse error.
     */
    [[nodiscard]] cgride::core::Result<ConfigDocument> load(
        const ConfigOptions &options) const;

  private:
    ConfigOptions options_{};
  };

  /**
   * @brief Load and parse a config file using default options.
   *
   * @return Parsed document or load/parse error.
   */
  [[nodiscard]] cgride::core::Result<ConfigDocument> load_config();

  /**
   * @brief Load and parse a config file path using default options.
   *
   * @param path Config file path.
   * @return Parsed document or load/parse error.
   */
  [[nodiscard]] cgride::core::Result<ConfigDocument> load_config(
      std::filesystem::path path);

  /**
   * @brief Load and parse a config file with explicit options.
   *
   * @param options Loader options.
   * @return Parsed document or load/parse error.
   */
  [[nodiscard]] cgride::core::Result<ConfigDocument> load_config(
      const ConfigOptions &options);

} // namespace cgride::config

#endif // CGRIDE_CONFIG_CONFIG_LOADER_HPP
