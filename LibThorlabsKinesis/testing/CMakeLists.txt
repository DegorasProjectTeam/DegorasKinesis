# ----------------------------------------------------------------------------------------------------------------------
# TESTING
#
# Assert-based, no-framework self-checks (one main() per file), matching the "testing is an app" convention. The
# files are organised in layer subfolders (Common / DCServo / Devices); each becomes its own executable named after
# the file. Hardware/simulator tests self-skip when no device is present.
# ----------------------------------------------------------------------------------------------------------------------

file(GLOB_RECURSE THORLABS_KINESIS_TESTS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

foreach(test_src ${THORLABS_KINESIS_TESTS})
    get_filename_component(test_name ${test_src} NAME_WE)
    add_executable(${test_name} ${test_src})
    target_link_libraries(${test_name} PRIVATE LibThorlabsKinesis)
endforeach()

# **********************************************************************************************************************
