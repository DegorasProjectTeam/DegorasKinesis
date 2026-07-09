# ----------------------------------------------------------------------------------------------------------------------
# LEOBASE MACROS INSTALL
# ----------------------------------------------------------------------------------------------------------------------
# Updated 07/04/2025
# ----------------------------------------------------------------------------------------------------------------------

# ----------------------------------------------------------------------------------------------------------------------

MACRO(leobase_macro_setup_global_install_dir project_name
                                             project_version
                                             default_install_dir)

    # Log
    message(STATUS "Preparing the default installation directory for ${project_name}.")
    message(STATUS "  Project: ${project_name}")
    message(STATUS "  Version: ${project_version}")

    # Default base folder.
    set(BASE_FOLDER "${project_name}_product_v${project_version}")

    # Set the installation path if forced or default initialized.
    if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT OR ${LEOBASE_MODULES_GLOBAL_FORCE_INSTALL_DIR})

        # Prepare the final installation dir.
        set(INSTALL_DIR "${default_install_dir}/${BASE_FOLDER}")

        # Resolve the path
        get_filename_component(REAL_BASE_DIR "${INSTALL_DIR}" REALPATH)

        message(STATUS "  Force the base installation directory: ${REAL_BASE_DIR}")
        set(CMAKE_INSTALL_PREFIX ${REAL_BASE_DIR} CACHE PATH "..." FORCE)

    else()

        set(CMAKE_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX}/${BASE_FOLDER} CACHE PATH "..." FORCE)

    endif()

    # Add properties for clean process.
    set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES ${CMAKE_INSTALL_PREFIX})

    # Compose the folder name
    leobase_macro_compose_current_architecture_folder_name(INSTALL_FOLDER)

    # Define the install directories.
    set(BIN_DIR ${CMAKE_INSTALL_PREFIX}/${INSTALL_FOLDER}/bin)
    set(LIB_DIR ${CMAKE_INSTALL_PREFIX}/${INSTALL_FOLDER}/lib)
    set(SHA_DIR ${CMAKE_INSTALL_PREFIX}/${INSTALL_FOLDER}/share)
    set(INC_DIR ${CMAKE_INSTALL_PREFIX}/${INSTALL_FOLDER}/include)

    # Specific SO configurations.
    if(WIN32)

        # Nothing specific for Windows.

    elseif(UNIX)

        # TODO CHECK

        # # Update RPATH.
        # message(STATUS "  Updating RPATH...")

        #  # Use, i.e. don't skip the full RPATH for the build tree
        # set(CMAKE_SKIP_BUILD_RPATH FALSE)

        # # When building, don't use the install RPATH already (but later on when installing).
        # set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
        # set(CMAKE_INSTALL_RPATH ${BIN_DIR})

        # # Add the automatically determined parts of the RPATH which point to
        # # directories outside the build tree.
        # set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

    else()
        message(FATAL_ERROR "  Operating system not supported by default.")
    endif()

    # Setup the global variables.
    leobase_macro_global_set_install_bin_path(${BIN_DIR})
    leobase_macro_global_set_install_lib_path(${LIB_DIR})
    leobase_macro_global_set_install_share_path(${SHA_DIR})
    leobase_macro_global_set_install_include_path(${INC_DIR})

    # Logs.
    message(STATUS "  Install prefix: ${CMAKE_INSTALL_PREFIX}")
    message(STATUS "  Binaries dir:   ${BIN_DIR}")
    message(STATUS "  Libraries dir:  ${LIB_DIR}")
    message(STATUS "  Shared dir:     ${SHA_DIR}")
    message(STATUS "  Includes dir:   ${INC_DIR}")

ENDMACRO()

# ----------------------------------------------------------------------------------------------------------------------

MACRO(leobase_macro_install_runtime_artifacts target dependency_set install_path)

    if(${LEOBASE_MODULES_GLOBAL_INSTALL_RUNTIME_ARTIFACTS})

        # Normalize the installation path
        file(TO_CMAKE_PATH "${install_path}" install_path)

        # Log.
        message(STATUS "Installing runtime artifacts for: ${target}")
        message(STATUS "  Dependency set: ${dependency_set}")
        message(STATUS "  Installation path: ${install_path}")

        # Installation process into installation dir.
        install(IMPORTED_RUNTIME_ARTIFACTS ${target}
                RUNTIME_DEPENDENCY_SET ${dependency_set}
                DESTINATION ${install_path})

    endif()

