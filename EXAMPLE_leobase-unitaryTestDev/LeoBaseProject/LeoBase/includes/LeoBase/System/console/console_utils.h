/*
 *      Copyright(C) Milethos Technologies SLU. 2026
 *      All rights reserved. Reproduction in whole or in
 *      part is prohibited without the written consent of
 *      the copyright holder.
 */

#pragma once

// C++ INCLUDES

//TEST
#include <iostream>

// LEOBASE INCLUDES
#include "LeoBase/Aliases/Global"


// LEOBASE MACROS
LEOBASE_MODULE_BEGIN(system)
BEGIN_NAMESPACE(console)

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Enables ANSI color support in the console.
 *
 * This function modifies the console mode to enable virtual terminal
 * processing, allowing ANSI escape codes for color and formatting to work.
 *
 * @note This function is only necessary on Windows. On Unix-based systems,
 *       ANSI color support is typically enabled by default.
 *
 * @warning This function has no effect if the standard output handle is
 *          invalid or if the console mode cannot be retrieved.
 */
LEOBASE_EXPORT void consoleEnableAnsiColors();

/**
 * @brief Checks if the console supports ANSI colors.
 *
 * This function determines whether the current console environment supports
 * ANSI escape sequences for color formatting. If running inside QtCreator,
 * the colors are always supported. Otherwise, it checks the console mode.
 *
 * @return true if ANSI colors are supported, false otherwise.
 *
 * @note This function is primarily for Windows. On Unix-based systems, ANSI color
 *       support is typically enabled by default.
 */
LEOBASE_EXPORT bool consoleSupportsAnsiColors();

//TEST
//LEOBASE_EXPORT void setConsoleColor(int colorCode);
LEOBASE_EXPORT void setConsoleColor(int colorCode, std::ostream& out = std::cout);
// ---------------------------------------------------------------------------------------------------------------------

END_NAMESPACE(console)
LEOBASE_MODULE_END
