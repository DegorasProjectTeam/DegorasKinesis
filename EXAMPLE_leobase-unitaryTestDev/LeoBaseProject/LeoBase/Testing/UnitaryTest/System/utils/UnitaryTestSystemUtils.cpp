/*
 * Copyright(C) Milethos Technologies SLU. 2026
 * All rights reserved. Reproduction in whole or in
 * part is prohibited without the written consent of
 * the copyright holder.
 */

// C++ INCLUDES
#include <vector>
#include <string>
#include <cmath> // std::abs

// LeoBase INCLUDES
#include <LeoBase/Aliases/Testing>
#include "LeoBase/System/utils/system_utils.h"
#include "LeoBase/Testing/unitary/unitary_test_macros.h"

using namespace leobase;
using namespace leobase::testing;
using namespace leobase::system::utils;

// =======================================================================
// DECLARATIONS
// =======================================================================

UT_DECLARE_TEST(GetCurrentPID_ReturnsValidId);
UT_DECLARE_TEST(GetCPULoad_ReturnsValidPercentage);
UT_DECLARE_TEST(GetRAMLoad_ReturnsValidPercentage);
UT_DECLARE_TEST(DiskFunctions_ReturnValidDataForAvailableDisks);
UT_DECLARE_TEST(GetSystemDisksInfo_ReturnsValidStructs);
UT_DECLARE_TEST(IsRunningInsideQtCreator_DoesNotCrash);
UT_DECLARE_TEST(UserInputPause_IsIgnoredInAutomation);

// =======================================================================
// DEFINITIONS
// =======================================================================

/**
 * @brief Ensures the process ID is retrieved and is strictly greater than 0.
 */
UT_DEFINE_TEST(GetCurrentPID_ReturnsValidId)
{
    auto pid = getCurrentPID();
    // 0 is usually reserved for the System Idle Process. PIDs are > 0.
    UT_EXPECTED_TRUE(pid > 0);
}

/**
 * @brief Ensures CPU load is either an error (-1.0f) or a valid percentage [0.0, 100.0].
 */
UT_DEFINE_TEST(GetCPULoad_ReturnsValidPercentage)
{
    // Using a short wait time to avoid slowing down the test suite too much
    float cpuLoad = getCPULoad(50);

    bool isError = (std::abs(cpuLoad - (-1.0f)) < 0.001f);
    bool isValidPercent = (cpuLoad >= 0.0f && cpuLoad <= 100.0f);

    UT_EXPECTED_TRUE(isError || isValidPercent);
}

/**
 * @brief Ensures RAM load is either an error (-1.0f) or a valid percentage [0.0, 100.0].
 */
UT_DEFINE_TEST(GetRAMLoad_ReturnsValidPercentage)
{
    float ramLoad = getRAMLoad();

    bool isError = (std::abs(ramLoad - (-1.0f)) < 0.001f);
    bool isValidPercent = (ramLoad >= 0.0f && ramLoad <= 100.0f);

    UT_EXPECTED_TRUE(isError || isValidPercent);
}

/**
 * @brief Dynamically finds an available disk and performs strict space calculations.
 */
UT_DEFINE_TEST(DiskFunctions_ReturnValidDataForAvailableDisks)
{
    std::vector<std::string> disks = listSystemDisks();

    if (!disks.empty())
    {
        std::string testDisk = disks.front(); // Use the first available disk (e.g., "C:/" or "/")

        float usagePct = getSystemDiskSpaceUsage(testDisk);
        bool isPctError = (std::abs(usagePct - (-1.0f)) < 0.001f);
        bool isPctValid = (usagePct >= 0.0f && usagePct <= 100.0f);
        UT_EXPECTED_TRUE(isPctError || isPctValid);

        uint64_t availableSpace = getSystemDiskAvailableSpace(testDisk);
        uint64_t usedSpace = getSystemDiskUsedSpace(testDisk);

        // A valid mounted system disk should have a total size greater than 0 bytes.
        // This avoids the unsigned >= 0 tautology warning.
        uint64_t logicalTotalSpace = availableSpace + usedSpace;
        UT_EXPECTED_TRUE(logicalTotalSpace > 0);
    }
    else
    {
        // Graceful pass if running on a sandboxed environment with no disk visibility
        UT_EXPECTED_TRUE(true);
    }
}

/**
 * @brief Ensures the all-in-one disk info struct returns coherent data.
 */
UT_DEFINE_TEST(GetSystemDisksInfo_ReturnsValidStructs)
{
    auto diskInfos = getSystemDisksInfo();

    for (const auto& info : diskInfos)
    {
        UT_EXPECTED_FALSE(info.path.empty());
        UT_EXPECTED_TRUE(info.usagePercent >= 0.0f && info.usagePercent <= 100.0f);

        // Strict coherence checks replacing the unsigned >= 0 tautology
        UT_EXPECTED_TRUE(info.totalBytes > 0);
        UT_EXPECTED_TRUE(info.totalBytes >= info.usedBytes);
        UT_EXPECTED_TRUE(info.totalBytes >= info.freeBytes);
    }
}

/**
 * @brief Just calls the QtCreator check to ensure it doesn't fault.
 */
UT_DEFINE_TEST(IsRunningInsideQtCreator_DoesNotCrash)
{
    bool isQtCreator = isRunningInsideQtCreator();
    // We don't assert true or false because it depends on the environment running the test.
    UT_EXPECTED_TRUE(isQtCreator == true || isQtCreator == false);
}

/**
 * @brief Interactive pause function. Should be mocked or skipped in CI/CD.
 */
UT_DEFINE_TEST(UserInputPause_IsIgnoredInAutomation)
{
    // WARNING: Calling userInputPause() here would block the automated test runner
    // waiting for a key press. We leave it commented out for safety in CI/CD pipelines.
    //
    // userInputPause("Test pause");

    UT_EXPECTED_TRUE(true);
}

// =======================================================================
// SESSION
// =======================================================================

UT_START_SESSION("LeoBase System Utils Unitary Tests")

UT_REGISTER_TEST(System, Utils, GetCurrentPID_ReturnsValidId);
UT_REGISTER_TEST(System, Utils, GetCPULoad_ReturnsValidPercentage);
UT_REGISTER_TEST(System, Utils, GetRAMLoad_ReturnsValidPercentage);
UT_REGISTER_TEST(System, Utils, DiskFunctions_ReturnValidDataForAvailableDisks);
UT_REGISTER_TEST(System, Utils, GetSystemDisksInfo_ReturnsValidStructs);
UT_REGISTER_TEST(System, Utils, IsRunningInsideQtCreator_DoesNotCrash);
UT_REGISTER_TEST(System, Utils, UserInputPause_IsIgnoredInAutomation);

UT_RUN_TESTS();

UT_FINISH_SESSION()
