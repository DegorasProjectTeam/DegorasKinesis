/*
 *      Copyright(C) Milethos Technologies SLU. 2025
 *      All rights reserved. Reproduction in whole or in
 *      part is prohibited without the written consent of
 *      the copyright holder.
 */

#pragma once

// C++ INCLUDES
#include <string>

// LEOBASE INCLUDES
#include "LeoBase/Aliases/Global"
#include "LeoBase/Timing/time_constants.h"
#include "LeoBase/Timing/types/base_time_types.h"

// LEOBASE MACROS
LEOBASE_MODULE_BEGIN(timing)
BEGIN_NAMESPACE(utils)

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Gets the current time as a time_point of the specified clock type.
 * @tparam Clock The clock type to use.
 * @return The current time as a std::chrono::time_point of the chosen clock.
 */
template <typename Clock = types::SysClock>
std::chrono::time_point<Clock> currentTimepoint()
{
    return Clock::now();
}

/**
 * @brief Converts a duration to a floating-point representation of a specified unit.
 *
 * This function converts any `std::chrono::duration` into a floating-point value
 * representing the time in a chosen resolution: seconds, milliseconds, microseconds, or nanoseconds.
 *
 * @tparam DurationType The input duration type (e.g., `std::chrono::nanoseconds`).
 * @param duration The duration to be converted.
 * @param resolution The target time resolution for the output value.
 * @return A `long double` representing the duration in the specified resolution.
 *
 * @note This function ensures high precision by explicitly casting the duration count to `long double`.
 */
template <typename DurationType>
long double convertDuration(const DurationType& duration, types::TimeResolution resolution)
{
    switch (resolution)
    {
    case types::TimeResolution::SECONDS:
        return static_cast<long double>(duration.count()) / kNsPerS;
    case types::TimeResolution::MILLISECONDS:
        return static_cast<long double>(duration.count()) / kNsPerMs;
    case types::TimeResolution::MICROSECONDS:
        return static_cast<long double>(duration.count()) / kNsPerUs;
    case types::TimeResolution::NANOSECONDS:
    default:
        return static_cast<long double>(duration.count());
    }
}

/**
 * @brief Computes the time difference (offset) between steady_clock and system_clock.
 * @return The offset as `std::chrono::nanoseconds`.
 */
LEOBASE_EXPORT types::Ns computeOffsetSteadyToSystem();

/**
 * @brief Computes the time difference (offset) between high_resolution_clock and system_clock.
 * @return The offset as `std::chrono::nanoseconds`.
 */
LEOBASE_EXPORT types::Ns computeOffsetHighResToSystem();

/**
 * Converts a time point to a string representation with specified format and resolution.
 *
 * This function formats a given time point according to the specified strftime-compatible
 * format string. The function allows specifying the time resolution for the fractional part
 * of the seconds.It supports displaying the time in either UTC or local time.
 *
 * @warning This function relies on std::gmtime and std::localtime, which are based on the
 * time_t type. Consequently, it may not correctly handle dates before January 1, 1970.
 *
 * @param tp The time point to format.
 * @param format The strftime format string for the date and time part.
 * @param resolution The resolution (seconds, milliseconds, microseconds, nanoseconds) for the time part.
 * @param utc Boolean flag to use UTC (true) or local time (false) for formatting.
 * @param rm_trailing_zeros If `true`, removes trailing zeros from the fractional part.
 * @return A string representation of the time point according to the specified format and resolution.
 *
 * @throws std::runtime_error If there's an error during formatting.
 */
LEOBASE_EXPORT std::string timepointToString(const types::ResTimePoint& tp,
                                             const std::string& format = "%Y-%m-%dT%H:%M:%S",
                                             types::TimeResolution res = types::TimeResolution::MILLISECONDS,
                                             bool utc = true,
                                             bool rm_trailing_zeros = false);

/**
 * Converts a high-resolution time point to an ISO 8601 formatted string.
 *
 * This function formats a given time point into a string following the ISO 8601 standard. The function
 * allows specifying the time resolution for the fractional part of the seconds. If the `utc`
 * flag is set to true, the formatted string is suffixed with 'Z' to indicate that the time is in
 * Coordinated Universal Time (UTC). If `utc` is false, the function does not qualify the time zone,
 * and the resulting string represents local time, unqualified by a specific time zone.
 *
 * @param tp The high-resolution time point to format.
 * @param resolution The desired resolution for the fractional part of the seconds in the output.
 * @param utc Boolean flag to use UTC (true) or local time (false) for formatting.
 * @param rm_trailing_zeros If `true`, removes trailing zeros from the fractional part.
 * @return A std::string containing the formatted date and time in ISO 8601 format.
 *
 * @warning If `utc` is false, the resulting string represents local time without specifying the
 * time zone. This means that the time zone qualification (e.g., 'Z' for UTC) is omitted, and the
 * time is assumed to be undefined local.
 *
 * @throws std::runtime_error If there's an error during formatting.
 */
LEOBASE_EXPORT types::Iso8601Str timepointToIso8601(const types::ResTimePoint& tp,
                                                    types::TimeResolution res = types::TimeResolution::MILLISECONDS,
                                                    bool utc = true,
                                                    bool rm_trailing_zeros = false);

/**
 * @brief Converts a steady clock time point to ISO 8601 format.
 *
 * Since `std::chrono::steady_clock` does not represent real-world time,
 * we convert it to `std::chrono::system_clock` using a reference point.
 *
 * @param tp The `std::chrono::steady_clock::time_point` to convert.
 * @param resolution The desired time resolution (`SECONDS`, `MILLISECONDS`, etc.).
 * @param utc Whether to return UTC time (`true`) or local time (`false`).
 * @param rm_trailing_zeros If `true`, removes trailing zeros from the fractional part.
 * @return The formatted timestamp string.
 */
