# FindThorlabsKinesis.cmake
#
# Locates the (vendored or system) Thorlabs Kinesis SDK and defines an imported interface target:
#
#     Thorlabs::Kinesis
#
# The Thorlabs Kinesis SDK is a proprietary, Windows-only, closed-source component and is NOT redistributed by this
# package. This module only *locates* it. Search order:
#   1. THORLABS_KINESIS_ROOT (CMake variable or environment variable)
#   2. the usual Thorlabs install location on Windows (C:/Program Files/Thorlabs/Kinesis)
#
# Result variables: ThorlabsKinesis_FOUND, ThorlabsKinesis_INCLUDE_DIR, ThorlabsKinesis_LIBRARIES.

if(TARGET Thorlabs::Kinesis)
    set(ThorlabsKinesis_FOUND TRUE)
    return()
endif()

set(_tk_root "${THORLABS_KINESIS_ROOT}")
if(NOT _tk_root AND DEFINED ENV{THORLABS_KINESIS_ROOT})
    set(_tk_root "$ENV{THORLABS_KINESIS_ROOT}")
endif()

set(_tk_hints "${_tk_root}" "$ENV{ProgramFiles}/Thorlabs/Kinesis" "C:/Program Files/Thorlabs/Kinesis")

# Headers: the SDK ships them either directly (system install) or under an 'includes/Thorlabs' subdir (vendored).
find_path(ThorlabsKinesis_INCLUDE_DIR
    NAMES Thorlabs/Thorlabs.MotionControl.Benchtop.DCServo.h Thorlabs.MotionControl.Benchtop.DCServo.h
    HINTS ${_tk_hints}
    PATH_SUFFIXES includes/Thorlabs includes include Thorlabs "")

# The Thorlabs import libraries are MSVC .lib files; MinGW links them for the extern "C" Kinesis API. Ensure .lib is
# searched even under a GNU toolchain (whose default library suffixes do not include .lib).
set(_tk_saved_suffixes "${CMAKE_FIND_LIBRARY_SUFFIXES}")
list(PREPEND CMAKE_FIND_LIBRARY_SUFFIXES ".lib")

find_library(ThorlabsKinesis_DCSERVO_LIBRARY
    NAMES Thorlabs.MotionControl.Benchtop.DCServo
    HINTS ${_tk_hints} PATH_SUFFIXES lib "")

find_library(ThorlabsKinesis_ISC_LIBRARY
    NAMES Thorlabs.MotionControl.IntegratedStepperMotors
    HINTS ${_tk_hints} PATH_SUFFIXES lib "")

set(CMAKE_FIND_LIBRARY_SUFFIXES "${_tk_saved_suffixes}")

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(ThorlabsKinesis
    REQUIRED_VARS
        ThorlabsKinesis_INCLUDE_DIR
        ThorlabsKinesis_DCSERVO_LIBRARY
        ThorlabsKinesis_ISC_LIBRARY)

if(ThorlabsKinesis_FOUND)
    # The header path may include the trailing 'Thorlabs' component; the code does #include <Thorlabs/...>, so expose
    # the directory that *contains* the Thorlabs folder.
    set(_tk_inc "${ThorlabsKinesis_INCLUDE_DIR}")
    get_filename_component(_tk_inc_name "${_tk_inc}" NAME)
    if(_tk_inc_name STREQUAL "Thorlabs")
        get_filename_component(_tk_inc "${_tk_inc}" DIRECTORY)
    endif()

    set(ThorlabsKinesis_LIBRARIES "${ThorlabsKinesis_DCSERVO_LIBRARY}" "${ThorlabsKinesis_ISC_LIBRARY}")

    add_library(Thorlabs::Kinesis INTERFACE IMPORTED)
    set_target_properties(Thorlabs::Kinesis PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${_tk_inc}"
        INTERFACE_LINK_LIBRARIES "${ThorlabsKinesis_LIBRARIES}")
endif()

mark_as_advanced(ThorlabsKinesis_INCLUDE_DIR ThorlabsKinesis_DCSERVO_LIBRARY ThorlabsKinesis_ISC_LIBRARY)
