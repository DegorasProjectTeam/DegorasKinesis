/*
 *      Copyright(C) Milethos Technologies SLU. 2025
 *      All rights reserved. Reproduction in whole or in
 *      part is prohibited without the written consent of
 *      the copyright holder.
 *
 *      Author: Ángel Vera Herrera
 */

// C++ INCLUDES
#include <iostream>
#ifdef _WIN32
#include <windows.h>
#include <tlhelp32.h>
#else
#endif

// LEOBASE INCLUDES
#include "LeoBase/System/console/console_utils.h"
#include "LeoBase/System/utils/system_utils.h"

// LEOBASE MACROS
LEOBASE_MODULE_BEGIN(system)
BEGIN_NAMESPACE(console)

// ---------------------------------------------------------------------------------------------------------------------
// Windows Implementations.

#ifdef _WIN32

void consoleEnableAnsiColors()
{
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    if (hOut == INVALID_HANDLE_VALUE || !GetConsoleMode(hOut, &dwMode))
        return;
    SetConsoleMode(hOut, dwMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
}

bool consoleSupportsAnsiColors()
{
    // If running inside QtCreator, ANSI colors are supported.
    if(utils::isRunningInsideQtCreator())
        return true;

    DWORD mode = 0;
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    if (hConsole == INVALID_HANDLE_VALUE) return false;

    if (GetConsoleMode(hConsole, &mode))
        return (mode & ENABLE_VIRTUAL_TERMINAL_PROCESSING);

    return false;
}

// void setConsoleColor(int colorCode)
// {
//     HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
//     if (hConsole == INVALID_HANDLE_VALUE) return;

//     if (consoleSupportsAnsiColors())
//     {
//         std::cout << "\033[" << colorCode << "m";
//     }
//     else
//     {
//         // Use Windows Console API (CMD) fallback
//         SetConsoleTextAttribute(hConsole, colorCode);
//     }
// }

void setConsoleColor(int colorCode, std::ostream& out)
{
    // 1. Try ANSI colors first (Modern Windows, Linux, QtCreator, Unit Tests)
    if (consoleSupportsAnsiColors())
    {
        // Write directly to the injected stream and flush immediately to avoid buffering issues
        out << "\033[" << colorCode << "m" << std::flush;
        return;
    }

    // 2. Windows Legacy Fallback (Old CMD)
#if defined(_WIN32)
    // This API changes console internal state, it does NOT write to the stream.
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    if (hConsole == INVALID_HANDLE_VALUE) return;

    SetConsoleTextAttribute(hConsole, colorCode);
#endif
}


// ---------------------------------------------------------------------------------------------------------------------
// Unix Implementations.

#else

void enableConsoleAnsiColors()
{
    return;
}

bool consoleSupportsAnsiColors()
{
    return true;
}

void setConsoleColor(int colorCode)
{
   std::cout << "\033[" << colorCode << "m";
}

#endif

// ---------------------------------------------------------------------------------------------------------------------


END_NAMESPACE(utils)
LEOBASE_MODULE_END
