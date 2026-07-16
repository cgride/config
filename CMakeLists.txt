cmake_minimum_required(VERSION 3.22)

project(
  cgride-config
  VERSION 0.1.0
  DESCRIPTION "Project configuration loading, parsing and project model reading for Cgride."
  HOMEPAGE_URL "https://github.com/cgride/config"
  LANGUAGES CXX
)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

option(CGRIDE_CONFIG_BUILD_TESTS "Build Cgride config tests" ON)
option(CGRIDE_CONFIG_INSTALL "Install Cgride config" ON)

if(NOT TARGET cgride::core)
  find_package(cgride-core CONFIG REQUIRED)
endif()

if(NOT TARGET cgride::project)
  find_package(cgride-project CONFIG REQUIRED)
endif()

add_library(cgride_config
  src/config_options.cpp
  src/config_source.cpp
  src/config_document.cpp
  src/config_parser.cpp
  src/config_loader.cpp
  src/project_reader.cpp
)

add_library(cgride::config ALIAS cgride_config)

target_compile_features(cgride_config
  PUBLIC
    cxx_std_23
)

target_include_directories(cgride_config
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(cgride_config
  PUBLIC
    cgride::core
    cgride::project
)

set_target_properties(cgride_config
  PROPERTIES
    OUTPUT_NAME cgride_config
    EXPORT_NAME config
    CXX_EXTENSIONS OFF
)

if(CGRIDE_CONFIG_BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()

if(CGRIDE_CONFIG_INSTALL)
  install(
    TARGETS cgride_config
    EXPORT cgride-config-targets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  )

  install(
    DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  )

  install(
    EXPORT cgride-config-targets
    NAMESPACE cgride::
    FILE cgride-config-targets.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/cgride-config
  )

  configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/cgride-config-config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/cgride-config-config.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/cgride-config
  )

  write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/cgride-config-config-version.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
  )

  install(
    FILES
      ${CMAKE_CURRENT_BINARY_DIR}/cgride-config-config.cmake
      ${CMAKE_CURRENT_BINARY_DIR}/cgride-config-config-version.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/cgride-config
  )
endif()
