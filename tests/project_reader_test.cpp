/**
 *
 *  @file project_reader_test.cpp
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
#include <string>

#include <cgride/config/project_reader.hpp>
#include <cgride/core/error.hpp>

namespace
{
  [[nodiscard]] cgride::config::ConfigDocument make_valid_document()
  {
    cgride::config::ConfigDocument document;

    document
        .set("project", "name", "app", 2)
        .set("target.app", "kind", "executable", 5)
        .set("target.app", "sources", "src/main.cpp", 6);

    return document;
  }

} // namespace

int main()
{
  {
    cgride::config::ProjectReader reader;

    cgride::config::ConfigDocument document;

    auto result = reader.read(document);

    assert(!result);
    assert(result.error().code() == cgride::core::ErrorCode::InvalidArgument);
    assert(result.error().message() == "Cannot read project from an invalid config document.");
  }

  {
    cgride::config::ProjectReader reader;

    cgride::config::ConfigDocument document;

    document
        .set("target.app", "kind", "executable", 3)
        .set("target.app", "sources", "src/main.cpp", 4);

    auto result = reader.read(document);

    assert(!result);
    assert(result.error().code() == cgride::core::ErrorCode::InvalidArgument);
    assert(result.error().message() == "Config document is missing [project] section.");
  }

  {
    cgride::config::ProjectReader reader;

    cgride::config::ConfigDocument document;

    document
        .set("project", "description", "missing name", 2)
        .set("target.app", "kind", "executable", 5)
        .set("target.app", "sources", "src/main.cpp", 6);

    auto result = reader.read(document);

    assert(!result);
    assert(result.error().code() == cgride::core::ErrorCode::InvalidArgument);
    assert(result.error().message() == "Config project section is missing name.");
  }

  {
    cgride::config::ProjectReader reader;

    cgride::config::ConfigDocument document;

    document.set("project", "name", "", 2);

    auto result = reader.read(document);

    assert(!result);
    assert(result.error().code() == cgride::core::ErrorCode::InvalidArgument);
    assert(result.error().message() == "Config project section is missing name.");
  }

  {
    cgride::config::ProjectReader reader;

    cgride::config::ConfigDocument document;

    document.set("project", "name", "app", 2);

    auto result = reader.read(document);

    assert(!result);
    assert(result.error().code() == cgride::core::ErrorCode::InvalidArgument);
    assert(result.error().message() == "Config document does not contain any target section.");
  }

  {
    cgride::config::ProjectReader reader;

    cgride::config::ConfigDocument document;

    document
        .set("project", "name", "app", 2)
        .set("target.app", "sources", "src/main.cpp", 6);

    auto result = reader.read(document);

    assert(!result);
    assert(result.error().code() == cgride::core::ErrorCode::InvalidArgument);
    assert(result.error().message() == "Config target section is missing kind.");
    assert(result.error().detail().has_value());
    assert(result.error().detail().value() == "target.app");
  }

  {
    cgride::config::ProjectReader reader;

    cgride::config::ConfigDocument document;

    document
        .set("project", "name", "app", 2)
        .set("target.app", "kind", "shared_library", 5)
        .set("target.app", "sources", "src/main.cpp", 6);

    auto result = reader.read(document);

    assert(!result);
    assert(result.error().code() == cgride::core::ErrorCode::InvalidArgument);
    assert(result.error().message() == "Unsupported config target kind.");
    assert(result.error().detail().value() == "target.app");
  }

  {
    cgride::config::ProjectReader reader;

    cgride::config::ConfigDocument document;

    document
        .set("project", "name", "app", 2)
        .set("target.app", "kind", "executable", 5);

    auto result = reader.read(document);

    assert(!result);
    assert(result.error().code() == cgride::core::ErrorCode::InvalidArgument);
    assert(result.error().message() == "Config target section is missing sources.");
    assert(result.error().detail().value() == "target.app");
  }

  {
    cgride::config::ProjectReader reader;

    cgride::config::ConfigDocument document;

    document
        .set("project", "name", "core", 2)
        .set("target.core", "kind", "static_library", 5)
        .set("target.core", "sources", "src/core.cpp", 6);

    auto result = reader.read(document);

    assert(result);
  }

  {
    cgride::config::ProjectReader reader;

    cgride::config::ConfigDocument document;

    document
        .set("project", "name", "headers", 2)
        .set("target.headers", "kind", "interface_library", 5);

    auto result = reader.read(document);

    assert(result);
  }

  {
    cgride::config::ProjectReader reader;

    auto document = make_valid_document();

    auto result = reader.read(document);

    assert(result);
  }

  {
    const std::string content =
        "[project]\n"
        "name = app\n"
        "\n"
        "[target.app]\n"
        "kind = executable\n"
        "sources = src/main.cpp\n";

    cgride::config::ProjectReader reader;

    auto result = reader.read_string(content);

    assert(result);
  }

  {
    const std::string content =
        "[project]\n"
        "name = app\n";

    cgride::config::ProjectReader reader;

    auto result = reader.read_string(content);

    assert(!result);
    assert(result.error().code() == cgride::core::ErrorCode::InvalidArgument);
    assert(result.error().message() == "Config document does not contain any target section.");
  }

  {
    cgride::config::ConfigSource source =
        cgride::config::ConfigSource::from_string(
            "[project]\n"
            "name = app\n"
            "\n"
            "[target.app]\n"
            "kind = executable\n"
            "sources = src/main.cpp\n");

    cgride::config::ProjectReader reader;

    auto result = reader.read(source);

    assert(result);
  }

  {
    cgride::config::ConfigSource source;

    cgride::config::ProjectReader reader;

    auto result = reader.read(source);

    assert(!result);
    assert(result.error().code() == cgride::core::ErrorCode::InvalidArgument);
    assert(result.error().message() == "Cannot parse an empty config source.");
  }

  {
    auto document = make_valid_document();

    auto result = cgride::config::read_project(document);

    assert(result);
  }

  {
    cgride::config::ConfigSource source =
        cgride::config::ConfigSource::from_string(
            "[project]\n"
            "name = lib\n"
            "\n"
            "[target.lib]\n"
            "kind = static_library\n"
            "sources = src/lib.cpp\n");

    auto result = cgride::config::read_project(source);

    assert(result);
  }

  {
    const std::string content =
        "[project]\n"
        "name = headers\n"
        "\n"
        "[target.headers]\n"
        "kind = interface_library\n";

    auto result = cgride::config::read_project_string(content);

    assert(result);
  }

  {
    const std::string content =
        "[project]\n"
        "name = broken\n"
        "\n"
        "[target.broken]\n"
        "kind = executable\n";

    auto result = cgride::config::read_project_string(content);

    assert(!result);
    assert(result.error().code() == cgride::core::ErrorCode::InvalidArgument);
    assert(result.error().message() == "Config target section is missing sources.");
  }

  return 0;
}
