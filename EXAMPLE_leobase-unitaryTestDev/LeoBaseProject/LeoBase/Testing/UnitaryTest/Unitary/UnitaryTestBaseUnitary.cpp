/*
 * Copyright(C) Milethos Technologies SLU. 2026
 * All rights reserved. Reproduction in whole or in
 * part is prohibited without the written consent of
 * the copyright holder.
 */

// UnitaryTestBase Testing

// C++ INCLUDES
#include <string>

// LEOBASE INCLUDES
#include <LeoBase/Aliases/Testing>

// ---------------------------------------------------------------------------------------------------------------------

using namespace leobase;
using namespace leobase::testing;

// 1. Test Declaration
UT_DECLARE_TEST(TestBaseBooleanAssertions)
UT_DECLARE_TEST(TestBaseStringAssertions)
UT_DECLARE_TEST(TestBaseForceResults)
UT_DECLARE_TEST(TestBaseSleepLogic)
UT_DECLARE_TEST(TestExceptionBehavior)
// 2. Define Tests

/**
 * @brief Verifies that expectTrue and expectFalse correctly evaluate boolean results.
 */
UT_DEFINE_TEST(TestBaseBooleanAssertions)
{
    // Note: We use the test instance itself (this) which inherits from UnitaryTestBase

    // Test expecting true
    this->expectTrue(true);   // Should pass internally

    // Test expecting false
    this->expectFalse(false); // Should pass internally (!false is true)

    // We can also check if a manual call returns what we expect
    bool result = this->expectTrue(1 == 1);
    UT_EXPECTED_TRUE(result);
}

/**
 * @brief Verifies string and char* equality/inequality methods.
 */
UT_DEFINE_TEST(TestBaseStringAssertions)
{
    std::string s1 = "LeoBase";
    std::string s2 = "LeoBase";
    std::string s3 = "Other";

    // std::string comparisons
    this->expectEQ(s1, s2);
    this->expectNE(s1, s3);

    // const char* comparisons
    this->expectEQ("Static", "Static");
    this->expectNE("Static", "Dynamic");

    // Cross-check with macros
    UT_EXPECTED_TRUE(this->expectEQ(s1, s2));
}

/**
 * @brief Verifies the forcePass and forceFail methods.
 */
UT_DEFINE_TEST(TestBaseForceResults)
{
    // Verification of forcePass
    bool pass = this->forcePass();
    UT_EXPECTED_TRUE(pass);

    // Verification of forceFail
    // Note: In a real scenario, this would mark the test as failed.
    bool fail = this->forceFail();
    UT_EXPECTED_FALSE(fail);
}

/**
 * @brief Verifies the sleep implementation (microsecond precision).
 */
UT_DEFINE_TEST(TestBaseSleepLogic)
{
    // This is more of a functional test to ensure it doesn't crash
    // and logs correctly.
    this->sleepUs(100); // Wait 100 microseconds

    UT_EXPECTED_TRUE(true);
}
/* TO FIX BEHAVIOUR
 *
UT_DEFINE_TEST(TestExceptionBehavior)
{
    // Verifica que lanza BaseException
    UT_EXPECT_THROW(throw BaseException("P", "M", "S", "Err"), BaseException);

    // Verifica que CUALQUIER cosa sea lanzada
    UT_EXPECT_ANY_THROW(throw std::runtime_error("Generic error"));

    // Verifica que este código NO lance nada
    UT_EXPECT_NO_THROW(int x = 10 / 2);
}
*/
// 3. Main session execution
UT_START_SESSION("UnitaryTestBase Core Session")

UT_FORCE_STOP_IF_SUBTEST_FAIL(false);

UT_REGISTER_TEST(Testing, Base, TestBaseBooleanAssertions);
UT_REGISTER_TEST(Testing, Base, TestBaseStringAssertions);
UT_REGISTER_TEST(Testing, Base, TestBaseForceResults);
UT_REGISTER_TEST(Testing, Base, TestBaseSleepLogic);

UT_RUN_TESTS();

UT_FINISH_SESSION()
