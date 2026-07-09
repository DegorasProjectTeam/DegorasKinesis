# ----------------------------------------------------------------------------------------------------------------------
# TESTING SUBGROUP: Devices
# One executable per .cpp in this subgroup (target name = file stem). These are integration tests that run against the
# Kinesis Simulator or real hardware and self-skip when no device is present.
# ----------------------------------------------------------------------------------------------------------------------

file(GLOB_RECURSE THORLABS_KINESIS_TESTS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

foreach(test_src ${THORLABS_KINESIS_TESTS})
    get_filename_component(test_name ${test_src} NAME_WE)
    add_executable(${test_name} ${test_src})
    target_link_libraries(${test_name} PRIVATE LibThorlabsKinesis)
endforeach()

# **********************************************************************************************************************
