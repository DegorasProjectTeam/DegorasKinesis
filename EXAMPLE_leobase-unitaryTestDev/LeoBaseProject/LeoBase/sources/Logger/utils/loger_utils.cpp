/*
 *      Copyright(C) Milethos Technologies SLU. 2025
 *      All rights reserved. Reproduction in whole or in
 *      part is prohibited without the written consent of
 *      the copyright holder.
 */

// LEOBASE INCLUDES
#include "LeoBase/Logger/utils/logger_utils.h"

// LEOBASE MACROS
LEOBASE_MODULE_BEGIN(logger)

// ---------------------------------------------------------------------------------------------------------------------

std::string logLevelToString(const LogLevel& id)
{
    static const std::unordered_map<LogLevel, std::string> to_leo_loglevel_string_map =
        {
            {LogLevel::LOGLEVEL_CRITICAL, "Critical"},
            {LogLevel::LOGLEVEL_ERROR,    "Error   "},
            {LogLevel::LOGLEVEL_WARNING,  "Warning "},
            {LogLevel::LOGLEVEL_INFO,     "Info    "},
            {LogLevel::LOGLEVEL_DEBUG,    "Debug   "}
        };

    auto it = to_leo_loglevel_string_map.find(id);
    return (it != to_leo_loglevel_string_map.end()) ? it->second : "Unknown ";
}

LogLevel stringToLogLevel(const std::string& name)
{
    static const std::unordered_map<std::string, LogLevel> to_leo_loglevel_map =
        {
            {"Critical", LogLevel::LOGLEVEL_CRITICAL},
            {"Error",    LogLevel::LOGLEVEL_ERROR},
            {"Warning",  LogLevel::LOGLEVEL_WARNING},
            {"Info",     LogLevel::LOGLEVEL_INFO},
            {"Debug",    LogLevel::LOGLEVEL_DEBUG}
        };

    auto it = to_leo_loglevel_map.find(name);
    return (it != to_leo_loglevel_map.end()) ? it->second : LogLevel::LOGLEVEL_UNKNOWN;
}

LogColor logLevelToColor(const LogLevel& id)
{
    static const std::unordered_map<LogLevel, LogColor> to_leo_loglevel_logcolor_map =
        {
            {LogLevel::LOGLEVEL_CRITICAL, LogColorBoldRed},
            {LogLevel::LOGLEVEL_ERROR,    LogColorRed},
            {LogLevel::LOGLEVEL_WARNING,  LogColorYellow},
            {LogLevel::LOGLEVEL_INFO,     LogColorWhite},
            {LogLevel::LOGLEVEL_DEBUG,    LogColorGreen}
        };

    auto it = to_leo_loglevel_logcolor_map.find(id);
    return (it != to_leo_loglevel_logcolor_map.end()) ? it->second : LogColorWhite;
}

// ---------------------------------------------------------------------------------------------------------------------

LEOBASE_MODULE_END
