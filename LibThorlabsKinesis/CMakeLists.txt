
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

# In MinGW, statically link libgcc, libstdc++, and pthread into the executables.
if(MINGW)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++ -static")
endif()

# ----------------------------------------------------------------------------------------------------------------------
# COPY THORLABS RUNTIME DLLS TO THE BUILD OUTPUT

execute_process(
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${THIRDPARTY_THORLABS}/bin"
    "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
    RESULT_VARIABLE COPY_RESULT)

# ----------------------------------------------------------------------------------------------------------------------
# SETUP LIBRARY

# Gather headers and sources (organised in layer subfolders: Common / DCServo / Devices).
file(GLOB_RECURSE HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/includes/*.h)
file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/sources/*.cpp)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/includes PREFIX "Headers" FILES ${HEADERS})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/sources  PREFIX "Sources" FILES ${SOURCES})

# Shared or static, controlled by the option.
if(LIBTHORLABSKINESIS_BUILD_SHARED)
    add_library(LibThorlabsKinesis SHARED ${HEADERS} ${SOURCES})
else()
    add_library(LibThorlabsKinesis STATIC ${HEADERS} ${SOURCES})
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
