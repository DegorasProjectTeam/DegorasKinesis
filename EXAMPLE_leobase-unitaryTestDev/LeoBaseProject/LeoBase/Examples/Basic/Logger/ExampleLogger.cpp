/*
 *      Copyright(C) Milethos Technologies SLU. 2025
 *      All rights reserved. Reproduction in whole or in
 *      part is prohibited without the written consent of
 *      the copyright holder.
 */

// C++ INCLUDES
#include <iostream>
#include <thread>

// LEOBASE INCLUDES
#include <LeoBase/Aliases/Logger>
#include <LeoBase/Aliases/System>

// ---------------------------------------------------------------------------------------------------------------------

// LeoBase namespaces.
using namespace leobase::logger;

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Main entry point of the program `ExampleLogger`.
 */
int main()
{
    // Initial log.
    std::cout << "===============================================" << std::endl;
    std::cout << "== Example Logger" << std::endl;
    std::cout << "===============================================" << std::endl;
    std::cout << std::endl;
    std::cout << "Starting example..." << std::endl;
    std::cout << std::endl;

    // ----------------------------------------------------------------------------------------------------

    // Init
    std::cout << "Init logger." << std::endl;

    LeoLog::instance()->setDir("F:/workspace/test_log");
    LeoLog::instance()->init("TEST_LOG", "TestFilename", LogLevel::LOGLEVEL_DEBUG);

    LeoLog::instance()->appendLogger("TEST_MODULE", LogLevel::LOGLEVEL_DEBUG);

    //----------------------------------------------------------------------------------------------------

    // Test.
    std::cout << "Test logger." << std::endl;

    auto async_future = std::async([&]()
        {
            LeoLog::instance()->debug("TEST_MODULE",    "Im test in async debug.");
            LeoLog::instance()->info("TEST_MODULE",     "Im test in async info.");
            LeoLog::instance()->warning("TEST_MODULE",  "Im test in async warning.");
            LeoLog::instance()->error("TEST_MODULE",    "Im test in async error.");
            LeoLog::instance()->critical("TEST_MODULE", "Im test in async critical.");
        });

    LeoLog::instance()->debug("TEST_MODULE",    "Im test debug.");
    LeoLog::instance()->info("TEST_MODULE",     "Im test info.");
    LeoLog::instance()->warning("TEST_MODULE",  "Im test warning.");
    LeoLog::instance()->error("TEST_MODULE",    "Im test error.");
    LeoLog::instance()->critical("TEST_MODULE", "Im test critical.");

    async_future.wait();

    //----------------------------------------------------------------------------------------------------

    // Stop.
    std::cout << "Stop logger." << std::endl;

    LeoLog::instance()->stop();

    //----------------------------------------------------------------------------------------------------
    // Final log.
    std::cout << std::endl << "Example finished!" << std::endl;
    std::cout << std::endl;

    // Final return.
    return 0;
}

// ---------------------------------------------------------------------------------------------------------------------