ENDMACRO()

# ----------------------------------------------------------------------------------------------------------------------

MACRO(leobase_macro_install_runtime_deps target
                                         dependency_set
                                         ext_deps_dirs
                                         bin_dest
                                         pre_exc_regexes
                                         post_exc_regexes)

    if(${LEOBASE_MODULES_GLOBAL_INSTALL_RUNTIME_DEPS})

        # Log
        message(STATUS "Installing runtime dependencies for: ${target}")
        message(STATUS "  Dependency set: ${dependency_set}")
        message(STATUS "  External dependencies paths: ${ext_deps_dirs}")
        message(STATUS "  Installation path: ${bin_dest}")

        # Initialize exclusion lists from parameters
        set(PRE_EXC ${pre_exc_regexes})
        set(POST_EXC ${post_exc_regexes})

        # Exclude SO dll and libs.
        if(${LEOBASE_MODULES_GLOBAL_EXCLUDE_SO_LIBS})
            if(WIN32)
                message(STATUS "  Excluding Windows SO libraries...")
                list(APPEND POST_EXC ".*system32/.*\\.dll")
            elseif(OS_NAME STREQUAL "Linux/Unix")
                message(STATUS "  Excluding Unix SO libraries...")
                list(APPEND POST_EXC "/lib" "/usr/lib" "/lib64" "/usr/lib64")
            endif()
        endif()

        # Mandatory SO dll exclusion.
        if(WIN32)
            list(APPEND PRE_EXC "api-ms-" "ext-ms-")
        endif()

        # WARNING Only for avoid the QTCREATOR 13 BUG WHEN EXECUTING LAUNCHERS.
        if (WIN32 AND AVOID_QTCREATOR13_DLL_SEARCH_BUG)
            message(STATUS "  Delaying installation due to QTCreator 13 bug.")
        else()
            message(STATUS "  Final complete excluding: ${POST_EXC}")
            # Install runtime dependencies for the set.
            install(RUNTIME_DEPENDENCY_SET ${dependency_set}
                    PRE_EXCLUDE_REGEXES ${PRE_EXC}
                    POST_EXCLUDE_REGEXES ${POST_EXC}
                    DIRECTORIES ${ext_deps_dirs}
                    DESTINATION ${bin_dest})
            endif()
    endif()

ENDMACRO()

# ----------------------------------------------------------------------------------------------------------------------

MACRO(leobase_macro_install_launcher target bin_dest)

    # Log.
    message(STATUS "Installing executable: ${target}")
    message(STATUS "  Deployment destination: ${bin_dest}")

    # Add the deploy dir.
    leobase_macro_global_add_launcher_deploys_dirs(${bin_dest})

    #Deploy binary files
    install(TARGETS ${target}
            RUNTIME DESTINATION ${bin_dest})

ENDMACRO()

# ----------------------------------------------------------------------------------------------------------------------

MACRO(leobase_macro_install_lib lib_name
                                inc_path
                                inc_dest
                                lib_dest
                                bin_dest
                                arch_dest
                                sha_dest)

    # Log information
    message(STATUS "Installing library: ${lib_name}")
    message(STATUS "  Includes path: ${inc_path}")
    message(STATUS "  Includes destination: ${inc_dest}")
    message(STATUS "  Library destination: ${lib_dest}")
    message(STATUS "  Binary destination: ${bin_dest}")
    message(STATUS "  Archive destination: ${arch_dest}")
    message(STATUS "  Shared destination: ${sha_dest}/cmake")

    # Config and version file names
    set(CONFIG_FILE_NAME "${lib_name}Config.cmake")
    set(VERSION_FILE_NAME "${lib_name}ConfigVersion.cmake")

    # Set target properties for include directories
    # target_include_directories(${lib_name} PUBLIC
    #                            $<BUILD_INTERFACE:${RELATIVE_INC_PATH}>
    #                            $<INSTALL_INTERFACE:${inc_dest}>)

    # Install include files
    install(DIRECTORY ${inc_path}
            DESTINATION ${inc_dest}
            PATTERN "*.txt" EXCLUDE)

    # Get the version of the library
    get_target_property(EXTRACTED_VERSION ${lib_name} VERSION)

    # Write the version to the package file
    write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/${VERSION_FILE_NAME}"
        VERSION ${EXTRACTED_VERSION}
        COMPATIBILITY SameMajorVersion)

    # Install the library (and binaries if applicable)
    install(TARGETS ${lib_name}
            EXPORT ${lib_name}Targets
            LIBRARY DESTINATION ${lib_dest}
            ARCHIVE DESTINATION ${arch_dest}
            RUNTIME DESTINATION ${bin_dest}
            INCLUDES DESTINATION ${inc_dest})

    # Export the target
    install(EXPORT ${lib_name}Targets
            FILE ${lib_name}Targets.cmake
            NAMESPACE ${lib_name}::
            DESTINATION ${sha_dest}/cmake)

    # Manually create the Config.cmake file
    file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/${CONFIG_FILE_NAME}"
         "include(\${CMAKE_CURRENT_LIST_DIR}/${lib_name}Targets.cmake)\n")

    # Check if the ".in" template file exists
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${lib_name}Config.cmake.in")

        # Configure the "Config.cmake" file from the template
        configure_file("${CMAKE_CURRENT_SOURCE_DIR}/${lib_name}Config.cmake.in"
                       "${CMAKE_CURRENT_BINARY_DIR}/${lib_name}Config.cmake" @ONLY)

    else()

        # Configuration without template.
        configure_file("${CMAKE_CURRENT_BINARY_DIR}/${lib_name}Config.cmake" @ONLY)

    endif()

    # Install the manually created Config.cmake file
    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${CONFIG_FILE_NAME}"
                  "${CMAKE_CURRENT_BINARY_DIR}/${VERSION_FILE_NAME}"
                  "${CMAKE_CURRENT_BINARY_DIR}/${lib_name}Config.cmake"
            DESTINATION ${sha_dest}/cmake)

