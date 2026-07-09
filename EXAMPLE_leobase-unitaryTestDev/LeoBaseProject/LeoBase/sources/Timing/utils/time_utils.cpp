/*
 *      Copyright(C) Milethos Technologies SLU. 2025
 *      All rights reserved. Reproduction in whole or in
 *      part is prohibited without the written consent of
 *      the copyright holder.
 */

// C++ INCLUDES
#include <regex>
#include <sstream>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <cmath>

// LEOBASE INCLUDES
#include "LeoBase/Global/global_definitions.h"
#include "LeoBase/Timing/utils/time_utils.h"
#include "LeoBase/Exceptions/leobase_exception.h"

// LEOBASE MACROS
LEOBASE_MODULE_BEGIN(timing)
BEGIN_NAMESPACE(utils)

// Namespaces.
using namespace std::chrono;
using namespace types;

// ---------------------------------------------------------------------------------------------------------------------

Ns computeOffsetSteadyToSystem()
{
    auto now_system = std::chrono::system_clock::now();
    auto now_steady = std::chrono::steady_clock::now();

    return std::chrono::duration_cast<Ns>(now_system.time_since_epoch()) -
           std::chrono::duration_cast<Ns>(now_steady.time_since_epoch());
}

Ns computeOffsetHighResToSystem()
{
    auto now_system = std::chrono::system_clock::now();
    auto now_highres = std::chrono::high_resolution_clock::now();

    return std::chrono::duration_cast<Ns>(now_system.time_since_epoch()) -
           std::chrono::duration_cast<Ns>(now_highres.time_since_epoch());
}

std::string timepointToString(const ResTimePoint &tp, const std::string& format, TimeResolution resolution,
                              bool utc, bool rm_trailing_zeros)
{
    std::ostringstream ss, frac;
    auto dur = tp.time_since_epoch();
    auto secs = std::chrono::duration_cast<std::chrono::seconds>(dur).count();
    auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(dur).count();
    auto sec_ns = secs * 1'000'000'000LL;
    auto remaining_ns = ns - sec_ns;

    std::time_t time_t = static_cast<std::time_t>(secs);
    const std::tm* tm = (utc ? std::gmtime(&time_t) : std::localtime(&time_t));
    if (!tm)
    {
        throw exceptions::LeoBaseException(kModN, "utils::timePointToString","Error in tm struct.");
    }

    ss << std::put_time(tm, format.c_str());

    if (resolution != TimeResolution::SECONDS && format.find("%S") != std::string::npos)
    {
        long long fraction = 0;
        int width = 0;

        switch (resolution)
        {
        case TimeResolution::MILLISECONDS:
            fraction = remaining_ns / 1'000'000;
            width = 3;
            break;
        case TimeResolution::MICROSECONDS:
            fraction = remaining_ns / 1'000;
            width = 6;
            break;
        case TimeResolution::NANOSECONDS:
            fraction = remaining_ns;
            width = 9;
            break;
        default:
            break;
        }

        frac << '.' << std::setw(width) << std::setfill('0') << fraction;

        // Remove last 0.
        if(rm_trailing_zeros)
        {
            std::string frac_str = frac.str();
            frac_str.erase(frac_str.find_last_not_of('0') + 1, std::string::npos);
            if (frac_str.back() == '.')
                frac_str.erase(frac_str.length() - 1);
            ss << frac_str;
        }
        else
            ss << frac.str();
    }

    // Return the string.
    return ss.str();
}

Iso8601Str timepointToIso8601(const ResTimePoint& tp, TimeResolution resolution, bool utc, bool rm_trailing_zeros)
{
    Iso8601Str result = timepointToString(tp, "%Y-%m-%dT%H:%M:%S", resolution, utc, rm_trailing_zeros);
    if(utc)
        result += 'Z';
    return result;
}

