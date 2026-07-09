/*
 *      Copyright(C) Milethos Technologies SLU. 2025
 *      All rights reserved. Reproduction in whole or in
 *      part is prohibited without the written consent of
 *      the copyright holder.
 */

#pragma once

// C++ INCLUDES
#include <map>

// LEOBASE INCLUDES
#include "LeoBase/Aliases/Global"

#include "LeoBase/Logger/types/logger_types.h"

// LEOBASE MACROS
LEOBASE_MODULE_BEGIN(logger)

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Converts a LogLevel enum value to its corresponding string representation.
 *
 * @param id The LogLevel enum value.
 * @return A string representing the log level name. Returns "Unknown" if the ID is
 *         not recognized.
 */
std::string LEOBASE_EXPORT logLevelToString(const LogLevel& id);

/**
 * @brief Converts a log level name string to its corresponding LogLevel enum value.
 *
 * @param name A string representing the log level name.
 * @return The corresponding LogLevel enum value. Returns LogLevel::LOGLEVEL_UNKNOWN
 *         if the string is not recognized.
 */
LogLevel LEOBASE_EXPORT stringToLogLevel(const std::string& name);

/**
 * @brief Converts a LogLevel enum value to its corresponding LogColor representation.
 *
 * @param id The LogLevel enum value.
 * @return A LogColor value. Returns "LogColorWhite" if the ID is
 *         not recognized.
 */
LogColor LEOBASE_EXPORT logLevelToColor(const LogLevel& id);

// ---------------------------------------------------------------------------------------------------------------------

LEOBASE_MODULE_END
