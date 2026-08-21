
# ----------------------------------------------------------------------------------------------------------------------
# INITIALIZATION

# Cmake minimum.
cmake_minimum_required(VERSION 3.21)

# Project name.
project(DegorasKinesis VERSION 0.0.1 LANGUAGES C CXX)

# Set cmake policy.
if(COMMAND cmake_policy)
    cmake_policy(SET CMP0054 NEW)
endif()

# Standard install directories (bin/lib/include/share/...) and our own cmake/ modules (FindThorlabsKinesis).
include(GNUInstallDirs)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# ----------------------------------------------------------------------------------------------------------------------
# OPTIONS

# Linkage: BUILD_SHARED_LIBS is the source of truth (vcpkg/triplets set it). DEGORASKINESIS_BUILD_SHARED stays for
# standalone ergonomics and simply seeds its default from BUILD_SHARED_LIBS when that is defined.
if(DEFINED BUILD_SHARED_LIBS)
    set(_lk_default_shared ${BUILD_SHARED_LIBS})
else()
    set(_lk_default_shared ON)
endif()

option(DEGORASKINESIS_BUILD_SHARED   "Build DegorasKinesis as a shared library." ${_lk_default_shared})
option(DEGORASKINESIS_BUILD_TESTING  "Build the assert-based test executables."      ON)
option(DEGORASKINESIS_BUILD_EXAMPLES "Build the example executables."                ON)
option(DEGORASKINESIS_BUILD_DOCS     "Install the documentation (README)."           OFF)
option(DEGORASKINESIS_BUILD_TOOLS    "Build and install the command-line tools."     OFF)
option(DEGORASKINESIS_INSTALL_EXAMPLES "Install the example sources."                OFF)
option(DEGORASKINESIS_INSTALL_THORLABS_RUNTIME
       "Install the vendored Thorlabs runtime DLLs next to the library." ON)

# Install rules default ON only when this is the top-level project (off when embedded via add_subdirectory).
option(DEGORASKINESIS_INSTALL "Generate install and export rules." ${PROJECT_IS_TOP_LEVEL})

# ----------------------------------------------------------------------------------------------------------------------
# BASIC PROJECT CONFIGURATION

# Vendored Thorlabs Kinesis SDK. Used as the default location for the ThorlabsKinesis package below; a consumer or a
# packager can point THORLABS_KINESIS_ROOT at a system install instead.
set(THIRDPARTY_THORLABS ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/Thorlabs)
if(NOT DEFINED THORLABS_KINESIS_ROOT AND NOT DEFINED ENV{THORLABS_KINESIS_ROOT})
    set(THORLABS_KINESIS_ROOT "${THIRDPARTY_THORLABS}" CACHE PATH "Root of the Thorlabs Kinesis SDK.")
endif()

# Output folders (build tree).
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Language standard.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# The shared library and the executables share the DYNAMIC MinGW C++ runtime on purpose: the public API passes
# std::string / std::vector / std::function across the DLL boundary, so a single libstdc++ instance must be used by
# both sides. Consumers must therefore use a compatible MinGW/UCRT64 toolchain (see docs/PACKAGING.md). The runtime
# DLLs are staged next to the build binaries below so they run without the toolchain on PATH.

# ----------------------------------------------------------------------------------------------------------------------
# THIRD-PARTY DEPENDENCY

# Locate the Thorlabs Kinesis SDK (vendored by default) -> imported target Thorlabs::Kinesis. Kept PRIVATE: only the
# Layer-2 adapter translation units see it, and it never leaks into the public compile interface.
find_package(ThorlabsKinesis REQUIRED)

