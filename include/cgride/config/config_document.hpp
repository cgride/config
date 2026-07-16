/**
 *
 *  @file config_document.hpp
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
#ifndef CGRIDE_CONFIG_CONFIG_DOCUMENT_HPP
#define CGRIDE_CONFIG_CONFIG_DOCUMENT_HPP

#include <cstddef>
#include <filesystem>
#include <optional>
#include <string>
#include <vector>

namespace cgride::config
{
  /**
   * @class ConfigEntry
   * @brief One parsed key/value entry inside a config section.
   */
  class ConfigEntry
  {
  public:
    ConfigEntry() = default;

    ConfigEntry(std::string key, std::string value);

    ConfigEntry(std::string key, std::string value, std::size_t line);

    [[nodiscard]] static ConfigEntry create(
        std::string key,
        std::string value,
        std::size_t line = 0);

    ConfigEntry &key(std::string key);

    ConfigEntry &value(std::string value);

    ConfigEntry &line(std::size_t line) noexcept;

    [[nodiscard]] const std::string &key() const noexcept;

    [[nodiscard]] const std::string &value() const noexcept;

    [[nodiscard]] std::size_t line() const noexcept;

    [[nodiscard]] bool has_line() const noexcept;

    [[nodiscard]] bool valid() const noexcept;

  private:
    std::string key_{};
    std::string value_{};
    std::size_t line_{0};
  };

  /**
   * @class ConfigSection
   * @brief One parsed config section.
   *
   * Section names are stored without brackets. For example, `[project]`
   * becomes `project`, and `[target.app]` becomes `target.app`.
   */
  class ConfigSection
  {
  public:
    ConfigSection() = default;

    explicit ConfigSection(std::string name);

    [[nodiscard]] static ConfigSection named(std::string name);

    ConfigSection &name(std::string name);

    ConfigSection &entry(ConfigEntry entry);

    ConfigSection &set(
        std::string key,
        std::string value,
        std::size_t line = 0);

    ConfigSection &clear() noexcept;

    [[nodiscard]] const std::string &name() const noexcept;

    [[nodiscard]] const std::vector<ConfigEntry> &entries() const noexcept;

    [[nodiscard]] bool empty() const noexcept;

    [[nodiscard]] bool valid() const noexcept;

    [[nodiscard]] bool has(std::string_view key) const noexcept;

    [[nodiscard]] std::optional<std::string> value(
        std::string_view key) const;

    [[nodiscard]] const ConfigEntry *find(
        std::string_view key) const noexcept;

  private:
    [[nodiscard]] ConfigEntry *find_mutable(
        std::string_view key) noexcept;

    std::string name_{};
    std::vector<ConfigEntry> entries_{};
  };

  /**
   * @class ConfigDocument
   * @brief Parsed configuration document.
   *
   * ConfigDocument stores parsed sections and key/value entries. It does not
   * read files, parse raw text, create project targets, or invoke the build
   * engine.
   */
  class ConfigDocument
  {
  public:
    ConfigDocument() = default;

    explicit ConfigDocument(std::filesystem::path path);

    [[nodiscard]] static ConfigDocument create();

    [[nodiscard]] static ConfigDocument from_path(
        std::filesystem::path path);

    ConfigDocument &path(std::filesystem::path path);

    ConfigDocument &clear_path() noexcept;

    ConfigDocument &section(ConfigSection section);

    ConfigDocument &set(
        std::string section,
        std::string key,
        std::string value,
        std::size_t line = 0);

    ConfigDocument &clear() noexcept;

    [[nodiscard]] const std::filesystem::path &path() const noexcept;

    [[nodiscard]] bool has_path() const noexcept;

    [[nodiscard]] const std::vector<ConfigSection> &sections() const noexcept;

    [[nodiscard]] bool empty() const noexcept;

    [[nodiscard]] bool valid() const noexcept;

    [[nodiscard]] bool has_section(std::string_view name) const noexcept;

    [[nodiscard]] bool has(
        std::string_view section,
        std::string_view key) const noexcept;

    [[nodiscard]] std::optional<std::string> value(
        std::string_view section,
        std::string_view key) const;

    [[nodiscard]] const ConfigSection *find_section(
        std::string_view name) const noexcept;

  private:
    [[nodiscard]] ConfigSection *find_section_mutable(
        std::string_view name) noexcept;

    std::filesystem::path path_{};
    std::vector<ConfigSection> sections_{};
  };

} // namespace cgride::config

#endif // CGRIDE_CONFIG_CONFIG_DOCUMENT_HPP
