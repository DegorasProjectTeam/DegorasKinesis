/*
 * Copyright(C) Milethos Technologies SLU. 2026
 * All rights reserved. Reproduction in whole or in
 * part is prohibited without the written consent of
 * the copyright holder.
 */

// LeoUUID UnitaryTest

// C++ INCLUDES
#include <array>
#include <cstddef>
#include <string>

// LEOBASE INCLUDES
#include <LeoBase/Aliases/Testing>
#include "LeoBase/Crypto/leo_uuid.h"

// ---------------------------------------------------------------------------------------------------------------------

using namespace leobase;
using namespace leobase::crypto;

// ---------------------------------------------------------------------------------------------------------------------
// 1. Test Declarations
// ---------------------------------------------------------------------------------------------------------------------

// --- Initialization & State Tests ---
UT_DECLARE_TEST(LeoUUID_DefaultConstructorAndClear);
UT_DECLARE_TEST(LeoUUID_Constructor_Parameterized);
UT_DECLARE_TEST(LeoUUID_CopyAndAssignmentSemantics);

// --- Formatting Tests ---
UT_DECLARE_TEST(LeoUUID_RFC4122String_Sequential);
UT_DECLARE_TEST(LeoUUID_RFC4122String_BoundaryValues);

// --- Comparison Tests ---
UT_DECLARE_TEST(LeoUUID_EqualityOperators);
UT_DECLARE_TEST(LeoUUID_Comparison_Basic);
UT_DECLARE_TEST(LeoUUID_Comparison_Lexicographical);

// ---------------------------------------------------------------------------------------------------------------------
// 2. Test Definitions
// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Verifies that a default UUID is completely cleared (all bytes to 0x00)
 * and that the explicit clear() method resets an existing UUID.
 */
UT_DEFINE_TEST(LeoUUID_DefaultConstructorAndClear)
{
    LeoUUID uuid;
    const auto& bytes = uuid.getBytes();

    bool allZeros = true;
    for(auto b : bytes) {
        if (b != std::byte{0}) {
            allZeros = false;
            break;
        }
    }
    UT_EXPECTED_TRUE(allZeros);

    // Test explicit clear on a fully populated UUID
    std::array<std::byte, 16> data;
    data.fill(std::byte{0xFF});
    LeoUUID uuidFull(data);

    uuidFull.clear();
    UT_EXPECTED_TRUE(uuidFull == uuid);
}

/**
 * @brief Ensures the parameterized constructor correctly copies the provided byte array.
 */
UT_DEFINE_TEST(LeoUUID_Constructor_Parameterized)
{
    std::array<std::byte, 16> data;
    for(size_t i = 0; i < 16; ++i) {
        data[i] = static_cast<std::byte>(i * 10);
    }

    LeoUUID uuid(data);
    const auto& storedBytes = uuid.getBytes();

    bool match = true;
    for(size_t i = 0; i < 16; ++i) {
        if (storedBytes[i] != data[i]) {
            match = false;
            break;
        }
    }
    UT_EXPECTED_TRUE(match);
}

/**
 * @brief Verifies compiler-generated copy constructor and assignment operator.
 */
UT_DEFINE_TEST(LeoUUID_CopyAndAssignmentSemantics)
{
    std::array<std::byte, 16> data;
    data.fill(std::byte{0xAA});

    LeoUUID original(data);

    // Test Copy Constructor
    LeoUUID copied(original);
    UT_EXPECTED_TRUE(copied == original);

    // Test Assignment Operator
    LeoUUID assigned;
    assigned = original;
    UT_EXPECTED_TRUE(assigned == original);

    // Modify original to ensure deep copy
    original.clear();
    UT_EXPECTED_TRUE(copied != original);
    UT_EXPECTED_TRUE(assigned != original);
}

/**
 * @brief Verifies the correct formatting of the UUID string according to RFC 4122
 * using sequential data.
 */
UT_DEFINE_TEST(LeoUUID_RFC4122String_Sequential)
{
    std::array<std::byte, 16> data;
    for(size_t i = 0; i < 16; ++i) {
        data[i] = static_cast<std::byte>(i);
    }

    LeoUUID uuid(data);
    std::string expectedStr = "00010203-0405-0607-0809-0a0b0c0d0e0f";

    UT_EXPECTED_TRUE(uuid.toRFC4122String() == expectedStr);
}

