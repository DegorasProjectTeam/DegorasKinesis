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
#include <LeoBase/Aliases/System>
#include <LeoBase/Aliases/Containers>
#include <LeoBase/Aliases/Helpers>

// ---------------------------------------------------------------------------------------------------------------------

// LeoBase namespaces.
using namespace leobase::helpers;
using namespace leobase::containers;

// ---------------------------------------------------------------------------------------------------------------------

std::mutex gMutex;

/**
 * @brief Main entry point of the program `ExampleFutureStorageHandler_1`.
 */
int main()
{    
    // Initial log.
    std::cout << strings::generateExampleTitle("Example FutureStorageHandler 1");

    //----------------------------------------------------------------------------------------------------
    // Testing Operations.

    FutureStorageHandler<std::future<void>> futureStorage;
    futureStorage.start();

    auto fut_1 = std::async(std::launch::async, [&]()
        {
            {
                std::lock_guard<std::mutex> lock(gMutex);
                std::cout << "Executing async of fut 1..." << std::endl;
            }

            std::this_thread::sleep_for(std::chrono::seconds(1));

            {
                std::lock_guard<std::mutex> lock(gMutex);
                std::cout << "Executing async of fut 1... Ok" << std::endl;
            }
        });

    auto fut_2 = std::async(std::launch::async, [&]()
        {
            {
                std::lock_guard<std::mutex> lock(gMutex);
                std::cout << "Executing async of fut 2..." << std::endl;
            }

            std::this_thread::sleep_for(std::chrono::seconds(2));

            {
                std::lock_guard<std::mutex> lock(gMutex);
                std::cout << "Executing async of fut 2... Ok" << std::endl;
            }
        });

    auto fut_3 = std::async(std::launch::async, [&]()
        {
            {
                std::lock_guard<std::mutex> lock(gMutex);
                std::cout << "Executing async of fut 3..." << std::endl;
            }

            std::this_thread::sleep_for(std::chrono::seconds(3));

            {
                std::lock_guard<std::mutex> lock(gMutex);
                std::cout << "Executing async of fut 3... Ok" << std::endl;
            }
        });

    futureStorage.add(std::move(fut_1));
    futureStorage.add(std::move(fut_2));
    futureStorage.add(std::move(fut_3));

    std::this_thread::sleep_for(std::chrono::seconds(2));

    futureStorage.stop();

    //----------------------------------------------------------------------------------------------------
    // Final log.
    std::cout << strings::generateExampleEnd();
    leobase::system::utils::userInputPause("Press <ENTER> to exit...");

    // Final return.
    return 0;
}

// ---------------------------------------------------------------------------------------------------------------------
