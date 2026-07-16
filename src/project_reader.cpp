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

#include <algorithm>
#include <cctype>
#include <string>
#include <string_view>
#include <vector>

#include <cgride/core/error.hpp>
#include <cgride/project/visibility.hpp>

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

    [[nodiscard]] std::string trim(std::string_view value)
    {
      auto begin = value.begin();
      auto end = value.end();

      while (begin != end && std::isspace(static_cast<unsigned char>(*begin)) != 0)
      {
        ++begin;
      }

      while (begin != end && std::isspace(static_cast<unsigned char>(*(end - 1))) != 0)
      {
        --end;
      }

      return std::string(begin, end);
    }

    [[nodiscard]] std::vector<std::string> split_list(std::string_view value)
    {
      std::vector<std::string> values;
      std::string current;

      for (const auto character : value)
      {
        if (character == ',' || character == ';')
        {
          auto item = trim(current);

          if (!item.empty())
          {
            values.push_back(std::move(item));
          }

          current.clear();
          continue;
        }

        current.push_back(character);
      }

      auto item = trim(current);

      if (!item.empty())
      {
        values.push_back(std::move(item));
      }

      return values;
    }

    [[nodiscard]] std::string target_name_from_section(std::string_view section)
    {
      return std::string(section.substr(std::string_view("target.").size()));
    }

    [[nodiscard]] bool contains_glob_pattern(std::string_view value) noexcept
    {
      return value.find_first_of("*?[") != std::string_view::npos;
    }

    [[nodiscard]] cgride::project::TargetKind target_kind_from_string(
        std::string_view kind) noexcept
    {
      if (kind == "static_library")
      {
        return cgride::project::TargetKind::StaticLibrary;
      }

      if (kind == "interface_library")
      {
        return cgride::project::TargetKind::InterfaceLibrary;
      }

      return cgride::project::TargetKind::Executable;
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

    void apply_optional_list(
        const ConfigSection &section,
        std::string_view key,
        const auto &callback)
    {
      const auto value = section.value(key);

      if (!value.has_value())
      {
        return;
      }

      for (auto &item : split_list(value.value()))
      {
        callback(std::move(item));
      }
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

    const auto *project_section = document.find_section("project");

    cgride::project::Project project(project_section->value("name").value());

    for (const auto &section : document.sections())
    {
      if (!is_target_section(section.name()))
      {
        continue;
      }

      auto &target = project.target(
          target_name_from_section(section.name()),
          target_kind_from_string(section.value("kind").value()));

      apply_optional_list(section, "sources", [&target](std::string source) {
        if (contains_glob_pattern(source))
        {
          target.sources(std::move(source));
          return;
        }

        target.source(std::move(source));
      });

      apply_optional_list(section, "include_dirs", [&target](std::string include_dir) {
        target.include_directory(std::move(include_dir));
      });

      apply_optional_list(section, "public_include_dirs", [&target](std::string include_dir) {
        target.include_directory(std::move(include_dir), cgride::project::Visibility::Public);
      });

      apply_optional_list(section, "definitions", [&target](std::string definition) {
        target.compile_definition(std::move(definition));
      });

      apply_optional_list(section, "compile_options", [&target](std::string option) {
        target.compile_option(std::move(option));
      });

      apply_optional_list(section, "link_options", [&target](std::string option) {
        target.link_option(std::move(option));
      });

      apply_optional_list(section, "libraries", [&target](std::string library) {
        target.link_library(std::move(library));
      });

      apply_optional_list(section, "links", [&target](std::string link) {
        target.link_named(std::move(link));
      });
    }

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