Iso8601Str timepointToIso8601(const types::StdyTimePoint &tp, types::TimeResolution resolution, bool utc, bool rm_trailing_zeros)
{
    // Get current time in both clocks
    auto now_system = std::chrono::system_clock::now();
    auto now_steady = std::chrono::steady_clock::now();

    // Compute the offset in nanoseconds
    auto offset_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(now_system.time_since_epoch()).count() -
                     std::chrono::duration_cast<std::chrono::nanoseconds>(now_steady.time_since_epoch()).count();

    // Convert steady clock time to system clock by applying offset
    auto system_time = std::chrono::system_clock::time_point(std::chrono::nanoseconds(tp.time_since_epoch().count() + offset_ns));

    // Return the converted time point.
    return timepointToIso8601(system_time, resolution, utc, rm_trailing_zeros);
}

std::string currentDatetimeIso8601(TimeResolution resolution, bool utc, bool rm_trailing_zeros, ClockType clock_type)
{
    switch (clock_type)
    {
    case ClockType::SYSTEM:
        return timepointToIso8601(currentTimepoint<std::chrono::system_clock>(), resolution, utc, rm_trailing_zeros);
    case ClockType::STEADY:
        return timepointToIso8601(currentTimepoint<std::chrono::steady_clock>(), resolution, utc, rm_trailing_zeros);
    case ClockType::HIGH_RES:
    default:
        return timepointToIso8601(currentTimepoint<std::chrono::high_resolution_clock>(), resolution, utc, rm_trailing_zeros);
    }
}

ResTimePoint iso8601ToResTimepoint(const std::string& datetime)
{
    int y, m, d, h, M, s;
    std::smatch match;
    std::regex iso8601_regex_extended(R"(^(\d{4})-(\d{2})-(\d{2})T(\d{2}):(\d{2}):(\d{2})(?:\.(\d+))?(Z)?$)");
    std::regex iso8601_regex_basic(R"(^(\d{4})(\d{2})(\d{2})T(\d{2})(\d{2})(\d{2})(?:\.(\d+))?(Z)?$)");

    if (!std::regex_search(datetime, match, iso8601_regex_extended) &&
        !std::regex_search(datetime, match, iso8601_regex_basic))
    {
        throw exceptions::LeoBaseException(kModN, "utils::iso8601ToResTimepoint","Invalid argument: " + datetime);
    }

    y = std::stoi(match[1].str());
    m = std::stoi(match[2].str());
    d = std::stoi(match[3].str());
    h = std::stoi(match[4].str());
    M = std::stoi(match[5].str());
    s = std::stoi(match[6].str());
    std::string fractional_seconds_str = match[7].str();
    bool is_utc = match[8].str() == "Z";

    auto days_since_epoch = daysFromCivil(y, static_cast<unsigned>(m), static_cast<unsigned>(d));
    ResTimePoint t = ResClock::time_point(std::chrono::duration<int, std::ratio<86400>>(days_since_epoch));

    t += std::chrono::hours(h);
    t += std::chrono::minutes(M);
    t += std::chrono::seconds(s);

    if (!fractional_seconds_str.empty()) {
        long long fractional_seconds = std::stoll(fractional_seconds_str);
        size_t length = fractional_seconds_str.length();

        if (length == 1) // Normalize to milliseconds
            t += std::chrono::milliseconds(fractional_seconds * 100);
        else if (length == 2) // Normalize to milliseconds
            t += std::chrono::milliseconds(fractional_seconds * 10);
        else if (length == 3) // Already in milliseconds
            t += std::chrono::milliseconds(fractional_seconds);
        else if (length <= 6) // Normalize to microseconds
            t += std::chrono::microseconds(fractional_seconds * static_cast<long long>(std::pow(10, 6 - length)));
        else // Normalize to nanoseconds
            t += std::chrono::nanoseconds(fractional_seconds * static_cast<long long>(std::pow(10, 9 - length)));
    }

    if (!is_utc) {
        // Adjust for local timezone if 'Z' is not present
        std::time_t now = std::time(nullptr);
        std::tm* now_tm = std::localtime(&now);
        std::tm* gm_tm = std::gmtime(&now);
        auto local_diff = std::mktime(now_tm) - std::mktime(gm_tm);
        t += std::chrono::seconds(local_diff);
    }

    return t;
}