ENDMACRO()

# ----------------------------------------------------------------------------------------------------------------------

MACRO(leobase_macro_default_library_installation lib_name)

    # Get include directories dynamically from the target
    get_target_property(LIB_INCLUDES_DIR ${lib_name} INTERFACE_INCLUDE_DIRECTORIES)

    if(NOT LIB_INCLUDES_DIR)
        message(FATAL_ERROR "Could not retrieve include directories for target: ${lib_name}")
    endif()

    # Set the external dependencies dir.
    set(external_deps_search_dirs ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} ${LEOBASE_MODULES_GLOBAL_LIBS_FOLDERS})

    # Default installation process for windows.
    if(WIN32)

        # Install the library.
        leobase_macro_install_lib("${lib_name}"
                          "${LIB_INCLUDES_DIR}"
                          "${LEOBASE_MODULES_GLOBAL_INSTALL_INCLUDE_PATH}"
                          "${LEOBASE_MODULES_GLOBAL_INSTALL_LIB_PATH}"
                          "${LEOBASE_MODULES_GLOBAL_INSTALL_BIN_PATH}"
                          "${LEOBASE_MODULES_GLOBAL_INSTALL_LIB_PATH}"
                          "${LEOBASE_MODULES_GLOBAL_INSTALL_SHARE_PATH}")

        # Install the runtime artifacts.
        leobase_macro_install_runtime_artifacts("${lib_name}"
                                        "${LEOBASE_MODULES_GLOBAL_MAIN_DEP_SET_NAME}"
                                        "${LEOBASE_MODULES_GLOBAL_INSTALL_LIB_PATH}")

        # Install external dependencies.
        leobase_macro_install_runtime_deps("${lib_name}"
                                   "${LEOBASE_MODULES_GLOBAL_MAIN_DEP_SET_NAME}"
                                   "${external_deps_search_dirs}"
                                   "${LEOBASE_MODULES_GLOBAL_INSTALL_LIB_PATH}"
                                   "" "")

    elseif(OS_NAME STREQUAL "Linux/Unix")

        # Install the library.
        leobase_macro_install_lib("${lib_name}"
                          "${LIB_INCLUDES_DIR}"
                          "${LEOBASE_MODULES_GLOBAL_INSTALL_INCLUDE_PATH}"
                          "${LEOBASE_MODULES_GLOBAL_INSTALL_LIB_PATH}"
                          "${LEOBASE_MODULES_GLOBAL_INSTALL_LIB_PATH}"
                          "${LEOBASE_MODULES_GLOBAL_INSTALL_LIB_PATH}"
                          "${LEOBASE_MODULES_GLOBAL_INSTALL_SHARE_PATH}")

        # Install the runtime artifacts.
        leobase_macro_install_runtime_artifacts("${lib_name}"
                                        "${LEOBASE_MODULES_GLOBAL_MAIN_DEP_SET_NAME}"
                                        "${LEOBASE_MODULES_GLOBAL_INSTALL_LIB_PATH}")

        # Install external dependencies.
        leobase_macro_install_runtime_deps("${lib_name}"
                                   "${LEOBASE_MODULES_GLOBAL_MAIN_DEP_SET_NAME}"
                                   "${external_deps_search_dirs}"
                                   "${LEOBASE_MODULES_GLOBAL_INSTALL_LIB_PATH}"
                                   "" "")

    else()
        message(FATAL_ERROR "Operating system not supported by default.")
    endif()

