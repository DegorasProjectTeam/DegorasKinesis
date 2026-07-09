# ----------------------------------------------------------------------------------------------------------------------
#    PROJECT LEOBASE INFRAESTRUCTURE LIBRARY
# ----------------------------------------------------------------------------------------------------------------------

# ----------------------------------------------------------------------------------------------------------------------
# Updated 07/04/2025
# ----------------------------------------------------------------------------------------------------------------------

# ----------------------------------------------------------------------------------------------------------------------
# SUBPROJECT LEOBASE MAIN CMAKELIST
# ----------------------------------------------------------------------------------------------------------------------

# ----------------------------------------------------------------------------------------------------------------------
# SETUP LEOBASE LIBRARY
#	TEMPORARY
cmake_minimum_required(VERSION 3.20)

#	TEMPORARY
# Basic configuration
set(LEOBASE_NAME LeoBase CACHE INTERNAL "")
set(LEOBASE_VER 2.9.99 CACHE INTERNAL "")

# Specific configuration.
set(LEOBASE_INCLUDES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/includes/)
set(LEOBASE_IS_GLOBAL TRUE)
set(LEOBASE_DEPS_OPT)
set(LEOBASE_DEPS_DEB)
set(LEOBASE_DEPS_GEN)
set(LEOBASE_BUILD_DIR "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/")

# Get the headers, sources and template files.
file(GLOB_RECURSE HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/includes/${LEOBASE_NAME}/*.h)
file(GLOB_RECURSE TEMPLTS ${CMAKE_CURRENT_SOURCE_DIR}/includes/${LEOBASE_NAME}/*.tpp)
file(GLOB_RECURSE INL ${CMAKE_CURRENT_SOURCE_DIR}/includes/${LEOBASE_NAME}/*.inl)
file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/sources/*.cpp)
file(GLOB_RECURSE SCRIPTS_PY ${CMAKE_CURRENT_SOURCE_DIR}/scripts/*.py)	

# Get the include aliases files.
leobase_macro_get_files_without_extension(ALIAS ${CMAKE_CURRENT_SOURCE_DIR}/includes/${LEOBASE_NAME}/*)

# Create custom groups.
set(INC_GROUP_BASE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/includes/${LEOBASE_NAME})
set(SRC_GROUP_BASE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/sources)
set(SCR_GROUP_BASE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/scripts)
leobase_macro_create_headers_group("${INC_GROUP_BASE_PATH}" ${HEADERS} ${TEMPLTS} ${INL})
leobase_macro_create_aliases_group("${INC_GROUP_BASE_PATH}/Aliases" ${ALIAS})
leobase_macro_create_sources_group("${SRC_GROUP_BASE_PATH}" ${SOURCES})
leobase_macro_create_scripts_group("${SCR_GROUP_BASE_PATH}" ${SCRIPTS_PY})

# Specific links in Windows plattform.
if(WIN32)
    set(LEOBASE_DEPS_GEN ${LEOBASE_DEPS_GEN} ws2_32 iphlpapi Shlwapi pdh)
endif()

if("${LEOBASE_BUILD_TYPE}" STREQUAL "")
    message(WARNING "Leobase build type not defined. Setting to default: ${LEOBASE_MODULES_GLOBAL_LIBRARY_BUILD_TYPE}")
	set(LEOBASE_BUILD_TYPE ${LEOBASE_MODULES_GLOBAL_LIBRARY_BUILD_TYPE})
else()
	message(STATUS "Set leobase global library build type: ${LEOBASE_BUILD_TYPE}")
endif()

leobase_macro_global_set_library_build_type(${LEOBASE_BUILD_TYPE})

if("${LEOBASE_BUILD_TYPE}" STREQUAL "SHARED")
    message(STATUS "Leobase build type is SHARED")

    # Setup the shared library.
    leobase_macro_setup_shared_lib(
        "${LEOBASE_NAME}"
        "${LEOBASE_VER}"
        "${LEOBASE_BUILD_DIR}"
        "${LEOBASE_INCLUDES_DIR}"
        "${LEOBASE_DEPS_OPT}"
        "${LEOBASE_DEPS_DEB}"
        "${LEOBASE_DEPS_GEN}"
        "${LEOBASE_IS_GLOBAL}"
        ${SOURCES} ${HEADERS} ${TEMPLTS} ${INL} ${ALIAS} ${SCRIPTS_PY})

elseif("${LEOBASE_BUILD_TYPE}" STREQUAL "STATIC")
    message(STATUS "LeoBase build type is STATIC")

    # Setup the shared library.
    leobase_macro_setup_static_lib(
        "${LEOBASE_NAME}"
        "${LEOBASE_VER}"
        "${LEOBASE_BUILD_DIR}"
        "${LEOBASE_INCLUDES_DIR}"
        "${LEOBASE_DEPS_OPT}"
        "${LEOBASE_DEPS_DEB}"
        "${LEOBASE_DEPS_GEN}"
        "${LEOBASE_IS_GLOBAL}"
        ${SOURCES} ${HEADERS} ${TEMPLTS} ${INL} ${ALIAS} ${SCRIPTS_PY})
else()
    message(FATAL_ERROR "LeoBase build type not defined and no default is set.")
endif()

# In mingw better do static linking of some libs.
if (MINGW)
    target_link_libraries(${LEOBASE_NAME} PRIVATE -static-libgcc -static-libstdc++ -static -lpthread)
endif()

# ----------------------------------------------------------------------------------------------------------------------
# INSTALLATION PROCESS

# Get the root dir.
leobase_macro_get_root_directory(ROOT_DIR)

# Default directories.
set(LEOBASE_DEFAULT_INSTALL_DIR_WINDOWS "${ROOT_DIR}/deploys")
set(LEOBASE_DEFAULT_INSTALL_DIR_UNIX "/opt")

# Automatic default configuration.
if(WIN32)
    set(LEOBASE_DEFAULT_INSTALL_DIR "${LEOBASE_DEFAULT_INSTALL_DIR_WINDOWS}")
else()
    set(LEOBASE_DEFAULT_INSTALL_DIR "${LEOBASE_DEFAULT_INSTALL_DIR_UNIX}")
endif()

# Setup the global base install directories for LeoBase.
leobase_macro_setup_global_install_dir(
    "${LEOBASE_NAME}"
    "${LEOBASE_VER}"
    "${LEOBASE_DEFAULT_INSTALL_DIR}")

# Default installation process.
leobase_macro_default_library_installation("${LEOBASE_NAME}")

# Specific installation for the LeoBase CMakeModules.
install(DIRECTORY ${CMAKE_SOURCE_DIR}/CMakeModulesLeoBase
        DESTINATION "${LEOBASE_MODULES_GLOBAL_INSTALL_SHARE_PATH}"
        FILES_MATCHING PATTERN "*.cmake")

# Specific installation for the LeoBase CMakeModules.
leobase_internal_macro_header_deploy_cleaning()

# ----------------------------------------------------------------------------------------------------------------------



#-----------------------------------------------------------------------------------------------------------------------
# CONFIGURE THE SUBPROJECTS

#Testing subproject.
        add_subdirectory(Testing)

# Examples subproject.
       add_subdirectory(Examples)

# **********************************************************************************************************************


