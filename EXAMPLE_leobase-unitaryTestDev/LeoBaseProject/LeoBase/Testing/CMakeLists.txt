# **********************************************************************************************************************
#    PROJECT LEOBASE INFRAESTRUCTURE LIBRARY                                                                                  *
# **********************************************************************************************************************

# **********************************************************************************************************************
# Updated 22/01/2026
# **********************************************************************************************************************

# **********************************************************************************************************************
# SUBPROJECT LEOBASE TESTS CMAKELIST
# **********************************************************************************************************************

# ----------------------------------------------------------------------------------------------------------------------
# Unit test

#
set(UNITARY_TEST_SOURCES_FOLDER ${CMAKE_CURRENT_SOURCE_DIR}/UnitaryTest/${MODULE_NAMES})
set(UNITARY_TEST_BUILD_FOLDER ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${LEOBASE_NAME}/UnitaryTest)
set(UNITARY_TEST_PATH_INSTALL ${LEOBASE_MODULES_GLOBAL_INSTALL_BIN_PATH}/UnitaryTest)
set(UNITARY_TEST_PROJECT_FOLDER ${LEOBASE_NAME}/Testing/UnitaryTest)
set(UNITARY_TEST_VER 0.0.1)
set(UNITARY_TEST_GLOBAL true)

# Module names list.
set(MODULE_NAMES "Containers"
                 "Crypto"
                 "Unitary"
                 "Timing"
                 "Helpers"
                 "Logger"
                 "System"
                 "Callbacks"
                 "Tasks"
                 "Patterns"
                 "Module"
                 "Network"
                 "Communication")

# Loop through each module name and configure the examples.
foreach(MODULE_NAME ${MODULE_NAMES})

   leobase_macro_setup_module_basic_apps(
       "${LEOBASE_NAME}"
        "${MODULE_NAME}"
        "${UNITARY_TEST_VER}"
        "${UNITARY_TEST_SOURCES_FOLDER}"
        "${UNITARY_TEST_BUILD_FOLDER}"
        "${UNITARY_TEST_PATH_INSTALL}"
        "${UNITARY_TEST_PROJECT_FOLDER}"
        ""
        ""
        ""
        "${UNITARY_TEST_GLOBAL}")


endforeach()
#
# ----------------------------------------------------------------------------------------------------------------------


# **********************************************************************************************************************
