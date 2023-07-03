cmake_minimum_required(VERSION 3.24)

project(remoduleTests LANGUAGES CXX)

include(../cmake/project-is-top-level.cmake)
include(../cmake/folders.cmake)

# ---- Dependencies ----

if(PROJECT_IS_TOP_LEVEL)
  find_package(remodule REQUIRED)
  enable_testing()
endif()

find_package(Catch2 REQUIRED)
include(Catch)

# ---- Tests ----

add_library(test_module_a SHARED
  source/common.hpp
  source/test_module_a.cpp
)

target_link_libraries(test_module_a PUBLIC remodule::remodule)

add_library(test_module_b SHARED
  source/common.hpp
  source/test_module_b.cpp
)

target_link_libraries(test_module_b PUBLIC remodule::remodule)

add_executable(remodule_test
  source/common.hpp
  source/remodule_test.cpp
)

target_link_libraries(
    remodule_test PRIVATE
    remodule::remodule
    Catch2::Catch2WithMain
)
target_compile_features(remodule_test PRIVATE cxx_std_17)

add_dependencies(remodule_test test_module_a test_module_b)

catch_discover_tests(remodule_test)

# ---- End-of-file commands ----

add_folders(Test)
