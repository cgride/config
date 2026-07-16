function(cgride_config_add_test name source)
  add_executable(${name}
    ${source}
  )

  target_link_libraries(${name}
    PRIVATE
      cgride::config
  )

  target_compile_features(${name}
    PRIVATE
      cxx_std_23
  )

  set_target_properties(${name}
    PROPERTIES
      CXX_EXTENSIONS OFF
  )

  add_test(
    NAME ${name}
    COMMAND ${name}
  )
endfunction()

cgride_config_add_test(cgride_config_config_options_test config_options_test.cpp)
cgride_config_add_test(cgride_config_config_source_test config_source_test.cpp)
cgride_config_add_test(cgride_config_config_document_test config_document_test.cpp)
cgride_config_add_test(cgride_config_config_parser_test config_parser_test.cpp)
cgride_config_add_test(cgride_config_config_loader_test config_loader_test.cpp)
cgride_config_add_test(cgride_config_project_reader_test project_reader_test.cpp)
