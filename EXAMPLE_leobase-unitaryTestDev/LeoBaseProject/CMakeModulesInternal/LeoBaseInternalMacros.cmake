# ----------------------------------------------------------------------------------------------------------------------
# LEOBASE INTERNAL MACROS
# ----------------------------------------------------------------------------------------------------------------------
# Updated 07/04/2025
# ----------------------------------------------------------------------------------------------------------------------

# ----------------------------------------------------------------------------------------------------------------------

MACRO(leobase_internal_macro_set_header_deploy_cleaning enabled)

    # Find Python.
    if(${enabled})
        find_package(Python3 REQUIRED COMPONENTS Interpreter)
        if(NOT Python3_FOUND)
            message(FATAL_ERROR "Cleaning headers in deploy step needs Python3.")
        endif()

        set(SCRIPT_CLEANHEADERS_NAME "ScriptCleanHeadersComments.py")
        set(SCRIPT_CLEANHEADERS_PATH "${CMAKE_SOURCE_DIR}/LeoBase/scripts/${SCRIPT_CLEANHEADERS_NAME}")

    endif()

    set(LEOBASE_INTERNAL_HEADER_DEPLOY_CLEANING ${enabled} CACHE STRING "" FORCE)

ENDMACRO()

# ----------------------------------------------------------------------------------------------------------------------

MACRO(leobase_internal_macro_header_deploy_cleaning)

    if(${LEOBASE_INTERNAL_HEADER_DEPLOY_CLEANING})

        message(STATUS "Cleaning deployed headers...")
        message(STATUS "  Script: ${SCRIPT_CLEANHEADERS_PATH}")

        if(NOT DEFINED LEOBASE_MODULES_GLOBAL_INSTALL_INCLUDE_PATH)
            message(FATAL_ERROR "Variable LEOBASE_MODULES_GLOBAL_INSTALL_INCLUDE_PATH is not defined!")
        endif()

        set(_LEOBASE_CLEAN_INPUT_DIR "${LEOBASE_MODULES_GLOBAL_INSTALL_INCLUDE_PATH}")
        set(_LEOBASE_CLEAN_OUTPUT_DIR "${LEOBASE_MODULES_GLOBAL_INSTALL_INCLUDE_PATH}")

        install(CODE "
            message(STATUS \" Running header cleaning at install time...\")
            execute_process(
                COMMAND \"${Python3_EXECUTABLE}\" \"${SCRIPT_CLEANHEADERS_PATH}\"
                        --input \"${_LEOBASE_CLEAN_INPUT_DIR}\"
                        --output \"${_LEOBASE_CLEAN_OUTPUT_DIR}\"
                RESULT_VARIABLE _clean_result
            )
            if(NOT _clean_result EQUAL 0)
                message(FATAL_ERROR \" Header cleaning script failed with code: \${_clean_result}\")
            endif()
        ")

    endif()

ENDMACRO()

# ----------------------------------------------------------------------------------------------------------------------
