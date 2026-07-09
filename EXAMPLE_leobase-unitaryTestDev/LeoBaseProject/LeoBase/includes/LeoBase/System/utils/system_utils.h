/*
 *      Copyright(C) Milethos Technologies SLU. 2025
 *      All rights reserved. Reproduction in whole or in
 *      part is prohibited without the written consent of
 *      the copyright holder.
 */

#pragma once

// C++ INCLUDES
#include <cstdint>
#include <string>
#include <vector>

// LEOBASE INCLUDES
#include "LeoBase/Aliases/Global"
#include "LeoBase/System/types/system_types.h"

// LEOBASE MACROS
LEOBASE_MODULE_BEGIN(system)
BEGIN_NAMESPACE(utils)

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Retrieves the process ID (PID) of the current process.
 * @return unsigned The process ID of the current process.
 */
LEOBASE_EXPORT types::PID getCurrentPID();

/**
 * @brief Pauses execution and waits for user input.
 *
 * This function displays a log message (if provided) and then waits for user input. It clears
 * any error flags on std::cin and discards any remaining characters in the input buffer.
 *
 * @param log_text A message to display before pausing. If empty, no message is displayed.
 */
LEOBASE_EXPORT void userInputPause(const std::string& log_text = std::string());

/**
 * @brief Lists all system disks (drives).
 *
 * - Windows: Lists all logical drives.
 * - Linux/Unix: Lists all mounted storage devices from `/etc/mtab`.
 *
 * @return A vector containing all available system disks.
 */
LEOBASE_EXPORT std::vector<std::string> listSystemDisks();

/**
 * @brief Checks if the program is running inside Qt Creator's internal terminal.
 *
 * @return True if running inside Qt Creator, false otherwise.
 */
LEOBASE_EXPORT bool isRunningInsideQtCreator();

/**
 * @brief Retrieves the total CPU load as a percentage.
 *
 * This function measures the system-wide CPU usage over a short sampling period,
 * returning the percentage of CPU time actively used (non-idle).
 *
 * The implementation is platform-specific:
 * - On Windows, it uses Performance Data Helper (PDH) with the "\\Processor Information(_Total)\\% Processor Utility"
 * counter, which closely matches Task Manager.
 * - On Linux, TODO.
 *
 * The sampling interval is defined by the wait_ms parameter. Shorter intervals
 * provide faster responsiveness, while longer intervals yield more stable and accurate readings.
 *
 * @param wait_ms Number of milliseconds to wait between samples (default is 250ms).
 *
 * @return A float representing total CPU usage in percentage (0.0 to 100.0),
 *         or -1.0f on failure or unsupported platform.
 */
LEOBASE_EXPORT float getCPULoad(unsigned wait_ms = 250);

/**
 * @brief Retrieves the total RAM usage as a percentage.
 *
 * This function measures the system-wide physical memory usage and returns
 * the percentage of RAM currently in use.
 *
 * The implementation is platform-specific:
 * - On Windows, it uses the GlobalMemoryStatusEx API to obtain memory statistics.
 * - On Linux, TODO.
 *
 * This function provides a quick snapshot of memory usage at the moment of invocation.
 *
 * @return A float representing total RAM usage in percentage (0.0 to 100.0),
 *         or -1.0f on failure or unsupported platform.
 */
LEOBASE_EXPORT float getRAMLoad();

// TODO
//LEOBASE_EXPORT float getDiskLoad(unsigned wait_ms = 250)

/**
 * @brief Retrieves the total used disk space percentage for a given disk.
 *
 * This function checks the specified drive or mount point and returns the
 * percentage of disk space currently used.
 *
 * The implementation is platform-specific:
 * - On Windows, it uses GetDiskFreeSpaceEx to determine space usage.
 * - On Linux, TODO.
 *
 * @param disk The root path of the disk (e.g., "C:/").
 * @return A float representing the percentage of used disk space (0.0 to 100.0),
 *         or -1.0f on failure or unsupported platform.
 */
LEOBASE_EXPORT float getSystemDiskSpaceUsage(const std::string& disk);

/**
 * @brief Retrieves the available disk space in bytes for a given disk.
 *
 * This function checks the specified drive or mount point and returns the
 * number of bytes available to the user.
 *
 * The implementation is platform-specific:
 * - On Windows, it uses GetDiskFreeSpaceEx to determine free space.
 * - On Linux, TODO.
 *
 * @param disk The root path of the disk (e.g., "C:/").
 * @return A uint64_t representing the number of available bytes,
 *         or 0 on failure or unsupported platform.
 */
LEOBASE_EXPORT uint64_t getSystemDiskAvailableSpace(const std::string& disk);

/**
 * @brief Retrieves the used disk space in bytes for a given disk.
 *
 * This function returns the number of bytes currently used on the specified disk.
 *
 * The implementation is platform-specific:
 * - On Windows, it uses GetDiskFreeSpaceEx to calculate the used space.
 * - On Linux, TODO.
 *
 * @param disk The root path of the disk (e.g., "C:/").
 * @return A uint64_t representing the number of used bytes,
 *         or 0 on failure or unsupported platform.
 */
LEOBASE_EXPORT uint64_t getSystemDiskUsedSpace(const std::string& disk);

/**
 * @brief Retrieves information for all logical system disks.
 *
 * This function enumerates all available logical disks and gathers usage statistics
 * for each one, including total size, used space, available space, and usage percentage.
 *
 * The implementation is platform-specific:
 * - On Windows, it uses GetLogicalDrives and GetDiskFreeSpaceEx to obtain drive
 *   availability and space statistics.
 * - On Linux, TODO.
 *
 * TODO: Disk types are currently marked as DiskType::UNKNOWN and can be extended with
 * platform-specific detection logic.
 *
 * @return A vector of DiskInfo structures describing each available logical disk.
 */
LEOBASE_EXPORT types::DiskInfoV getSystemDisksInfo();

// ---------------------------------------------------------------------------------------------------------------------

END_NAMESPACE(utils)
LEOBASE_MODULE_END
