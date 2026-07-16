/**
 *
 *  @file config_parser_test.cpp
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

#include <cgride/config/config_parser.hpp>
#include <cgride/core/error.hpp>

int main()
{
  {
    cgride::config::ConfigParser parser;

    assert(parser.strict());

    parser.strict(false);

    assert(!parser.strict());

    parser.strict(true);

    assert(parser.strict());
  }

  {
    cgride::config::ConfigParser parser(false);

    assert(!parser.strict());
  }

  {
    cgride::config::ConfigParser parser;

    auto result = parser.parse_string("");

    assert(!result);
    assert(result.error().code() == cgride::core::ErrorCode::InvalidArgument);
    assert(result.error().message() == "Cannot parse empty config content.");
  }

  {
    cgride::config::ConfigParser parser;

    cgride::config::ConfigSource source;

    auto result = parser.parse(source);

    assert(!result);
    assert(result.error().code() == cgride::core::ErrorCode::InvalidArgument);
    assert(result.error().message() == "Cannot parse an empty config source.");
  }

  {
    const std::string content =
        "[project]\n"
        "name = app\n"
        "\n"
        "[target.app]\n"
        "kind = executable\n"
        "sources = src/main.cpp\n";

    cgride::config::ConfigParser parser;

    auto result = parser.parse_string(content);

    assert(result);

    auto document = result.value();

    assert(document.valid());
    assert(document.sections().size() == 2);

    assert(document.has_section("project"));
    assert(document.has("project", "name"));
    assert(document.value("project", "name").value() == "app");

    assert(document.has_section("target.app"));
    assert(document.value("target.app", "kind").value() == "executable");
    assert(document.value("target.app", "sources").value() == "src/main.cpp");
  }

  {
    const std::string content =
        "# project metadata\n"
        "[project]\n"
        "name = app # inline comment\n"
        "\n"
        "; target comment\n"
        "[target.app]\n"
        "kind = executable ; inline comment\n"
        "sources = src/main.cpp\n";

    auto result = cgride::config::parse_config_string(content);

    assert(result);

    auto document = result.value();

    assert(document.value("project", "name").value() == "app");
    assert(document.value("target.app", "kind").value() == "executable");
    assert(document.value("target.app", "sources").value() == "src/main.cpp");
  }

  {
    const std::string content =
        "[project]\n"
        "name = \"hello world\"\n"
        "description = 'native C++ app'\n"
        "escaped = \"line\\nvalue\"\n";

    auto result = cgride::config::parse_config_string(content);

    assert(result);

    auto document = result.value();

    assert(document.value("project", "name").value() == "hello world");
    assert(document.value("project", "description").value() == "native C++ app");
    assert(document.value("project", "escaped").value() == "line\nvalue");
  }

  {
    const std::string content =
        "[project]\n"
        "name = \"app # not comment\"\n"
        "description = 'text ; not comment'\n";

    auto result = cgride::config::parse_config_string(content);

    assert(result);

    auto document = result.value();

    assert(document.value("project", "name").value() == "app # not comment");
    assert(document.value("project", "description").value() == "text ; not comment");
  }

  {
    const std::string content =
        "[project]\n"
        "name = app\n"
        "name = demo\n";

    auto result = cgride::config::parse_config_string(content);

    assert(result);

    auto document = result.value();

    assert(document.sections().size() == 1);
    assert(document.value("project", "name").value() == "demo");
  }

  {
    const std::string content =
        "name = app\n";

    cgride::config::ConfigParser parser;

    auto result = parser.parse_string(content);

    assert(!result);
    assert(result.error().code() == cgride::core::ErrorCode::InvalidArgument);
    assert(result.error().message() == "Config key/value entry must belong to a section.");
    assert(result.error().detail().has_value());
    assert(result.error().detail().value() == "line 1");
  }

  {
    const std::string content =
        "[project]\n"
        "name app\n";

    cgride::config::ConfigParser parser;

    auto result = parser.parse_string(content);

    assert(!result);
    assert(result.error().code() == cgride::core::ErrorCode::InvalidArgument);
    assert(result.error().message() == "Invalid config line. Expected key = value.");
    assert(result.error().detail().value() == "line 2");
  }

  {
    const std::string content =
        "[project name]\n"
        "name = app\n";

    cgride::config::ConfigParser parser;

    auto result = parser.parse_string(content);

    assert(!result);
    assert(result.error().code() == cgride::core::ErrorCode::InvalidArgument);
    assert(result.error().message() == "Invalid config section name.");
    assert(result.error().detail().value() == "line 1");
  }

  {
    const std::string content =
        "[project]\n"
        "bad key = app\n";

    cgride::config::ConfigParser parser;

    auto result = parser.parse_string(content);

    assert(!result);
    assert(result.error().code() == cgride::core::ErrorCode::InvalidArgument);
    assert(result.error().message() == "Invalid config key.");
    assert(result.error().detail().value() == "line 2");
  }

  {
    const std::string content =
        "bad line\n"
        "[project]\n"
        "name = app\n"
        "[target.app]\n"
        "kind = executable\n"
        "sources = src/main.cpp\n";

    cgride::config::ConfigParser parser(false);

    auto result = parser.parse_string(content);

    assert(result);

    auto document = result.value();

    assert(document.value("project", "name").value() == "app");
    assert(document.value("target.app", "kind").value() == "executable");
  }

  {
    const std::string content =
        "[bad section]\n"
        "name = ignored\n"
        "[project]\n"
        "name = app\n";

    cgride::config::ConfigParser parser(false);

    auto result = parser.parse_string(content);

    assert(result);

    auto document = result.value();

    assert(!document.has_section("bad section"));
    assert(document.has_section("project"));
    assert(document.value("project", "name").value() == "app");
  }

  {
    const std::string content =
        "[project]\n"
        "bad key = ignored\n"
        "name = app\n";

    cgride::config::ConfigParser parser(false);

    auto result = parser.parse_string(content);

    assert(result);

    auto document = result.value();

    assert(!document.has("project", "bad key"));
    assert(document.value("project", "name").value() == "app");
  }

  {
    const std::string content =
        "# only comments\n"
        "; no sections\n";

    cgride::config::ConfigParser parser;

    auto result = parser.parse_string(content);

    assert(!result);
    assert(result.error().code() == cgride::core::ErrorCode::InvalidArgument);
    assert(result.error().message() == "Config document does not contain any section.");
  }

  {
    const std::string content =
        "[project]\n"
        "name = app\n";

    auto source = cgride::config::ConfigSource::from_string(
        std::filesystem::path("cgride.config"),
        content);

    auto result = cgride::config::parse_config(source);

    assert(result);

    auto document = result.value();

    assert(document.has_path());
    assert(document.path() == std::filesystem::path("cgride.config"));
    assert(document.value("project", "name").value() == "app");
  }

  {
    const std::string content =
        "[project]\n"
        "name = app\n"
        "\n"
        "[target.lib_core]\n"
        "kind = static_library\n"
        "sources = src/core.cpp, src/path.cpp\n";

    cgride::config::ConfigParser parser;

    auto result = parser.parse_string(content);

    assert(result);

    auto document = result.value();

    assert(document.has_section("target.lib_core"));
    assert(document.value("target.lib_core", "kind").value() == "static_library");
    assert(document.value("target.lib_core", "sources").value() == "src/core.cpp, src/path.cpp");
  }

  return 0;
}
