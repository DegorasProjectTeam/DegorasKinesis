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
#include <chrono>
#include <iostream>

// PROJECT INCLUDES
#include "DegorasKinesis/Kinesis/kinesis_simulator.h"
#include "DegorasKinesis/Devices/k10cr2.h"
#include "DegorasKinesis/Helpers/wait_for.h"


using namespace dpkin;
using namespace dpkin::kinesis;
using dpkin::types::Channel;
using dpkin::types::OperationResult;
using dpkin::types::StopMode;

// ---------------------------------------------------------------------------------------------------------------------
// LIVE simulator self-check for the K10CR2/M single-axis rotation stage (integrated stepper). Requires the Kinesis
// Simulator running with a virtual K10CR2 (type 55, serial 55000002). Self-skips (exit 0) when unavailable.
//
// It verifies that the device is CONTROLLABLE even without a stage/settings profile (the simulator ships the K10CR2
// with none): connect, homing, device-unit motion, position and status all work. Real-world-unit (degrees) methods
// are gated - they return LOAD_SETTINGS_ERROR until a profile is loaded (real hardware), rather than reporting a
// wrong value. If a profile IS present (real hardware), the degree-based path is exercised too.
// ---------------------------------------------------------------------------------------------------------------------

// Usage: Test_K10CR2Sim [serial]   (serial defaults to the first discovered K10CR2, e.g. sim 55000002).
int main(int argc, char** argv)
{
    using namespace std::chrono;

    KinesisSimulatorSession sim;
    std::cout << "simulator session: " << toString(sim.result()) << "\n";
    if (!sim.usable())
    {
        std::cout << "SKIP: Kinesis simulator not available.\n";
        return 0;
    }

    types::ThorlabsSNList list;
    assert(K10CR2::getDeviceList(list) == OperationResult::OPERATION_OK);

    // Target the serial given on the command line (validated against the K10CR2 type id), else the first discovered.
    std::string serial;
    if (argc > 1)
    {
        serial = argv[1];
        if (!K10CR2::isCompatibleSerial(serial))
        {
            std::cout << "SKIP: serial '" << serial << "' is not a K10CR2 (type 55).\n";
            return 0;
        }
    }
    else if (!list.empty())
    {
        serial = list.front();
    }
    else
    {
        std::cout << "SKIP: no virtual K10CR2 (type 55) configured in the simulator.\n";
        return 0;
    }
    std::cout << "using K10CR2 serial: " << serial << "\n";

    K10CR2 dev(serial);
    assert(dev.getChannelCount() == 1);
    assert(!dev.isConnected());

    // Channel validation is checked before any device I/O, so it works without a connection (single axis only).
    assert(dev.doMoveAbsoluteDeviceUnits(Channel::Y_CHANNEL, 1000) == OperationResult::INVALID_CHANNEL);
    {
        int dummy = 0;
        assert(dev.getChannelPositionDeviceUnits(Channel::Y_CHANNEL, dummy) == OperationResult::INVALID_CHANNEL);
    }

    // Connect now SUCCEEDS even when the stage/settings profile is absent (LoadSettings is non-fatal).
    const OperationResult conn = dev.doConnect();
    std::cout << "doConnect: " << types::toString(conn) << "\n";
    assert(conn == OperationResult::OPERATION_OK);
    assert(dev.isConnected());
    assert(dev.doConnect() == OperationResult::ALREADY_CONNECTED);   // idempotent for the owning object.
    std::cout << "hasRealUnits: " << (dev.hasRealUnits() ? "true" : "false") << "\n";

    // Homing and status work regardless of the unit profile.
    assert(dev.doEnable(Channel::X_CHANNEL, true) == OperationResult::OPERATION_OK);

    // Force a known idle state before homing: a previous aborted run (assert -> abort skips the destructor's close)
    // can leave the simulated device mid-motion, which would make the SDK reject the next command.
    dev.doStop(Channel::X_CHANNEL, StopMode::IMMEDIATE);
    dev.waitForMoveFinished(seconds(5));

    assert(dev.doHome(Channel::X_CHANNEL) == OperationResult::OPERATION_OK);
    dev.waitForHomed(seconds(30));   // homing must finish before a move is accepted

    types::K10CR2DeviceStatus status;
    assert(dev.getDeviceStatus(status) == OperationResult::OPERATION_OK);
    assert(status.connected && status.chann.valid);
    std::cout << "status: " << status.toJsonStr() << "\n";

    // Device-unit (motor count) control works with or without a profile - this is what the simulator/GUI does.
    assert(dev.doMoveRelativeDeviceUnits(Channel::X_CHANNEL, 100000) == OperationResult::OPERATION_OK);
    dev.waitForMoveFinished(seconds(10));
    int counts = 0;
    assert(dev.getChannelPositionDeviceUnits(Channel::X_CHANNEL, counts) == OperationResult::OPERATION_OK);
    std::cout << "device-unit position after relative move: " << counts << " counts\n";

    if (dev.hasRealUnits())
    {
        // A loaded profile (real hardware) -> real-world degrees also work.
        assert(dev.doMoveAbsolute(Channel::X_CHANNEL, 10.0) == OperationResult::OPERATION_OK);
        dev.waitForMoveFinished(seconds(10));
        double deg = 0.0;
        assert(dev.getChannelPosition(Channel::X_CHANNEL, deg) == OperationResult::OPERATION_OK);
        std::cout << "angular position: " << deg << " deg\n";
    }
    else
    {
        // No profile (the simulator default) -> real-world methods refuse cleanly rather than returning garbage.
        assert(dev.doMoveAbsolute(Channel::X_CHANNEL, 10.0) == OperationResult::LOAD_SETTINGS_ERROR);
        double deg = 0.0;
        assert(dev.getChannelPosition(Channel::X_CHANNEL, deg) == OperationResult::LOAD_SETTINGS_ERROR);
        std::cout << "no profile: degree methods correctly return LOAD_SETTINGS_ERROR; device-unit control verified.\n";
    }

    assert(dev.doStop(Channel::X_CHANNEL, StopMode::PROFILED) == OperationResult::OPERATION_OK);
    assert(dev.doDisconnect() == OperationResult::OPERATION_OK);
    assert(!dev.isConnected());

    std::cout << "Test_K10CR2Sim: ALL CHECKS PASSED" << std::endl;
    return 0;
}
