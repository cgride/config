# Cgride Config

`cgride-config` provides configuration loading, parsing, and project reading for Cgride.

It reads a small section-based configuration format and turns it into structured config data. The module can also validate the document shape before handing it to `cgride-project`.

The config module does not build, execute commands, discover compilers, manage the cache, or own the CLI.

## Module boundary

This module depends on:

- `cgride::core`
- `cgride::project`

It must not depend on:

- `cgride::graph`
- `cgride::toolchains`
- `cgride::executor`
- `cgride::cache`
- `cgride::engine`
- `cgride::cli`

The CLI can use this module to read a project configuration file, then pass the resulting project model to the engine.

## Config format

The first supported format is a simple section-based format.

Example:

```text
[project]
name = app

[target.app]
kind = executable
sources = src/main.cpp
```

Sections use square brackets.

Supported section examples:

```text
[project]
[target.app]
[target.core]
[target.headers]
```

Entries use `key = value`:

```text
name = app
kind = executable
sources = src/main.cpp
```

Comments are supported:

```text
# This is a comment
; This is also a comment
```

Inline comments are supported outside quotes:

```text
name = app # application name
kind = executable ; target kind
```

Quoted values are supported:

```text
name = "hello world"
description = 'native C++ application'
```

## Supported target kinds

The first reader accepts these target kinds:

```text
executable
static_library
interface_library
```

For `executable` and `static_library`, `sources` is required.

For `interface_library`, `sources` is not required.

## Main components

### ConfigOptions

`ConfigOptions` describes how a configuration file should be loaded.

It includes:

- project root
- config file path
- config format
- allow missing file mode
- strict parsing mode

Default values:

```text
project root: .
config path: cgride.config
format: auto
allow missing: false
strict: true
```

Example:

```cpp
cgride::config::ConfigOptions options;

options
    .project_root(".")
    .config_path("cgride.config")
    .strict(true);
```

### ConfigSource

`ConfigSource` stores raw configuration content before parsing.

It can be created from a string:

```cpp
auto source = cgride::config::ConfigSource::from_string(
    "[project]\n"
    "name = app\n"
);
```

Or loaded from a file:

```cpp
auto source = cgride::config::ConfigSource::from_file("cgride.config");
```

### ConfigDocument

`ConfigDocument` stores parsed sections and key/value entries.

Example access:

```cpp
auto name = document.value("project", "name");

if (name.has_value())
{
  // use name.value()
}
```

### ConfigParser

`ConfigParser` turns a `ConfigSource` or raw string into a `ConfigDocument`.

Example:

```cpp
cgride::config::ConfigParser parser;

auto document = parser.parse_string(
    "[project]\n"
    "name = app\n"
);
```

Strict mode rejects malformed lines. Non-strict mode skips invalid lines when possible.

### ConfigLoader

`ConfigLoader` reads a file from disk and parses it.

Example:

```cpp
cgride::config::ConfigLoader loader;

auto document = loader.load("cgride.config");

if (!document)
{
  return 1;
}
```

### ProjectReader

`ProjectReader` validates a parsed config document and returns a `cgride::project::Project`.

Example:

```cpp
cgride::config::ProjectReader reader;

auto project = reader.read(document.value());

if (!project)
{
  return 1;
}
```

The first version validates the config shape. Full target-to-project mapping will be expanded as the project module API stabilizes.

## Example

```cpp
#include <cgride/config/config_loader.hpp>
#include <cgride/config/project_reader.hpp>

int main()
{
  cgride::config::ConfigOptions options;

  options
      .project_root(".")
      .config_path("cgride.config")
      .strict(true);

  cgride::config::ConfigLoader loader(options);

  auto document = loader.load();

  if (!document)
  {
    return 1;
  }

  cgride::config::ProjectReader reader;

  auto project = reader.read(document.value());

  if (!project)
  {
    return 1;
  }

  return 0;
}
```

## Build

From the module directory, use the Vix workflow:

```bash
vix build
```

For a release build:

```bash
vix build --preset release
```

## Run tests

```bash
vix check --tests
```

Or run the test command directly:

```bash
vix tests
```

## Install

```bash
vix install
```

The install step exposes the `cgride::config` integration target, public headers, and package metadata.

