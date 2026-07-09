# ----------------------------------------------------------------------------------------------------------------------
# LEOBASE VERSION
# ----------------------------------------------------------------------------------------------------------------------
# Updated 28/01/2025
# ----------------------------------------------------------------------------------------------------------------------

# ----------------------------------------------------------------------------------------------------------------------

macro(leobase_macro_find_libs paths result_var)

    set(ALL_LIB_FILES "")

    if (WIN32)
        foreach(dir ${paths})
            file(GLOB_RECURSE LIBS_IN_DIR "${dir}/*.dll")
            list(APPEND ALL_LIB_FILES ${LIBS_IN_DIR})
        endforeach()
    elseif(UNIX)
        foreach(dir ${paths})
            file(GLOB_RECURSE LIBS_IN_DIR "${dir}/*.so")
            list(APPEND ALL_LIB_FILES ${LIBS_IN_DIR})
        endforeach()
    endif()

    list(REMOVE_DUPLICATES ALL_LIB_FILES)

    set(${result_var} "${ALL_LIB_FILES}")

endmacro()

# ----------------------------------------------------------------------------------------------------------------------

# Function to get the root directory of the current drive
function(leobase_macro_get_root_directory output_var)
    if(WIN32)
        # Get the current source directory and extract the drive letter dynamically
        string(SUBSTRING "${CMAKE_CURRENT_SOURCE_DIR}" 0 2 DRIVE_LETTER)
        set(ROOT_DIR "${DRIVE_LETTER}/")
    else()
        # For Unix/Linux, use the root directory "/"
        set(ROOT_DIR "/")
    endif()
    set(${output_var} "${ROOT_DIR}" PARENT_SCOPE)
endfunction()

# ----------------------------------------------------------------------------------------------------------------------

macro(leobase_macro_search_file file_name current_path result_var)
    # Initial search for the file in the current directory
    file(GLOB_RECURSE found_files RELATIVE "${current_path}" "${current_path}/*/${file_name}")

    if(found_files)
        list(GET found_files 0 first_found_file)
        get_filename_component(first_found_dir "${first_found_file}" DIRECTORY)
        set(${result_var} "${current_path}/${first_found_dir}")
    else()
        # Recursively search in subdirectories
        file(GLOB children RELATIVE "${current_path}" "${current_path}/*")
        foreach(child IN LISTS children)
            if(IS_DIRECTORY "${current_path}/${child}")
                leobase_macro_search_file(${file_name} "${current_path}/${child}" ${result_var})
                if(${result_var})
                    break()  # Stop if the file has been found
                endif()
            endif()
        endforeach()
    endif()
endmacro()

# ----------------------------------------------------------------------------------------------------------------------

macro(leobase_macro_search_file_in_paths file_name paths result_var append_filename)
    # Split file_name into directory and the actual file name
    string(FIND "${file_name}" "/" last_slash REVERSE)
    if(last_slash GREATER -1)
        string(SUBSTRING "${file_name}" 0 ${last_slash} sub_path)
        string(SUBSTRING "${file_name}" ${last_slash} -1 actual_file_name)
    else()
        set(sub_path "")
        set(actual_file_name "${file_name}")
    endif()

    set(local_result)
    foreach(dir ${paths})
        if(IS_DIRECTORY ${dir})
            leobase_macro_search_file("${file_name}" "${dir}" local_result)
        endif()
        if(local_result)
            # Check if the filename should be appended to the result
            if(${append_filename})
                set(${result_var} "${local_result}/${file_name}")  # Append filename to path
            else()
                # Determine if the sub_path should be removed from the result
                string(REGEX REPLACE "/${sub_path}$" "" trimmed_path "${local_result}")
                set(${result_var} ${trimmed_path})  # Just set the directory path
            endif()
            break()  # Stop if the file has been found
        endif()
    endforeach()
endmacro()

# ----------------------------------------------------------------------------------------------------------------------

MACRO(leobase_macro_search_file_in_folder_list file_name paths result)

    message(STATUS "HERE: ${paths}" )

    # Ensure paths are treated as a list
    set(path_list "${paths}")
    if(NOT "${paths}" MATCHES ";")
        set(path_list "${paths}")
    endif()

    foreach(dir IN LISTS path_list)
        message(STATUS "    Candidate: ${dir}" )

         if(IS_DIRECTORY ${dir})
             leobase_macro_search_file(${file_name} ${dir} result)
         endif()
         if(result)
             message(STATUS "    Found 1: ${result}" )
             break()  # Stop if the file has been found
         endif()
     endforeach()

