/*
 *  LibThorlabsKinesis - An extensible C++ library for controlling Thorlabs Kinesis motion devices.
 *
 *  Copyright (C) 2018-2026 Degoras Project Team
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
#include <cassert>
#include <iostream>

// PROJECT INCLUDES
#include "LibThorlabsKinesis/DCServo/dcservo_channel.h"


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
