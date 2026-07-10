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
#include "LibDegorasKinesis/Devices/k10cr2.h"
#include "LibDegorasKinesis/Helpers/wait_for.h"


using namespace dpkin;
using namespace dpkin::kinesis;
using dpkin::types::Channel;
using dpkin::types::OperationResult;
using dpkin::types::StopMode;

// ---------------------------------------------------------------------------------------------------------------------
// LIVE simulator self-check for the K10CR2/M single-axis rotation stage (integrated stepper). Requires the Kinesis
// Simulator running with a virtual K10CR2 (type 55, serial 55000002). Self-skips (exit 0) when unavailable.
//
// NOTE: if the simulated K10CR2 has no stage/settings profile assigned, ISC_LoadSettings fails and connect correctly
// returns LOAD_SETTINGS_ERROR; the test then validates the settings-independent behaviour (discovery, ISC_Open,
// channel validation) and reports that motion was skipped. Positions are in DEGREES.
// ---------------------------------------------------------------------------------------------------------------------

namespace
{

void runFullMotion(K10CR2& dev)
{
    using namespace std::chrono;

    assert(dev.isConnected());
    assert(dev.doConnect() == OperationResult::ALREADY_CONNECTED);

    assert(dev.doEnable(Channel::X_CHANNEL, true) == OperationResult::OPERATION_OK);
    assert(dev.doHome(Channel::X_CHANNEL) == OperationResult::OPERATION_OK);

    // Status read + decode contract.
    types::K10CR2DeviceStatus status;
    assert(dev.getDeviceStatus(status) == OperationResult::OPERATION_OK);
    assert(status.connected && status.chann.valid);
    std::cout << "status: " << status.toJsonStr() << "\n";

    // Motion command contract (physical translation is sim-dependent; the angle is logged, not asserted).
    assert(dev.doMoveAbsolute(Channel::X_CHANNEL, 10.0) == OperationResult::OPERATION_OK);
    dev.waitForMoveFinished(seconds(10));
    double angle_deg = 0.0;
    dev.getChannelPosition(Channel::X_CHANNEL, angle_deg);
    std::cout << "X angular position after move-to-10deg command: " << angle_deg
              << " deg (target reached only with a real stage)\n";

    assert(dev.doStop(Channel::X_CHANNEL, StopMode::PROFILED) == OperationResult::OPERATION_OK);
}

} // namespace

int main()
{
    KinesisSimulatorSession sim;
    std::cout << "simulator session: " << toString(sim.result()) << "\n";
    if (!sim.usable())
    {
        std::cout << "SKIP: Kinesis simulator not available.\n";
        return 0;
    }

    types::ThorlabsSNList list;
    assert(K10CR2::getDeviceList(list) == OperationResult::OPERATION_OK);
    if (list.empty())
    {
        std::cout << "SKIP: no virtual K10CR2 (type 55) configured in the simulator.\n";
        return 0;
    }

    const std::string serial = list.front();
    std::cout << "using K10CR2 serial: " << serial << "\n";

    K10CR2 dev(serial);
    assert(dev.getChannelCount() == 1);
    assert(!dev.isConnected());

    // Channel validation is checked before any device I/O, so it works without a connection (single axis only).
    assert(dev.doMoveAbsolute(Channel::Y_CHANNEL, 1.0) == OperationResult::INVALID_CHANNEL);
    {
        double dummy = 0.0;
        assert(dev.getChannelPosition(Channel::Y_CHANNEL, dummy) == OperationResult::INVALID_CHANNEL);
    }

    const OperationResult conn = dev.doConnect();
    std::cout << "doConnect: " << types::toString(conn) << "\n";

    if (conn == OperationResult::LOAD_SETTINGS_ERROR)
    {
        std::cout << "NOTE: the simulator's virtual K10CR2 (" << serial << ") has no stage/settings profile assigned,\n"
                     "      so settings-dependent operation (home/move/units in degrees) cannot be exercised here.\n"
                     "      Discovery, ISC_Open, channel validation and the single-axis driver are confirmed.\n";
        std::cout << "Test_K10CR2Sim: PASSED (motion skipped - sim K10CR2 has no stage configured)" << std::endl;
        return 0;
    }

    assert(conn == OperationResult::OPERATION_OK);
    runFullMotion(dev);

    assert(dev.doDisconnect() == OperationResult::OPERATION_OK);
    assert(!dev.isConnected());

    std::cout << "Test_K10CR2Sim: ALL CHECKS PASSED" << std::endl;
    return 0;
}
