include(cmake/folders.cmake)

option(REMODULE_BUILD_MCSS_DOCS "Build documentation using Doxygen and m.css" OFF)
if(REMODULE_BUILD_MCSS_DOCS)
  include(cmake/docs.cmake)
endif()

option(REMODULE_ENABLE_COVERAGE "Enable coverage support separate from CTest's" OFF)
if(REMODULE_ENABLE_COVERAGE)
  include(cmake/coverage.cmake)
endif()

if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Windows")
  include(cmake/open-cpp-coverage.cmake OPTIONAL)
endif()

include(cmake/lint-targets.cmake)
include(cmake/spell-targets.cmake)

add_folders(Project)
