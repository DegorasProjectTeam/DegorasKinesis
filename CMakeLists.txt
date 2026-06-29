
# ----------------------------------------------------------------------------------------------------------------------
# INITIALIZATION

# Cmake minimum.
cmake_minimum_required(VERSION 3.19)

# Project name.
project(LibThorlabsKinesis VERSION 0.0.1 LANGUAGES C CXX)

# Set cmake policy.
if(COMMAND cmake_policy)
    cmake_policy(SET CMP0054 NEW)
endif()

# ----------------------------------------------------------------------------------------------------------------------
# BASIC PROJECT CONFIGURATION

# Specific configuration.
set(THIRDPARTY_THORLABS ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/Thorlabs)

# Output folder.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Basic configurations.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Initial configuration of the flags.
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -fPIC -Wpedantic -Wall -Wextra -O0")
elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -fPIC -Wpedantic -Wall -Wextra -O3")
endif()

# In MinGW, enable static linking for libgcc, libstdc++, and pthread.
if(MINGW)
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++ -static")
endif()

# ----------------------------------------------------------------------------------------------------------------------
# COPY THORLABS DLL TO BUILD

execute_process(
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${THIRDPARTY_THORLABS}/bin"
    "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
    RESULT_VARIABLE COPY_RESULT
)

# ----------------------------------------------------------------------------------------------------------------------
# SETUP LIBRARY

# Get the headers and sources.
file(GLOB_RECURSE HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/includes/*.h)
file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/sources/*.cpp)

# Create custom groups.
source_group("Headers" FILES ${HEADERS})
source_group("Sources" FILES ${SOURCES})

# Create the shared library.
add_library(LibThorlabsKinesis SHARED ${HEADERS} ${SOURCES})

set_target_properties(LibThorlabsKinesis PROPERTIES PREFIX "")

target_compile_definitions(LibThorlabsKinesis PRIVATE LIBTHORLABSKINESIS_LIBRARY)

# Public so the example/test executables inherit the include path.
target_include_directories(LibThorlabsKinesis PUBLIC
                           ${CMAKE_CURRENT_SOURCE_DIR}/includes)

# The vendor SDK is included ONLY by Layer-2 translation units (PRIVATE), so consumers never see the Thorlabs
# header. The Benchtop.DCServo import library is linked into the shared library; its runtime DLLs are staged above.
target_include_directories(LibThorlabsKinesis PRIVATE
                           ${THIRDPARTY_THORLABS}/includes)

target_link_libraries(LibThorlabsKinesis PRIVATE
   ${THIRDPARTY_THORLABS}/lib/Thorlabs.MotionControl.Benchtop.DCServo.lib)

# ----------------------------------------------------------------------------------------------------------------------
# SETUP APPLICATIONS

# Each application is one self-contained main(): examples and assert()-based, no-hardware self-checks.
set(THORLABS_KINESIS_APPS
    AppErrorMappingTesting
    AppPollerWaitTesting
    AppDCServoPureTesting
    AppDCServoMappingTesting)

foreach(app ${THORLABS_KINESIS_APPS})
    add_executable(${app} ${CMAKE_CURRENT_SOURCE_DIR}/applications/${app}.cpp)
    source_group("Sources" FILES ${CMAKE_CURRENT_SOURCE_DIR}/applications/${app}.cpp)
    target_link_libraries(${app} PRIVATE LibThorlabsKinesis)
endforeach()

# **********************************************************************************************************************
