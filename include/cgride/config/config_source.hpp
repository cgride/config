/**
 *
 *  @file config_source.hpp
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
#ifndef CGRIDE_CONFIG_CONFIG_SOURCE_HPP
#define CGRIDE_CONFIG_CONFIG_SOURCE_HPP

#include <filesystem>
#include <string>

#include <cgride/core/result.hpp>

namespace cgride::config
{
  /**
   * @class ConfigSource
   * @brief Raw configuration source before parsing.
   *
   * ConfigSource stores the origin path and raw text content of a configuration
   * document. It does not parse the content and it does not create a project
   * model.
   */
  class ConfigSource
  {
  public:
    /**
     * @brief Construct an empty config source.
     */
    ConfigSource() = default;

    /**
     * @brief Construct a config source from content.
     *
     * @param content Raw config content.
     */
    explicit ConfigSource(std::string content);

    /**
     * @brief Construct a config source from path and content.
     *
     * @param path Source file path.
     * @param content Raw config content.
     */
    ConfigSource(std::filesystem::path path, std::string content);

    /**
     * @brief Create a config source from raw content.
     *
     * @param content Raw config content.
     * @return Config source.
     */
    [[nodiscard]] static ConfigSource from_string(std::string content);

    /**
     * @brief Create a config source from path and raw content.
     *
     * @param path Source file path.
     * @param content Raw config content.
     * @return Config source.
     */
    [[nodiscard]] static ConfigSource from_string(
        std::filesystem::path path,
        std::string content);

    /**
     * @brief Read a config source from a file.
     *
     * @param path Source file path.
     * @return Config source or filesystem error.
     */
    [[nodiscard]] static cgride::core::Result<ConfigSource> from_file(
        std::filesystem::path path);

    /**
     * @brief Set the source path.
     *
     * @param path Source file path.
     * @return Reference to this source.
     */
    ConfigSource &path(std::filesystem::path path);

    /**
     * @brief Set the raw source content.
     *
     * @param content Raw config content.
     * @return Reference to this source.
     */
    ConfigSource &content(std::string content);

    /**
     * @brief Clear the source path.
     *
     * @return Reference to this source.
     */
    ConfigSource &clear_path() noexcept;

    /**
     * @brief Access the source path.
     */
    [[nodiscard]] const std::filesystem::path &path() const noexcept;

    /**
     * @brief Access the raw source content.
     */
    [[nodiscard]] const std::string &content() const noexcept;

    /**
     * @brief Return true when a source path is available.
     */
    [[nodiscard]] bool has_path() const noexcept;

    /**
     * @brief Return true when the raw content is empty.
     */
    [[nodiscard]] bool empty() const noexcept;

    /**
     * @brief Return the raw content size in bytes.
     */
    [[nodiscard]] std::size_t size() const noexcept;

    /**
     * @brief Return true when the source has usable content.
     */
    [[nodiscard]] bool valid() const noexcept;

  private:
    std::filesystem::path path_{};
    std::string content_{};
  };

} // namespace cgride::config

#endif // CGRIDE_CONFIG_CONFIG_SOURCE_HPP
