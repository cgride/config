/**
 *
 *  @file project_reader.hpp
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
#ifndef CGRIDE_CONFIG_PROJECT_READER_HPP
#define CGRIDE_CONFIG_PROJECT_READER_HPP

#include <string_view>

#include <cgride/config/config_document.hpp>
#include <cgride/config/config_parser.hpp>
#include <cgride/config/config_source.hpp>
#include <cgride/core/result.hpp>
#include <cgride/project/project.hpp>

namespace cgride::config
{
  /**
   * @class ProjectReader
   * @brief Converts a parsed config document into a project model.
   *
   * ProjectReader is the bridge between the textual configuration layer and
   * cgride::project. It validates the expected config sections and returns a
   * project model.
   *
   * This first version keeps the conversion intentionally conservative. It
   * validates the document shape and returns a Project object. As the project
   * module API stabilizes, this reader can map config targets, sources,
   * includes and links into the full project model.
   */
  class ProjectReader
  {
  public:
    /**
     * @brief Construct a project reader.
     */
    ProjectReader() = default;

    /**
     * @brief Read a project from a parsed config document.
     *
     * @param document Parsed config document.
     * @return Project model or validation error.
     */
    [[nodiscard]] cgride::core::Result<cgride::project::Project> read(
        const ConfigDocument &document) const;

    /**
     * @brief Parse a source and read a project from it.
     *
     * @param source Raw config source.
     * @return Project model or parse/validation error.
     */
    [[nodiscard]] cgride::core::Result<cgride::project::Project> read(
        const ConfigSource &source) const;

    /**
     * @brief Parse raw config content and read a project from it.
     *
     * @param content Raw config content.
     * @return Project model or parse/validation error.
     */
    [[nodiscard]] cgride::core::Result<cgride::project::Project> read_string(
        std::string_view content) const;
  };

  /**
   * @brief Read a project from a parsed config document.
   *
   * @param document Parsed config document.
   * @return Project model or validation error.
   */
  [[nodiscard]] cgride::core::Result<cgride::project::Project> read_project(
      const ConfigDocument &document);

  /**
   * @brief Read a project from a raw config source.
   *
   * @param source Raw config source.
   * @return Project model or parse/validation error.
   */
  [[nodiscard]] cgride::core::Result<cgride::project::Project> read_project(
      const ConfigSource &source);

  /**
   * @brief Read a project from raw config content.
   *
   * @param content Raw config content.
   * @return Project model or parse/validation error.
   */
  [[nodiscard]] cgride::core::Result<cgride::project::Project> read_project_string(
      std::string_view content);

} // namespace cgride::config

#endif // CGRIDE_CONFIG_PROJECT_READER_HPP