long double timePointToSecsDay(const ResTimePoint &tp)
{
    std::time_t current = ResTimePoint::clock::to_time_t(tp);
    std::tm* current_date = std::gmtime(&current);
    current_date->tm_hour = 0;
    current_date->tm_min = 0;
    current_date->tm_sec = 0;
    current_date->tm_isdst = 0;
    ResTimePoint day_start = ResTimePoint::clock::from_time_t(MKGMTIME(current_date));
    return duration_cast<duration<long double>>(tp - day_start).count();
}


ResTimePoint dateAndTimeToTimePoint(types::Year y, types::Month m,
                                    types::Day d, types::Hour h, types::Min  min, types::Sec s)
{
    tm datetime;
    datetime.tm_year = y - 1900;
    datetime.tm_mon = m - 1;
    datetime.tm_mday = d;
    datetime.tm_hour = h;
    datetime.tm_min = min;
    datetime.tm_sec = s;
    return high_resolution_clock::from_time_t(MKGMTIME(&datetime));
}

long long daysFromCivil(int y, unsigned m, unsigned d)
{
    // Check the numeric limits.
    static_assert(std::numeric_limits<unsigned>::digits >= 18,
                  "[LeoBase,Timing,utils::daysFromCivil] >= 16 bit unsigned integer");
    static_assert(std::numeric_limits<int>::digits >= 20,
                  "[LeoBase,Timing,utils::daysFromCivil] >= 16 bit signed integer");
    // Calculate the number of days since 1970-01-01.
    y -= m <= 2;
    const int era = (y >= 0 ? y : y-399) / 400;
    const unsigned yoe = static_cast<unsigned>(y - era * 400);
    const unsigned doy = (153*(m > 2 ? m-3 : m+9) + 2)/5 + d-1;
    const unsigned doe = yoe * 365 + yoe/4 - yoe/100 + doy;
    return era * 146097LL + static_cast<int>(doe) - 719468LL;
}

unsigned monthStrToNumber(const std::string &month_str)
{
    static const std::unordered_map<std::string, unsigned> months_map_short =
    {
        {"jan", 1}, {"feb", 2}, {"mar", 3}, {"apr", 4}, {"may", 5}, {"jun", 6},
        {"jul", 7}, {"aug", 8}, {"sep", 9}, {"oct", 10}, {"nov", 11}, {"dec", 12}
    };

    static const std::unordered_map<std::string, unsigned> months_map_full =
    {
        {"january", 1}, {"february", 2}, {"march", 3}, {"april", 4}, {"may", 5}, {"june", 6},
        {"july", 7}, {"august", 8}, {"september", 9}, {"october", 10}, {"november", 11}, {"december", 12}
    };

    // Convert full month name to lowercase before checking.
    std::string lower_month = month_str;
    std::transform(lower_month.begin(), lower_month.end(), lower_month.begin(), ::tolower);

    // Try short format first
    auto it_short = months_map_short.find(lower_month);
    if (it_short != months_map_short.end())
        return it_short->second;

    // Try full format
    auto it_full = months_map_full.find(lower_month);
    if (it_full != months_map_full.end())
        return it_full->second;

    // Throw an exception if month is not found.
    throw exceptions::LeoBaseException(kModN, "utils::monthStrToNumber","Invalid argument: " + month_str);
}




// TODO CHECk
bool isValidIso8601Datetime(const std::string &datetime)
{
    std::smatch match;

    // Regex.
    const std::regex iso8601_regex_extended(
        R"(^(\d{4})-(\d{2})-(\d{2})T(\d{2}):(\d{2}):(\d{2})(?:\.(\d+))?(?:(Z)|((\+|\-)(\d{2}):(\d{2})))?$)");
    const std::regex iso8601_regex_basic(
        R"(^(\d{4})(\d{2})(\d{2})T(\d{2})(\d{2})(\d{2})(?:\.(\d+))?(?:(Z)|((\+|\-)(\d{2}):(\d{2})))?$)");

    // Check if datetime matches one of ISO datetime patterns.
    return std::regex_search(datetime, match, iso8601_regex_extended) ||
           std::regex_search(datetime, match, iso8601_regex_basic);
}

// ---------------------------------------------------------------------------------------------------------------------

END_NAMESPACE(utils)
LEOBASE_MODULE_END
