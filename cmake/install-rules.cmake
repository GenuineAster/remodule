if(PROJECT_IS_TOP_LEVEL)
  set(CMAKE_INSTALL_INCLUDEDIR "./include/remodule" CACHE STRING "")
endif()

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

# find_package(<package>) call for consumers to find this project
set(package remodule)

install(
    DIRECTORY
    "${PROJECT_SOURCE_DIR}/include/"
    "${PROJECT_BINARY_DIR}/export/"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    COMPONENT remodule_Development
)

install(
    TARGETS remodule_remodule
    EXPORT remoduleTargets
    RUNTIME #
    COMPONENT remodule_Runtime
    LIBRARY #
    COMPONENT remodule_Runtime
    NAMELINK_COMPONENT remodule_Development
    ARCHIVE #
    COMPONENT remodule_Development
    INCLUDES #
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)

write_basic_package_version_file(
    "${package}ConfigVersion.cmake"
    COMPATIBILITY SameMajorVersion
)

# Allow package maintainers to freely override the path for the configs
set(
    remodule_INSTALL_CMAKEDIR "${CMAKE_INSTALL_DATADIR}/${package}"
    CACHE PATH "CMake package config location relative to the install prefix"
)
mark_as_advanced(remodule_INSTALL_CMAKEDIR)

install(
    FILES cmake/install-config.cmake
    DESTINATION "${remodule_INSTALL_CMAKEDIR}"
    RENAME "${package}Config.cmake"
    COMPONENT remodule_Development
)

install(
    FILES "${PROJECT_BINARY_DIR}/${package}ConfigVersion.cmake"
    DESTINATION "${remodule_INSTALL_CMAKEDIR}"
    COMPONENT remodule_Development
)

install(
    EXPORT remoduleTargets
    NAMESPACE remodule::
    DESTINATION "${remodule_INSTALL_CMAKEDIR}"
    COMPONENT remodule_Development
)

if(PROJECT_IS_TOP_LEVEL)
  include(CPack)
endif()
