cmake_minimum_required(VERSION 3.24)
set(REMODULE_VERSION 0.0.0)

include(cmake/prelude.cmake)

project(
    remodule
    VERSION ${REMODULE_VERSION}
    DESCRIPTION "C++ library for hot-reloadable modules"
    HOMEPAGE_URL "https://alff.tech"
    LANGUAGES CXX
)

include(cmake/project-is-top-level.cmake)
include(cmake/variables.cmake)

add_compile_definitions(_CRT_SECURE_NO_WARNINGS=1)

# ---- build options ----
option(REMODULE_ENABLE_CLANG_TIDY "Enables clang-tidy" OFF)
option(REMODULE_ENABLE_CPPCHECK "Enables static analysis with cppcheck" OFF)
option(REMODULE_ENABLE_SANITIZERS "Enables runtime sanitizers" OFF)
option(REMODULE_ENABLE_TESTS "Enables unit tests" ON)

if (REMODULE_ENABLE_CLANG_TIDY)
  set(CMAKE_CXX_CLANG_TIDY "clang-tidy;--header-filter=${PROJECT_SOURCE_DIR}/source,${PROJECT_SOURCE_DIR}/include")
endif()

if (REMODULE_ENABLE_CPPCHECK)
  set(CMAKE_CXX_CPPCHECK "cppcheck;--inline-suppr")
endif()

if (REMODULE_ENABLE_SANITIZERS)
  set(CMAKE_CXX_FLAGS_SANITIZE "-O2 -g -fsanitize=address,undefined -fno-omit-frame-pointer -fno-common")
  set(CMAKE_MAP_IMPORTED_CONFIG_SANITIZE "Sanitize;RelWithDebInfo;Release;Debug;")
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  set(CMAKE_CXX_FLAGS "-Werror -Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion -Wcast-qual -Wshadow -Wformat=2 -Wundef")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  set(CMAKE_CXX_FLAGS "-Werror -Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion -Wcast-qual -Wshadow -Wformat=2 -Wundef")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  set (CMAKE_CXX_FLAGS "/utf-8 /WX /W4 /wd4251 /permissive- /volatile:iso /Zc:preprocessor /EHsc /Zc:__cplusplus /Zc:externConstexpr /Zc:throwingNew")
endif()

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---- Developer mode ----

if (REMODULE_DEVELOPER_MODE)
  if (NOT PROJECT_IS_TOP_LEVEL)
    message(
        AUTHOR_WARNING
        "Developer mode is intended for developers of remodule"
    )
  endif()
  include(cmake/dev-mode.cmake)
endif()

# ---- REMODULE library ----
add_library(
    remodule_remodule
    include/remodule/loader.hpp
    source/remodule/loader.cpp
    include/remodule/context_loader.hpp
    include/remodule/library_fwd.hpp
    include/remodule/library.hpp
    source/remodule/library.cpp
    include/remodule/module_interface_fwd.hpp
    include/remodule/module_interface.hpp
    source/remodule/module_interface.cpp
    include/remodule/context_module_interface.hpp
)
add_library(remodule::remodule ALIAS remodule_remodule)

include(GenerateExportHeader)
generate_export_header(
    remodule_remodule
    BASE_NAME remodule
    EXPORT_FILE_NAME export/remodule/remodule_export.hpp
    CUSTOM_CONTENT_FROM_VARIABLE export_header_extra_content
)

if(NOT BUILD_SHARED_LIBS)
  target_compile_definitions(remodule_remodule PUBLIC REMODULE_STATIC_DEFINE)
endif()

set_target_properties(
    remodule_remodule PROPERTIES
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN YES
    VERSION "${PROJECT_VERSION}"
    SOVERSION "${PROJECT_VERSION_MAJOR}"
    EXPORT_NAME remodule
    OUTPUT_NAME remodule
)

target_include_directories(
    remodule_remodule ${warning_guard}
    PUBLIC
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
)

target_include_directories(
    remodule_remodule SYSTEM
    PUBLIC
    "$<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/export>"
)

target_compile_features(remodule_remodule PUBLIC cxx_std_20)

# ---- Tests ----

if (REMODULE_DEVELOPER_MODE)
  include(CTest)
  if (BUILD_TESTING)
    add_subdirectory(test)
  endif()
endif()

# ---- Install rules ----

if(NOT CMAKE_SKIP_INSTALL_RULES)
  include(cmake/install-rules.cmake)
endif()
