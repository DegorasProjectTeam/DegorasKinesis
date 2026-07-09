# ----------------------------------------------------------------------------------------------------------------------
# LEOBASE MACROS PROJECTS
# ----------------------------------------------------------------------------------------------------------------------
# Updated 28/01/2025
# ----------------------------------------------------------------------------------------------------------------------

# ----------------------------------------------------------------------------------------------------------------------

# Macro to set up the base project with configurable checks
macro(leobase_macro_setup_base_project compatible_compilers
                               compatible_systems
                               compatible_build_types
                               compatible_sys_archs
                               compatible_proc_archs)

    # Retrieve project name, version, and languages
    set(project_name ${CMAKE_PROJECT_NAME})
    if (DEFINED CMAKE_PROJECT_VERSION)
        set(project_version ${CMAKE_PROJECT_VERSION})
    elseif (DEFINED PROJECT_VERSION)
        set(project_version ${PROJECT_VERSION})
    else()
        set(project_version "undefined")
    endif()

    # Initial log.
    message(STATUS "Preparing project: ${project_name} - ${project_version}")
    message(STATUS "  Initial configurations...")

    # Call the macro to retrieve the project languages
    leobase_macro_get_project_languages(detected_languages)

    # Get the OS name
    if(WIN32)
        set(OS_NAME "Windows")
    elseif(APPLE)
        set(OS_NAME "Apple")
    elseif(UNIX AND NOT APPLE)
        set(OS_NAME "Linux/Unix")
    else()
        set(OS_NAME "Unknown")
    endif()

    # Set the color diagnostics to true.
    set(CMAKE_COLOR_DIAGNOSTICS ON)
    set(CMAKE_VERBOSE_MAKEFILE ON)

    # Set the default prefixes for libraries.
    if(WIN32)
        set(CMAKE_SHARED_LIBRARY_PREFIX "")
        set(CMAKE_STATIC_LIBRARY_PREFIX "lib")
    else()
        set(CMAKE_SHARED_LIBRARY_PREFIX "lib")
        set(CMAKE_STATIC_LIBRARY_PREFIX "lib")
    endif()

    # Set the default debug postfix for all targets.
    set(CMAKE_DEBUG_POSTFIX "_d" CACHE STRING "Add a postfix for debug builds")
    set(CMAKE_RELEASE_POSTFIX "" CACHE STRING "Add a postfix for release builds")

    # Set the postfix.
    if(CMAKE_BUILD_TYPE MATCHES "Release")
        set(CMAKE_BUILD_POSTFIX "${CMAKE_RELEASE_POSTFIX}")
    elseif(CMAKE_BUILD_TYPE MATCHES "Debug")
        set(CMAKE_BUILD_POSTFIX "${CMAKE_DEBUG_POSTFIX}")
    endif()

    # Set the default find package configuration
    set(CMAKE_FIND_PACKAGE_PREFER_CONFIG TRUE)

    # Output directories
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

    # --------------------------------------------------------------------------------------

    # Logs and show the received lists
    message(STATUS "    Initial configurations ok.")
    message(STATUS "  --------------------------------------")
    message(STATUS "  Initial compatibility check...")
    message(STATUS "    Compatible Compilers: ${compatible_compilers}")
    message(STATUS "    Compatible Systems: ${compatible_systems}")
    message(STATUS "    Compatible Build Types: ${compatible_build_types}")
    message(STATUS "    Compatible System Architectures: ${compatible_sys_archs}")
    message(STATUS "    Compatible Processor Architectures: ${compatible_proc_archs}")

    # Retrieve the system architecture
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(SYS_ARCH "64-bit")
    elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
        set(SYS_ARCH "32-bit")
    else()
        message(FATAL_ERROR "    Unknown system architecture size: ${CMAKE_SIZEOF_VOID_P}")
    endif()

    # Check the compiler.
    set(COMPILER_FOUND FALSE)
    foreach(compiler ${compatible_compilers})
        if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "${compiler}")
            set(COMPILER_FOUND TRUE)
            break()
        endif()
    endforeach()
    if(NOT COMPILER_FOUND)
        message(FATAL_ERROR "    Incompatible compiler: ${CMAKE_CXX_COMPILER_ID}")
    endif()

    # Check the operating system.
    set(OS_FOUND FALSE)
    foreach(os ${compatible_systems})
        if("${CMAKE_SYSTEM_NAME}" STREQUAL "${os}")
            set(OS_FOUND TRUE)
            break()
        endif()
    endforeach()
    if(NOT OS_FOUND)
        message(FATAL_ERROR "    Incompatible OS: ${CMAKE_SYSTEM_NAME}")
    endif()

    # Check the build type.
    set(BUILD_TYPE_FOUND FALSE)
    foreach(type ${compatible_build_types})
        if("${CMAKE_BUILD_TYPE}" STREQUAL "${type}")
            set(BUILD_TYPE_FOUND TRUE)
            break()
        endif()
    endforeach()
    if(NOT BUILD_TYPE_FOUND)
        message(FATAL_ERROR "    Incompatible CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
    endif()

    # Check the processor architecture.
    set(PROC_ARCH_FOUND FALSE)
    foreach(arch ${compatible_proc_archs})
        if("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "${arch}")
            set(PROC_ARCH_FOUND TRUE)
            break()
        endif()
    endforeach()
    if(NOT PROC_ARCH_FOUND)
        message(FATAL_ERROR "    Unsupported processor architecture: ${CMAKE_SYSTEM_PROCESSOR}")
    endif()

    # Check the system architecture
    # Compact System Architecture Check
    set(SYS_ARCH_FOUND FALSE)
    foreach(arch ${compatible_sys_archs})
        if("${SYS_ARCH}" STREQUAL "${arch}")
            set(SYS_ARCH_FOUND TRUE)
            break()
        endif()
    endforeach()
    if(NOT SYS_ARCH_FOUND)
        message(FATAL_ERROR "    Unsupported system architecture: ${SYS_ARCH}")
    endif()

    # --------------------------------------------------------------------------------------

    # Logs
    message(STATUS "    All compatibility checks have passed.")
    message(STATUS "  --------------------------------------")
    message(STATUS "  Project Name: ${CMAKE_PROJECT_NAME}")
    message(STATUS "  Project Version: ${CMAKE_PROJECT_VERSION}")
    message(STATUS "  Project Languages: ${detected_languages}")
    message(STATUS "  C++ Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
    message(STATUS "  C++ Compiler Path: ${CMAKE_CXX_COMPILER}")
    message(STATUS "  Make Program: ${CMAKE_MAKE_PROGRAM}")
    message(STATUS "  Toolchain File: ${CMAKE_TOOLCHAIN_FILE}")
    message(STATUS "  Operating System: ${OS_NAME}")
    message(STATUS "  Processor Architecture: ${CMAKE_SYSTEM_PROCESSOR}")
    message(STATUS "  System Architecture: ${SYS_ARCH}")
    message(STATUS "  Build Type: ${CMAKE_BUILD_TYPE}")
    message(STATUS "  Base build Archive: ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}")
    message(STATUS "  Base build Library: ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")
    message(STATUS "  Base build Runtime: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")

    # --------------------------------------------------------------------------------------

    # Initial configuration of the flags.
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wpedantic -Wall -Wextra -O0")
    else()
        set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wpedantic -Wall -Wextra -O3")
    endif()

    # In MinGW, enable static linking for libgcc, libstdc++, and pthread.
    if(MINGW)
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++ -static")
    endif()

    # --------------------------------------------------------------------------------------

ENDMACRO()

# ----------------------------------------------------------------------------------------------------------------------

# Macro to detect enabled project languages and return the list
macro(leobase_macro_get_project_languages output_list)

    # Initialize the languages list
    set(_detected_languages "")

    # Check for common compilers and append detected languages
    if(CMAKE_C_COMPILER)
        list(APPEND _detected_languages "C")
    endif()

    if(CMAKE_CXX_COMPILER)
        list(APPEND _detected_languages "CXX")
    endif()

    if(CMAKE_Fortran_COMPILER)
        list(APPEND _detected_languages "Fortran")
    endif()

    if(CMAKE_OBJC_COMPILER)
        list(APPEND _detected_languages "Objective-C")
    endif()

    if(CMAKE_OBJCXX_COMPILER)
        list(APPEND _detected_languages "Objective-C++")
    endif()

    if(CMAKE_SWIFT_COMPILER)
        list(APPEND _detected_languages "Swift")
    endif()

    if(CMAKE_ISPC_COMPILER)
        list(APPEND _detected_languages "ISPC")
    endif()

    # Fallback if no languages are detected
    if(_detected_languages STREQUAL "")
        set(_detected_languages "None")
    endif()

    # Check if called from top-level scope
    set(${output_list} "${_detected_languages}" CACHE INTERNAL "Detected project languages")

ENDMACRO()

# ----------------------------------------------------------------------------------------------------------------------

# Macro to search for packages.
MACRO(leobase_macro_find_package_default package version version_mode extra_search_paths extra_search_patterns)

    # Compose the configuration folder.
    leobase_macro_compose_current_architecture_folder_name(CONFIG_FOLDER)

    # Convert ${package} to lowercase
    string(TOLOWER ${package} PACKAGE_LOWERCASE)

    # List of CMake configuration file patterns.
    set(CONFIG_FILE_PATTERNS
        "${package}Config.cmake"
        "${package}-config.cmake"
        "${PACKAGE_LOWERCASE}Config.cmake"
        "${PACKAGE_LOWERCASE}-config.cmake"
    )

    # Log.
    message(STATUS "Searching for package configuration file: ${package}")
    message(STATUS "  Determined architecture folder: ${CONFIG_FOLDER}")

    # Installation folder pattern.
    set(SEARCH_PATTERNS
        ${extra_search_patterns}
        "${CONFIG_FOLDER}/cmake/"
        "${CONFIG_FOLDER}/share/"
        "${CONFIG_FOLDER}/share/cmake/"
        "*product*/${CONFIG_FOLDER}"
        "*product*/${CONFIG_FOLDER}/share/"
        "*product*/${CONFIG_FOLDER}/share/cmake"
        )

    # List of possible relative paths where package might be installed.
    if(WIN32)
        set(SEARCH_PATHS
            ${extra_search_paths}
            "external/*${package}*"                 # In the same project.
            "../*${package}*"                       # As subproject installation
            "../../*${package}*"                    # As subproject installation
            "C:/*${package}*"                       # In standard root folder.
            "C:/Program Files/*${package}*"         # Standard installation.
            "C:/Program Files (x86)/*${package}*"   # Standard installation.
        )
    else()
        set(SEARCH_PATHS
            ${extra_search_paths}
            "../${package}"              # As subproject installation
            "../../${package}"           # As subproject installation
            "/usr/local/${package}"      # Standard local installation
            "/usr/lib/${package}"        # Standard library path
            "/usr/lib/cmake/${package}"  # Standard library path
            "lib/cmake/${package}"       # Standard library path
            "lib/${package}"             # Standard library path
            "/opt/${package}"            # Optional software installations
            "~/${package}"               # Home directory installation
            )
    endif()

    # List to store all found paths
    set(FOUND_PATHS "")

    # Search for library.
    foreach(PATH IN LISTS SEARCH_PATHS)
        foreach(PATTERN IN LISTS SEARCH_PATTERNS)

            # Get the matchs.
            set(CURRENT_SEARCH "${PATH}/${PATTERN}")
            file(GLOB PATTERN_DIRS "${CURRENT_SEARCH}")

            # Iterate through the list of paths
            foreach(MATCH IN LISTS PATTERN_DIRS)
                get_filename_component(MATCH "${MATCH}" ABSOLUTE)

                # Check if any of the lowercase CMake configuration files exist in the directory.
                 foreach(CONFIG_PATTERN IN LISTS CONFIG_FILE_PATTERNS)
                     message(STATUS "  Checking: ${MATCH}/${CONFIG_PATTERN}")
                     if(EXISTS "${MATCH}/${CONFIG_PATTERN}")
                         list(APPEND FOUND_PATHS "${MATCH}")
                     endif()
                 endforeach()

            endforeach()
        endforeach()
    endforeach()

    # Log all matches
    foreach(MATCH IN LISTS FOUND_PATHS)
        message(STATUS "  Matches: ${MATCH}")
    endforeach()

    # Check if any matches were found
    if(NOT FOUND_PATHS)
        message(FATAL_ERROR "  The package was not found: ${package}")
    else()

        # Check if version is specified and update the find_package command accordingly
        if(version STREQUAL "")
            message(STATUS "  Ignoring version.")
            find_package(${package} REQUIRED PATHS ${FOUND_PATHS} NO_DEFAULT_PATH)
        else()
            message(STATUS "Using version: ${version} with mode: ${version_mode}")
            find_package(${package} ${version} ${version_mode} REQUIRED PATHS ${FOUND_PATHS})
        endif()

        if(NOT ${package}_FOUND)
             message(FATAL_ERROR "  Configuration file for ${package} not found.")
        else()
            # Set the found configuration path.
            set(${package}_CONFIG_PATH ${FOUND_PATHS})
            # Log all found package details
            message(STATUS "  Package '${package}' found.")
            message(STATUS "  Package version: ${${package}_VERSION}")
            message(STATUS "  Config dir: ${${package}_DIR}")
        endif()

    endif()

ENDMACRO()

# ----------------------------------------------------------------------------------------------------------------------
