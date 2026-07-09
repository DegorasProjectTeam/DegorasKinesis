/*
 *      Copyright(C) Milethos Technologies SLU. 2025
 *      All rights reserved. Reproduction in whole or in
 *      part is prohibited without the written consent of
 *      the copyright holder.
 */

#pragma once

// C++ INCLUDES
#include <string>
#include <cstdint>

// LEOBASE INCLUDES
#include "LeoBase/Aliases/Global"

// LEOBASE MACROS
LEOBASE_MODULE_BEGIN(logger)

// ---------------------------------------------------------------------------------------------------------------------

using LoggerId = std::string;

using LogLevelType = std::uint8_t;

enum class LogLevel : LogLevelType
{
    LOGLEVEL_CRITICAL,
    LOGLEVEL_ERROR,
    LOGLEVEL_WARNING,
    LOGLEVEL_INFO,
    LOGLEVEL_DEBUG,
    LOGLEVEL_UNKNOWN
};

using AppendedLoggerMap = std::map<LoggerId, LogLevel>;

using LogColor = std::string;

const LogColor LogColourReset      = "\033[0m";
const LogColor LogColourBold       = "\033[1m";
const LogColor LogColorRed         = "\033[31m";
const LogColor LogColorGreen       = "\033[32m";
const LogColor LogColorYellow      = "\033[33m";
const LogColor LogColorBlue        = "\033[34m";
const LogColor LogColorMagenta     = "\033[35m";
const LogColor LogColorCyan        = "\033[36m";
const LogColor LogColorWhite       = "\033[37m";
const LogColor LogColorBoldRed     = "\033[1;31m";
const LogColor LogColorBoldGreen   = "\033[1;32m";
const LogColor LogColorBoldYellow  = "\033[1;33m";
const LogColor LogColorBoldBlue    = "\033[1;34m";
const LogColor LogColorBoldMagenta = "\033[1;35m";
const LogColor LogColorBoldCyan    = "\033[1;36m";
const LogColor LogColorBoldWhite   = "\033[1;37m";

// ---------------------------------------------------------------------------------------------------------------------

LEOBASE_MODULE_END
