# ----------------------------------------------------------------------------------------------------------------------
# LEOBASE MACROS GLOBALS
# ----------------------------------------------------------------------------------------------------------------------
# Updated 07/04/2025
# ----------------------------------------------------------------------------------------------------------------------

# ----------------------------------------------------------------------------------------------------------------------

MACRO(leobase_macro_cmakemodules_init version)

    # Set the version.
    set(LEOBASE_MODULES_GLOBAL_CMAKEMODULES_VERSION LeoBase-070425)

    # Check the CMakeModules version.
    if(NOT "${version}" STREQUAL "${LEOBASE_MODULES_GLOBAL_CMAKEMODULES_VERSION}")
        message(FATAL_ERROR "CMakeModules version (${LEOBASE_MODULES_GLOBAL_CMAKEMODULES_VERSION}) not match: ${version}")
    endif()

    # Version log.
    message(STATUS  "LeoBase CMakeModules version <${version}> initialized.")

    # CMake includes.
    include(CMakePackageConfigHelpers)

    # Includes.
    include(LeoBaseMacrosInstall)
    include(LeoBaseMacrosLauncher)
    include(LeoBaseMacrosUtils)
    include(LeoBaseMacrosProjects)
    include(LeoBaseMacrosLibraries)

    # Default global variables values.
    leobase_macro_global_set_main_dep_set_name("")
    leobase_macro_global_set_libs_debug("")
    leobase_macro_global_set_libs_optimized("")
    leobase_macro_global_set_install_bin_path("")
    leobase_macro_global_set_install_lib_path("")
    leobase_macro_global_set_install_share_path("")
    leobase_macro_global_set_install_include_path("")
    leobase_macro_global_set_install_include_path("")
    leobase_macro_global_set_libs_full_paths("")
    leobase_macro_global_set_libs_folders("")
    leobase_macro_global_set_launcher_dirs("")
    leobase_macro_global_set_launcher_deploys_dirs("")
    leobase_macro_global_set_force_install_dir(TRUE)
    leobase_macro_global_set_install_runtime_deps(FALSE)
    leobase_macro_global_set_install_runtime_artifacts(TRUE)
    leobase_macro_global_set_exclude_so_libraries(TRUE)
    leobase_macro_global_set_copy_libs_to_launcher_build_dir(FALSE)
    leobase_macro_global_set_library_build_type("")

ENDMACRO()

# ----------------------------------------------------------------------------------------------------------------------

FUNCTION(leobase_macro_global_set_launcher_deploys_dirs launcher_deploy_dir)

    set(LEOBASE_MODULES_GLOBAL_LAUNCHER_DEPLOYS_DIRS ${launcher_deploy_dir}
        CACHE STRING "Global list of launchers deploys dirs" FORCE)

ENDFUNCTION()

# ----------------------------------------------------------------------------------------------------------------------

FUNCTION(leobase_macro_global_add_launcher_deploys_dirs launcher_deploys_dirs)
    string(REGEX REPLACE "/$" "" launcher_dirs_rep "${launcher_deploys_dirs}")
    # Check if already in the list
    if(NOT "${LEOBASE_MODULES_GLOBAL_LAUNCHER_DEPLOYS_DIRS};" MATCHES "${launcher_dirs_rep};")
        # Conditionally append the semicolon only if the list is not empty
        if(LEOBASE_MODULES_GLOBAL_LAUNCHER_DEPLOYS_DIRS STREQUAL "")
            set(LEOBASE_MODULES_GLOBAL_LAUNCHER_DEPLOYS_DIRS "${launcher_dirs_rep}" CACHE STRING "Global list of launchers deploys dirs" FORCE)
        else()
            set(LEOBASE_MODULES_GLOBAL_LAUNCHER_DEPLOYS_DIRS "${LEOBASE_MODULES_GLOBAL_LAUNCHER_DEPLOYS_DIRS};${launcher_dirs_rep}"
                CACHE STRING "Global list of launchers deploys dirs" FORCE)
        endif()
    endif()
ENDFUNCTION()

# ----------------------------------------------------------------------------------------------------------------------

