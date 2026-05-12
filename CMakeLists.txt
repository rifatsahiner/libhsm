# Set CMake minimum version
cmake_minimum_required(VERSION 3.15)

# Set project name
project(hsm VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    if(NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel" FORCE)
    endif()
    add_compile_options(-Wall -Wextra)
endif()
message(STATUS "Current build type: ${CMAKE_BUILD_TYPE}")

# Set target
set(TARGET hsm)

# Define directories
set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")

# Gather source files
set(SOURCES
    ${SRC_DIR}/state_machine.cpp)

# Create library
add_library(${PROJECT_NAME} STATIC ${SOURCES})

# Include directories 
target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${INCLUDE_DIR}>
    $<INSTALL_INTERFACE:include/hsm>)

# INSTALL #
# Install headers
install(DIRECTORY ${INCLUDE_DIR}/
        DESTINATION include/hsm
        FILES_MATCHING PATTERN "*.h")

# Install the library and export targets
install(TARGETS ${PROJECT_NAME}
        EXPORT hsmTargets
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin)

install(EXPORT hsmTargets
        FILE hsmTargets.cmake
        NAMESPACE hsm::
        DESTINATION lib/cmake/hsm)

# Generate and install cmake package config
include(CMakePackageConfigHelpers)

write_basic_package_version_file(
    "${PROJECT_BINARY_DIR}/hsmConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion)

install(FILES
        "${PROJECT_BINARY_DIR}/hsmConfigVersion.cmake"
        DESTINATION lib/cmake/hsm)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/hsmConfig.cmake.in"
    "${PROJECT_BINARY_DIR}/hsmConfig.cmake"
    INSTALL_DESTINATION lib/cmake/hsm)
    
install(FILES
        "${PROJECT_BINARY_DIR}/hsmConfig.cmake"
        DESTINATION lib/cmake/hsm)

# Generate and install pkg-config file
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/pkgconfig/hsm.pc.in"
    "${PROJECT_BINARY_DIR}/hsm.pc"
    @ONLY)

install(FILES "${PROJECT_BINARY_DIR}/hsm.pc"
        DESTINATION lib/pkgconfig)



# Build examples
add_subdirectory(examples)

# add option for building shared library
# add uninstall
# option(hsm_MODULE "Build a module instead of a traditional library." OFF) --> bunu ekle ve node.js tarzı module yap (örneği FMT'den çaldım)
# add tests
# add examples
# cmake with examples (add final, fmt, etc. as modules)
