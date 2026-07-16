/**
 *
 *  @file config_document_test.cpp
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
#include <cassert>
#include <filesystem>
#include <string>
#include <utility>

#include <cgride/config/config_document.hpp>

int main()
{
  {
    cgride::config::ConfigEntry entry;

    assert(entry.key().empty());
    assert(entry.value().empty());
    assert(entry.line() == 0);
    assert(!entry.has_line());
    assert(!entry.valid());
  }

  {
    cgride::config::ConfigEntry entry("name", "app");

    assert(entry.key() == "name");
    assert(entry.value() == "app");
    assert(entry.line() == 0);
    assert(!entry.has_line());
    assert(entry.valid());
  }

  {
    cgride::config::ConfigEntry entry("name", "app", 12);

    assert(entry.key() == "name");
    assert(entry.value() == "app");
    assert(entry.line() == 12);
    assert(entry.has_line());
    assert(entry.valid());
  }

  {
    auto entry = cgride::config::ConfigEntry::create(
        "kind",
        "executable",
        8);

    assert(entry.key() == "kind");
    assert(entry.value() == "executable");
    assert(entry.line() == 8);
    assert(entry.has_line());
    assert(entry.valid());

    entry
        .key("sources")
        .value("src/main.cpp")
        .line(9);

    assert(entry.key() == "sources");
    assert(entry.value() == "src/main.cpp");
    assert(entry.line() == 9);
    assert(entry.valid());
  }

  {
    cgride::config::ConfigSection section;

    assert(section.name().empty());
    assert(section.entries().empty());
    assert(section.empty());
    assert(!section.valid());
    assert(!section.has("name"));
    assert(!section.value("name").has_value());
    assert(section.find("name") == nullptr);
  }

  {
    cgride::config::ConfigSection section("project");

    assert(section.name() == "project");
    assert(section.valid());
    assert(section.empty());

    section.set("name", "app", 2);

    assert(!section.empty());
    assert(section.entries().size() == 1);
    assert(section.has("name"));
    assert(section.value("name").has_value());
    assert(section.value("name").value() == "app");
    assert(section.find("name") != nullptr);
    assert(section.find("name")->line() == 2);
  }

  {
    auto section = cgride::config::ConfigSection::named("target.app");

    section
        .set("kind", "executable", 5)
        .set("sources", "src/main.cpp", 6);

    assert(section.name() == "target.app");
    assert(section.valid());
    assert(section.entries().size() == 2);
    assert(section.value("kind").value() == "executable");
    assert(section.value("sources").value() == "src/main.cpp");
  }

  {
    cgride::config::ConfigSection section("project");

    section.set("name", "old", 1);
    section.set("name", "new", 2);

    assert(section.entries().size() == 1);
    assert(section.value("name").value() == "new");
    assert(section.find("name")->line() == 2);
  }

  {
    cgride::config::ConfigSection section("project");

    section.entry(cgride::config::ConfigEntry{});

    assert(section.entries().empty());

    section.entry(cgride::config::ConfigEntry::create("name", "app", 3));

    assert(section.entries().size() == 1);
    assert(section.value("name").value() == "app");

    section.clear();

    assert(section.entries().empty());
    assert(section.empty());
    assert(section.valid());
  }

  {
    cgride::config::ConfigDocument document;

    assert(document.path().empty());
    assert(!document.has_path());
    assert(document.sections().empty());
    assert(document.empty());
    assert(!document.valid());
    assert(!document.has_section("project"));
    assert(!document.has("project", "name"));
    assert(!document.value("project", "name").has_value());
    assert(document.find_section("project") == nullptr);
  }

  {
    cgride::config::ConfigDocument document("cgride.config");

    assert(document.has_path());
    assert(document.path() == std::filesystem::path("cgride.config"));
    assert(document.empty());
    assert(!document.valid());

    document.clear_path();

    assert(!document.has_path());
    assert(document.path().empty());
  }

  {
    auto document = cgride::config::ConfigDocument::create();

    assert(document.empty());
    assert(!document.valid());

    auto with_path = cgride::config::ConfigDocument::from_path("project.cgride");

    assert(with_path.has_path());
    assert(with_path.path() == std::filesystem::path("project.cgride"));
  }

  {
    cgride::config::ConfigDocument document;

    cgride::config::ConfigSection project("project");
    project.set("name", "app", 2);

    document.section(project);

    assert(!document.empty());
    assert(document.valid());
    assert(document.sections().size() == 1);
    assert(document.has_section("project"));
    assert(document.has("project", "name"));
    assert(document.value("project", "name").value() == "app");
    assert(document.find_section("project") != nullptr);
  }

  {
    cgride::config::ConfigDocument document;

    document
        .set("project", "name", "app", 2)
        .set("target.app", "kind", "executable", 5)
        .set("target.app", "sources", "src/main.cpp", 6);

    assert(document.valid());
    assert(document.sections().size() == 2);

    assert(document.has_section("project"));
    assert(document.has_section("target.app"));

    assert(document.has("project", "name"));
    assert(document.value("project", "name").value() == "app");

    assert(document.has("target.app", "kind"));
    assert(document.has("target.app", "sources"));
    assert(document.value("target.app", "kind").value() == "executable");
    assert(document.value("target.app", "sources").value() == "src/main.cpp");
  }

  {
    cgride::config::ConfigDocument document;

    document.set("", "name", "ignored", 1);

    assert(document.empty());
    assert(!document.valid());
  }

  {
    cgride::config::ConfigDocument document;

    document.set("project", "name", "old", 1);
    document.set("project", "name", "new", 2);

    assert(document.sections().size() == 1);
    assert(document.value("project", "name").value() == "new");

    const auto *section = document.find_section("project");

    assert(section != nullptr);
    assert(section->entries().size() == 1);
    assert(section->find("name")->line() == 2);
  }

  {
    cgride::config::ConfigDocument document;

    cgride::config::ConfigSection project("project");
    project.set("name", "old", 1);

    document.section(project);

    cgride::config::ConfigSection replacement("project");
    replacement.set("name", "new", 2);

    document.section(replacement);

    assert(document.sections().size() == 1);
    assert(document.value("project", "name").value() == "new");
  }

  {
    cgride::config::ConfigDocument document;

    document
        .path("cgride.config")
        .set("project", "name", "app", 2)
        .set("target.app", "kind", "executable", 5);

    assert(document.has_path());
    assert(document.path() == std::filesystem::path("cgride.config"));
    assert(document.valid());

    document.clear();

    assert(document.sections().empty());
    assert(document.empty());
    assert(!document.valid());
    assert(document.has_path());
  }

  return 0;
}