FUNCTION(leobase_macro_global_add_launcher_dirs launcher_dirs)
    string(REGEX REPLACE "/$" "" launcher_dirs_rep "${launcher_dirs}")
    # Check if already in the list
    if(NOT ";${LEOBASE_MODULES_GLOBAL_LAUNCHER_DIRS};" MATCHES ";${launcher_dirs_rep};")
        # Conditionally append the semicolon only if the list is not empty
        if(LEOBASE_MODULES_GLOBAL_LAUNCHER_DIRS STREQUAL "")
            set(LEOBASE_MODULES_GLOBAL_LAUNCHER_DIRS "${launcher_dirs_rep}"
                CACHE STRING "Global list of launchers" FORCE)
        else()
            set(LEOBASE_MODULES_GLOBAL_LAUNCHER_DIRS "${LEOBASE_MODULES_GLOBAL_LAUNCHER_DIRS};${launcher_dirs_rep}"
                CACHE STRING "Global list of launchers" FORCE)
        endif()
    endif()
ENDFUNCTION()

# ----------------------------------------------------------------------------------------------------------------------

FUNCTION(leobase_macro_global_set_launcher_dirs launcher_dir)

    set(LEOBASE_MODULES_GLOBAL_LAUNCHER_DIRS ${launcher_dir} CACHE STRING "Global list of launchers" FORCE)

ENDFUNCTION()

# ----------------------------------------------------------------------------------------------------------------------

FUNCTION(leobase_macro_global_set_libs_folders libs_folders)

    set(LEOBASE_MODULES_GLOBAL_LIBS_FOLDERS ${libs_folders} CACHE STRING "Global list of folder with libraries" FORCE)

ENDFUNCTION()

# ----------------------------------------------------------------------------------------------------------------------

FUNCTION(leobase_macro_global_set_libs_full_paths libs_dirs)

    set(LEOBASE_MODULES_GLOBAL_LIBS_FULL_PATHS ${libs_dirs} CACHE STRING "Global list of libraries" FORCE)

ENDFUNCTION()

# ----------------------------------------------------------------------------------------------------------------------

