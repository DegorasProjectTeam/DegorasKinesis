/*
 *      Copyright(C) Milethos Technologies SLU. 2025
 *      All rights reserved. Reproduction in whole or in
 *      part is prohibited without the written consent of
 *      the copyright holder.
 */

#pragma once

// C++ INCLUDES
#include <chrono>
#include <string>

// LEOBASE INCLUDES
#include "LeoBase/Aliases/Global"

// LEOBASE MACROS
LEOBASE_MODULE_BEGIN(timing)
BEGIN_NAMESPACE(types)

/// High resolution clock.
using ResClock = std::chrono::high_resolution_clock;

/// System clock.
using SysClock = std::chrono::system_clock;

/// Steady clock.
using StdyClock = std::chrono::steady_clock;

/**
 * @brief High-resolution clock time point used for storing precise timestamps.
 *
 * This type represents time points based on `std::chrono::high_resolution_clock`, which provides
 * the highest available precision for timing operations. However, the epoch (starting point) of
 * this clock is not necessarily aligned with the system clock, meaning direct conversion to system
 * time may require additional adjustments.
 *
 * It is best used for precise measurements and event timestamping when high
 * accuracy is needed.
 *
 * @note Depending on the platform, may be an alias for either `system_clock` or `steady_clock`.
 */
using ResTimePoint = ResClock::time_point;

/**
 * @brief System clock time point used for storing real-world datetime values.
 *
 * This type is based on `std::chrono::system_clock`, which represents the current time according
 * to the system's wall clock. It is suitable for storing timestamps that correspond to actual
 * calendar dates and times.
 *
 * Since this clock can be synchronized with external time sources (e.g., NTP), it is ideal for
 * logging events that need to be associated with real-world time.
 *
 * @note This clock is subject to system clock adjustments like daylight saving time changes or
 * manual user modifications).
 */
using SysTimePoint = SysClock::time_point;

/**
 * @brief Steady clock time point for measuring time intervals.
 *
 * This type is based on `std::chrono::steady_clock`, which guarantees a monotonic clock that is
 * not affected by system clock changes. It is the best choice for measuring elapsed time and
 * time intervals reliably.
 *
 * This clock is not tied to real-world time and cannot be converted to a calendar-based
 * timestamp. It is ideal for timing operations where consistency and stability are required.
 *
 * @note This clock is immune to system clock adjustments.
 */
using StdyTimePoint = StdyClock::time_point;

/// Short way of referring to seconds.
using Secs = std::chrono::seconds;

/// Short way of referring to milliseconds.
using Ms = std::chrono::milliseconds;

/// Short way of referring to microseconds.
using Us = std::chrono::microseconds;

/// Short way of referring to nanoseconds.
using Ns = std::chrono::nanoseconds;

/// Alias for ISO 8601 string.
using Iso8601Str = std::string;

using Year = int;

using Month = unsigned;

using Day = unsigned;

using Hour = unsigned;

using Min = unsigned;

using Sec = unsigned;

/**
 * Enum class for specifying the time resolution in the utilities.
 */
enum class TimeResolution : std::uint8_t
{
    SECONDS,        ///< Represents the seconds.
    MILLISECONDS,   ///< Represents the milliseconds.
    MICROSECONDS,   ///< Represents the microseconds.
    NANOSECONDS,    ///< Represents the nanoseconds.
};

/**
 * @brief Enum class for specifying the clock type used in time-related utilities.
 *
 * This enum allows selecting the clock source for timestamp retrieval.
 *
 * @note The default clock type in functions is `SYSTEM`, as it is best suited for
 * logging and timestamp storage.
 */
enum class ClockType : std::uint8_t
{
    SYSTEM,    ///< System-wide clock, suitable for logging and database timestamps. (Default)
    HIGH_RES,  ///< High-precision clock, best for performance timing.
    STEADY     ///< Monotonic clock, suitable for measuring elapsed time.
};

// ---------------------------------------------------------------------------------------------------------------------

END_NAMESPACE(types)
LEOBASE_MODULE_END
