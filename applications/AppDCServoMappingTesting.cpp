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

// PROJECT INCLUDES
#include "dcservo_channel.h"


using namespace thorlabs::types;
using thorlabs::dcservo::DCServoChannel;
using thorlabs::dcservo::categoryFromKinesis;

// ---------------------------------------------------------------------------------------------------------------------
// No-hardware self-check for the Layer-2 adapter (milestone M4): FT_*->OperationResult mapping and channel binding.
// The actual BDC_* I/O requires the Kinesis simulator or real hardware and is exercised by AppM30XYHardwareTesting.
// ---------------------------------------------------------------------------------------------------------------------

namespace
{

void testKinesisCodeMapping()
{
    // Verified against Thorlabs.MotionControl.Benchtop.DCServo.h (FT_* enum).
    assert(categoryFromKinesis(0x00) == OperationResult::OPERATION_OK);            // FT_OK
    assert(categoryFromKinesis(0x02) == OperationResult::DEVICE_NOT_FOUND);        // FT_DeviceNotFound
    assert(categoryFromKinesis(0x03) == OperationResult::NOT_CONNECTED);           // FT_DeviceNotOpened
    assert(categoryFromKinesis(0x04) == OperationResult::NOT_CONNECTED);           // FT_IOError
    assert(categoryFromKinesis(0x07) == OperationResult::NOT_CONNECTED);           // FT_DeviceNotPresent
    assert(categoryFromKinesis(0x01) == OperationResult::THORLABS_INTERNAL_ERROR); // FT_InvalidHandle
    assert(categoryFromKinesis(0x05) == OperationResult::THORLABS_INTERNAL_ERROR); // FT_InsufficientResources
    assert(categoryFromKinesis(0x06) == OperationResult::THORLABS_INTERNAL_ERROR); // FT_InvalidParameter
    assert(categoryFromKinesis(0x08) == OperationResult::THORLABS_INTERNAL_ERROR); // FT_IncorrectDevice
    assert(categoryFromKinesis(0x42) == OperationResult::THORLABS_INTERNAL_ERROR); // unknown code
}

void testDeviceErrorCarriesRawCode()
{
    // A non-zero code maps to a category but the raw code must survive in DeviceError.
    DeviceError e;
    e.category = categoryFromKinesis(0x06);
    e.kinesis_code = 0x06;
    e.context = "BDC_MoveToPosition";
    assert(!e.ok());
    assert(e.category == OperationResult::THORLABS_INTERNAL_ERROR);
    assert(e.kinesis_code == 0x06);
    assert(e.toString().find("BDC_MoveToPosition") != std::string::npos);
}

void testChannelBinding()
{
    DCServoChannel x("SIM-CTRL-1", Channel::X_CHANNEL);
    DCServoChannel y("SIM-CTRL-1", Channel::Y_CHANNEL);
    assert(x.serialNo() == "SIM-CTRL-1");
    assert(x.channel() == Channel::X_CHANNEL);
    assert(y.channel() == Channel::Y_CHANNEL);
}

} // namespace

int main()
{
    testKinesisCodeMapping();
    testDeviceErrorCarriesRawCode();
    testChannelBinding();

    std::cout << "AppDCServoMappingTesting: ALL CHECKS PASSED" << std::endl;
    return 0;
}
