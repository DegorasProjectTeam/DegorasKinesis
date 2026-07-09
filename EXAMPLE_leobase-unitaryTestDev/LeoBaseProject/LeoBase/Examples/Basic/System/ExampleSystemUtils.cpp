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

// ---------------------------------------------------------------------------------------------------------------------

// LeoBase namespaces.
using namespace leobase::system::utils;
using namespace leobase::system::types;
using namespace leobase::system::files;
using namespace leobase::system::console;

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Resets console text color to default.
 */
// void resetConsoleColor()
// {
// #if defined(_WIN32)
//     HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
//     if (hConsole == INVALID_HANDLE_VALUE) return;

//     if (supportsAnsiColors())
//     {
//         std::cout << "\033[0m"; // Reset with ANSI
//     }
//     else
//     {
//         SetConsoleTextAttribute(hConsole, 7); // Default Windows console color
//     }
// #else
//     std::cout << "\033[0m"; // Reset ANSI colors
// #endif
// }

/**
 * @brief Main entry point of the program `ExampleSystemUtils`.
 */
int main()
{
    // LeoBase alias.
    using namespace leobase::system;

    // Initial log.
    std::cout << strings::generateExampleTitle("Example SystemUtils");

    // Configure the console.
    console::CustomConsole& console_cfg = console::CustomConsole::getInstance();
    console_cfg.configureConsole(false, false, true);

    // ----------------------------------------------------------------------------------------------------
    // Retrieving current process PID.

    std::cout << "[INFO] Retrieving the current PID..." << std::endl;
    PID pid = getCurrentPID();
    std::cout << "[SUCCESS] Current PID: " << pid << std::endl << std::endl;

    // ----------------------------------------------------------------------------------------------------
    // Listing System Disks.

    std::cout << "[INFO] Retrieving all available system disks..." << std::endl;
    std::vector<std::string> systemDisks = listSystemDisks();

    if (systemDisks.empty())
    {
        std::cout << "[WARNING] No system disks found!" << std::endl << std::endl;
    }
    else
    {
        std::cout << "[SUCCESS] System Disks:" << std::endl;
        for (const auto& disk : systemDisks)
        {
            std::cout << "  - " << disk << std::endl;
        }
        std::cout << std::endl;
    }

    // ----------------------------------------------------------------------------------------------------
    // Check if the example is running inside QtCreator.

    if (isRunningInsideQtCreator())
    {
        std::cout << "[INFO] Running inside Qt Creator." << std::endl;
    }
    else
    {
        std::cout << "[INFO] Not running inside Qt Creator." << std::endl;
    }

    std::cout << "[SUCCESS] Qt Creator detection test completed." << std::endl << std::endl;

    // ----------------------------------------------------------------------------------------------------
    // Retrieving the OS Root Directory.

    std::cout << "[INFO] Retrieving the OS root directory..." << std::endl;
    std::string root_dir = getOsRootDir();
    std::cout << "[SUCCESS] OS Root Directory: " << root_dir << std::endl << std::endl;

    // ----------------------------------------------------------------------------------------------------
    // Testing CPU Usage.

    std::cout << "[INFO] Sampling CPU usage for 2 seconds..." << std::endl;

    for (int i = 0; i < 2; ++i)
    {
        float cpu = utils::getCPULoad();
        std::cout << "  [" << i + 1 << "s] CPU Usage: " << cpu << "%" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    std::cout << "[SUCCESS] CPU usage sampling complete." << std::endl << std::endl;

    // ----------------------------------------------------------------------------------------------------
    // Testing RAM Usage.

    std::cout << "[INFO] Sampling RAM usage for 2 seconds..." << std::endl;

    for (int i = 0; i < 2; ++i)
    {
        float ram = utils::getRAMLoad();
        std::cout << "  [" << i + 1 << "s] RAM Usage: " << ram << "%" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    std::cout << "[SUCCESS] RAM usage sampling complete." << std::endl << std::endl;

    // ----------------------------------------------------------------------------------------------------
    // Testing disks space usage.

    std::cout << "[INFO] Retrieving disks space usage..." << std::endl;

    constexpr double kGB = 1024.0 * 1024.0 * 1024.0;

    for (const auto& disk : systemDisks)
    {
        float usagePercent = utils::getSystemDiskSpaceUsage(disk);
        uint64_t freeBytes = utils::getSystemDiskAvailableSpace(disk);
        uint64_t usedBytes = utils::getSystemDiskUsedSpace(disk);
        double freeGB = static_cast<double>(freeBytes) / kGB;
        double usedGB = static_cast<double>(usedBytes) / kGB;

        std::cout << std::fixed << std::setprecision(2);
        std::cout << "  [" << disk << "] Usage: " << usagePercent << "% | "
                  << "Used: " << usedGB << " GB | "
                  << "Free: " << freeGB << " GB" << std::endl;
    }

    std::cout << "[SUCCESS] Disk space usage retrieved." << std::endl << std::endl;

    // ----------------------------------------------------------------------------------------------------
    // Testing advanced disk info.

    std::cout << "[INFO] Retrieving advanced disk information..." << std::endl;

    DiskInfoV disks = getSystemDisksInfo();

    for (const auto& disk : disks)
    {
        std::cout << "  " << disk.toJsonStr() << std::endl;
    }

    std::cout << "[SUCCESS] Advanced disk information retrieved." << std::endl << std::endl;

    //----------------------------------------------------------------------------------------------------
    // Final log.
    std::cout << strings::generateExampleEnd();
    leobase::system::utils::userInputPause("Press <ENTER> to exit...");

    // Final return.
    return 0;
}

// ---------------------------------------------------------------------------------------------------------------------
