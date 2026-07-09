# ----------------------------------------------------------------------------------------------------------------------
#    PROJECT LEOBASE INFRAESTRUCTURE LIBRARY
# ----------------------------------------------------------------------------------------------------------------------

# ----------------------------------------------------------------------------------------------------------------------
# Updated 07/04/2025
# ----------------------------------------------------------------------------------------------------------------------

# ----------------------------------------------------------------------------------------------------------------------
# PROJECT MAIN CMAKELIST
# ----------------------------------------------------------------------------------------------------------------------

# ----------------------------------------------------------------------------------------------------------------------
# INITIALIZATION

# Cmake minimum.
cmake_minimum_required(VERSION 3.31)

# Project name.
project(LeoBaseProject VERSION 3.0.0 LANGUAGES C CXX)

# Set cmake policy.
if(COMMAND cmake_policy)
    cmake_policy(SET CMP0054 NEW)
endif()

# Set path to additional CMake modules.
set(CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    ${CMAKE_SOURCE_DIR}/CMakeModulesLeoBase
    ${CMAKE_SOURCE_DIR}/CMakeModulesInternal)

# Init LeoBase CMake modules.
include(LeoBaseMacrosGlobals)
leobase_macro_cmakemodules_init("LeoBase-070425")

# Include internal CMake modules.
include(LeoBaseInternalMacros)
include(LeoBaseIntegrationMacros)

# ----------------------------------------------------------------------------------------------------------------------
# COMPATIBILITY CONFIGURATION

# Define compatible compilers, OS, and build types
set(COMPATIBLE_COMPILERS "GNU")
set(COMPATIBLE_SYSTEMS "Windows")
set(COMPATIBLE_BUILD_TYPES "Debug" "Release")
set(COMPATIBLE_SYS_ARCHITECTURES "32-bit" "64-bit")
set(COMPATIBLE_PROCESSOR_ARCHITECTURES "x86_64" "AMD64")

# ----------------------------------------------------------------------------------------------------------------------
# BASIC PROJECT CONFIGURATION

# Macros global configuration.
leobase_macro_global_set_library_build_type("SHARED")
leobase_macro_global_set_copy_libs_to_launcher_build_dir(FALSE)
leobase_macro_global_set_force_install_dir(TRUE)
leobase_macro_global_set_install_runtime_artifacts(FALSE)
leobase_macro_global_set_install_runtime_deps(FALSE)
leobase_macro_global_set_exclude_so_libraries(TRUE)

# Macros internal configuration.
leobase_internal_macro_set_header_deploy_cleaning(TRUE)

# Setup the base project.
leobase_macro_setup_base_project(
    "${COMPATIBLE_COMPILERS}"
    "${COMPATIBLE_SYSTEMS}"
    "${COMPATIBLE_BUILD_TYPES}"
    "${COMPATIBLE_SYS_ARCHITECTURES}"
    "${COMPATIBLE_PROCESSOR_ARCHITECTURES}")

# External configurations.
set(CMAKE_CXX_STANDARD 17)
set(SHOW_EXTERNAL_DEPS_VARIABLES TRUE)
set(SHOW_EXTERNAL_DEPS_PROPERTIES TRUE)

# ----------------------------------------------------------------------------------------------------------------------
# SPECIFIC COMPILER CONFIGURATION

# Initial configuration of the flags.
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -fPIC -fopenmp")
elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -fPIC -fopenmp")
endif()

# ----------------------------------------------------------------------------------------------------------------------
# CONFIGURE THE SUBPROJECTS

# LeoBase subproject.
add_subdirectory(LeoBase)

# **********************************************************************************************************************
