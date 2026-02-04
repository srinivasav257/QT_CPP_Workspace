option(ENABLE_PCH "Enable Precompiled Headers" ON)

function(enable_pch project_name)
  if(NOT ENABLE_PCH)
    message(STATUS "PCH is disabled skipping for ${project_name}")
    return()
  endif()

  # Parse arguments: enable_pch(MyApp HEADERS <list>)
  cmake_parse_arguments(PCH "" "" "HEADERS" ${ARGN})

  if(NOT PCH_HEADERS)
    message(WARNING "enable_pch called without HEADERS")
    return()
  endif()

  message(STATUS "Enabling PCH for target: ${project_name}")
  target_precompile_headers(${project_name} PRIVATE ${PCH_HEADERS})

endfunction()