LEOBASE_EXPORT types::Iso8601Str timepointToIso8601(const types::StdyTimePoint& tp,
                                                    types::TimeResolution resolution,
                                                    bool utc = true,
                                                    bool rm_trailing_zeros = false);

/**
 * @brief Generates the current date and time as a string formatted according to ISO 8601.
 *
 * This function captures the current time using the specified clock type (`ClockType`)
 * and formats it into a standardized ISO 8601 date and time string. The function allows
 * specifying the time resolution for the fractional part of the seconds. If the `utc` flag
 * is set to `true`, the formatted string is suffixed with 'Z' to indicate that the time is in
 * Coordinated Universal Time (UTC). If `utc` is `false`, the resulting string represents the
 * local time without timezone information.
 *
 * @param resolution The desired resolution for the fractional part of the seconds in the output.
 *                   Options include seconds, milliseconds, microseconds, and nanoseconds.
 * @param utc Boolean flag to use UTC (true) or local time (false) for formatting.
 * @param rm_trailing_zeros If `true`, removes trailing zeros from the fractional part.
 * @param clock_type Specifies the clock type to use when capturing the current time (default `ClockType::SYSTEM`).
 * @return A `types::Iso8601Str` (alias for `std::string`) containing the formatted date and time in ISO 8601 format.
 *
 * @warning The precision of the clock and the specified resolution may affect the accuracy of the output. Also
 * when using `utc=false`, the output string does not include timezone information. The steady clock should be used
 * with caution, as it does not represent real-world time.
 *
 * @note The default clock type is `ClockType::SYSTEM`, as it is the most suitable for logging and database storage.
 * Also, the high resolution clock may not always provide better accuracy than system clock, depending on the platform.
 */
LEOBASE_EXPORT types::Iso8601Str currentDatetimeIso8601(types::TimeResolution res = types::TimeResolution::MILLISECONDS,
                                                        bool utc = true,
                                                        bool rm_trailing_zeros = false,
                                                        types::ClockType clock_type = types::ClockType::SYSTEM);

/**
 * @brief Parses an ISO 8601 UTC datetime string and converts it to a ResTimePoint.
 *
 * This function parses a UTC datetime string formatted according to ISO 8601, supporting both extended
 * and basic formats, including up to nanosecond precision, and converts it into a ResTimePoint time point.
 * The input string must conform to the ISO 8601 formats:
 * - Extended format: "YYYY-MM-DDTHH:MM:SS.sssZ"
 * - Basic format: "YYYYMMDDTHHMMSS.sssZ"
 * In both formats, the fractional seconds (".sss") are optional and can represent milliseconds up to nanoseconds.
 *
 * @param datetime The ISO 8601 UTC datetime string to be parsed, in either extended or basic format.
 * @return ResTimePoint A time point representing the specified datetime.
 *
 * @throws std::invalid_argument If the input string does not match the ISO 8601 format.
 *
 * @warning This function uses regular expressions, which may not be fully supported or performant on some older
 * compiler versions. Ensure compatibility with your compiler's regex implementation when using this function.
 */
LEOBASE_EXPORT types::ResTimePoint iso8601ToResTimepoint(const types::Iso8601Str& datetime);

/**
 * @brief Get seconds of day from timepoint
 * @param tp The timepoint.
 * @return The seconds elapsed since day start with fraction.
 */
LEOBASE_EXPORT long double timePointToSecsDay(const types::ResTimePoint& tp);

/**
 * @brief Calculates the number of days since the civil date of 1970-01-01.
 *
 * This function computes the number of days from the Gregorian calendar date
 * specified by the year (y), month (m), and day (d) parameters to the epoch date
 * of 1970-01-01. Negative return values indicate dates prior to 1970-01-01. The
 * input date must be in the civil (Gregorian) calendar.
 *
 * @param y The year of the date, can be any integer representing the year.
 * @param m The month of the date, must be in the range [1, 12].
 * @param d The day of the month, must be in the valid range for the given month and year.
 * @return The number of days since 1970-01-01. Negative values indicate dates before 1970-01-01.
 *
 * @warning The function uses static_assert to ensure that the size of unsigned and int types
 * meets the minimum requirements for the calculation.
 */
LEOBASE_EXPORT long long daysFromCivil(types::Year y, types::Month m, types::Day d);

/**
 * @brief Convert a given date and time to a ResTimePoint.
 *
 * This function converts the provided date and time components to a ResTimePoint
 * based on the high_resolution_clock clock.
 *
 * @param y   The year.
 * @param m   The month (1-12).
 * @param d   The day of the month.
 * @param h   The hour of the day.
 * @param min The minute of the hour.
 * @param s   The second of the minute.
 * @return The ResTimePoint representing the specified date and time.
 */
LEOBASE_EXPORT types::ResTimePoint dateAndTimeToTimePoint(types::Year y,
                                                          types::Month m,
                                                          types::Day d,
                                                          types::Hour h = 0,
                                                          types::Min min = 0,
                                                          types::Sec s = 0);

LEOBASE_EXPORT unsigned monthStrToNumber(const std::string& month_str);

// TODO TEST
LEOBASE_EXPORT bool isValidIso8601Datetime(const std::string &datetime);

// ---------------------------------------------------------------------------------------------------------------------

END_NAMESPACE(utils)
LEOBASE_MODULE_END
