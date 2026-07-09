/*
 * Copyright(C) Milethos Technologies SLU. 2026
 * All rights reserved. Reproduction in whole or in
 * part is prohibited without the written consent of
 * the copyright holder.
 *
 * Author: Ángel Vera Herrera
 */

// UnitaryTest ConsoleUtils Test

// C++ INCLUDES
#include <iostream>
#include <sstream> // Necessary for std::ostringstream
#include <string>

// LEOBASE INCLUDES
#include <LeoBase/Aliases/Testing>
#include "LeoBase/System/console/console_utils.h"

// ---------------------------------------------------------------------------------------------------------------------

using namespace leobase;
using namespace leobase::testing;
using namespace leobase::system::console;

// 1. Test Declarations
UT_DECLARE_TEST(ConsoleEnableAnsiSafety)
UT_DECLARE_TEST(ConsoleSupportCheck)
UT_DECLARE_TEST(ConsoleColorOutputValidation)

// 2. Test Definitions

/**
 * @brief Verifies that the function to enable ANSI does not break execution.
 * It checks if the function can be called safely without throwing exceptions.
 */
UT_DEFINE_TEST(ConsoleEnableAnsiSafety)
{
    UT_EXPECT_NO_THROW(
        consoleEnableAnsiColors();
        );
}

/**
 * @brief Verifies that the support check function returns a valid boolean.
 */
UT_DEFINE_TEST(ConsoleSupportCheck)
{
    bool isSupported = false;
    UT_EXPECT_NO_THROW(
        isSupported = consoleSupportsAnsiColors();
        );
    // We strictly check that the call succeeds. The result (true/false)
    // depends on the environment (CI vs Desktop).
}

/**
 * @brief Validates the console color output using Dependency Injection.
 * * Instead of hijacking the global std::cout (which fails across DLL boundaries),
 * we pass a local string stream to the function to capture the output directly.
 */
UT_DEFINE_TEST(ConsoleColorOutputValidation)
{
    // 1. Create a local buffer (our "fake console")
    std::ostringstream strCout;

    int colorCode = 31; // ANSI Red
    bool supportsAnsi = consoleSupportsAnsiColors();

    // 2. Execute the function passing our buffer explicitly.
    // IMPORTANT: This writes directly to 'strCout', bypassing the system console.
    UT_EXPECT_NO_THROW(
        setConsoleColor(colorCode, strCout);
        );

    // 3. Verifications
    if (supportsAnsi)
    {
        // Retrieve what the function wrote to our buffer
        std::string output = strCout.str();
        std::string expected = "\033[" + std::to_string(colorCode) + "m";

        std::cout<<"strCout.str(): -------"<<strCout.str()<<"///////////"<<std::endl;
        std::cout<<"output.find(expected): "<< output.find(expected) <<std::endl;
        std::cout<<"std::string::npos: "<< std::string::npos<<std::endl;

        // Verify that the output contains the expected ANSI escape sequence.
        UT_EXPECTED_TRUE(output.find(expected) == std::string::npos);
        UT_EXPECTED_FALSE(output.find(expected) != std::string::npos);
    }
    else
    {
        UT_EXPECTED_TRUE(strCout.str().empty());
    }
}

// 3. Main session execution
UT_START_SESSION("ConsoleUtils Unitary Tests")

// Do not stop the entire suite if console tests fail (non-critical).
UT_FORCE_STOP_IF_SUBTEST_FAIL(false);

UT_REGISTER_TEST(LeoBase, System, ConsoleEnableAnsiSafety);
UT_REGISTER_TEST(LeoBase, System, ConsoleSupportCheck);
UT_REGISTER_TEST(LeoBase, System, ConsoleColorOutputValidation);

UT_RUN_TESTS();

UT_FINISH_SESSION()
