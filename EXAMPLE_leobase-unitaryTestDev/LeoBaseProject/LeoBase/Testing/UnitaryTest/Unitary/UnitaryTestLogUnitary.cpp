/*
 * Copyright(C) Milethos Technologies SLU. 2026
 * All rights reserved. Reproduction in whole or in
 * part is prohibited without the written consent of
 * the copyright holder.
 */

// UnitaryTestLog UnitaryTest

// C++ INCLUDES
#include <string>
#include <vector>

// LEOBASE INCLUDES
#include <LeoBase/Aliases/Testing>
// ---------------------------------------------------------------------------------------------------------------------

using namespace leobase;
using namespace leobase::testing;

// 1. Test Declaration
UT_DECLARE_TEST(TestLogBasicFormatting)
UT_DECLARE_TEST(TestLogDetailedSubtests)
UT_DECLARE_TEST(TestLogGetters)

// 2. Define Tests

/**
 * @brief Validates the basic construction of the log string and result formatting.
 */
UT_DEFINE_TEST(TestLogBasicFormatting)
{
    // Prepare dummy results
    UnitTestResult emptyResults;

    // Create a log for a passing test
    UnitaryTestLog logPass("Core", "Math", "Addition", "", true, "2025-01-01", 150, emptyResults);

    std::string output = logPass.makeLog(false);

    // Check if output contains basic info
    UT_EXPECTED_TRUE(output.find("Core") != std::string::npos);
    UT_EXPECTED_TRUE(output.find("Math") != std::string::npos);
    UT_EXPECTED_TRUE(output.find("PASS") != std::string::npos);
    UT_EXPECTED_TRUE(output.find("150us") != std::string::npos);
}

/**
 * @brief Validates that subtest details (PASS/FAIL) are correctly appended to the log string.
 */
UT_DEFINE_TEST(TestLogDetailedSubtests)
{
    // Prepare results with subtests: {ID, Status, Message}
    UnitTestResult results = {
        {1, true, "Check 1 ok"},
        {2, false, "Check 2 failed"}
    };

    UnitaryTestLog logFail("Network", "Socket", "Connection", "Timeout", false, "12:00:00", 500, results);

    // Detailed log (force_show = true)
    std::string detailedOutput = logFail.makeLog(true);

    // Verify visibility of subtests and exception message
    UT_EXPECTED_TRUE(detailedOutput.find("FAIL") != std::string::npos);
    UT_EXPECTED_TRUE(detailedOutput.find("Check 2 failed") != std::string::npos);
    UT_EXPECTED_TRUE(detailedOutput.find("Except: Timeout") != std::string::npos);
}

/**
 * @brief Verifies simple getter methods for module and result status.
 */
UT_DEFINE_TEST(TestLogGetters)
{
    UnitTestResult empty;
    UnitaryTestLog log("FileSystem", "IO", "Write", "", true, "timestamp", 10, empty);

    UT_EXPECTED_TRUE(this->expectEQ(log.getModuleName(), "FileSystem"));
    UT_EXPECTED_TRUE(this->expectEQ(log.getSubmoduleName(), "IO"));
    UT_EXPECTED_TRUE(log.getResult() == true);
}

// 3. Main session execution
UT_START_SESSION("UnitaryTestLog Logic Session")

UT_FORCE_STOP_IF_SUBTEST_FAIL(false);

UT_REGISTER_TEST(Testing, Logging, TestLogBasicFormatting);
UT_REGISTER_TEST(Testing, Logging, TestLogDetailedSubtests);
UT_REGISTER_TEST(Testing, Logging, TestLogGetters);

UT_RUN_TESTS();

UT_FINISH_SESSION()
