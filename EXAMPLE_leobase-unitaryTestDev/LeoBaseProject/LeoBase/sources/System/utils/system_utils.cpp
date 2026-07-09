/*
 *      Copyright(C) Milethos Technologies SLU. 2025
 *      All rights reserved. Reproduction in whole or in
 *      part is prohibited without the written consent of
 *      the copyright holder.
 *
 *      Author: Ángel Vera Herrera
 */

// C++ INCLUDES
#include <chrono>
#include <iostream>
#include <limits>
#include <thread>
#ifdef _WIN32
#include <windows.h>
#include <tlhelp32.h>
#include <pdh.h>
#include <pdhmsg.h>
#else
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#endif

// LEOBASE INCLUDES
#include "LeoBase/System/utils/system_utils.h"

// LEOBASE MACROS
LEOBASE_MODULE_BEGIN(system)
BEGIN_NAMESPACE(utils)

// ---------------------------------------------------------------------------------------------------------------------
// Generic implementations.

void userInputPause(const std::string &log_text)
{
    // Do the pause.
    if (!log_text.empty())
        std::cout << log_text << std::endl;
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

// ---------------------------------------------------------------------------------------------------------------------
// Windows Implementations.

#ifdef _WIN32

types::PID getCurrentPID()
{
    return GetCurrentProcessId();
}

std::vector<std::string> listSystemDisks()
{
    std::vector<std::string> disks;

    DWORD driveMask = GetLogicalDrives();

    for (char letter = 'A'; letter <= 'Z'; ++letter)
    {
        if (driveMask & (1 << (letter - 'A')))
        {
            disks.emplace_back(std::string(1, letter) + ":/");
        }
    }

    if (disks.empty())
        disks.push_back("C:/");

    return disks;
}

bool isRunningInsideQtCreator()
{
    if (getenv("QT_CREATOR_RUN_MODE") || getenv("QT_LOGGING_TO_CONSOLE"))
        return true;

    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE)
        return false;

    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);

    if (Process32First(hSnapshot, &pe32))
    {
        DWORD currentPID = GetCurrentProcessId();
        do
        {
            if (pe32.th32ProcessID == currentPID)
            {
                DWORD parentPID = pe32.th32ParentProcessID;
                Process32First(hSnapshot, &pe32);
                do
                {
                    if (pe32.th32ProcessID == parentPID)
                    {
                        std::string parentProcess = pe32.szExeFile;
                        if (parentProcess.find("qtcreator") != std::string::npos)
                        {
                            CloseHandle(hSnapshot);
                            return true;
                        }
                    }
                } while (Process32Next(hSnapshot, &pe32));
            }
        } while (Process32Next(hSnapshot, &pe32));
    }

    CloseHandle(hSnapshot);
    return false;
}

float getCPULoad(unsigned wait_ms)
{
    PDH_HQUERY query = nullptr;
    PDH_HCOUNTER counter;
    PDH_FMT_COUNTERVALUE value;

    // Open the performance query.
    if (PdhOpenQuery(nullptr, 0, &query) != ERROR_SUCCESS)
        return -1.0f;

    // Use the English-invariant counter for accurate CPU utility percentage.
    const wchar_t* counterPath = L"\\Processor Information(_Total)\\% Processor Utility";
    if (PdhAddEnglishCounterW(query, counterPath, 0, &counter) != ERROR_SUCCESS)
    {
        PdhCloseQuery(query);
        return -1.0f;
    }

    // Collect initial data and wait for the specified interval.
    PdhCollectQueryData(query);
    std::this_thread::sleep_for(std::chrono::milliseconds(wait_ms));
    PdhCollectQueryData(query);

    // Retrieve the formatted value from the counter.
    if (PdhGetFormattedCounterValue(counter, PDH_FMT_DOUBLE, nullptr, &value) != ERROR_SUCCESS)
    {
        PdhCloseQuery(query);
        return -1.0f;
    }

    // Clean up and return the CPU load percentage.
    PdhCloseQuery(query);
    return static_cast<float>(value.doubleValue);
}

float getRAMLoad()
{
    MEMORYSTATUSEX memStatus = {};

    memStatus.dwLength = sizeof(memStatus);

    if (!GlobalMemoryStatusEx(&memStatus))
        return -1.0f;

    return static_cast<float>(memStatus.dwMemoryLoad);
}






float getSystemDiskSpaceUsage(const std::string& disk)
{
    ULARGE_INTEGER freeBytesAvailable, totalBytes, totalFreeBytes;
    if (!GetDiskFreeSpaceExA(disk.c_str(), &freeBytesAvailable, &totalBytes, &totalFreeBytes))
        return -1.0f;

    if (totalBytes.QuadPart == 0)
        return -1.0f;

    uint64_t usedBytes = totalBytes.QuadPart - totalFreeBytes.QuadPart;
    return static_cast<float>(usedBytes * 100.0 / totalBytes.QuadPart);
}

uint64_t getSystemDiskAvailableSpace(const std::string& disk)
{
    ULARGE_INTEGER freeBytesAvailable;
    if (!GetDiskFreeSpaceExA(disk.c_str(), &freeBytesAvailable, nullptr, nullptr))
        return 0;

    return static_cast<uint64_t>(freeBytesAvailable.QuadPart);
}

uint64_t getSystemDiskUsedSpace(const std::string& disk)
{
    ULARGE_INTEGER totalBytes, totalFreeBytes;
    if (!GetDiskFreeSpaceExA(disk.c_str(), nullptr, &totalBytes, &totalFreeBytes))
        return 0;

    return static_cast<uint64_t>(totalBytes.QuadPart - totalFreeBytes.QuadPart);
}

types::DiskInfoV getSystemDisksInfo()
{
    types::DiskInfoV result;
    const auto disks = utils::listSystemDisks();

    for (const auto& disk : disks)
    {
        float usagePercent = utils::getSystemDiskSpaceUsage(disk);
        if (usagePercent < 0.0f)
            continue;

        uint64_t freeBytes = utils::getSystemDiskAvailableSpace(disk);
        uint64_t usedBytes = utils::getSystemDiskUsedSpace(disk);
        uint64_t totalBytes = usedBytes + freeBytes;

        types::DiskInfo info;
        info.path = disk;
        info.type = types::DiskType::UNKNOWN;  // TODO
        info.usagePercent = usagePercent;
        info.totalBytes = totalBytes;
        info.usedBytes = usedBytes;
        info.freeBytes = freeBytes;

        result.emplace_back(std::move(info));
    }

    return result;
}

// ---------------------------------------------------------------------------------------------------------------------
// Unix Implementations.

#else

#endif

// ---------------------------------------------------------------------------------------------------------------------


END_NAMESPACE(utils)
LEOBASE_MODULE_END
