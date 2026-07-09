/*
 * Copyright(C) Milethos Technologies SLU. 2026
 * All rights reserved. Reproduction in whole or in
 * part is prohibited without the written consent of
 * the copyright holder.
 */

// AnyEqual UnitaryTest

// C++ INCLUDES
#include <iostream>
#include <thread>
#include <cstdint>
#include <string>
#include <any>

// LEOBASE INCLUDES
#include <LeoBase/Aliases/Testing>
#include <LeoBase/Aliases/Containers>

#include <LeoBase/Containers/any_equal.h>


// ---------------------------------------------------------------------------------------------------------------------

using namespace leobase;
using namespace leobase::containers;

// 1. Test Declaration
UT_DECLARE_TEST(FullTypeCoverageEquality)
UT_DECLARE_TEST(DifferentTypeSameValue)
UT_DECLARE_TEST(SameTypeDifferentValue)
UT_DECLARE_TEST(UnsupportedTypeException)

// 2. Define Tests

/**
 * @brief Tests all supported types to ensure equality works for each registered type.
 */
UT_DEFINE_TEST(FullTypeCoverageEquality)
{
    leobase::containers::AnyEqual ae;

    // Standard Integer Types (Single word types work fine with parentheses)
    UT_EXPECTED_TRUE(ae(std::any(int(10)),            std::any(int(10))));
    UT_EXPECTED_TRUE(ae(std::any(short(5)),           std::any(short(5))));
    UT_EXPECTED_TRUE(ae(std::any(long(100)),          std::any(long(100))));

    // Use uniform initialization {} for multi-word types to avoid confusion
    UT_EXPECTED_TRUE(ae(std::any{ (long long)1000 },  std::any{ (long long)1000 }));

    // Unsigned Integer Types
    // Using explicit casting with parentheses inside std::any solves the macro parsing issue
    UT_EXPECTED_TRUE(ae(std::any{ (unsigned int)10 },   std::any{ (unsigned int)10 }));
    UT_EXPECTED_TRUE(ae(std::any{ (unsigned short)5 },  std::any{ (unsigned short)5 }));
    UT_EXPECTED_TRUE(ae(std::any{ (unsigned long)100 }, std::any{ (unsigned long)100 }));
    UT_EXPECTED_TRUE(ae(std::any{ (unsigned long long)1000 }, std::any{ (unsigned long long)1000 }));

    // Specific Fixed-Width Types (Single-word aliases work fine)
    UT_EXPECTED_TRUE(ae(std::any(std::int8_t(1)),     std::any(std::int8_t(1))));
    UT_EXPECTED_TRUE(ae(std::any(std::uint8_t(10)),   std::any(std::uint8_t(10))));

    // Other supported types
    UT_EXPECTED_TRUE(ae(std::any(std::size_t(1024)),  std::any(std::size_t(1024))));
    UT_EXPECTED_TRUE(ae(std::any(std::string("Leo")), std::any(std::string("Leo"))));

    const char* text = "Base";
    UT_EXPECTED_TRUE(ae(std::any(text),               std::any(text)));
}
/**
 * @brief Tests that different types result in false even if they represent the same numerical value.
 */
UT_DEFINE_TEST(DifferentTypeSameValue)
{
    leobase::containers::AnyEqual ae;

    // Although both represent '5', typeid() is different (int vs long)
    UT_EXPECTED_FALSE(ae(std::any(int(5)),std::any(long(5))));
    UT_EXPECTED_FALSE(ae(std::any(std::uint8_t(1)), std::any(std::int8_t(1))));
    UT_EXPECTED_FALSE(ae(std::any(std::string("1")), std::any(int(1))));

}

/**
 * @brief Tests that the same type with different values correctly returns false.
 */
UT_DEFINE_TEST(SameTypeDifferentValue)
{
    leobase::containers::AnyEqual ae;

    UT_EXPECTED_FALSE(ae(std::any(int(10)),     std::any(int(20))));
    UT_EXPECTED_FALSE(ae(std::any(std::string("A")), std::any(std::string("B"))));
    UT_EXPECTED_FALSE(ae(std::any(std::uint64_t(100)), std::any(std::uint64_t(200))));
}

/**
 * @brief Verifies that using a type not present in the supported list triggers a LeoBaseException.
 */

UT_DEFINE_TEST(UnsupportedTypeException)
{
    leobase::containers::AnyEqual ae;

    try {
        // Double is not supported in the current AnyEqual.cpp implementation
        double d = 3.1415;
        ae(std::any(d), std::any(d));
        UT_FORCE_FAIL(); // Should not reach this line
    } catch (const leobase::exceptions::LeoBaseException& e) {
        // Success: Exception caught as expected
        UT_EXPECTED_TRUE(true);
    } catch (...) {
        // Failed: Threw something that is not LeoBaseException
        UT_FORCE_FAIL();
    }
}

// 3. Main session execution
UT_START_SESSION("AnyEqual Full Coverage Session")

UT_FORCE_STOP_IF_SUBTEST_FAIL(false);

UT_REGISTER_TEST(Containers, Utilities, FullTypeCoverageEquality);
UT_REGISTER_TEST(Containers, Utilities, DifferentTypeSameValue);
UT_REGISTER_TEST(Containers, Utilities, SameTypeDifferentValue);
UT_REGISTER_TEST(Containers, Utilities, UnsupportedTypeException);

UT_RUN_TESTS();

UT_FINISH_SESSION()
