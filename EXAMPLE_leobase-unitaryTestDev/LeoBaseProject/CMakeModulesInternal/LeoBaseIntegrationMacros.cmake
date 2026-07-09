# ----------------------------------------------------------------------------------------------------------------------
# LEOBASE INTEGRATION MACROS
# ----------------------------------------------------------------------------------------------------------------------
# Updated 07/04/2025
# ----------------------------------------------------------------------------------------------------------------------

# ----------------------------------------------------------------------------------------------------------------------

MACRO(leobase_macro_initial_configuration_example)

    # Initial log.
    message(STATUS "LEOBASE CMAKE MACROS INITIAL CONFIGURATION")

    # Enforce LeoBase root configuration.
    if(NOT DEFINED ENV{LEOBASE_ROOT})
        message(FATAL_ERROR "  LEOBASE_ROOT not set.")
    endif()

    # Prepare the LeoBase CMake modules path.
    set(LEOBASE_CMAKE_MODULES_PATH $ENV{LEOBASE_ROOT}/share/CMakeModulesLeoBase)

    # Set path to LeoBase CMake modules.
    set(CMAKE_MODULE_PATH
        ${CMAKE_MODULE_PATH}
        ${LEOBASE_CMAKE_MODULES_PATH})

    # Log initial configuration.
    message(STATUS "  LEOBASE_ROOT: $ENV{LEOBASE_ROOT}")
    message(STATUS "  CMakeModulesLeoBase path: ${LEOBASE_CMAKE_MODULES_PATH}")

ENDMACRO()

# **********************************************************************************************************************
