/*
 *      Copyright(C) Milethos Technologies SLU. 2025
 *      All rights reserved. Reproduction in whole or in
 *      part is prohibited without the written consent of
 *      the copyright holder.
 */

// C++ INCLUDES
#include <iostream>

// LEOBASE INCLUDES
#include <LeoBase/Aliases/System>
#include <LeoBase/Aliases/Helpers>
#include <LeoBase/Aliases/Network>

// ---------------------------------------------------------------------------------------------------------------------

// LeoBase namespaces.
using namespace leobase::network::utils;
using namespace leobase::network::types;

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Main entry point of the program `ExampleNetworkUtils`.
 */
int main()
{
    // Common LeoBase alias.
    using namespace leobase::system;
    using namespace leobase::helpers;

    // Initial log.
    std::cout << strings::generateExampleTitle("Example SystemUtils");

    // Configure the console.
    console::CustomConsole& console_cfg = console::CustomConsole::getInstance();
    console_cfg.configureConsole(false, false, true);

    // ----------------------------------------------------------------------------------------------------
    // Testing TCP Port Connectivity (SFTP/SSH probe)

    std::cout << "[INFO] Testing TCP connectivity to SFTP port (22) on localhost..." << std::endl;

    const std::string host = "192.168.8.169";
    const uint16_t port = 22;
    const int timeoutMs = 200;

    if (isTcpPortOpen(host, port, timeoutMs))
    {
        std::cout << "[SUCCESS] Connection to " << host << ":" << port << " succeeded. Port is open." << std::endl;
    }
    else
    {
        std::cout << "[WARNING] Connection to " << host << ":" << port << " failed. Port is closed or unreachable." << std::endl;
    }

    std::cout << std::endl;

    // ----------------------------------------------------------------------------------------------------
    // Retrieving All Active Network Adapters

    std::cout << "[INFO] Retrieving all active IPv4 network adapters..." << std::endl;

    NetAdptInfoV adapters = getActiveNetAdptInfo();

    if (adapters.empty())
    {
        std::cout << "[WARNING] No active network adapters found!" << std::endl << std::endl;
    }
    else
    {
        std::cout << "[SUCCESS] Active Network Adapters:" << std::endl;

        for (const auto& adapter : adapters)
        {
            std::cout << adapter.toJsonStr(2);
            std::cout << std::endl;
        }

        std::cout << "[INFO] Total adapters retrieved: " << adapters.size() << std::endl << std::endl;
    }

    //----------------------------------------------------------------------------------------------------
    // Final log.
    std::cout << strings::generateExampleEnd();
    leobase::system::utils::userInputPause("Press <ENTER> to exit...");

    // Final return.
    return 0;
}

// ---------------------------------------------------------------------------------------------------------------------