ENDMACRO()

# ----------------------------------------------------------------------------------------------------------------------

MACRO(leobase_macro_get_files_without_extension result curdir)
    set(FILELIST)
    file(GLOB_RECURSE FILES ${curdir})
    foreach(FILE ${FILES})
        get_filename_component(FILENAME ${FILE} NAME)
        if(${FILENAME} MATCHES "^[^.]+$")
            set(FILELIST ${FILELIST} ${FILE})
        endif()
    endforeach()
    set(${result} ${FILELIST})
ENDMACRO()

# ----------------------------------------------------------------------------------------------------------------------

MACRO(leobase_macro_subdir_list result curdir)
    file(GLOB children RELATIVE ${curdir} ${curdir}/*)
    set(dirlist "")
    foreach(child ${children})
    if(IS_DIRECTORY ${curdir}/${child})
        list(APPEND dirlist ${child})
    endif()
    endforeach()
    set(${result} ${dirlist})
ENDMACRO()

# ----------------------------------------------------------------------------------------------------------------------

MACRO(leobase_macro_add_subdirs curdir)
    leobase_macro_subdir_list(subdirs ${curdir})
    foreach(subdir ${subdirs})
        # Check if CMakeLists.txt exists in the subdir.
        if(EXISTS "${curdir}/${subdir}/CMakeLists.txt")
            add_subdirectory(${subdir})
        endif()
    endforeach()
ENDMACRO()

# ----------------------------------------------------------------------------------------------------------------------

MACRO(leobase_macro_add_subdirs_recursive curdir ignore_paths)
    # Get a list of all subdirectories in the current directory
    leobase_macro_subdir_list(subdirs ${curdir})

    foreach(subdir ${subdirs})
        # Full path of the current subdir
        set(full_subdir_path "${curdir}/${subdir}")

        # Calculate relative path from the base directory (CMAKE_CURRENT_SOURCE_DIR)
        file(RELATIVE_PATH relative_subdir_path "${CMAKE_CURRENT_SOURCE_DIR}" ${full_subdir_path})

        # Check if current subdir's relative path is in the ignore list
        set(IGNORE_SUBDIR FALSE)
        foreach(IGNORE_PATH ${ignore_paths})
            # Check if the relative subdir path matches the ignore pattern
            if("${relative_subdir_path}" MATCHES "${IGNORE_PATH}")
                set(IGNORE_SUBDIR TRUE)
                message(STATUS "Ignoring subdir: ${relative_subdir_path}")
                break()  # Exit the inner loop if we find a match
            endif()
        endforeach()

        if(NOT IGNORE_SUBDIR)
            # Check if CMakeLists.txt exists in the current subdir
            if(EXISTS "${full_subdir_path}/CMakeLists.txt")
                add_subdirectory(${full_subdir_path})
            endif()

            # Recursively call this macro for each subdir, regardless of CMakeLists.txt existence
            leobase_macro_add_subdirs_recursive("${full_subdir_path}" "${ignore_paths}")
        endif()
    endforeach()
ENDMACRO()

# ----------------------------------------------------------------------------------------------------------------------

MACRO(leobase_macro_compose_current_architecture_folder_name result_var)

    # Check if 64-bit or 32-bit
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(ARCH "x86_64")
    else()
        set(ARCH "x86")
    endif()

    # Check if debug or release.
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(BUILD_TYPE "debug")
    elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
        set(BUILD_TYPE "release")
    else()
        set(BUILD_TYPE "unknown")
    endif()

    # Get the compiler version.
    set(COMP_V "${CMAKE_CXX_COMPILER_VERSION}")

    # Check the compiler name.
    if(MINGW)
        set(COMP_N "mingw")
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        set(COMP_N "msvc")
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        set(COMP_N "gnu")
    else()
        message(FATAL_ERROR "Compiler not supported by default.")
    endif()

    if("${LEOBASE_MODULES_GLOBAL_LIBRARY_BUILD_TYPE}" STREQUAL "SHARED")
        set(LIB_BUILD_TYPE "shared")
    elseif("${LEOBASE_MODULES_GLOBAL_LIBRARY_BUILD_TYPE}" STREQUAL "STATIC")
        set(LIB_BUILD_TYPE "static")
    else()
        message(FATAL_ERROR "Library build type not set correctly.")
    endif()

    # Construct the configuration folder name
    set(${result_var} "${COMP_N}-${ARCH}-${BUILD_TYPE}-${LIB_BUILD_TYPE}-${COMP_V}")

ENDMACRO()

# ----------------------------------------------------------------------------------------------------------------------

macro(leobase_macro_show_related_variables related_name)

    message(STATUS "---------------------------------------------------")
    message(STATUS "Variables related with: ${related_name}")
    get_cmake_property(_variableNames VARIABLES)
    foreach (_variableName ${_variableNames})
        if (_variableName MATCHES ${related_name})
            message(STATUS "  ${_variableName} = ${${_variableName}}")
        endif()
    endforeach()
    message(STATUS "---------------------------------------------------")

endmacro()

# ----------------------------------------------------------------------------------------------------------------------

macro(leobase_macro_show_library_properties target_name)

    message(STATUS "---------------------------------------------------")
    message(STATUS "Properties related with: ${target_name}")

    # List of standard properties to check
    set(properties
        INTERFACE_INCLUDE_DIRECTORIES
        INTERFACE_COMPILE_DEFINITIONS
        INTERFACE_COMPILE_OPTIONS
        INTERFACE_LINK_LIBRARIES
        INTERFACE_LINK_DIRECTORIES
        INTERFACE_LINK_OPTIONS
        INTERFACE_SOURCES
        LOCATION
        LOCATION_DEBUG
        LOCATION_RELEASE
        IMPORTED_CONFIGURATIONS
        IMPORTED_LOCATION
        IMPORTED_LOCATION_DEBUG
        IMPORTED_LOCATION_RELEASE
        IMPORTED_LINK_INTERFACE_LIBRARIES
        IMPORTED_LINK_INTERFACE_LANGUAGES)

    # Iterate through properties and print their values
    foreach(prop ${properties})
        get_target_property(prop_value ${target_name} ${prop})
        if (prop_value)
            message(STATUS "  ${target_name} - ${prop}: ${prop_value}")
        else()
            message(STATUS "  ${target_name} - ${prop}: (not set)")
        endif()
    endforeach()

    message(STATUS "---------------------------------------------------")

endmacro()

# ----------------------------------------------------------------------------------------------------------------------

MACRO(leobase_macro_create_headers_group base_path)

    set(file_list ${ARGN})

    source_group(TREE ${base_path}
        PREFIX "Headers"
        FILES ${file_list})

ENDMACRO()

# ----------------------------------------------------------------------------------------------------------------------

MACRO(leobase_macro_create_sources_group base_path)

    set(file_list ${ARGN})

    source_group(TREE ${base_path}
        PREFIX "Sources"
        FILES ${file_list})

ENDMACRO()

# ----------------------------------------------------------------------------------------------------------------------

MACRO(leobase_macro_create_scripts_group base_path)

    set(file_list ${ARGN})

    source_group(TREE ${base_path}
        PREFIX "Scripts"
        FILES ${file_list})

ENDMACRO()

# ----------------------------------------------------------------------------------------------------------------------

MACRO(leobase_macro_create_aliases_group base_path)

    set(file_list ${ARGN})

    source_group(TREE ${base_path}
        PREFIX "Aliases"
        FILES ${file_list})

ENDMACRO()

# ----------------------------------------------------------------------------------------------------------------------

MACRO(leobase_macro_create_protos_group proto_files
                                        proto_headers
                                        proto_sources)

    source_group("Proto Files" FILES ${proto_files})
    source_group("Proto Headers" FILES ${proto_headers})
    source_group("Proto Sources" FILES ${proto_sources})

ENDMACRO()

# ----------------------------------------------------------------------------------------------------------------------

MACRO(leobase_macro_command_executor command
                                     result_var)

    # Collect all additional arguments into the command arguments list
    set(command_args ${ARGN})

    # Execute the process with the provided command and arguments
    execute_process(
        COMMAND "${command}" "${command_args}"
        RESULT_VARIABLE ${result_var})

ENDMACRO()

# ----------------------------------------------------------------------------------------------------------------------
