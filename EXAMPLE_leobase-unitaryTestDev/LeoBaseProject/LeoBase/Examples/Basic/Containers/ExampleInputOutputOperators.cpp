/*
 *      Copyright(C) Milethos Technologies SLU. 2025
 *      All rights reserved. Reproduction in whole or in
 *      part is prohibited without the written consent of
 *      the copyright holder.
 */

// C++ INCLUDES
#include <iostream>
#include <vector>
#include <map>

// LEOBASE INCLUDES
#include <LeoBase/Aliases/Containers>
#include <LeoBase/Aliases/System>
#include <LeoBase/Aliases/Helpers>

// ---------------------------------------------------------------------------------------------------------------------

// LeoBase namespaces.
using namespace leobase::helpers;
using namespace leobase::containers::operators;

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Main entry point of the program `ExampleGlobalOperators`.
 */
int main()
{
    // Initial log.
    std::cout << strings::generateExampleTitle("Example GlobalOperators");

    // ----------------------------------------------------------------------------------------------------
    // Vector Example

    std::cout << "[INFO] Testing std::vector output formatting..." << std::endl;
    std::vector<int> vec = {1, 2, 3, 4, 5};
    std::cout << "Vector: " << vec << std::endl;
    std::cout << "[SUCCESS] Vector formatted correctly." << std::endl << std::endl;

    // ----------------------------------------------------------------------------------------------------
    // Pair Example

    std::cout << "[INFO] Testing std::pair output formatting..." << std::endl;
    std::pair<std::string, int> pair = {"Alice", 25};
    std::cout << "Pair: " << pair << std::endl;
    std::cout << "[SUCCESS] Pair formatted correctly." << std::endl << std::endl;

    // ----------------------------------------------------------------------------------------------------
    // Map Example

    std::cout << "[INFO] Testing std::map output formatting..." << std::endl;
    std::map<std::string, int> map = {{"Alice", 25}, {"Bob", 30}, {"Charlie", 22}};
    std::cout << "Map: " << map << std::endl;
    std::cout << "[SUCCESS] Map formatted correctly." << std::endl << std::endl;

    // ----------------------------------------------------------------------------------------------------
    // Multimap Example

    std::cout << "[INFO] Testing std::multimap output formatting..." << std::endl;
    std::multimap<std::string, int> multimap = {{"Alice", 25}, {"Alice", 26}, {"Bob", 30}};
    std::cout << "Multimap: " << multimap << std::endl;
    std::cout << "[SUCCESS] Multimap formatted correctly." << std::endl << std::endl;

    //----------------------------------------------------------------------------------------------------
    // Final log.
    std::cout << strings::generateExampleEnd();
    leobase::system::utils::userInputPause("Press <ENTER> to exit...");

    // Final return.
    return 0;
}

// ---------------------------------------------------------------------------------------------------------------------
