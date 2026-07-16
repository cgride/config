/**
 *
 *  @file config_parser.hpp
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
#ifndef CGRIDE_CONFIG_CONFIG_PARSER_HPP
#define CGRIDE_CONFIG_CONFIG_PARSER_HPP

#include <string>
#include <string_view>

#include <cgride/config/config_document.hpp>
#include <cgride/config/config_source.hpp>
#include <cgride/core/result.hpp>

namespace cgride::config
{
  /**
   * @class ConfigParser
   * @brief Parser for the first Cgride config format.
   *
   * The parser accepts a small section-based format:
   *
   * [project]
   * name = app
   *
   * [target.app]
   * kind = executable
   * sources = src/main.cpp
   *
   * It stores parsed values as strings inside ConfigDocument. Higher-level
   * readers decide how values become project targets, source sets, requirements
   * and build options.
   */
  class ConfigParser
  {
  public:
    /**
     * @brief Construct a strict config parser.
     */
    ConfigParser() = default;

    /**
     * @brief Construct a config parser with explicit strict mode.
     *
     * @param strict True to reject malformed lines.
     */
    explicit ConfigParser(bool strict);

    /**
     * @brief Enable or disable strict parsing.
     *
     * @param value True to reject malformed lines.
     * @return Reference to this parser.
     */
    ConfigParser &strict(bool value) noexcept;

    /**
     * @brief Return true when strict parsing is enabled.
     */
    [[nodiscard]] bool strict() const noexcept;

    /**
     * @brief Parse a config source.
     *
     * @param source Raw config source.
     * @return Parsed document or parse error.
     */
    [[nodiscard]] cgride::core::Result<ConfigDocument> parse(
        const ConfigSource &source) const;

    /**
     * @brief Parse raw config content.
     *
     * @param content Raw config content.
     * @return Parsed document or parse error.
     */
    [[nodiscard]] cgride::core::Result<ConfigDocument> parse_string(
        std::string_view content) const;

  private:
    bool strict_{true};
  };

  /**
   * @brief Parse a config source with default strict parser.
   *
   * @param source Raw config source.
   * @return Parsed document or parse error.
   */
  [[nodiscard]] cgride::core::Result<ConfigDocument> parse_config(
      const ConfigSource &source);

  /**
   * @brief Parse raw config content with default strict parser.
   *
   * @param content Raw config content.
   * @return Parsed document or parse error.
   */
  [[nodiscard]] cgride::core::Result<ConfigDocument> parse_config_string(
      std::string_view content);

} // namespace cgride::config

#endif // CGRIDE_CONFIG_CONFIG_PARSER_HPP
