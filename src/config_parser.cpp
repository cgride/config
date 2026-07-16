/**
 *
 *  @file config_parser.cpp
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
#include <cgride/config/config_parser.hpp>

#include <cctype>
#include <sstream>
#include <string>
#include <utility>

#include <cgride/core/error.hpp>

namespace cgride::config
{
  namespace
  {
    using cgride::core::Error;
    using cgride::core::ErrorCode;

    [[nodiscard]] bool is_space(char character) noexcept
    {
      return std::isspace(static_cast<unsigned char>(character)) != 0;
    }

    [[nodiscard]] std::string trim(std::string_view value)
    {
      std::size_t begin = 0;

      while (begin < value.size() && is_space(value[begin]))
      {
        ++begin;
      }

      std::size_t end = value.size();

      while (end > begin && is_space(value[end - 1]))
      {
        --end;
      }

      return std::string(value.substr(begin, end - begin));
    }

    [[nodiscard]] bool is_identifier_character(char character) noexcept
    {
      return std::isalnum(static_cast<unsigned char>(character)) != 0 ||
             character == '_' ||
             character == '-' ||
             character == '.';
    }

    [[nodiscard]] bool valid_identifier(std::string_view value) noexcept
    {
      if (value.empty())
      {
        return false;
      }

      for (const auto character : value)
      {
        if (!is_identifier_character(character))
        {
          return false;
        }
      }

      return true;
    }

    [[nodiscard]] std::string strip_inline_comment(std::string_view line)
    {
      bool in_single_quote = false;
      bool in_double_quote = false;
      bool escaped = false;

      for (std::size_t index = 0; index < line.size(); ++index)
      {
        const auto character = line[index];

        if (escaped)
        {
          escaped = false;
          continue;
        }

        if (character == '\\')
        {
          escaped = true;
          continue;
        }

        if (character == '\'' && !in_double_quote)
        {
          in_single_quote = !in_single_quote;
          continue;
        }

        if (character == '"' && !in_single_quote)
        {
          in_double_quote = !in_double_quote;
          continue;
        }

        if (!in_single_quote && !in_double_quote)
        {
          if (character == '#' || character == ';')
          {
            return std::string(line.substr(0, index));
          }
        }
      }

      return std::string(line);
    }

    [[nodiscard]] std::string unquote(std::string value)
    {
      if (value.size() < 2)
      {
        return value;
      }

      const auto first = value.front();
      const auto last = value.back();

      if (!((first == '"' && last == '"') || (first == '\'' && last == '\'')))
      {
        return value;
      }

      std::string output;
      output.reserve(value.size() - 2);

      bool escaped = false;

      for (std::size_t index = 1; index + 1 < value.size(); ++index)
      {
        const auto character = value[index];

        if (escaped)
        {
          switch (character)
          {
          case 'n':
            output.push_back('\n');
            break;

          case 't':
            output.push_back('\t');
            break;

          case 'r':
            output.push_back('\r');
            break;

          default:
            output.push_back(character);
            break;
          }

          escaped = false;
          continue;
        }

        if (character == '\\')
        {
          escaped = true;
          continue;
        }

        output.push_back(character);
      }

      if (escaped)
      {
        output.push_back('\\');
      }

      return output;
    }

    [[nodiscard]] bool looks_like_section(std::string_view line) noexcept
    {
      return line.size() >= 2 &&
             line.front() == '[' &&
             line.back() == ']';
    }

    [[nodiscard]] std::string parse_section_name(std::string_view line)
    {
      return trim(line.substr(1, line.size() - 2));
    }

    [[nodiscard]] std::size_t find_assignment(std::string_view line) noexcept
    {
      bool in_single_quote = false;
      bool in_double_quote = false;
      bool escaped = false;

      for (std::size_t index = 0; index < line.size(); ++index)
      {
        const auto character = line[index];

        if (escaped)
        {
          escaped = false;
          continue;
        }

        if (character == '\\')
        {
          escaped = true;
          continue;
        }

        if (character == '\'' && !in_double_quote)
        {
          in_single_quote = !in_single_quote;
          continue;
        }

        if (character == '"' && !in_single_quote)
        {
          in_double_quote = !in_double_quote;
          continue;
        }

        if (!in_single_quote && !in_double_quote && character == '=')
        {
          return index;
        }
      }

      return std::string_view::npos;
    }

    [[nodiscard]] Error parse_error(
        std::string message,
        std::size_t line)
    {
      return Error(
          ErrorCode::InvalidArgument,
          std::move(message),
          "line " + std::to_string(line));
    }

  } // namespace

  ConfigParser::ConfigParser(bool strict)
      : strict_(strict)
  {
  }

  ConfigParser &ConfigParser::strict(bool value) noexcept
  {
    strict_ = value;
    return *this;
  }

  bool ConfigParser::strict() const noexcept
  {
    return strict_;
  }

  cgride::core::Result<ConfigDocument> ConfigParser::parse(
      const ConfigSource &source) const
  {
    if (!source.valid())
    {
      return Error(
          ErrorCode::InvalidArgument,
          "Cannot parse an empty config source.");
    }

    auto parsed = parse_string(source.content());

    if (!parsed)
    {
      return parsed.error();
    }

    auto document = std::move(parsed.value());

    if (source.has_path())
    {
      document.path(source.path());
    }

    return document;
  }

  cgride::core::Result<ConfigDocument> ConfigParser::parse_string(
      std::string_view content) const
  {
    if (content.empty())
    {
      return Error(
          ErrorCode::InvalidArgument,
          "Cannot parse empty config content.");
    }

    ConfigDocument document;
    std::string current_section;

    std::istringstream stream{std::string(content)};
    std::string raw_line;
    std::size_t line_number = 0;

    while (std::getline(stream, raw_line))
    {
      ++line_number;

      auto line = trim(strip_inline_comment(raw_line));

      if (line.empty())
      {
        continue;
      }

      if (looks_like_section(line))
      {
        auto section_name = parse_section_name(line);

        if (!valid_identifier(section_name))
        {
          if (strict_)
          {
            return parse_error(
                "Invalid config section name.",
                line_number);
          }

          continue;
        }

        current_section = std::move(section_name);

        if (!document.has_section(current_section))
        {
          document.section(ConfigSection::named(current_section));
        }

        continue;
      }

      const auto assignment = find_assignment(line);

      if (assignment == std::string_view::npos)
      {
        if (strict_)
        {
          return parse_error(
              "Invalid config line. Expected key = value.",
              line_number);
        }

        continue;
      }

      if (current_section.empty())
      {
        if (strict_)
        {
          return parse_error(
              "Config key/value entry must belong to a section.",
              line_number);
        }

        continue;
      }

      auto key = trim(std::string_view(line).substr(0, assignment));
      auto value = trim(std::string_view(line).substr(assignment + 1));

      if (!valid_identifier(key))
      {
        if (strict_)
        {
          return parse_error(
              "Invalid config key.",
              line_number);
        }

        continue;
      }

      document.set(
          current_section,
          std::move(key),
          unquote(std::move(value)),
          line_number);
    }

    if (!document.valid())
    {
      return Error(
          ErrorCode::InvalidArgument,
          "Config document does not contain any section.");
    }

    return document;
  }

  cgride::core::Result<ConfigDocument> parse_config(
      const ConfigSource &source)
  {
    ConfigParser parser;

    return parser.parse(source);
  }

  cgride::core::Result<ConfigDocument> parse_config_string(
      std::string_view content)
  {
    ConfigParser parser;

    return parser.parse_string(content);
  }

} // namespace cgride::config
