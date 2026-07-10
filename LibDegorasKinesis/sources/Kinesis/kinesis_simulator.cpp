/*
 *  LibDegorasKinesis - An extensible C++ library for controlling Thorlabs Kinesis motion devices.
 *
 *  Developed as free software by and for the Spanish Navy Observatory SLR station (SFEL) in San Fernando.
 *
 *  Copyright (C) 2024-2026 Degoras Project Team
 *                          < Ángel Vera Herrera, avera@roa.es - angelvh.engr@gmail.com >
 *                          < Jesús Relinque Madroñal, jrelinque@roa.es >
 *
 *  This program is free software: you can redistribute it and/or modify it under the terms of the GNU General
 *  Public License as published by the Free Software Foundation, either version 3 of the License, or (at your
 *  option) any later version.
 *
 *  This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the
 *  implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 *  for more details.
 *
 *  You should have received a copy of the GNU General Public License along with this program. If not, see
 *  <https://www.gnu.org/licenses/>.
 *
 *  SPDX-License-Identifier: GPL-3.0-or-later
 */

// C++ INCLUDES
#include <mutex>
#include <string>
#include <windows.h>
#include <tlhelp32.h>

// THORLABS INCLUDES
#include <Thorlabs/Thorlabs.MotionControl.Benchtop.DCServo.h>

// PROJECT INCLUDES
#include "LibDegorasKinesis/Kinesis/kinesis_simulator.h"
#include "LibDegorasKinesis/Kinesis/kinesis_api_lock.h"


// NAMESPACES
namespace dpkin
{
namespace kinesis
{

namespace
{

constexpr wchar_t kKinesisSimulatorProcessName[] = L"Thorlabs.MotionControl.KinesisSimulator.exe";
constexpr wchar_t kKinesisSimulatorMutexName[] = L"Global\\Degoras_Thorlabs_KinesisSimulator_Init";

std::mutex gLocalSimulatorMtx;
HANDLE gSimulatorMtxHandle = nullptr;
bool gSimulatorInitializedInProcess = false;

} // namespace

// ---------------------------------------------------------------------------------------------------------------------

bool isKinesisSimulatorRunning()
{
    const std::wstring target_process = kKinesisSimulatorProcessName;

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE)
        return false;

    PROCESSENTRY32W entry{};
    entry.dwSize = sizeof(entry);

    bool found = false;
    if (Process32FirstW(snapshot, &entry))
    {
        do
        {
            if (_wcsicmp(entry.szExeFile, target_process.c_str()) == 0)
            {
                found = true;
                break;
            }
        }
        while (Process32NextW(snapshot, &entry));
    }

    CloseHandle(snapshot);
    return found;
}

KinesisSimulatorResult connectKinesisSimulator()
{
    if (!isKinesisSimulatorRunning())
        return KinesisSimulatorResult::SIMULATOR_NOT_RUNNING;

    const std::lock_guard<std::mutex> local_lock(gLocalSimulatorMtx);

    if (gSimulatorInitializedInProcess)
        return KinesisSimulatorResult::ALREADY_CONNECTED;

    HANDLE mutex_handle = CreateMutexW(nullptr, FALSE, kKinesisSimulatorMutexName);
    if (mutex_handle == nullptr)
        return KinesisSimulatorResult::MUTEX_ERROR;

    const DWORD wait_result = WaitForSingleObject(mutex_handle, 0);

    if (wait_result == WAIT_TIMEOUT)
    {
        CloseHandle(mutex_handle);
        return KinesisSimulatorResult::BUSY_BY_OTHER_PROCESS;
    }

    if (wait_result != WAIT_OBJECT_0 && wait_result != WAIT_ABANDONED)
    {
        CloseHandle(mutex_handle);
        return KinesisSimulatorResult::MUTEX_ERROR;
    }

    {
        const std::lock_guard<std::mutex> api_lock(discoveryMtx());

        // The Thorlabs C API does not normally throw, but this keeps the interprocess mutex from being leaked
        // if the wrapper/runtime does.
        try
        {
            TLI_InitializeSimulations();
        }
        catch (...)
        {
            ReleaseMutex(mutex_handle);
            CloseHandle(mutex_handle);
            return KinesisSimulatorResult::KINESIS_INITIALIZE_ERROR;
        }
    }

    if (!isKinesisSimulatorRunning())
    {
        ReleaseMutex(mutex_handle);
        CloseHandle(mutex_handle);
        return KinesisSimulatorResult::SIMULATOR_NOT_RUNNING;
    }

    gSimulatorMtxHandle = mutex_handle;
    gSimulatorInitializedInProcess = true;

    return KinesisSimulatorResult::OK;
}

KinesisSimulatorResult disconnectKinesisSimulator()
{
    const std::lock_guard<std::mutex> local_lock(gLocalSimulatorMtx);

    if (!gSimulatorInitializedInProcess)
        return KinesisSimulatorResult::ALREADY_DISCONNECTED;

    {
        const std::lock_guard<std::mutex> api_lock(discoveryMtx());

        try
        {
            TLI_UninitializeSimulations();
        }
        catch (...)
        {
            return KinesisSimulatorResult::KINESIS_UNINITIALIZE_ERROR;
        }
    }

    gSimulatorInitializedInProcess = false;

    if (gSimulatorMtxHandle != nullptr)
    {
        const BOOL released = ReleaseMutex(gSimulatorMtxHandle);
        const BOOL closed = CloseHandle(gSimulatorMtxHandle);
        gSimulatorMtxHandle = nullptr;

        if (!released || !closed)
            return KinesisSimulatorResult::MUTEX_ERROR;
    }

    return KinesisSimulatorResult::OK;
}

std::string toString(KinesisSimulatorResult result)
{
    switch (result)
    {
        case KinesisSimulatorResult::OK:                         return "OK";
        case KinesisSimulatorResult::ALREADY_CONNECTED:          return "ALREADY_CONNECTED";
        case KinesisSimulatorResult::ALREADY_DISCONNECTED:       return "ALREADY_DISCONNECTED";
        case KinesisSimulatorResult::SIMULATOR_NOT_RUNNING:      return "SIMULATOR_NOT_RUNNING";
        case KinesisSimulatorResult::BUSY_BY_OTHER_PROCESS:      return "BUSY_BY_OTHER_PROCESS";
        case KinesisSimulatorResult::MUTEX_ERROR:                return "MUTEX_ERROR";
        case KinesisSimulatorResult::KINESIS_INITIALIZE_ERROR:   return "KINESIS_INITIALIZE_ERROR";
        case KinesisSimulatorResult::KINESIS_UNINITIALIZE_ERROR: return "KINESIS_UNINITIALIZE_ERROR";
    }
    return "UNKNOWN";
}

// ---------------------------------------------------------------------------------------------------------------------

KinesisSimulatorSession::KinesisSimulatorSession() :
    result_(connectKinesisSimulator())
{}

KinesisSimulatorSession::~KinesisSimulatorSession()
{
    // Only tear down if THIS session established the connection; ALREADY_CONNECTED means another owner holds it.
    if (this->result_ == KinesisSimulatorResult::OK)
        static_cast<void>(disconnectKinesisSimulator());
}

// ---------------------------------------------------------------------------------------------------------------------

}} // END NAMESPACES

// ---------------------------------------------------------------------------------------------------------------------
