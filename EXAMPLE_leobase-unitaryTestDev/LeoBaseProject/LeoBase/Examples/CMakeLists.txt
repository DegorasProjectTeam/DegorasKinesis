# **********************************************************************************************************************
#    LeoBase Infrastructure Library                                                                                    *
#                                                                                                                      *
#    Subproject Examples                                                                                               *
#                                                                                                                      *
#    This file is part of LeoBase.                                                                                     *
# **********************************************************************************************************************

# **********************************************************************************************************************
# Updated 16/02/2025
# **********************************************************************************************************************

# **********************************************************************************************************************
# SUBPROJECT LEOBASE EXAMPLES CMAKELIST
# **********************************************************************************************************************

# ----------------------------------------------------------------------------------------------------------------------
# CONFIGURATION

# Set if examples are global targets.
set(EXAMPLES_IS_GLOBAL TRUE)

# ----------------------------------------------------------------------------------------------------------------------
# BASIC EXAMPLES

# Examples configuration.
set(BASIC_EXAMPLES_SOURCES_FOLDER ${CMAKE_CURRENT_SOURCE_DIR}/Basic)
set(BASIC_EXAMPLES_BUILD_FOLDER ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${LEOBASE_NAME}/Examples/Basic)
set(BASIC_EXAMPLES_PATH_INSTALL ${LEOBASE_MODULES_GLOBAL_INSTALL_BIN_PATH}/Examples/Basic)
set(BASIC_EXAMPLES_PROJECT_FOLDER ${LEOBASE_NAME}/Examples/Basic)

set(BASIC_EXAMPLES_VER 1.0.0)

# Module names list.
set(MODULE_NAMES "Callbacks"
                 "Containers"
                 "Crypto"
                 "Global"
                 "Logger"
                 "Module"
                 "Network"
                 "Patterns"
                 "System"
                 "Tasks"
                 "Testing"
                 "Timing"
                 "Helpers")

# Loop through each module name and configure the examples.
foreach(MODULE_NAME ${MODULE_NAMES})

    leobase_macro_setup_module_basic_apps(
        "${LEOBASE_NAME}"
        "${MODULE_NAME}"
        "${BASIC_EXAMPLES_VER}"
        "${BASIC_EXAMPLES_SOURCES_FOLDER}"
        "${BASIC_EXAMPLES_BUILD_FOLDER}"
        "${BASIC_EXAMPLES_PATH_INSTALL}"
        "${BASIC_EXAMPLES_PROJECT_FOLDER}"
        ""
        ""
        ""
        "${EXAMPLES_IS_GLOBAL}")

endforeach()

# ----------------------------------------------------------------------------------------------------------------------
# COMPLEX EXAMPLES

# Add the complex examples in the subdirectories.
#macro_add_subdirs_recursive("${CMAKE_CURRENT_SOURCE_DIR}" "${IGNORE_PATHS}")

# ----------------------------------------------------------------------------------------------------------------------