/**
 * @brief Verifies string formatting with upper boundary values (0xFF).
 * To prevent bugs where:
 *
 * static_cast<int>(std::byte) might sign-extend
 * and print "ffffffff" instead of "ff".
 *
 *
 */
UT_DEFINE_TEST(LeoUUID_RFC4122String_BoundaryValues)
{
    std::array<std::byte, 16> maxData;
    maxData.fill(std::byte{0xFF});
    LeoUUID maxUuid(maxData);

    std::string expectedMaxStr = "ffffffff-ffff-ffff-ffff-ffffffffffff";
    UT_EXPECTED_TRUE(maxUuid.toRFC4122String() == expectedMaxStr);
}

/**
 * @brief Verifies == and != operators.
 */
UT_DEFINE_TEST(LeoUUID_EqualityOperators)
{
    std::array<std::byte, 16> data1, data2;
    data1.fill(std::byte{0xAA});
    data2.fill(std::byte{0xAA});

    LeoUUID uuid1(data1);
    LeoUUID uuid2(data2);
    LeoUUID uuid3; // All zeros

    UT_EXPECTED_TRUE(uuid1 == uuid2);
    UT_EXPECTED_FALSE(uuid1 == uuid3);
    UT_EXPECTED_TRUE(uuid1 != uuid3);

    // Self equality
    UT_EXPECTED_TRUE(uuid1 == uuid1);
    UT_EXPECTED_FALSE(uuid1 != uuid1);
}

/**
 * @brief Verifies <, >, <=, >= operators using completely different arrays.
 */
UT_DEFINE_TEST(LeoUUID_Comparison_Basic)
{
    std::array<std::byte, 16> smallData, largeData;
    smallData.fill(std::byte{0x01});
    largeData.fill(std::byte{0x02});

    LeoUUID small(smallData);
    LeoUUID large(largeData);

    UT_EXPECTED_TRUE(small < large);
    UT_EXPECTED_TRUE(large > small);
    UT_EXPECTED_TRUE(small <= large);
    UT_EXPECTED_TRUE(large >= small);

    // Self comparison
    UT_EXPECTED_FALSE(small < small);
    UT_EXPECTED_TRUE(small <= small);
    UT_EXPECTED_TRUE(large >= large);
}

/**
 * @brief Tests lexicographical comparison edge cases (differing only by one byte).
 */
UT_DEFINE_TEST(LeoUUID_Comparison_Lexicographical)
{
    std::array<std::byte, 16> baseData;
    baseData.fill(std::byte{0x55});
    LeoUUID base(baseData);

    // Create an array that is larger only in the very last byte
    std::array<std::byte, 16> lastByteLarger = baseData;
    lastByteLarger[15] = std::byte{0x56};
    LeoUUID largerAtEnd(lastByteLarger);

    UT_EXPECTED_TRUE(base < largerAtEnd);

    // Create an array that is larger in the first byte, but smaller everywhere else
    std::array<std::byte, 16> firstByteLarger;
    firstByteLarger.fill(std::byte{0x00});
    firstByteLarger[0] = std::byte{0x56};
    LeoUUID largerAtStart(firstByteLarger);

    UT_EXPECTED_TRUE(base < largerAtStart);
}

// ---------------------------------------------------------------------------------------------------------------------
// 3. Execution Session
// ---------------------------------------------------------------------------------------------------------------------

UT_START_SESSION("LeoUUID Exhaustive Test Session")

UT_FORCE_STOP_IF_SUBTEST_FAIL(false);

// Initialization
UT_REGISTER_TEST(Crypto, Identity, LeoUUID_DefaultConstructorAndClear);
UT_REGISTER_TEST(Crypto, Identity, LeoUUID_Constructor_Parameterized);
UT_REGISTER_TEST(Crypto, Identity, LeoUUID_CopyAndAssignmentSemantics);

// Formatting
UT_REGISTER_TEST(Crypto, Identity, LeoUUID_RFC4122String_Sequential);
UT_REGISTER_TEST(Crypto, Identity, LeoUUID_RFC4122String_BoundaryValues);

// Comparisons
UT_REGISTER_TEST(Crypto, Identity, LeoUUID_EqualityOperators);
UT_REGISTER_TEST(Crypto, Identity, LeoUUID_Comparison_Basic);
UT_REGISTER_TEST(Crypto, Identity, LeoUUID_Comparison_Lexicographical);

UT_RUN_TESTS();

UT_FINISH_SESSION()
