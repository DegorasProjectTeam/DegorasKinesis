# ----------------------------------------------------------------------------------------------------------------------
# LEOBASE MACROS LAUNCHER
# ----------------------------------------------------------------------------------------------------------------------
# Updated 28/01/2025
# ----------------------------------------------------------------------------------------------------------------------

# ----------------------------------------------------------------------------------------------------------------------

# Note: Add all resources as last arguments.
MACRO(leobase_macro_setup_launcher launcher_name
                                   launcher_version
                                   build_path
                                   lib_opt
                                   lib_deb
                                   lib_general
                                   add_to_global)

    # Combine all additional arguments into a single list of resources.
    set(resources ${ARGN})

    # Prepare the dependencies.
    set(FULL_LIB_OPT ${LEOBASE_MODULES_GLOBAL_LIBS_OPTIMIZED} ${lib_opt})
    set(FULL_LIB_DEB ${LEOBASE_MODULES_GLOBAL_LIBS_DEBUG} ${lib_deb})
    set(FULL_LIB_GEN ${LEOBASE_MODULES_GLOBAL_LIBS_GENERAL} ${lib_general})

    # Log.
    message(STATUS "Setup executable: ${launcher_name}, Version: ${launcher_version}")
    message(STATUS "  Global registration: ${add_to_global}")
    message(STATUS "  Build folder: ${build_path}")
    message(STATUS "  Linking optimized libraries: ${FULL_LIB_OPT}")
    message(STATUS "  Linking debug libraries: ${FULL_LIB_DEB}")
    message(STATUS "  Linking general libraries: ${FULL_LIB_GEN}")

    # Add the executable.
    add_executable(${launcher_name} ${resources})

    # Link the external libraries.
    target_link_libraries(${launcher_name} PRIVATE
        optimized "${FULL_LIB_OPT}"
        debug "${FULL_LIB_DEB}"
        general "${FULL_LIB_GEN}")

    # Set target properties.
    set(LAUNCHER_NAME_WITH_VER ${launcher_name}_v${launcher_version})
    set_target_properties(${launcher_name} PROPERTIES VERSION ${launcher_version})
    set_target_properties(${launcher_name} PROPERTIES DEBUG_OUTPUT_NAME "${LAUNCHER_NAME_WITH_VER}${CMAKE_DEBUG_POSTFIX}")
    set_target_properties(${launcher_name} PROPERTIES RELEASE_OUTPUT_NAME "${LAUNCHER_NAME_WITH_VER}${CMAKE_RELEASE_POSTFIX}")
    set_target_properties(${launcher_name} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${build_path}")

    # MinGW-specific static linking
    if(MINGW)
        target_link_options(${launcher_name} PRIVATE -static-libgcc -static-libstdc++ -static)
    endif()

    # Add to launcher list.
    if(${add_to_global})
        message(STATUS "  Adding to launcher dirs: ${build_path}")
        leobase_macro_global_add_launcher_dirs(${build_path})
    else()
        message(STATUS "  Skipping global registration for ${launcher_name}")
    endif()

    # Copy dependencies to the launcher build dir.
    if (LEOBASE_MODULES_GLOBAL_COPY_LIBS_TO_LAUNCHER_BUILD_DIR)

        message(STATUS "  Copy dependencies libs for launcher: ${launcher_name}")
        message(STATUS "    Optimized libraries: ${lib_opt}")
        message(STATUS "    Debug libraries: ${lib_deb}")
        message(STATUS "    General libraries: ${lib_general}")

        # Ensure the target build directory exists
            file(MAKE_DIRECTORY "${build_path}")

        # Copy optimized libraries to the build directory
        foreach(lib ${lib_opt})
            add_custom_command(TARGET ${launcher_name} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    "$<TARGET_FILE:${lib}>"
                    "${build_path}"
                COMMENT "Copying ${lib} DLL to the output directory.")
        endforeach()

        # Copy debug libraries to the build directory
        foreach(lib ${lib_deb})
            add_custom_command(TARGET ${launcher_name} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    "$<TARGET_FILE:${lib}>"
                    "${build_path}"
                COMMENT "Copying ${lib} DLL to the output directory.")
        endforeach()

        # Copy debug libraries to the build directory
        foreach(lib ${lib_general})
            add_custom_command(TARGET ${launcher_name} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    "$<TARGET_FILE:${lib}>"
                    "${build_path}"
                COMMENT "Copying ${lib} DLL to the output directory.")
        endforeach()

    endif()

ENDMACRO()

# ----------------------------------------------------------------------------------------------------------------------

MACRO(leobase_macro_setup_module_basic_apps project_name
                                            module_name
                                            apps_version
                                            apps_path_sources
                                            apps_path_build
                                            apps_path_install
                                            apps_path_project_folder
                                            lib_opt
                                            lib_deb
                                            lib_general
                                            add_to_global)

    # --------------------------------------------------------------------------
    # Local route to not colide with other modules
    # --------------------------------------------------------------------------
    set(_local_path_sources "${apps_path_sources}/${module_name}")
    set(_local_path_build   "${apps_path_build}/${module_name}")
    set(_local_path_install "${apps_path_install}/${module_name}")
    set(_local_folder       "${apps_path_project_folder}/${module_name}")

    # Log.
    message(STATUS "Setup basic apps for project: ${project_name}")
    message(STATUS "  Module: ${module_name}")
    message(STATUS "  Global registration: ${add_to_global}")
    message(STATUS "  Build sources: ${_local_path_build}")

    # List of basic tests.
    file(GLOB_RECURSE EXAMPLE_SOURCES RELATIVE "${_local_path_sources}" "${_local_path_sources}/*.cpp")

    # Loop through the example names and configure each basic example.
    foreach(EXAMPLE_SOURCE_FILE ${EXAMPLE_SOURCES})

        # Get the example name without extension.
        get_filename_component(EXAMPLE_NAME ${EXAMPLE_SOURCE_FILE} NAME_WE)

        # Construct the full source file path using the LOCAL path
        set(SOURCE_FILE_PATH "${_local_path_sources}/${EXAMPLE_SOURCE_FILE}")

        # Simplify the group.
        leobase_macro_create_sources_group("${_local_path_sources}" ${SOURCE_FILE_PATH})

        # Setup the launcher.
        leobase_macro_setup_launcher(
            "${EXAMPLE_NAME}"
            "${apps_version}"
            "${_local_path_build}"
            "${lib_opt}"
            "${lib_deb}"
            "${lib_general}"
            "${add_to_global}"
            "${SOURCE_FILE_PATH}")

        # Set properties using local folder
        set_target_properties(${EXAMPLE_NAME} PROPERTIES FOLDER "${_local_folder}")

        # Install using local install path
        leobase_macro_install_launcher("${EXAMPLE_NAME}"
                                       "${_local_path_install}")

        # Install runtime artifacts.
        leobase_macro_install_runtime_artifacts(${EXAMPLE_NAME}
                                                ${LEOBASE_MODULES_GLOBAL_MAIN_DEP_SET_NAME}
                                                "${_local_path_install}")

        # Install the runtime dependencies.
        leobase_macro_install_runtime_deps("${EXAMPLE_NAME}"
                                           "${LEOBASE_MODULES_GLOBAL_MAIN_DEP_SET_NAME}"
                                           "${CMAKE_BINARY_DIR}/bin"
                                           "${_local_path_install}"
                                           ""
                                           "")

    endforeach()

ENDMACRO()
# ----------------------------------------------------------------------------------------------------------------------
