# ----------------------------------------------------------------------------------------------------------------------
# LEOBASE MACROS LIBRARIES
# ----------------------------------------------------------------------------------------------------------------------
# Updated 07/04/2025
# ----------------------------------------------------------------------------------------------------------------------

# ----------------------------------------------------------------------------------------------------------------------

# Note: Add all resources as last arguments.
MACRO(leobase_macro_setup_shared_lib lib_name
                                     lib_version
                                     build_path
                                     lib_includes_dir
                                     lib_opt
                                     lib_deb
                                     lib_general
                                     add_to_global)

    # Combine all additional arguments into a single list of resources.
    set(resources ${ARGN})

    # Convert the library name to uppercase for compile definitions.
    string(TOUPPER ${lib_name} LIB_NAME_UPPER)

    # Prepare the dependencies.
    set(FULL_LIB_OPT ${LEOBASE_MODULES_GLOBAL_LIBS_OPTIMIZED} ${lib_opt})
    set(FULL_LIB_DEB ${LEOBASE_MODULES_GLOBAL_LIBS_DEBUG} ${lib_deb})
    set(FULL_LIB_GEN ${LEOBASE_MODULES_GLOBAL_LIBS_GENERAL} ${lib_general})

    # Log.
    message(STATUS "Setup shared library: ${lib_name}, Version: ${lib_version}")
    message(STATUS "  Global registration: ${add_to_global}")
    message(STATUS "  Build folder: ${build_path}")
    message(STATUS "  Linking optimized libraries: ${FULL_LIB_OPT}")
    message(STATUS "  Linking debug libraries: ${FULL_LIB_DEB}")
    message(STATUS "  Linking general libraries: ${FULL_LIB_GEN}")

    # Add the shared library target.
    add_library(${lib_name} SHARED ${resources})

    # Link the external libraries.
    target_link_libraries(${lib_name} PRIVATE
        optimized "${FULL_LIB_OPT}"
        debug "${FULL_LIB_DEB}"
        general "${FULL_LIB_GEN}")

    # Add include directories.
    # TODO BUG CHECK?
    target_include_directories(${lib_name} PUBLIC $<BUILD_INTERFACE:${lib_includes_dir}>)

    # Set properties
    set(LIB_NAME_WITH_VER ${lib_name}_v${lib_version})
    set_target_properties(${lib_name} PROPERTIES VERSION ${lib_version})
    set_target_properties(${lib_name} PROPERTIES DEBUG_OUTPUT_NAME "${LIB_NAME_WITH_VER}")
    set_target_properties(${lib_name} PROPERTIES RELEASE_OUTPUT_NAME "${LIB_NAME_WITH_VER}")
    set_target_properties(${lib_name} PROPERTIES PROJECT_LABEL ${lib_name})
    set_target_properties(${lib_name} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${build_path}")

    # Define compile macro for the library.
    target_compile_definitions(${lib_name} PUBLIC -D${LIB_NAME_UPPER}_LIBRARY)

    # Get the full path of the generated library
    get_target_property(full_lib_folder ${lib_name} RUNTIME_OUTPUT_DIRECTORY)
    set(full_lib_path "${full_lib_folder}/${LIB_NAME_WITH_VER}${CMAKE_BUILD_POSTFIX}${CMAKE_SHARED_LIBRARY_SUFFIX}")
    set_target_properties(${lib_name} PROPERTIES IMPORTED_LOCATION "${full_lib_path}")

    # Debug output to verify the full path
    message(STATUS "  Library full path: ${full_lib_path}")

    # Add to global scope.
    if(${add_to_global})

        # Append the new library to global scope.
        leobase_macro_global_add_libs_auto("${lib_name}")

        # Add the new library dir to global scope.
        leobase_macro_global_add_libs_full_paths("${full_lib_path}")
        leobase_macro_global_add_libs_folders("${full_lib_folder}")

        # Define the global dependency set name.
        set(DEPENDENCY_SET_NAME "${lib_name}_deps")
        leobase_macro_global_set_main_dep_set_name(${DEPENDENCY_SET_NAME})

        # Set the dependency set.
        set(CMAKE_RUNTIME_DEPENDENCY_SET_${dependency_set_name} ${lib_name})
        set_target_properties(${lib_name} PROPERTIES RUNTIME_DEPENDENCY_SET ${DEPENDENCY_SET_NAME})
        get_target_property(dependency_check ${lib_name} RUNTIME_DEPENDENCY_SET)

        # Global logs.
        message(STATUS "  Verifying RUNTIME_DEPENDENCY_SET for ${lib_name}: ${dependency_check}")
        message(STATUS "  Current global libs: ${LEOBASE_MODULES_GLOBAL_LIBS_FULL_PATHS}")
        message(STATUS "  Current global libs folders: ${LEOBASE_MODULES_GLOBAL_LIBS_FOLDERS}")

    else()

        message(STATUS "  Skipping global registration for ${lib_name}")

    endif()

ENDMACRO()

# ----------------------------------------------------------------------------------------------------------------------

MACRO(leobase_macro_setup_static_lib lib_name
                                     lib_version
                                     build_path
                                     lib_includes_dir
                                     lib_opt
                                     lib_deb
                                     lib_general
                                     add_to_global)

    # Combine all additional arguments into a single list of resources.
    set(resources ${ARGN})

    # Convert the library name to uppercase for compile definitions.
    string(TOUPPER ${lib_name} LIB_NAME_UPPER)

    # Prepare the dependencies.
    set(FULL_LIB_OPT ${LEOBASE_MODULES_GLOBAL_LIBS_OPTIMIZED} ${lib_opt})
    set(FULL_LIB_DEB ${LEOBASE_MODULES_GLOBAL_LIBS_DEBUG} ${lib_deb})
    set(FULL_LIB_GEN ${LEOBASE_MODULES_GLOBAL_LIBS_GENERAL} ${lib_general})

    # Log.
    message(STATUS "Setup static library: ${lib_name}, Version: ${lib_version}")
    message(STATUS "  Global registration: ${add_to_global}")
    message(STATUS "  Build folder: ${build_path}")
    message(STATUS "  Linking optimized libraries: ${FULL_LIB_OPT}")
    message(STATUS "  Linking debug libraries: ${FULL_LIB_DEB}")
    message(STATUS "  Linking general libraries: ${FULL_LIB_GEN}")

    # Add the static library target.
    add_library(${lib_name} STATIC ${resources})

    # Link the external libraries.
    target_link_libraries(${lib_name} PRIVATE
        optimized "${FULL_LIB_OPT}"
        debug "${FULL_LIB_DEB}"
        general "${FULL_LIB_GEN}")

    # Add include directories.
    target_include_directories(${lib_name} PUBLIC $<BUILD_INTERFACE:${lib_includes_dir}>)

    # Set properties
    set(LIB_NAME_WITH_VER ${lib_name}_v${lib_version})
    set_target_properties(${lib_name} PROPERTIES VERSION ${lib_version})
    set_target_properties(${lib_name} PROPERTIES DEBUG_OUTPUT_NAME "${LIB_NAME_WITH_VER}")
    set_target_properties(${lib_name} PROPERTIES RELEASE_OUTPUT_NAME "${LIB_NAME_WITH_VER}")
    set_target_properties(${lib_name} PROPERTIES PROJECT_LABEL ${lib_name})
    set_target_properties(${lib_name} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY "${build_path}")

    # Define compile macro for the library.
    target_compile_definitions(${lib_name} PUBLIC -D${LIB_NAME_UPPER}_STATIC)

    # Get the full path of the generated library
    get_target_property(full_lib_folder ${lib_name} ARCHIVE_OUTPUT_DIRECTORY)
    set(full_lib_path "${full_lib_folder}/${LIB_NAME_WITH_VER}${CMAKE_BUILD_POSTFIX}${CMAKE_STATIC_LIBRARY_SUFFIX}")
    set_target_properties(${lib_name} PROPERTIES IMPORTED_LOCATION "${full_lib_path}")

    # Debug output to verify the full path
    message(STATUS "  Library full path: ${full_lib_path}")

    # Add to global scope.
    if(${add_to_global})

        # Append the new library to global scope.
        leobase_macro_global_add_libs_auto("${lib_name}")

        # Add the new library dir to global scope.
        leobase_macro_global_add_libs_full_paths("${full_lib_path}")
        leobase_macro_global_add_libs_folders("${full_lib_folder}")

        # Define the global dependency set name.
        set(DEPENDENCY_SET_NAME "${lib_name}_deps")
        leobase_macro_global_set_main_dep_set_name(${DEPENDENCY_SET_NAME})

        # Set the dependency set.
        set(CMAKE_ARCHIVE_DEPENDENCY_SET_${dependency_set_name} ${lib_name})
        set_target_properties(${lib_name} PROPERTIES ARCHIVE_DEPENDENCY_SET ${DEPENDENCY_SET_NAME})
        get_target_property(dependency_check ${lib_name} ARCHIVE_DEPENDENCY_SET)

        # Global logs.
        message(STATUS "  Verifying ARCHIVE_DEPENDENCY_SET for ${lib_name}: ${dependency_check}")
        message(STATUS "  Current global libs: ${LEOBASE_MODULES_GLOBAL_LIBS_FULL_PATHS}")
        message(STATUS "  Current global libs folders: ${LEOBASE_MODULES_GLOBAL_LIBS_FOLDERS}")

    else()

        message(STATUS "  Skipping global registration for ${lib_name}")

    endif()

ENDMACRO()

# ----------------------------------------------------------------------------------------------------------------------

MACRO(leobase_macro_setup_interface_lib lib_name
                                        lib_version
                                        lib_includes_dir
                                        lib_opt
                                        lib_deb
                                        lib_general
                                        add_to_global)

    # Combine all additional arguments into a single list of resources.
    set(resources ${ARGN})

    # Convert the library name to uppercase for compile definitions.
    string(TOUPPER ${lib_name} LIB_NAME_UPPER)

    # Prepare the dependencies.
    set(FULL_LIB_OPT ${LEOBASE_MODULES_GLOBAL_LIBS_OPTIMIZED} ${lib_opt})
    set(FULL_LIB_DEB ${LEOBASE_MODULES_GLOBAL_LIBS_DEBUG} ${lib_deb})
    set(FULL_LIB_GEN ${LEOBASE_MODULES_GLOBAL_LIBS_GENERAL} ${lib_general})

    # Log.
    message(STATUS "Setup interface library: ${lib_name}, Version: ${lib_version}")
    message(STATUS "  Global registration: ${add_to_global}")
    message(STATUS "  Linking optimized libraries: ${FULL_LIB_OPT}")
    message(STATUS "  Linking debug libraries: ${FULL_LIB_DEB}")
    message(STATUS "  Linking general libraries: ${FULL_LIB_GEN}")

    # Create the interface library.
    add_library(${lib_name} INTERFACE ${resources})

    # Link external dependencies.
    target_link_libraries(${lib_name} INTERFACE
        optimized "${FULL_LIB_OPT}"
        debug "${FULL_LIB_DEB}"
        general "${FULL_LIB_GEN}")

    # Set include directories.
    target_include_directories(${lib_name} INTERFACE $<BUILD_INTERFACE:${lib_includes_dir}>)

    # Set properties for debugging.
    set_target_properties(${lib_name} PROPERTIES PROJECT_LABEL ${lib_name})

    # Define a compile macro for the library.
    target_compile_definitions(${lib_name} INTERFACE -D${LIB_NAME_UPPER}_INTERFACE)

    # Add to global scope.
    if(${add_to_global})

        # Append the new library to global scope.
        leobase_macro_global_add_libs_auto("${lib_name}")

        # Define the global dependency set name.
        set(DEPENDENCY_SET_NAME "${lib_name}_deps")
        leobase_macro_global_set_main_dep_set_name(${DEPENDENCY_SET_NAME})

        # Set the dependency set.
        set(CMAKE_INTERFACE_DEPENDENCY_SET_${DEPENDENCY_SET_NAME} ${lib_name})
        set_target_properties(${lib_name} PROPERTIES INTERFACE_DEPENDENCY_SET ${DEPENDENCY_SET_NAME})

        get_target_property(dependency_check ${lib_name} INTERFACE_DEPENDENCY_SET)
        message(STATUS "  Verifying INTERFACE_DEPENDENCY_SET for ${lib_name}: ${dependency_check}")

        # Global logs.
        message(STATUS "  Current global interface libs: ${LEOBASE_MODULES_GLOBAL_LIBS_GENERAL}")

    else()

        message(STATUS "  Skipping global registration for ${lib_name}")

    endif()

ENDMACRO()

# ----------------------------------------------------------------------------------------------------------------------

MACRO(leobase_macro_setup_lib_basic_unit_tests tests_sources_path install_path ignore_paths)

    # Log.
    message(STATUS "Setup library unit tests...")

    # Configure build path.
    set(APP_BUILD_FOLDER ${CMAKE_BINARY_DIR}/bin/Testing/)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${APP_BUILD_FOLDER})

    # Initialize an empty list for the final test sources.
    set(FILTERED_TESTS_SOURCES)

    # List of basic tests.
    file(GLOB_RECURSE TESTS_SOURCES RELATIVE "${tests_sources_path}" "${tests_sources_path}/*.cpp")

    # For simple test we will avoit the include the external resources.
    set(EXTERN)

    # Filter out ignored paths.
    foreach(SOURCE_PATH ${TESTS_SOURCES})

        message(STATUS "  Checking source: ${SOURCE_PATH}")

        set(IGNORE_FILE FALSE)
        foreach(IGNORE_PATH ${ignore_paths})
            if("${SOURCE_PATH}" MATCHES "${IGNORE_PATH}")
                set(IGNORE_FILE TRUE)
                message(STATUS "  Ignore path: ${SOURCE_PATH}")
            endif()
        endforeach()
        if(NOT IGNORE_FILE)
            list(APPEND FILTERED_TESTS_SOURCES ${SOURCE_PATH})
        endif()

    endforeach()

    # Prepare the external library search folders.
    set(ext_libs_loc
        ${LEOBASE_MODULES_GLOBAL_INSTALL_LIB_PATH}
        ${LEOBASE_MODULES_GLOBAL_INSTALL_BIN_PATH}
        ${CMAKE_BINARY_DIR}/bin)

    # Loop through the test names and configure each basic test.
    foreach(TESTS_SOURCE_FILE ${FILTERED_TESTS_SOURCES})

        # Get the test name and source.
        get_filename_component(TEST_NAME ${TESTS_SOURCE_FILE} NAME_WE)
        set(SOURCES ${tests_sources_path}/${TESTS_SOURCE_FILE})

        # Setup the launcher.
        leobase_macro_setup_launcher("${TEST_NAME}"
                             "${LEOBASE_MODULES_GLOBAL_LIBS_OPTIMIZED}"
                             "${LEOBASE_MODULES_GLOBAL_LIBS_DEBUG}"
                             "${SOURCES}")

        # Install the launcher.
        leobase_macro_install_launcher(${TEST_NAME} ${install_path})

        # Install runtime artifacts.
        leobase_macro_install_runtime_artifacts(${TEST_NAME}
                                        ${LEOBASE_MODULES_GLOBAL_MAIN_DEP_SET_NAME}
                                        ${install_path})

        # Install the runtime dependencies.
        leobase_macro_install_runtime_deps("${TEST_NAME}"
                                   "${LEOBASE_MODULES_GLOBAL_MAIN_DEP_SET_NAME}"
                                   "${LEOBASE_MODULES_GLOBAL_LIBS_FOLDERS}"
                                   "${install_path}"
                                   "" "")

    endforeach()

ENDMACRO()

# ----------------------------------------------------------------------------------------------------------------------
