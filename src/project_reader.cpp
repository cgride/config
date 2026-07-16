/**
 *
 *  @file project_reader.cpp
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
#include <cgride/config/project_reader.hpp>

#include <string>

#include <cgride/core/error.hpp>

namespace cgride::config
{
  namespace
  {
    using cgride::core::Error;
    using cgride::core::ErrorCode;

    [[nodiscard]] bool is_target_section(std::string_view name) noexcept
    {
      return name.starts_with("target.") && name.size() > std::string_view("target.").size();
    }

    [[nodiscard]] bool is_supported_target_kind(std::string_view kind) noexcept
    {
      return kind == "executable" ||
             kind == "static_library" ||
             kind == "interface_library";
    }

    [[nodiscard]] cgride::core::Result<void> validate_project_section(
        const ConfigDocument &document)
    {
      const auto *project_section = document.find_section("project");

      if (project_section == nullptr)
      {
        return Error(
            ErrorCode::InvalidArgument,
            "Config document is missing [project] section.");
      }

      const auto name = project_section->value("name");

      if (!name.has_value() || name.value().empty())
      {
        return Error(
            ErrorCode::InvalidArgument,
            "Config project section is missing name.");
      }

      return cgride::core::Result<void>::ok();
    }

    [[nodiscard]] cgride::core::Result<void> validate_target_sections(
        const ConfigDocument &document)
    {
      bool has_target = false;

      for (const auto &section : document.sections())
      {
        if (!is_target_section(section.name()))
        {
          continue;
        }

        has_target = true;

        const auto kind = section.value("kind");

        if (!kind.has_value() || kind.value().empty())
        {
          return Error(
              ErrorCode::InvalidArgument,
              "Config target section is missing kind.",
              section.name());
        }

        if (!is_supported_target_kind(kind.value()))
        {
          return Error(
              ErrorCode::InvalidArgument,
              "Unsupported config target kind.",
              section.name());
        }

        if (kind.value() != "interface_library")
        {
          const auto sources = section.value("sources");

          if (!sources.has_value() || sources.value().empty())
          {
            return Error(
                ErrorCode::InvalidArgument,
                "Config target section is missing sources.",
                section.name());
          }
        }
      }

      if (!has_target)
      {
        return Error(
            ErrorCode::InvalidArgument,
            "Config document does not contain any target section.");
      }

      return cgride::core::Result<void>::ok();
    }

    [[nodiscard]] cgride::core::Result<void> validate_document(
        const ConfigDocument &document)
    {
      if (!document.valid())
      {
        return Error(
            ErrorCode::InvalidArgument,
            "Cannot read project from an invalid config document.");
      }

      auto project_section = validate_project_section(document);

      if (!project_section)
      {
        return project_section.error();
      }

      auto target_sections = validate_target_sections(document);

      if (!target_sections)
      {
        return target_sections.error();
      }

      return cgride::core::Result<void>::ok();
    }

  } // namespace

  cgride::core::Result<cgride::project::Project> ProjectReader::read(
      const ConfigDocument &document) const
  {
    auto validated = validate_document(document);

    if (!validated)
    {
      return validated.error();
    }

    cgride::project::Project project;

    /*
     * The config document is validated here, but target-to-project mapping is
     * intentionally left for the next step because the project module API is
     * still evolving independently.
     */

    return project;
  }

  cgride::core::Result<cgride::project::Project> ProjectReader::read(
      const ConfigSource &source) const
  {
    ConfigParser parser;

    auto document = parser.parse(source);

    if (!document)
    {
      return document.error();
    }

    return read(document.value());
  }

  cgride::core::Result<cgride::project::Project> ProjectReader::read_string(
      std::string_view content) const
  {
    ConfigParser parser;

    auto document = parser.parse_string(content);

    if (!document)
    {
      return document.error();
    }

    return read(document.value());
  }

  cgride::core::Result<cgride::project::Project> read_project(
      const ConfigDocument &document)
  {
    ProjectReader reader;

    return reader.read(document);
  }

  cgride::core::Result<cgride::project::Project> read_project(
      const ConfigSource &source)
  {
    ProjectReader reader;

    return reader.read(source);
  }

  cgride::core::Result<cgride::project::Project> read_project_string(
      std::string_view content)
  {
    ProjectReader reader;

    return reader.read_string(content);
  }

} // namespace cgride::config
