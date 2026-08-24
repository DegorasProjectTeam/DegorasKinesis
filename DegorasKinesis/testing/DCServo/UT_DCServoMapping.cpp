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

// ASSERTIONS ARE THE POINT OF A TEST, so NDEBUG must not be allowed to delete them. A Release build defines it and
// assert() then expands to NOTHING AT ALL -- the whole expression, not just the abort, so any call written inside an
// assert stops happening too. Measured on this suite before the fix: objdump found ZERO references to assert in all
// nine Release test objects, i.e. every one of the 195 checks was dead, and the suite reported success without
// evaluating any of them. After the fix the same objects carry 197 references and the suite still passes 9/9 in both
// configurations -- so nothing had been failing behind the dead layer here; it simply was not being checked.
//
// BOTH LINES ARE REQUIRED, in this order, above the first #include. The bare #undef is not enough: if anything has
// already pulled in <cassert> while NDEBUG was defined, assert is already expanded away and stays dead. Including
// the header again re-arms it, because assert.h undefines and redefines the macro on every inclusion and <cassert>
// deliberately has no include guard.
#undef NDEBUG
#include <cassert>

// C++ INCLUDES
#include <cassert>
#include <iostream>

// PROJECT INCLUDES
#include "DegorasKinesis/DCServo/dcservo_channel.h"
#include "DegorasKinesis/Kinesis/kinesis_error.h"
#include "DegorasKinesis/Kinesis/kinesis_discovery.h"
#include <DegorasKinesis/Modules/Devices>


using namespace dpkin::types;
using dpkin::dcservo::DCServoChannel;
using dpkin::kinesis::categoryFromKinesis;
using dpkin::kinesis::serialMatchesTypeId;

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

void testSerialTypeId()
{
    // Prefix match against a known type id (Thorlabs serials begin with the device type id).
    assert(serialMatchesTypeId("55000002", 55));     // K10CR2
    assert(serialMatchesTypeId("105000002", 105));   // M30X
    assert(serialMatchesTypeId("101000002", 101));   // M30XY

    // Wrong / cross type: 101 and 105 must not cross-match despite sharing a leading digit.
    assert(!serialMatchesTypeId("105000002", 55));
    assert(!serialMatchesTypeId("101000002", 105));
    assert(!serialMatchesTypeId("105000002", 101));

    // Malformed: non-digits, empty, type-id with no unit number, non-positive type id.
    assert(!serialMatchesTypeId("55ABCDEF", 55));
    assert(!serialMatchesTypeId("", 55));
    assert(!serialMatchesTypeId("55", 55));
    assert(!serialMatchesTypeId("55000002", 0));

    // Device-level convenience forwards to the correct type id.
    assert(dpkin::K10CR2::isCompatibleSerial("55000002"));
    assert(!dpkin::K10CR2::isCompatibleSerial("105000002"));
    assert(dpkin::M30X::isCompatibleSerial("105000002"));
    assert(dpkin::M30XY::isCompatibleSerial("101000002"));
    assert(!dpkin::M30XY::isCompatibleSerial("105000002"));
}

} // namespace

int main()
{
    testKinesisCodeMapping();
    testDeviceErrorCarriesRawCode();
    testChannelBinding();
    testSerialTypeId();

    std::cout << "UT_DCServoMapping: ALL CHECKS PASSED" << std::endl;
    return 0;
}
