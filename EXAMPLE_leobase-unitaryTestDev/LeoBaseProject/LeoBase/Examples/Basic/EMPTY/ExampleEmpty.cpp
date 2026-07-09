/*
 *      Copyright(C) Milethos Technologies SLU. 2025
 *      All rights reserved. Reproduction in whole or in
 *      part is prohibited without the written consent of
 *      the copyright holder.
 */

// C++ INCLUDES
#include <iostream>
#include <thread>

// LEOBASE INCLUDES
#include <LeoBase/Aliases/Timing>
#include <LeoBase/Aliases/System>

// ---------------------------------------------------------------------------------------------------------------------

// LeoBase namespaces.
using namespace leobase::timing::utils;
using namespace leobase::timing::types;

// ---------------------------------------------------------------------------------------------------------------------


// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Main entry point of the program `ExampleTimeUtils`.
 */
int main()
{
    // Initial log.
    std::cout << "===============================================" << std::endl;
    std::cout << "== Example TimeUtils" << std::endl;
    std::cout << "===============================================" << std::endl;
    std::cout << std::endl;
    std::cout << "Starting example..." << std::endl;
    std::cout << std::endl;

    // ----------------------------------------------------------------------------------------------------
    // GETTING CLOCKS OFFSTETS.

   
    //----------------------------------------------------------------------------------------------------
    // Final log.
    std::cout << std::endl << "Example finished!" << std::endl;
    std::cout << std::endl;
    leobase::system::utils::userInputPause("Press <ENTER> to exit...");

    // Final return.
    return 0;
}

// ---------------------------------------------------------------------------------------------------------------------
