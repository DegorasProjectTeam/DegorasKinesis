
# ----------------------------------------------------------------------------------------------------------------------
# INITIALIZATION

# Cmake minimum.
cmake_minimum_required(VERSION 3.21)

# Project name.
project(LibThorlabsKinesis VERSION 0.0.1 LANGUAGES C CXX)

# Set cmake policy.
if(COMMAND cmake_policy)
    cmake_policy(SET CMP0054 NEW)
endif()

# ----------------------------------------------------------------------------------------------------------------------
# OPTIONS

option(LIBTHORLABSKINESIS_BUILD_SHARED   "Build LibThorlabsKinesis as a shared library." ON)
option(LIBTHORLABSKINESIS_BUILD_TESTING  "Build the assert-based test executables."      ON)
option(LIBTHORLABSKINESIS_BUILD_EXAMPLES "Build the example executables."                ON)

# ----------------------------------------------------------------------------------------------------------------------
# BASIC PROJECT CONFIGURATION

# Vendored Thorlabs Kinesis SDK.
set(THIRDPARTY_THORLABS ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/Thorlabs)

# Output folders.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Language standard.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Flags per build type.
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -Wpedantic -Wall -Wextra -O0")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -Wpedantic -Wall -Wextra -O3")
endif()

# The shared library and the executables share the DYNAMIC MinGW C++ runtime on purpose: the public API passes
# std::string / std::vector / std::function across the DLL boundary, so a single libstdc++ instance must be used by
# both sides. The runtime DLLs are staged next to the binaries below, so they run without the toolchain on PATH.

# ----------------------------------------------------------------------------------------------------------------------
# STAGE RUNTIME DLLS NEXT TO THE BINARIES

# Vendored Thorlabs Kinesis runtime.
execute_process(
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${THIRDPARTY_THORLABS}/bin"
    "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
    RESULT_VARIABLE COPY_RESULT)

# MinGW C++ runtime (resolved from the active compiler's bin directory) so binaries launch without MSYS2 on PATH.
get_filename_component(MINGW_BIN_DIR "${CMAKE_CXX_COMPILER}" DIRECTORY)
foreach(runtime_dll libstdc++-6.dll libgcc_s_seh-1.dll libwinpthread-1.dll)
    if(EXISTS "${MINGW_BIN_DIR}/${runtime_dll}")
        execute_process(
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${MINGW_BIN_DIR}/${runtime_dll}"
            "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${runtime_dll}")
    endif()
endforeach()

# ----------------------------------------------------------------------------------------------------------------------
# SETUP LIBRARY

# Gather headers and sources (organised in layer subfolders: Common / DCServo / Devices).
file(GLOB_RECURSE HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/includes/*.h)
file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/sources/*.cpp)

# Module aggregator headers (extensionless) that re-export a whole layer, e.g. <LibThorlabsKinesis/Modules/Devices>.
file(GLOB MODULES ${CMAKE_CURRENT_SOURCE_DIR}/includes/LibThorlabsKinesis/Modules/*)
set_source_files_properties(${MODULES} PROPERTIES HEADER_FILE_ONLY ON)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/includes PREFIX "Headers" FILES ${HEADERS})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/sources  PREFIX "Sources" FILES ${SOURCES})
source_group("Modules" FILES ${MODULES})

# Shared or static, controlled by the option.
if(LIBTHORLABSKINESIS_BUILD_SHARED)
    add_library(LibThorlabsKinesis SHARED ${HEADERS} ${SOURCES} ${MODULES})
else()
    add_library(LibThorlabsKinesis STATIC ${HEADERS} ${SOURCES} ${MODULES})
    target_compile_definitions(LibThorlabsKinesis PUBLIC LIBTHORLABSKINESIS_STATIC)
endif()

set_target_properties(LibThorlabsKinesis PROPERTIES PREFIX "")
target_compile_definitions(LibThorlabsKinesis PRIVATE LIBTHORLABSKINESIS_LIBRARY)

# Public include root so consumers write #include "LibThorlabsKinesis/...".
# The vendor SDK header is PRIVATE: only the Layer-2 adapter translation units see it.
target_include_directories(LibThorlabsKinesis
    PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}/includes
    PRIVATE ${THIRDPARTY_THORLABS}/includes)

target_link_libraries(LibThorlabsKinesis PRIVATE
    ${THIRDPARTY_THORLABS}/lib/Thorlabs.MotionControl.Benchtop.DCServo.lib)

# ----------------------------------------------------------------------------------------------------------------------
# SUBDIRECTORIES

if(LIBTHORLABSKINESIS_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if(LIBTHORLABSKINESIS_BUILD_TESTING)
    add_subdirectory(testing)
endif()

# **********************************************************************************************************************