# ----------------------------------------------------------------------------------------------------------------------
# STAGE RUNTIME DLLS NEXT TO THE BUILD BINARIES (developer convenience; independent of install)

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
file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/includes/*.h)
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/sources/*.cpp)

# Module aggregator headers (extensionless) that re-export a whole layer, e.g. <DegorasKinesis/Modules/Devices>.
file(GLOB MODULES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/includes/DegorasKinesis/Modules/*)
set_source_files_properties(${MODULES} PROPERTIES HEADER_FILE_ONLY ON)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/includes PREFIX "Headers" FILES ${HEADERS})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/sources  PREFIX "Sources" FILES ${SOURCES})
source_group("Modules" FILES ${MODULES})

# Shared or static, controlled by the option (seeded from BUILD_SHARED_LIBS).
if(DEGORASKINESIS_BUILD_SHARED)
    add_library(DegorasKinesis SHARED ${HEADERS} ${SOURCES} ${MODULES})
else()
    add_library(DegorasKinesis STATIC ${HEADERS} ${SOURCES} ${MODULES})
    target_compile_definitions(DegorasKinesis PUBLIC DEGORASKINESIS_STATIC)
endif()

# Namespaced alias so in-tree consumers (examples/tests) and add_subdirectory users link the same name as installed
# consumers (find_package(DegorasKinesis) -> Degoras::Kinesis).
add_library(Degoras::Kinesis ALIAS DegorasKinesis)

set_target_properties(DegorasKinesis PROPERTIES
    PREFIX ""                       # output DegorasKinesis.{dll,lib,a}
    EXPORT_NAME Kinesis             # exported as Degoras::Kinesis
    POSITION_INDEPENDENT_CODE ON)

target_compile_definitions(DegorasKinesis PRIVATE DEGORASKINESIS_LIBRARY)

# Warnings / optimisation as PRIVATE build flags (never global CMAKE_CXX_FLAGS; multi-config safe; GCC/Clang only).
target_compile_options(DegorasKinesis PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wall>
    $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wextra>
    $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wpedantic>
    $<$<AND:$<CONFIG:Debug>,$<CXX_COMPILER_ID:GNU,Clang,AppleClang>>:-O0>
    $<$<AND:$<CONFIG:Release>,$<CXX_COMPILER_ID:GNU,Clang,AppleClang>>:-O3>)

# Public include root (build tree vs installed tree). Consumers write #include "DegorasKinesis/...".
target_include_directories(DegorasKinesis
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/includes>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

# The vendored SDK is PRIVATE: its headers/link requirements do not propagate to consumers of a shared build.
target_link_libraries(DegorasKinesis PRIVATE Thorlabs::Kinesis)

# ----------------------------------------------------------------------------------------------------------------------
# SUBDIRECTORIES

if(DEGORASKINESIS_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if(DEGORASKINESIS_BUILD_TESTING)
    include(CTest)
    add_subdirectory(testing)
endif()

# ----------------------------------------------------------------------------------------------------------------------
# INSTALL / EXPORT
#
# One prefix-relative layout (GNUInstallDirs) that serves a standalone `cmake --install` AND a vcpkg port: vcpkg
# re-roots this tree into installed/<triplet>/(debug/) and vcpkg_cmake_config_fixup() relocates the CMake package
# files from lib/cmake/ to share/. See docs/PACKAGING.md.

if(DEGORASKINESIS_INSTALL)

    include(CMakePackageConfigHelpers)

    set(LK_PKG_NAME  DegorasKinesis)
    set(LK_CMAKE_DIR ${CMAKE_INSTALL_LIBDIR}/cmake/${LK_PKG_NAME})

    # Library + export set. Runtime component = what an end user runs; Development = what a downstream builder needs.
    install(TARGETS DegorasKinesis
        EXPORT ${LK_PKG_NAME}Targets
        RUNTIME  DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT Runtime
        LIBRARY  DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Runtime NAMELINK_COMPONENT Development
        ARCHIVE  DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Development
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

    # Public headers, including the extensionless Modules aggregators.
    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/includes/DegorasKinesis
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        COMPONENT Development)

    # Namespaced targets file.
    install(EXPORT ${LK_PKG_NAME}Targets
        FILE ${LK_PKG_NAME}Targets.cmake
        NAMESPACE Degoras::
        DESTINATION ${LK_CMAKE_DIR}
        COMPONENT Development)

    # A STATIC build leaks the PRIVATE Thorlabs dependency into the link interface ($<LINK_ONLY:Thorlabs::Kinesis>),
    # so a static consumer must re-find it; emit find_dependency only then. A shared build seals it into the DLL.
    if(DEGORASKINESIS_BUILD_SHARED)
        set(LK_THORLABS_FIND_DEPENDENCY "")
    else()
        set(LK_THORLABS_FIND_DEPENDENCY "find_dependency(ThorlabsKinesis)")
    endif()

    configure_package_config_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake/${LK_PKG_NAME}Config.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/${LK_PKG_NAME}Config.cmake
        INSTALL_DESTINATION ${LK_CMAKE_DIR})

    write_basic_package_version_file(
        ${CMAKE_CURRENT_BINARY_DIR}/${LK_PKG_NAME}ConfigVersion.cmake
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMinorVersion)

    install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/${LK_PKG_NAME}Config.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/${LK_PKG_NAME}ConfigVersion.cmake
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake/FindThorlabsKinesis.cmake
        DESTINATION ${LK_CMAKE_DIR}
        COMPONENT Development)

    # License -> vcpkg-style copyright (also useful standalone) and a usage snippet.
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/../LICENSE
        DESTINATION ${CMAKE_INSTALL_DATADIR}/${LK_PKG_NAME}
        RENAME copyright
        COMPONENT Runtime)
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/cmake/usage
        DESTINATION ${CMAKE_INSTALL_DATADIR}/${LK_PKG_NAME}
        COMPONENT Runtime)

    # Vendored Thorlabs runtime DLLs (opt-out via DEGORASKINESIS_INSTALL_THORLABS_RUNTIME=OFF).
    if(DEGORASKINESIS_INSTALL_THORLABS_RUNTIME)
        install(DIRECTORY ${THIRDPARTY_THORLABS}/bin/
            DESTINATION ${CMAKE_INSTALL_BINDIR}
            COMPONENT Runtime
            FILES_MATCHING PATTERN "*.dll")
    endif()

    # Optional documentation.
    if(DEGORASKINESIS_BUILD_DOCS)
        install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/../README.md
            DESTINATION ${CMAKE_INSTALL_DOCDIR}
            COMPONENT Documentation)
    endif()

    # Optional example sources (as reference material under the doc dir).
    if(DEGORASKINESIS_INSTALL_EXAMPLES)
        install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/examples/
            DESTINATION ${CMAKE_INSTALL_DOCDIR}/examples
            COMPONENT Examples)
    endif()

endif()

# **********************************************************************************************************************
