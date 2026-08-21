/*
 *  DegorasKinesis - An extensible C++ library for controlling Thorlabs Kinesis motion devices.
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

// THORLABS INCLUDES
#include <Thorlabs/Thorlabs.MotionControl.Benchtop.DCServo.h>

// PROJECT INCLUDES
#include "DegorasKinesis/Kinesis/kinesis_discovery.h"
#include "DegorasKinesis/Kinesis/kinesis_api_lock.h"


// NAMESPACES
namespace dpkin
{
namespace kinesis
{

using namespace dpkin::types;

// ---------------------------------------------------------------------------------------------------------------------

OperationResult enumerateByTypeId(int type_id, ThorlabsSNList& list)
{
    list.clear();

    // Discovery touches the process-global Kinesis device list: serialise it (fixes the PoC's lock-bypassing
    // static getDeviceList).
    const std::lock_guard<std::mutex> lock(discoveryMtx());

    if (TLI_BuildDeviceList() != 0)
        return OperationResult::THORLABS_INTERNAL_ERROR;

    SAFEARRAY* arr = nullptr;
    const short error = TLI_GetDeviceListByType(&arr, type_id);
    if (error != 0 || arr == nullptr)
    {
        if (arr != nullptr)
            SafeArrayDestroy(arr);
        return OperationResult::THORLABS_INTERNAL_ERROR;
    }

    if (arr->cDims != 1 || arr->cbElements != sizeof(BSTR))
    {
        SafeArrayDestroy(arr);
        return OperationResult::THORLABS_INTERNAL_ERROR;
    }

    LONG lower = 0;
    LONG upper = -1;
    SafeArrayGetLBound(arr, 1, &lower);
    SafeArrayGetUBound(arr, 1, &upper);

    for (LONG i = lower; i <= upper; ++i)
    {
        BSTR bstr_serial = nullptr;
        const HRESULT hr = SafeArrayGetElement(arr, &i, &bstr_serial);
        if (SUCCEEDED(hr) && bstr_serial != nullptr)
        {
            const int len = WideCharToMultiByte(CP_UTF8, 0, bstr_serial, -1, nullptr, 0, nullptr, nullptr);
            if (len > 1)
            {
                std::string serial(static_cast<std::size_t>(len) - 1, '\0');
                WideCharToMultiByte(CP_UTF8, 0, bstr_serial, -1, &serial[0], len - 1, nullptr, nullptr);
                list.push_back(serial);
            }
            SysFreeString(bstr_serial);
        }
    }

    SafeArrayDestroy(arr);
    return OperationResult::OPERATION_OK;
}

// ---------------------------------------------------------------------------------------------------------------------

bool serialMatchesTypeId(const ThorlabsSN& serial, int type_id)
{
    if (type_id <= 0)
        return false;

    const std::string prefix = std::to_string(type_id);

    // Must be the type-id digits followed by at least one unit digit.
    if (serial.size() <= prefix.size())
        return false;

    // A Thorlabs serial is all digits.
    for (const char c : serial)
        if (c < '0' || c > '9')
            return false;

    return serial.compare(0, prefix.size(), prefix) == 0;
}

// ---------------------------------------------------------------------------------------------------------------------

}} // END NAMESPACES

// ---------------------------------------------------------------------------------------------------------------------