FUNCTION(leobase_macro_global_add_libs_auto lib_names)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(LEOBASE_MODULES_GLOBAL_LIBS_DEBUG ${LEOBASE_MODULES_GLOBAL_LIBS_DEBUG} ${lib_names} CACHE STRING "Global debug libraries." FORCE)
else(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(LEOBASE_MODULES_GLOBAL_LIBS_OPTIMIZED ${LEOBASE_MODULES_GLOBAL_LIBS_OPTIMIZED} ${lib_names} CACHE STRING "Global release libraries." FORCE)
endif()

ENDFUNCTION()

# ----------------------------------------------------------------------------------------------------------------------

FUNCTION(leobase_macro_global_add_libs_folders)

    foreach(arg IN LISTS ARGN)
        foreach(path IN LISTS arg)
            if(NOT ";${LEOBASE_MODULES_GLOBAL_LIBS_FOLDERS};" MATCHES ";${path};")
                # Check if the list is empty before appending
                if("${LEOBASE_MODULES_GLOBAL_LIBS_FOLDERS}" STREQUAL "")
                    set(LEOBASE_MODULES_GLOBAL_LIBS_FOLDERS "${path}" CACHE STRING "Global list of folder with libraries" FORCE)
                else()
                    set(LEOBASE_MODULES_GLOBAL_LIBS_FOLDERS "${LEOBASE_MODULES_GLOBAL_LIBS_FOLDERS};${path}" CACHE STRING "Global list of folder with libraries" FORCE)
                endif()
            endif()
        endforeach()
    endforeach()

ENDFUNCTION()

# ----------------------------------------------------------------------------------------------------------------------

FUNCTION(leobase_macro_global_add_libs_full_paths)

    foreach(arg IN LISTS ARGN)
        foreach(path IN LISTS arg)
            if(NOT ";${LEOBASE_MODULES_GLOBAL_LIBS_FULL_PATHS};" MATCHES ";${path};")
                # Check if the list is empty before appending
                if("${LEOBASE_MODULES_GLOBAL_LIBS_FULL_PATHS}" STREQUAL "")
                    set(LEOBASE_MODULES_GLOBAL_LIBS_FULL_PATHS "${path}" CACHE STRING "Global list of libraries" FORCE)
                else()
                    set(LEOBASE_MODULES_GLOBAL_LIBS_FULL_PATHS "${LEOBASE_MODULES_GLOBAL_LIBS_FULL_PATHS};${path}" CACHE STRING "Global list of libraries" FORCE)
                endif()
            endif()
        endforeach()
    endforeach()

ENDFUNCTION()

# ----------------------------------------------------------------------------------------------------------------------

MACRO(leobase_macro_global_set_main_dep_set_name dep_set_name)

    set(LEOBASE_MODULES_GLOBAL_MAIN_DEP_SET_NAME ${dep_set_name} CACHE STRING "" FORCE)

ENDMACRO()

# ----------------------------------------------------------------------------------------------------------------------

MACRO(leobase_macro_global_set_libs_debug lib_names)

    set(LEOBASE_MODULES_GLOBAL_LIBS_DEBUG ${lib_names} CACHE STRING "Global debug libraries." FORCE)

ENDMACRO()

# ----------------------------------------------------------------------------------------------------------------------

MACRO(leobase_macro_global_set_libs_optimized lib_names)

    set(LEOBASE_MODULES_GLOBAL_LIBS_OPTIMIZED ${lib_names} CACHE STRING "Global optmized libraries." FORCE)

ENDMACRO()

# ----------------------------------------------------------------------------------------------------------------------

MACRO(leobase_macro_global_add_libs_debug lib_names)

    set(LEOBASE_MODULES_GLOBAL_LIBS_DEBUG ${LEOBASE_MODULES_GLOBAL_LIBS_DEBUG} ${lib_names} CACHE STRING "" FORCE)

ENDMACRO()

# ----------------------------------------------------------------------------------------------------------------------

MACRO(leobase_macro_global_add_libs_optimized lib_names)

    set(LEOBASE_MODULES_GLOBAL_LIBS_OPTIMIZED ${LEOBASE_MODULES_GLOBAL_LIBS_OPTIMIZED} ${lib_names} CACHE STRING "" FORCE)

ENDMACRO()

# ----------------------------------------------------------------------------------------------------------------------

MACRO(leobase_macro_global_set_install_bin_path path)

    set(LEOBASE_MODULES_GLOBAL_INSTALL_BIN_PATH ${path} CACHE STRING "" FORCE)

ENDMACRO()

# ----------------------------------------------------------------------------------------------------------------------

MACRO(leobase_macro_global_set_install_lib_path path)

    set(LEOBASE_MODULES_GLOBAL_INSTALL_LIB_PATH ${path}  CACHE STRING "" FORCE)

ENDMACRO()

# ----------------------------------------------------------------------------------------------------------------------

MACRO(leobase_macro_global_set_install_share_path path)

    set(LEOBASE_MODULES_GLOBAL_INSTALL_SHARE_PATH ${path} CACHE STRING "" FORCE)

ENDMACRO()

# ----------------------------------------------------------------------------------------------------------------------

MACRO(leobase_macro_global_set_install_include_path path)

    set(LEOBASE_MODULES_GLOBAL_INSTALL_INCLUDE_PATH ${path} CACHE STRING "" FORCE)

ENDMACRO()

# ----------------------------------------------------------------------------------------------------------------------

MACRO(leobase_macro_global_set_copy_libs_to_launcher_build_dir enabled)

    set(LEOBASE_MODULES_GLOBAL_COPY_LIBS_TO_LAUNCHER_BUILD_DIR ${enabled} CACHE STRING "" FORCE)

ENDMACRO()

# ----------------------------------------------------------------------------------------------------------------------

MACRO(leobase_macro_global_set_force_install_dir enabled)

    set(LEOBASE_MODULES_GLOBAL_FORCE_INSTALL_DIR ${enabled}  CACHE STRING "" FORCE)

ENDMACRO()

# ----------------------------------------------------------------------------------------------------------------------

MACRO(leobase_macro_global_set_install_runtime_deps enabled)

    set(LEOBASE_MODULES_GLOBAL_INSTALL_RUNTIME_DEPS ${enabled} CACHE STRING "" FORCE)

ENDMACRO()

# ----------------------------------------------------------------------------------------------------------------------

MACRO(leobase_macro_global_set_install_runtime_artifacts enabled)

    set(LEOBASE_MODULES_GLOBAL_INSTALL_RUNTIME_ARTIFACTS ${enabled} CACHE STRING "" FORCE)

ENDMACRO()

# ----------------------------------------------------------------------------------------------------------------------

MACRO(leobase_macro_global_set_exclude_so_libraries enabled)

    set(LEOBASE_MODULES_GLOBAL_EXCLUDE_SO_LIBS ${enabled} CACHE STRING "" FORCE)

ENDMACRO()

# ----------------------------------------------------------------------------------------------------------------------

MACRO(leobase_macro_global_set_library_build_type build_type)

    set(LEOBASE_MODULES_GLOBAL_LIBRARY_BUILD_TYPE ${build_type} CACHE STRING "" FORCE)

ENDMACRO()

# ----------------------------------------------------------------------------------------------------------------------
