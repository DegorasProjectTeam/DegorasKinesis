/*
 *      Copyright(C) Milethos Technologies SLU. 2025
 *      All rights reserved. Reproduction in whole or in
 *      part is prohibited without the written consent of
 *      the copyright holder.
 */

#pragma once

// C++ INCLUDES
#ifdef _WIN32
#endif

// LEOBASE INCLUDES
#include "LeoBase/Aliases/Global"


// LEOBASE MACROS
LEOBASE_MODULE_BEGIN(system)
BEGIN_NAMESPACE(types)

#define WINAPI_FOREGROUND_BLUE 0x0001
#define WINAPI_FOREGROUND_GREEN 0x0002
#define WINAPI_FOREGROUND_RED 0x0004
#define WINAPI_FOREGROUND_INTENSITY 0x0008

// ---------------------------------------------------------------------------------------------------------------------
// Windows Implementations.

#ifdef _WIN32

/**
 * @brief Represents a console color that supports both ANSI escape codes
 *        and Windows `SetConsoleTextAttribute`.
 */
struct ConsoleColor
{
    int red, green, blue;   //< RGB values for modern ANSI support.
    int winColor;

    /**
     * @brief Default constructor initializes to white.
     */
    ConsoleColor() : red(255), green(255), blue(255), winColor(WINAPI_FOREGROUND_RED | WINAPI_FOREGROUND_GREEN | WINAPI_FOREGROUND_BLUE) {}

    /**
     * @brief Parameterized constructor to set both RGB and Windows console color.
     */
    ConsoleColor(int r, int g, int b, int win) : red(r), green(g), blue(b), winColor(win) {}

    /**
     * @brief Generates an ANSI escape code for the given RGB color.
     *
     * @return std::string The ANSI escape sequence to set text color.
     */
    std::string toAnsiCode() const {
        std::ostringstream ansiCode;
        ansiCode << "\033[38;2;" << red << ";" << green << ";" << blue << "m";
        return ansiCode.str();
    }

    /**
     * @brief Maps an RGB color to the closest Windows console color.
     */
    static ConsoleColor fromRGB(int r, int g, int b)
    {
        // Convert RGB to nearest legacy Windows console color
        if (r == 0 && g == 0 && b == 0)        return ConsoleColor(0, 0, 0, 0); // Black
        if (r == 128 && g == 0 && b == 0)      return ConsoleColor(128, 0, 0, WINAPI_FOREGROUND_RED); // Dark Red
        if (r == 0 && g == 128 && b == 0)      return ConsoleColor(0, 128, 0, WINAPI_FOREGROUND_GREEN); // Dark Green
        if (r == 128 && g == 128 && b == 0)    return ConsoleColor(128, 128, 0, WINAPI_FOREGROUND_RED | WINAPI_FOREGROUND_GREEN); // Dark Yellow
        if (r == 0 && g == 0 && b == 128)      return ConsoleColor(0, 0, 128, WINAPI_FOREGROUND_BLUE); // Dark Blue
        if (r == 128 && g == 0 && b == 128)    return ConsoleColor(128, 0, 128, WINAPI_FOREGROUND_RED | WINAPI_FOREGROUND_BLUE); // Dark Magenta
        if (r == 0 && g == 128 && b == 128)    return ConsoleColor(0, 128, 128, WINAPI_FOREGROUND_GREEN | WINAPI_FOREGROUND_BLUE); // Dark Cyan
        if (r == 192 && g == 192 && b == 192)  return ConsoleColor(192, 192, 192, WINAPI_FOREGROUND_RED | WINAPI_FOREGROUND_GREEN | WINAPI_FOREGROUND_BLUE); // Light Gray
        if (r == 255 && g == 0 && b == 0)      return ConsoleColor(255, 0, 0, WINAPI_FOREGROUND_RED | WINAPI_FOREGROUND_INTENSITY); // Bright Red
        if (r == 0 && g == 255 && b == 0)      return ConsoleColor(0, 255, 0, WINAPI_FOREGROUND_GREEN | WINAPI_FOREGROUND_INTENSITY); // Bright Green
        if (r == 255 && g == 255 && b == 0)    return ConsoleColor(255, 255, 0, WINAPI_FOREGROUND_RED | WINAPI_FOREGROUND_GREEN | WINAPI_FOREGROUND_INTENSITY); // Bright Yellow
        if (r == 0 && g == 0 && b == 255)      return ConsoleColor(0, 0, 255, WINAPI_FOREGROUND_BLUE | WINAPI_FOREGROUND_INTENSITY); // Bright Blue
        if (r == 255 && g == 0 && b == 255)    return ConsoleColor(255, 0, 255, WINAPI_FOREGROUND_RED | WINAPI_FOREGROUND_BLUE | WINAPI_FOREGROUND_INTENSITY); // Bright Magenta
        if (r == 0 && g == 255 && b == 255)    return ConsoleColor(0, 255, 255, WINAPI_FOREGROUND_GREEN | WINAPI_FOREGROUND_BLUE | WINAPI_FOREGROUND_INTENSITY); // Bright Cyan
        if (r == 255 && g == 255 && b == 255)  return ConsoleColor(255, 255, 255, WINAPI_FOREGROUND_RED | WINAPI_FOREGROUND_GREEN | WINAPI_FOREGROUND_BLUE | WINAPI_FOREGROUND_INTENSITY); // White

        return ConsoleColor(255, 255, 255, WINAPI_FOREGROUND_RED | WINAPI_FOREGROUND_GREEN | WINAPI_FOREGROUND_BLUE | WINAPI_FOREGROUND_INTENSITY); // Default White
    }
};

// ---------------------------------------------------------------------------------------------------------------------
// Unix Implementations.

#else

struct ConsoleColor
{

};

#endif

// ---------------------------------------------------------------------------------------------------------------------

END_NAMESPACE(types)
LEOBASE_MODULE_END
