# ----------------------------------------------------------------------------------------------------------------------
# TESTING SUBGROUP: Devices
# One executable per .cpp in this subgroup (target name = file stem). These are integration tests that run against the
# Kinesis Simulator or real hardware and self-skip when no device is present.
# ----------------------------------------------------------------------------------------------------------------------

file(GLOB_RECURSE THORLABS_KINESIS_TESTS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

foreach(test_src ${THORLABS_KINESIS_TESTS})
    get_filename_component(test_name ${test_src} NAME_WE)
    add_executable(${test_name} ${test_src})
    target_link_libraries(${test_name} PRIVATE DegorasKinesis)
    # Register with CTest, except the long-running monitor and the hardware-gated test.
    if(NOT test_name MATCHES "Monitor|Hardware")
        add_test(NAME ${test_name} COMMAND ${test_name})

        # A TEST THAT SKIPS ITSELF MUST NOT REPORT A PASS. These tests exit 0 when no device or simulator is
        # present, and without this CTest cannot tell that apart from success: with the simulator stopped the
        # suite reported "100% tests passed, 9 of 9" in 0.73 s, three of which had run nothing at all. Exit
        # code 77 is the GNU Automake convention CTest adopts for "skipped".
        set_tests_properties(${test_name} PROPERTIES SKIP_RETURN_CODE 77)
    endif()
endforeach()

# **********************************************************************************************************************
