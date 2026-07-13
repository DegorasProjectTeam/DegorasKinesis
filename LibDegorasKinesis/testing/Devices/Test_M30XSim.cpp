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
#include <cassert>
#include <chrono>
#include <iostream>

// PROJECT INCLUDES
#include "LibDegorasKinesis/Kinesis/kinesis_simulator.h"
#include "LibDegorasKinesis/Devices/m30x.h"
#include "LibDegorasKinesis/Helpers/wait_for.h"


using namespace dpkin;
using namespace dpkin::kinesis;
using dpkin::types::Channel;
using dpkin::types::OperationResult;
using dpkin::types::StopMode;

// ---------------------------------------------------------------------------------------------------------------------
// LIVE simulator self-check for the M30X single-axis device. Requires the Kinesis Simulator running with a virtual
// M30X (type 105). Self-skips (exit 0) when unavailable.
//
// It verifies that the device is CONTROLLABLE even without a stage/settings profile (the simulator ships the M30X
// with none): connect, homing, device-unit motion, position and status all work. Real-world-unit (mm) methods are
// gated - they return LOAD_SETTINGS_ERROR until a profile is loaded (real hardware) - rather than reporting a wrong
// value. If a profile IS present (e.g. real hardware), the mm-based path is exercised too.
// ---------------------------------------------------------------------------------------------------------------------

// Usage: Test_M30XSim [serial]   (serial defaults to the first discovered M30X, e.g. sim 105000002).
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
    assert(M30X::getDeviceList(list) == OperationResult::OPERATION_OK);

    // Target the serial given on the command line (validated against the M30X type id), else the first discovered.
    std::string serial;
    if (argc > 1)
    {
        serial = argv[1];
        if (!M30X::isCompatibleSerial(serial))
        {
            std::cout << "SKIP: serial '" << serial << "' is not an M30X (type 105).\n";
            return 0;
        }
    }
    else if (!list.empty())
    {
        serial = list.front();
    }
    else
    {
        std::cout << "SKIP: no virtual M30X (type 105) configured in the simulator.\n";
        return 0;
    }
    std::cout << "using M30X serial: " << serial << "\n";

    M30X dev(serial);
    assert(dev.getChannelCount() == 1);
    assert(!dev.isConnected());

    // Channel validation is checked before any device I/O, so it works without a connection.
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
    assert(dev.doConnect() == OperationResult::ALREADY_CONNECTED);
    std::cout << "hasRealUnits: " << (dev.hasRealUnits() ? "true" : "false") << "\n";

    // Homing and status work regardless of the unit profile.
    assert(dev.doEnable(Channel::X_CHANNEL, true) == OperationResult::OPERATION_OK);

    // Force a known idle state before homing: a previous aborted run (assert -> abort skips the destructor's close)
    // can leave the simulated device mid-motion, which would make the SDK reject the next command.
    dev.doStop(Channel::X_CHANNEL, StopMode::IMMEDIATE);
    dev.waitForMoveFinished(seconds(5));

    assert(dev.doHome(Channel::X_CHANNEL) == OperationResult::OPERATION_OK);
    dev.waitForHomed(seconds(30));   // homing must finish before a move is accepted

    types::M30XDeviceStatus status;
    assert(dev.getDeviceStatus(status) == OperationResult::OPERATION_OK);
    assert(status.connected && status.chann.valid);
    std::cout << "status: " << status.toJsonStr() << "\n";

    // Device-unit (motor count) control works with or without a profile.
    assert(dev.doMoveRelativeDeviceUnits(Channel::X_CHANNEL, 100000) == OperationResult::OPERATION_OK);
    dev.waitForMoveFinished(seconds(10));
    int counts = 0;
    assert(dev.getChannelPositionDeviceUnits(Channel::X_CHANNEL, counts) == OperationResult::OPERATION_OK);
    std::cout << "device-unit position after relative move: " << counts << " counts\n";

    if (dev.hasRealUnits())
    {
        assert(dev.doMoveAbsolute(Channel::X_CHANNEL, 3.0) == OperationResult::OPERATION_OK);
        dev.waitForMoveFinished(seconds(10));
        double mm = 0.0;
        assert(dev.getChannelPosition(Channel::X_CHANNEL, mm) == OperationResult::OPERATION_OK);
        std::cout << "position: " << mm << " mm\n";
    }
    else
    {
        assert(dev.doMoveAbsolute(Channel::X_CHANNEL, 3.0) == OperationResult::LOAD_SETTINGS_ERROR);
        double mm = 0.0;
        assert(dev.getChannelPosition(Channel::X_CHANNEL, mm) == OperationResult::LOAD_SETTINGS_ERROR);
        std::cout << "no profile: mm methods correctly return LOAD_SETTINGS_ERROR; device-unit control verified.\n";
    }

    assert(dev.doStop(Channel::X_CHANNEL, StopMode::PROFILED) == OperationResult::OPERATION_OK);
    assert(dev.doDisconnect() == OperationResult::OPERATION_OK);
    assert(!dev.isConnected());

    std::cout << "Test_M30XSim: ALL CHECKS PASSED" << std::endl;
    return 0;
}
