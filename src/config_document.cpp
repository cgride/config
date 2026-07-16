/**
 *
 *  @file config_document.cpp
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
#include <cgride/config/config_document.hpp>

#include <utility>

namespace cgride::config
{
  ConfigEntry::ConfigEntry(std::string key, std::string value)
      : key_(std::move(key)),
        value_(std::move(value))
  {
  }

  ConfigEntry::ConfigEntry(
      std::string key,
      std::string value,
      std::size_t line)
      : key_(std::move(key)),
        value_(std::move(value)),
        line_(line)
  {
  }

  ConfigEntry ConfigEntry::create(
      std::string key,
      std::string value,
      std::size_t line)
  {
    return ConfigEntry(
        std::move(key),
        std::move(value),
        line);
  }

  ConfigEntry &ConfigEntry::key(std::string key)
  {
    key_ = std::move(key);
    return *this;
  }

  ConfigEntry &ConfigEntry::value(std::string value)
  {
    value_ = std::move(value);
    return *this;
  }

  ConfigEntry &ConfigEntry::line(std::size_t line) noexcept
  {
    line_ = line;
    return *this;
  }

  const std::string &ConfigEntry::key() const noexcept
  {
    return key_;
  }

  const std::string &ConfigEntry::value() const noexcept
  {
    return value_;
  }

  std::size_t ConfigEntry::line() const noexcept
  {
    return line_;
  }

  bool ConfigEntry::has_line() const noexcept
  {
    return line_ > 0;
  }

  bool ConfigEntry::valid() const noexcept
  {
    return !key_.empty();
  }

  ConfigSection::ConfigSection(std::string name)
      : name_(std::move(name))
  {
  }

  ConfigSection ConfigSection::named(std::string name)
  {
    return ConfigSection(std::move(name));
  }

  ConfigSection &ConfigSection::name(std::string name)
  {
    name_ = std::move(name);
    return *this;
  }

  ConfigSection &ConfigSection::entry(ConfigEntry entry)
  {
    if (!entry.valid())
    {
      return *this;
    }

    if (auto *existing = find_mutable(entry.key()))
    {
      *existing = std::move(entry);
      return *this;
    }

    entries_.push_back(std::move(entry));

    return *this;
  }

  ConfigSection &ConfigSection::set(
      std::string key,
      std::string value,
      std::size_t line)
  {
    return entry(ConfigEntry::create(
        std::move(key),
        std::move(value),
        line));
  }

  ConfigSection &ConfigSection::clear() noexcept
  {
    entries_.clear();
    return *this;
  }

  const std::string &ConfigSection::name() const noexcept
  {
    return name_;
  }

  const std::vector<ConfigEntry> &ConfigSection::entries() const noexcept
  {
    return entries_;
  }

  bool ConfigSection::empty() const noexcept
  {
    return entries_.empty();
  }

  bool ConfigSection::valid() const noexcept
  {
    return !name_.empty();
  }

  bool ConfigSection::has(std::string_view key) const noexcept
  {
    return find(key) != nullptr;
  }

  std::optional<std::string> ConfigSection::value(
      std::string_view key) const
  {
    const auto *entry = find(key);

    if (entry == nullptr)
    {
      return std::nullopt;
    }

    return entry->value();
  }

  const ConfigEntry *ConfigSection::find(
      std::string_view key) const noexcept
  {
    for (const auto &entry : entries_)
    {
      if (entry.key() == key)
      {
        return &entry;
      }
    }

    return nullptr;
  }

  ConfigEntry *ConfigSection::find_mutable(
      std::string_view key) noexcept
  {
    for (auto &entry : entries_)
    {
      if (entry.key() == key)
      {
        return &entry;
      }
    }

    return nullptr;
  }

  ConfigDocument::ConfigDocument(std::filesystem::path path)
      : path_(std::move(path))
  {
  }

  ConfigDocument ConfigDocument::create()
  {
    return ConfigDocument{};
  }

  ConfigDocument ConfigDocument::from_path(
      std::filesystem::path path)
  {
    return ConfigDocument(std::move(path));
  }

  ConfigDocument &ConfigDocument::path(std::filesystem::path path)
  {
    path_ = std::move(path);
    return *this;
  }

  ConfigDocument &ConfigDocument::clear_path() noexcept
  {
    path_.clear();
    return *this;
  }

  ConfigDocument &ConfigDocument::section(ConfigSection section)
  {
    if (!section.valid())
    {
      return *this;
    }

    if (auto *existing = find_section_mutable(section.name()))
    {
      *existing = std::move(section);
      return *this;
    }

    sections_.push_back(std::move(section));

    return *this;
  }

  ConfigDocument &ConfigDocument::set(
      std::string section,
      std::string key,
      std::string value,
      std::size_t line)
  {
    if (section.empty())
    {
      return *this;
    }

    auto *target = find_section_mutable(section);

    if (target == nullptr)
    {
      sections_.push_back(ConfigSection::named(section));
      target = &sections_.back();
    }

    target->set(
        std::move(key),
        std::move(value),
        line);

    return *this;
  }

  ConfigDocument &ConfigDocument::clear() noexcept
  {
    sections_.clear();
    return *this;
  }

  const std::filesystem::path &ConfigDocument::path() const noexcept
  {
    return path_;
  }

  bool ConfigDocument::has_path() const noexcept
  {
    return !path_.empty();
  }

  const std::vector<ConfigSection> &ConfigDocument::sections() const noexcept
  {
    return sections_;
  }

  bool ConfigDocument::empty() const noexcept
  {
    return sections_.empty();
  }

  bool ConfigDocument::valid() const noexcept
  {
    return !sections_.empty();
  }

  bool ConfigDocument::has_section(std::string_view name) const noexcept
  {
    return find_section(name) != nullptr;
  }

  bool ConfigDocument::has(
      std::string_view section,
      std::string_view key) const noexcept
  {
    const auto *target = find_section(section);

    if (target == nullptr)
    {
      return false;
    }

    return target->has(key);
  }

  std::optional<std::string> ConfigDocument::value(
      std::string_view section,
      std::string_view key) const
  {
    const auto *target = find_section(section);

    if (target == nullptr)
    {
      return std::nullopt;
    }

    return target->value(key);
  }

  const ConfigSection *ConfigDocument::find_section(
      std::string_view name) const noexcept
  {
    for (const auto &section : sections_)
    {
      if (section.name() == name)
      {
        return &section;
      }
    }

    return nullptr;
  }

  ConfigSection *ConfigDocument::find_section_mutable(
      std::string_view name) noexcept
  {
    for (auto &section : sections_)
    {
      if (section.name() == name)
      {
        return &section;
      }
    }

    return nullptr;
  }

} // namespace cgride::config
