/*
 *      Copyright(C) Milethos Technologies SLU. 2025
 *      All rights reserved. Reproduction in whole or in
 *      part is prohibited without the written consent of
 *      the copyright holder.
 *
 *      Author:
 *      Summary:
 */

// C++ INCLUDES
#include <iostream>

// LEOBASE INCLUDES
#include <LeoBase/Aliases/System>

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Main entry point of the program `ExampleCustomConsole`.
 */
int main()
{
    // LeoBase alias.
    using namespace leobase::system;

    // Initial log.
    std::cout << "===============================================" << std::endl;
    std::cout << "== Example CustomConsole " << std::endl;
    std::cout << "===============================================" << std::endl;
    std::cout << std::endl;
    std::cout << "Starting example..." << std::endl;
    std::cout << std::endl;

    // Configure the console.
    console::CustomConsole& console_cfg = console::CustomConsole::getInstance();
    console_cfg.configureConsole(false, false, true);

    //----------------------------------------------------------------------------------------------------

    // Final log.
    std::cout << "Example finished!" << std::endl;
    std::cout << std::endl;
    utils::userInputPause("Press <ENTER> to exit...");

    // Final return.
    return 0;
}

// ---------------------------------------------------------------------------------------------------------------------
