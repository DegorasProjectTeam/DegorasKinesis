/*
 * Copyright(C) Milethos Technologies SLU. 2026
 * All rights reserved. Reproduction in whole or in
 * part is prohibited without the written consent of
 * the copyright holder.
 */

// C++ INCLUDES
#include <string>
#include <vector>

// LeoBase INCLUDES
#include <LeoBase/Aliases/Testing>
#include "LeoBase/System/types/system_types.h"
#include "LeoBase/Testing/unitary/unitary_test_macros.h"

using namespace leobase;
using namespace leobase::testing;
using namespace leobase::system::types;

// =======================================================================
// DECLARATIONS
// =======================================================================

UT_DECLARE_TEST(AssignsAndRetainsData);
UT_DECLARE_TEST(ToJsonStr_GeneratesKeysProperly);
UT_DECLARE_TEST(ToJsonStr_HandlesEmptyPath);
UT_DECLARE_TEST(ToJsonStr_RespectsBracesFlag);
UT_DECLARE_TEST(ToJsonStr_HandlesZeroValues);
UT_DECLARE_TEST(ToJsonStr_HandlesIndentation);

// =======================================================================
// DEFINITIONS
// =======================================================================

/**
 * @brief Tests basic assignment and data retention of the DiskInfo struct.
 */
UT_DEFINE_TEST(AssignsAndRetainsData)
{
    DiskInfo disk;
    disk.path = "C:\\";
    disk.type = DiskType::SSD;
    disk.usagePercent = 45.5f;
    disk.totalBytes = 1000000;
    disk.usedBytes = 455000;
    disk.freeBytes = 545000;

    UT_EXPECTED_EQ(disk.path, std::string("C:\\"));
    UT_EXPECTED_EQ(static_cast<std::uint16_t>(disk.type), static_cast<std::uint16_t>(DiskType::SSD));
    UT_EXPECTED_TRUE(disk.usagePercent > 45.0f && disk.usagePercent < 46.0f);
    UT_EXPECTED_EQ(disk.totalBytes, static_cast<uint64_t>(1000000));
}

/**
 * @brief Ensures the JSON string generation includes all the expected struct keys.
 */
UT_DEFINE_TEST(ToJsonStr_GeneratesKeysProperly)
{
    DiskInfo disk{"/mnt/data", DiskType::HDD, 80.0f, 2000, 1600, 400};
    
    // Act
    std::string jsonStr = disk.toJsonStr();

    // Assert: We check that the string contains the keys
    UT_EXPECTED_FALSE(jsonStr.empty());
    UT_EXPECTED_TRUE(jsonStr.find("\"path\"") != std::string::npos);
    UT_EXPECTED_TRUE(jsonStr.find("\"type\"") != std::string::npos);
    UT_EXPECTED_TRUE(jsonStr.find("\"usagePercent\"") != std::string::npos);
    UT_EXPECTED_TRUE(jsonStr.find("\"totalBytes\"") != std::string::npos);
    UT_EXPECTED_TRUE(jsonStr.find("\"usedBytes\"") != std::string::npos);
    UT_EXPECTED_TRUE(jsonStr.find("\"freeBytes\"") != std::string::npos);
    
    // Ensure the path value is present
    UT_EXPECTED_TRUE(jsonStr.find("/mnt/data") != std::string::npos);
}

/**
 * @brief Tests the ternary operator branch where path is empty (triggers OptionalStr()).
 */
UT_DEFINE_TEST(ToJsonStr_HandlesEmptyPath)
{
    // Path left intentionally empty
    DiskInfo disk{"", DiskType::UNKNOWN, 0.0f, 0, 0, 0};
    
    std::string jsonStr = disk.toJsonStr();

    // The JSON generation should complete without crashing
    UT_EXPECTED_FALSE(jsonStr.empty());
    
    // Depending on how generateJsonStr handles OptionalStr(), it might output "null"
    // or omit it. But at the very least, it shouldn't have an empty string value "".
    UT_EXPECTED_TRUE(jsonStr.find("\"\"") == std::string::npos); 
}

/**
 * @brief Validates the `braces` boolean parameter of toJsonStr.
 */
UT_DEFINE_TEST(ToJsonStr_RespectsBracesFlag)
{
    DiskInfo disk{"/home", DiskType::SSD, 10.0f, 100, 10, 90};

    // Default or braces = true
    std::string jsonWithBraces = disk.toJsonStr(0, 0, true);
    
    // Strip leading/trailing whitespaces to be safe before checking characters
    size_t firstCharIdx = jsonWithBraces.find_first_not_of(" \n\r\t");
    size_t lastCharIdx = jsonWithBraces.find_last_not_of(" \n\r\t");
    
    if (firstCharIdx != std::string::npos && lastCharIdx != std::string::npos) {
        UT_EXPECTED_EQ(jsonWithBraces[firstCharIdx], '{');
        UT_EXPECTED_EQ(jsonWithBraces[lastCharIdx], '}');
    } else {
        UT_EXPECTED_TRUE(false); // String is entirely whitespace or empty
    }

    // Braces = false
    std::string jsonWithoutBraces = disk.toJsonStr(0, 0, false);
    size_t firstCharIdxNoBraces = jsonWithoutBraces.find_first_not_of(" \n\r\t");
    size_t lastCharIdxNoBraces = jsonWithoutBraces.find_last_not_of(" \n\r\t");
    
    if (firstCharIdxNoBraces != std::string::npos && lastCharIdxNoBraces != std::string::npos) {
        UT_EXPECTED_TRUE(jsonWithoutBraces[firstCharIdxNoBraces] != '{');
        UT_EXPECTED_TRUE(jsonWithoutBraces[lastCharIdxNoBraces] != '}');
    }
}

/**
 * @brief Verifies that extreme edge cases (0 values) do not break the formatting.
 */
UT_DEFINE_TEST(ToJsonStr_HandlesZeroValues)
{
    DiskInfo disk{"/dev/null", DiskType::VIRTUAL, 0.0f, 0, 0, 0};
    std::string jsonStr = disk.toJsonStr();

    UT_EXPECTED_FALSE(jsonStr.empty());
    // Checking that a zero is actually serialized
    UT_EXPECTED_TRUE(jsonStr.find("0") != std::string::npos);
}

/**
 * @brief Basic sanity check to ensure the indent size parameter changes the output length.
 */
UT_DEFINE_TEST(ToJsonStr_HandlesIndentation)
{
    DiskInfo disk{"D:", DiskType::REMOVABLE, 50.0f, 1000, 500, 500};
    
    std::string jsonFlat = disk.toJsonStr(0);
    std::string jsonIndented = disk.toJsonStr(4); // 4 spaces indentation

    // An indented JSON string should logically be longer than a flat one
    UT_EXPECTED_TRUE(jsonIndented.length() > jsonFlat.length());
}

// =======================================================================
// SESSION
// =======================================================================

UT_START_SESSION("LeoBase FileSystem Types Unitary Tests")

UT_REGISTER_TEST(System, Types, AssignsAndRetainsData);
UT_REGISTER_TEST(System, Types, ToJsonStr_GeneratesKeysProperly);
UT_REGISTER_TEST(System, Types, ToJsonStr_HandlesEmptyPath);
UT_REGISTER_TEST(System, Types, ToJsonStr_RespectsBracesFlag);
UT_REGISTER_TEST(System, Types, ToJsonStr_HandlesZeroValues);
UT_REGISTER_TEST(System, Types, ToJsonStr_HandlesIndentation);

UT_RUN_TESTS();

UT_FINISH_SESSION()
