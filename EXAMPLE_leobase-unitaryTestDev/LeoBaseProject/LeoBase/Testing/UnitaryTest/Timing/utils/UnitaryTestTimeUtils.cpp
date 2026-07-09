/*
 * Copyright(C) Milethos Technologies SLU. 2026
 * All rights reserved. Reproduction in whole or in
 * part is prohibited without the written consent of
 * the copyright holder.
 */

// TimeUtils Testing

// C++ INCLUDES
#include <string>
#include <chrono>
#include <thread>

// LEOBASE INCLUDES
#include <LeoBase/Aliases/Testing>
#include <LeoBase/Timing/utils/time_utils.h>
#include <LeoBase/Timing/types/base_time_types.h>
#include <LeoBase/Exceptions/leobase_exception.h>

// ---------------------------------------------------------------------------------------------------------------------

using namespace leobase;
using namespace leobase::testing;
using namespace leobase::timing::utils;
using namespace leobase::exceptions; //  LeoBaseException

// 1. Test Declaration
UT_DECLARE_TEST(TestMonthParsing)
UT_DECLARE_TEST(TestIsoValidation)
UT_DECLARE_TEST(TestIsoConversionRoundTrip)
UT_DECLARE_TEST(TestTimeStringFormatting)
UT_DECLARE_TEST(TestDateConstruction)
UT_DECLARE_TEST(TestClockOffsets)

// 2. Define Tests

/**
 * @brief Verifies string to month number conversion (case insensitive).
 */
UT_DEFINE_TEST(TestMonthParsing)
{
    // Short names
    UT_EXPECTED_EQ(monthStrToNumber("jan"), 1u);
    UT_EXPECTED_EQ(monthStrToNumber("DEC"), 12u);
    
    // Full names
    UT_EXPECTED_EQ(monthStrToNumber("February"), 2u);
    UT_EXPECTED_EQ(monthStrToNumber("august"), 8u);

    // Invalid months should throw LeoBaseException
    UT_EXPECT_THROW(monthStrToNumber("NotAMonth"), LeoBaseException);
    UT_EXPECT_THROW(monthStrToNumber(""), LeoBaseException);
}

/**
 * @brief Verifies the Regex validation for ISO8601 strings.
 */
UT_DEFINE_TEST(TestIsoValidation)
{
    // Extended format
    UT_EXPECTED_TRUE(isValidIso8601Datetime("2025-01-01T12:00:00Z"));
    UT_EXPECTED_TRUE(isValidIso8601Datetime("2025-01-01T12:00:00.123Z"));
    
    // Basic format
    UT_EXPECTED_TRUE(isValidIso8601Datetime("20250101T120000Z"));
    
    // Invalid formats
    UT_EXPECTED_FALSE(isValidIso8601Datetime("2025/01/01"));
    UT_EXPECTED_FALSE(isValidIso8601Datetime("RandomString"));
    UT_EXPECTED_FALSE(isValidIso8601Datetime("2025-01-01 12:00:00")); // Missing T
}


/**
 * @brief Checks conversion from ISO string to TimePoint and back.
 */
UT_DEFINE_TEST(TestIsoConversionRoundTrip)
{
    std::string original = "2025-05-10T15:30:45Z";
    
    // String -> TimePoint
    auto tp = iso8601ToResTimepoint(original);
    
    // TimePoint -> String (Seconds resolution to match input)
    std::string result = timepointToIso8601(tp, leobase::timing::types::TimeResolution::SECONDS, true, false);
    
    UT_EXPECTED_EQ(original, result);

    // Test with milliseconds
    std::string original_ms = "2025-05-10T15:30:45.123Z";
    auto tp_ms = iso8601ToResTimepoint(original_ms);
    std::string result_ms = timepointToIso8601(tp_ms, leobase::timing::types::TimeResolution::MILLISECONDS, true, false);
    
    UT_EXPECTED_EQ(original_ms, result_ms);
}

/**
 * @brief Tests formatting options (trailing zeros, resolutions).
 */
UT_DEFINE_TEST(TestTimeStringFormatting)
{
    // Construct a specific time manually: 2025-01-01 12:00:00.500
    auto tp = dateAndTimeToTimePoint(2025, 1, 1, 12, 0, 0);
    tp += std::chrono::milliseconds(500);

    // Test removing trailing zeros .500 -> .5
    std::string formatted = timepointToString(tp, "%Y-%m-%dT%H:%M:%S", leobase::timing::types::TimeResolution::MILLISECONDS, true, true);
    
    bool contains_partial = formatted.find(".5") != std::string::npos;
    bool contains_full = formatted.find(".500") != std::string::npos;
    
    UT_EXPECTED_TRUE(contains_partial);
    UT_EXPECTED_FALSE(contains_full);

    // Test keeping trailing zeros
    std::string formatted_full = timepointToString(tp, "%Y-%m-%dT%H:%M:%S", leobase::timing::types::TimeResolution::MILLISECONDS, true, false);
    UT_EXPECTED_TRUE(formatted_full.find(".500") != std::string::npos);
}

/**
 * @brief Tests manual date construction and seconds in day calculation.
 */
UT_DEFINE_TEST(TestDateConstruction)
{
    // 01:00:00 UTC
    auto tp = dateAndTimeToTimePoint(2025, 1, 1, 1, 0, 0);
    
    // Should be 3600 seconds from start of that day
    long double seconds = timePointToSecsDay(tp);
    
    UT_EXPECTED_EQ(static_cast<int>(seconds), 3600);
}

/**
 * @brief Test for offset computations (ensure they don't crash).
 */
UT_DEFINE_TEST(TestClockOffsets)
{
    // We can't predict the offset, but we can ensure the function runs
    // and returns a duration (implied by not crashing).
    
    UT_EXPECT_NO_THROW({
        auto offset1 = computeOffsetSteadyToSystem();
        auto offset2 = computeOffsetHighResToSystem();
        (void)offset1; 
        (void)offset2;
    });

    // Test getting current string time
    std::string now_str = currentDatetimeIso8601(leobase::timing::types::TimeResolution::SECONDS, true, false, leobase::timing::types::ClockType::SYSTEM);
    UT_EXPECTED_FALSE(now_str.empty());
}

// 3. Main session execution
UT_START_SESSION("Timing Utils Test Session")

UT_FORCE_STOP_IF_SUBTEST_FAIL(false);

// Register Tests
UT_REGISTER_TEST(Timing, Utils, TestMonthParsing);
UT_REGISTER_TEST(Timing, Utils, TestIsoValidation);
UT_REGISTER_TEST(Timing, Utils, TestIsoConversionRoundTrip);
UT_REGISTER_TEST(Timing, Utils, TestTimeStringFormatting);
UT_REGISTER_TEST(Timing, Utils, TestDateConstruction);
UT_REGISTER_TEST(Timing, Utils, TestClockOffsets);

UT_RUN_TESTS();

UT_FINISH_SESSION();