ENDMACRO()

# ----------------------------------------------------------------------------------------------------------------------

MACRO(leobase_macro_library_install_relative lib_name)

# Get include directories dynamically from the target
get_target_property(LIB_INCLUDES_DIR ${lib_name} INTERFACE_INCLUDE_DIRECTORIES)

if(NOT LIB_INCLUDES_DIR)
    message(FATAL_ERROR "Could not retrieve include directories for target: ${lib_name}")
endif()

# Set the install prefix
# set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/../build_cpack/install")

# Log information
message(STATUS "Installing library: ${lib_name}")
message(STATUS "  CMAKE_INSTALL_PREFIX: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "  Includes path: ${LIB_INCLUDES_DIR}")

# Config and version file names
set(CONFIG_FILE_NAME "${lib_name}Config.cmake")
set(VERSION_FILE_NAME "${lib_name}ConfigVersion.cmake")

# Install include files
install(DIRECTORY ${LIB_INCLUDES_DIR}/
        DESTINATION include
        PATTERN "*.txt" EXCLUDE)

# Get the version of the library
get_target_property(EXTRACTED_VERSION ${lib_name} VERSION)

# Write the version to the package version file
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/${VERSION_FILE_NAME}"
    VERSION ${EXTRACTED_VERSION}
    COMPATIBILITY SameMajorVersion)

# Install the library and export targets
install(TARGETS ${lib_name}
        EXPORT ${lib_name}Targets
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        INCLUDES DESTINATION include)

# Export targets to a cmake file
install(EXPORT ${lib_name}Targets
        FILE ${lib_name}Targets.cmake
        NAMESPACE ${lib_name}::
        DESTINATION share/${lib_name})

# Configure the config file
configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/${lib_name}Config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/${CONFIG_FILE_NAME}"
    INSTALL_DESTINATION share/${lib_name}
)

# Install the config and version files
install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/${CONFIG_FILE_NAME}"
        "${CMAKE_CURRENT_BINARY_DIR}/${VERSION_FILE_NAME}"
        DESTINATION share/${lib_name})

# Install extra CMake modules
install(DIRECTORY ${CMAKE_SOURCE_DIR}/CMakeModulesLeoBase
        DESTINATION share/${lib_name}/cmake
        FILES_MATCHING PATTERN "*.cmake")

ENDMACRO()

# ----------------------------------------------------------------------------------------------------------------------

MACRO(leobase_macro_configure_cpack_installation lib_name lib_ver lib_build_type)

set(CPACK_GENERATOR "ZIP") # or "TGZ" for .tar.gz
set(CPACK_PACKAGE_NAME "${lib_name}")
set(CPACK_PACKAGE_VERSION "${lib_ver}")

if(WIN32)
    set(OS_NAME_CPACK "win")
else()
    set(OS_NAME_CPACK "linux")
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    if (MINGW)
        set(COMPILER_ID_CPACK "mingw")
    else()
        set(COMPILER_ID_CPACK "gcc")
    endif()

    set(COMPILER_VER_CPACK ${CMAKE_CXX_COMPILER_VERSION})
endif()

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(ARCH_CPACK "x64")
else()
    set(ARCH_CPACK "x86")
endif()

set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}_v${CPACK_PACKAGE_VERSION}_${OS_NAME_CPACK}_${ARCH_CPACK}_${lib_build_type}_${COMPILER_ID_CPACK}-${COMPILER_VER_CPACK}")

string(TOLOWER "${CPACK_PACKAGE_FILE_NAME}" CPACK_PACKAGE_FILE_NAME)

# set(CPACK_INSTALL_CMAKE_PROJECTS
#     "debug;${CMAKE_PROJECT_NAME};ALL;/"
#     "release;${CMAKE_PROJECT_NAME};ALL;/"
#     )

include(CPack)

ENDMACRO()

# ----------------------------------------------------------------------------------------------------------------------
