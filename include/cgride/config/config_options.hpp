/**
 *
 *  @file config_options.hpp
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
#ifndef CGRIDE_CONFIG_CONFIG_OPTIONS_HPP
#define CGRIDE_CONFIG_CONFIG_OPTIONS_HPP

#include <filesystem>
#include <string_view>

namespace cgride::config
{
  /**
   * @enum ConfigFormat
   * @brief Supported project configuration formats.
   */
  enum class ConfigFormat
  {
    Auto,
    Cgride
  };

  /**
   * @brief Convert a config format to a stable string.
   *
   * @param format Config format.
   * @return Stable string representation.
   */
  [[nodiscard]] std::string_view to_string(ConfigFormat format) noexcept;

  /**
   * @class ConfigOptions
   * @brief Options used when loading and parsing project configuration.
   *
   * ConfigOptions describes where a project configuration file is located and
   * how strict the loader should be. It does not read files, parse content or
   * create a project model by itself.
   */
  class ConfigOptions
  {
  public:
    /**
     * @brief Construct default config options.
     */
    ConfigOptions() = default;

    /**
     * @brief Construct config options with a config path.
     *
     * @param path Configuration file path.
     */
    explicit ConfigOptions(std::filesystem::path path);

    /**
     * @brief Create default config options.
     *
     * @return Config options.
     */
    [[nodiscard]] static ConfigOptions defaults();

    /**
     * @brief Set the project root directory.
     *
     * @param path Project root directory.
     * @return Reference to these options.
     */
    ConfigOptions &project_root(std::filesystem::path path);

    /**
     * @brief Set the configuration file path.
     *
     * @param path Configuration file path.
     * @return Reference to these options.
     */
    ConfigOptions &config_path(std::filesystem::path path);

    /**
     * @brief Set the expected config format.
     *
     * @param format Config format.
     * @return Reference to these options.
     */
    ConfigOptions &format(ConfigFormat format) noexcept;

    /**
     * @brief Allow or reject missing config files.
     *
     * @param value True to allow a missing config file.
     * @return Reference to these options.
     */
    ConfigOptions &allow_missing(bool value) noexcept;

    /**
     * @brief Enable or disable strict parsing.
     *
     * @param value True to reject malformed or unknown config lines.
     * @return Reference to these options.
     */
    ConfigOptions &strict(bool value) noexcept;

    /**
     * @brief Access the project root directory.
     */
    [[nodiscard]] const std::filesystem::path &project_root() const noexcept;

    /**
     * @brief Access the configuration file path.
     */
    [[nodiscard]] const std::filesystem::path &config_path() const noexcept;

    /**
     * @brief Access the resolved configuration file path.
     *
     * Relative config paths are resolved from the project root.
     *
     * @return Resolved configuration file path.
     */
    [[nodiscard]] std::filesystem::path resolved_config_path() const;

    /**
     * @brief Access the config format.
     */
    [[nodiscard]] ConfigFormat format() const noexcept;

    /**
     * @brief Return true when missing config files are allowed.
     */
    [[nodiscard]] bool allow_missing() const noexcept;

    /**
     * @brief Return true when strict parsing is enabled.
     */
    [[nodiscard]] bool strict() const noexcept;

    /**
     * @brief Return true when options are usable.
     */
    [[nodiscard]] bool valid() const noexcept;

  private:
    std::filesystem::path project_root_{"."};
    std::filesystem::path config_path_{"cgride.config"};
    ConfigFormat format_{ConfigFormat::Auto};
    bool allow_missing_{false};
    bool strict_{true};
  };

} // namespace cgride::config

#endif // CGRIDE_CONFIG_CONFIG_OPTIONS_HPP
