/*
 *  Copyright (C) 2018-2026 Degoras Project Team
 *
 *  This file is part of a small-scale research or utility tool built atop
 *  the Degoras Project infrastructure, released under the MIT License.
 *
 *  SPDX-License-Identifier: MIT
 *
 *  See the LICENSE file in the root directory for full license details.
 */

// C++ INCLUDES
#include <cassert>
#include <iostream>
#include <set>
#include <string>

// PROJECT INCLUDES
#include "common_types.h"


using namespace thorlabs::types;

// ---------------------------------------------------------------------------------------------------------------------
// No-hardware self-check for the Layer-1 vocabulary and error model (milestone M1).
// Built as an assert()-based executable, matching the proof-of-concept "testing is an app" convention.
// ---------------------------------------------------------------------------------------------------------------------

namespace
{

void testOperationResultToStringIsTotalAndUnique()
{
    const OperationResult all[] = {
        OperationResult::OPERATION_OK,            OperationResult::NOT_CONNECTED,
        OperationResult::DEVICE_NOT_FOUND,        OperationResult::ALREADY_CONNECTED,
        OperationResult::SERIAL_IN_USE,           OperationResult::LOAD_SETTINGS_ERROR,
        OperationResult::START_POLLING_ERROR,     OperationResult::OPERATION_TIMEOUT,
        OperationResult::THORLABS_INTERNAL_ERROR, OperationResult::WORKER_ALREADY_RUNNING,
        OperationResult::WORKER_NOT_RUNNING,      OperationResult::WORKER_START_ERROR,
        OperationResult::READ_FAILED,             OperationResult::INVALID_CHANNEL
    };

    std::set<std::string> seen;
    for (OperationResult r : all)
    {
        const std::string s = toString(r);
        assert(!s.empty() && "every result must stringify");
        assert(s != "UNKNOWN_OPERATION_RESULT" && "every enumerator must be handled by toString");
        assert(seen.insert(s).second && "toString values must be unique");
    }
}

void testDeviceErrorSemantics()
{
    DeviceError ok_err;
    assert(ok_err.ok());
    assert(ok_err.category == OperationResult::OPERATION_OK);
    assert(ok_err.kinesis_code == 0);

    DeviceError err;
    err.category = OperationResult::THORLABS_INTERNAL_ERROR;
    err.kinesis_code = 4;
    err.context = "BDC_Home(ch=1)";
    assert(!err.ok());

    const std::string msg = err.toString();
    assert(msg.find("THORLABS_INTERNAL_ERROR") != std::string::npos);
    assert(msg.find("4") != std::string::npos);
    assert(msg.find("BDC_Home(ch=1)") != std::string::npos);
}

void testNumericAdapters()
{
    assert(toType(Channel::X_CHANNEL) == 1);
    assert(toType(Channel::Y_CHANNEL) == 2);
    assert(toType(StopMode::IMMEDIATE) == 1);
    assert(toType(StopMode::PROFILED) == 2);
    assert(toType(JogMode::CONTINUOUS) == 1);
    assert(toType(PhysicalUnit::DISTANCE) == 0);
    assert(toType(PhysicalUnit::ACCELERATION) == 2);
    assert(toType(TravelDirection::FORWARDS) == 1);
    assert(toType(TravelDirection::REVERSE) == 2);
}

} // namespace

int main()
{
    // Compile-time guarantees: explicit/stable result values, and the TravelDirection bug fix
    // (the proof of concept defined UNDEFINED and FORWARDS both as 0x01).
    static_assert(static_cast<int>(OperationResult::OPERATION_OK) == 0, "OPERATION_OK must be 0");
    static_assert(static_cast<int>(OperationResult::READ_FAILED) == 12, "READ_FAILED must be 12");
    static_assert(static_cast<int>(TravelDirection::UNDEFINED) == 0x00, "UNDEFINED must be 0x00 (bug fix)");
    static_assert(static_cast<int>(TravelDirection::FORWARDS) == 0x01, "FORWARDS must be 0x01");
    static_assert(static_cast<int>(TravelDirection::UNDEFINED) != static_cast<int>(TravelDirection::FORWARDS),
                  "UNDEFINED and FORWARDS must be distinguishable");

    testOperationResultToStringIsTotalAndUnique();
    testDeviceErrorSemantics();
    testNumericAdapters();

    std::cout << "AppErrorMappingTesting: ALL CHECKS PASSED" << std::endl;
    return 0;
}
