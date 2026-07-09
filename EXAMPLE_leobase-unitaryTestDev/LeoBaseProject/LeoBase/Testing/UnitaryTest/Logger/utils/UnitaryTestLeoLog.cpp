/*
 * Copyright(C) Milethos Technologies SLU. 2025
 * All rights reserved. Reproduction in whole or in
 * part is prohibited without the written consent of
 * the copyright holder.
 */

// UnitaryTest LeoLog Test

// C++ INCLUDES
#include <string>
#include <thread>
#include <vector>
#include <filesystem>

// LEOBASE INCLUDES
#include <LeoBase/Aliases/Testing>
#include <LeoBase/Logger/leo_log.h>

// ---------------------------------------------------------------------------------------------------------------------

using namespace leobase;
using namespace leobase::testing;
using namespace leobase::logger;

// 1. Test Declarations
UT_DECLARE_TEST(LeoLogSingletonCheck)
UT_DECLARE_TEST(LeoLogConfiguration)
UT_DECLARE_TEST(LeoLogInitializationAndRun)
UT_DECLARE_TEST(LeoLogLoggingFlow)
UT_DECLARE_TEST(LeoLogShutdown)

// 2. Define Tests

/**
 * @brief Verifies the integrity of the Singleton pattern.
 * Ensures that only one instance exists and is accessible.
 */
UT_DEFINE_TEST(LeoLogSingletonCheck)
{
    // 1. Get instance
    LeoLog* logInstance = nullptr;

    UT_EXPECT_NO_THROW(
        logInstance = LeoLog::instance();
        );

    // 2. Verify it is not null
    UT_EXPECTED_TRUE(logInstance != nullptr);

    // 3. Verify that subsequent calls return the same memory address
    LeoLog* logInstance2 = LeoLog::instance();
    UT_EXPECTED_TRUE(logInstance == logInstance2);
}

/**
 * @brief Verifies log level manipulation and attached loggers (append/remove).
 * Performed before init() to configure the environment.
 */
UT_DEFINE_TEST(LeoLogConfiguration)
{
    LeoLog* logger = LeoLog::instance();
    std::string childModule = "SubModuleA";

    // 1. Append a secondary logger with WARNING level
    UT_EXPECT_NO_THROW(
        logger->appendLogger(childModule, LogLevel::LOGLEVEL_WARNING);
        );

    // 2. Verify the level is saved correctly
    UT_EXPECTED_TRUE(logger->getLogLevel(childModule) == LogLevel::LOGLEVEL_WARNING);

    // 3. Modify the level of the secondary logger to DEBUG
    UT_EXPECT_NO_THROW(
        logger->setLogLevel(childModule, LogLevel::LOGLEVEL_DEBUG);
        );

    // 4. Verify change
    UT_EXPECTED_TRUE(logger->getLogLevel(childModule) == LogLevel::LOGLEVEL_DEBUG);

    // 5. Remove logger
    UT_EXPECT_NO_THROW(
        logger->removeLogger(childModule);
        );

    // 6. Verify that requesting a non-existent logger returns the main logger level
    LogLevel currentMainLevel = logger->getLogLevel("MainID");
    UT_EXPECTED_TRUE(logger->getLogLevel(childModule) == currentMainLevel);
}

/**
 * @brief Tests the initialization of the writing thread and directory creation.
 */
UT_DEFINE_TEST(LeoLogInitializationAndRun)
{
    LeoLog* logger = LeoLog::instance();

    // Prepare temporary directory for logs
    std::string tempLogDir = "./ut_logs";

    // Create directory if it doesn't exist
    if (!std::filesystem::exists(tempLogDir)) {
        std::filesystem::create_directory(tempLogDir);
    }

    // 1. Set directory
    UT_EXPECT_NO_THROW(
        logger->setDir(tempLogDir);
        );

    // 2. Initialize (This starts the Task and internal thread)
    // ID: "UnitTests", File: "test_run", Level: INFO
    UT_EXPECT_NO_THROW(
        logger->init("UnitTests", "test_run", LogLevel::LOGLEVEL_INFO);
        );
}

/**
 * @brief Tests the log submission flow using the public template methods.
 * Since 'log()' is private, we use info(), debug(), error(), etc.
 */
UT_DEFINE_TEST(LeoLogLoggingFlow)
{
    LeoLog* logger = LeoLog::instance();
    std::string testId = "UnitTests";

    // 1. Info Log (Level INFO >= INFO initialized)
    UT_EXPECT_NO_THROW(
        logger->info(testId, "Testing Info Message");
        );

    // 2. Debug Log
    // This calls the template method 'debug', which calls private 'log' with LOGLEVEL_DEBUG.
    UT_EXPECT_NO_THROW(
        logger->debug(testId, "Hidden Debug Message");
        );

    // 3. Error Log
    UT_EXPECT_NO_THROW(
        logger->error(testId, "Critical Error Simulation");
        );

    // 4. Critical Log
    UT_EXPECT_NO_THROW(
        logger->critical(testId, "System Failure Simulation");
        );

    // 5. Variadic Template Test
    // Verify that the template supports multiple arguments (Types&&... msgs)
    UT_EXPECT_NO_THROW(
        int errorCode = 500;
        logger->warning(testId, "Warning with numbers: ", errorCode, " | Status: ", "Fail");
        );

    // 6. Test dynamic attached logger on the fly
    UT_EXPECT_NO_THROW(
        logger->appendLogger("DynamicModule", LogLevel::LOGLEVEL_DEBUG);
        // Using the public template wrapper for debug
        logger->debug("DynamicModule", "Debug message from dynamic module");
        );
}

/**
 * @brief Verifies the controlled shutdown of the logging system.
 */
UT_DEFINE_TEST(LeoLogShutdown)
{
    LeoLog* logger = LeoLog::instance();

    // 1. Stop the logger (must stop the thread and close files)
    UT_EXPECT_NO_THROW(
        logger->stop();
        );

    // 2. Verify that subsequent log calls do not cause a crash
    // The internal implementation should check "if (stopped_) return;"
    UT_EXPECT_NO_THROW(
        logger->error("UnitTests", "This should not be logged (System Stopped)");
        );

    // Optional cleanup
    // std::filesystem::remove_all("./ut_logs");
}


// 3. Main session execution
UT_START_SESSION("LeoLog Unitary Tests")

// Stop if initialization or singleton fails
UT_FORCE_STOP_IF_SUBTEST_FAIL(true);

UT_REGISTER_TEST(Logger, Utils, LeoLogSingletonCheck);
UT_REGISTER_TEST(Logger, Utils, LeoLogConfiguration);
UT_REGISTER_TEST(Logger, Utils, LeoLogInitializationAndRun);

// Keep running flow tests even if one fails
UT_REGISTER_TEST(Logger, Utils, LeoLogLoggingFlow);
UT_REGISTER_TEST(Logger, Utils, LeoLogShutdown);

UT_RUN_TESTS();

UT_FINISH_SESSION()
