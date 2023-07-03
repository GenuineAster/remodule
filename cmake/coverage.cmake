# ---- Variables ----

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS_COVERAGE "-Og -g --coverage -fkeep-static-functions")
    set(CMAKE_EXE_LINKER_FLAGS_COVERAGE "--coverage -Wl,-dynamic-list-data")
    set(CMAKE_SHARED_LINKER_FLAGS_COVERAGE "--coverage -Wl,--dynamic-list-data")
    set(CMAKE_MAP_IMPORTED_CONFIG_COVERAGE "Coverage;RelWithDebInfo;Release;Debug;")
else()
    message(FATAL_ERROR "Coverage not supported on compiler '${CMAKE_CXX_COMPILER_ID}'")
endif()

# We use variables separate from what CTest uses, because those have
# customization issues
set(
    COVERAGE_PRETRACE_COMMAND
    lcov -c -i -q
    -o "${PROJECT_BINARY_DIR}/initial_coverage.info"
    -d "${PROJECT_BINARY_DIR}"
    -b "${PROJECT_SOURCE_DIR}"
    --include "${PROJECT_SOURCE_DIR}/source/*"
    --include "${PROJECT_SOURCE_DIR}/include/*"
    --exclude "${PROJECT_SOURCE_DIR}/external/*"
    --exclude "${PROJECT_SOURCE_DIR}/test/*"
    --no-external
    CACHE STRING
    "; separated command to generate initial coverage data for the 'coverage' target"
)

set(
    COVERAGE_TRACE_COMMAND
    lcov -c -q
    -o "${PROJECT_BINARY_DIR}/test_coverage.info"
    -d "${PROJECT_BINARY_DIR}"
    -b "${PROJECT_SOURCE_DIR}"
    --include "${PROJECT_SOURCE_DIR}/source/*"
    --include "${PROJECT_SOURCE_DIR}/include/*"
    --exclude "${PROJECT_SOURCE_DIR}/external/*"
    --exclude "${PROJECT_SOURCE_DIR}/test/*"
    --no-external
    CACHE STRING
    "; separated command to generate a trace for the 'coverage' target"
)

set(
    COVERAGE_RESULT_COMMAND
    lcov
    -q
    -o "${PROJECT_BINARY_DIR}/coverage.info"
    -a "${PROJECT_BINARY_DIR}/initial_coverage.info"
    -a "${PROJECT_BINARY_DIR}/test_coverage.info"
    CACHE STRING
    "; separated command to merge traces for the 'coverage' target"
)

set(
    COVERAGE_HTML_COMMAND
    genhtml --legend -f -q
    "${PROJECT_BINARY_DIR}/coverage.info"
    -p "${PROJECT_SOURCE_DIR}"
    -o "${PROJECT_BINARY_DIR}/coverage_html"
    CACHE STRING
    "; separated command to generate an HTML report for the 'coverage' target"
)

# ---- Coverage target ----

add_custom_target(
    coverage
    COMMAND ${COVERAGE_PRETRACE_COMMAND}
    COMMAND ${COVERAGE_TRACE_COMMAND}
    COMMAND ${COVERAGE_RESULT_COMMAND}
    COMMAND ${COVERAGE_HTML_COMMAND}
    COMMENT "Generating coverage report"
    VERBATIM
)
