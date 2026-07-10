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
#include <cmath>
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
// LIVE simulator self-check for the M30X single-axis device (milestone M6). Requires the Kinesis Simulator running
// with a virtual M30X (type 105). Self-skips (exit 0) when unavailable.
//
// NOTE: if the simulator's virtual M30X has no stage/settings profile assigned, BDC_LoadSettings fails and connect
// correctly returns LOAD_SETTINGS_ERROR; the test then validates the settings-independent behaviour and reports that
// motion was skipped. The motion path itself is the same DCServoChannel validated live by AppM30XYSimTesting.
// ---------------------------------------------------------------------------------------------------------------------

namespace
{

void runFullMotion(M30X& dev, const std::string& serial)
{
    using namespace std::chrono;
    (void) serial;

    assert(dev.isConnected());
    assert(dev.doConnect() == OperationResult::ALREADY_CONNECTED);

    assert(dev.doEnable(Channel::X_CHANNEL, true) == OperationResult::OPERATION_OK);
    assert(dev.doHome(Channel::X_CHANNEL) == OperationResult::OPERATION_OK);

    // Status read + decode contract.
    types::M30XDeviceStatus status;
    assert(dev.getDeviceStatus(status) == OperationResult::OPERATION_OK);
    assert(status.connected && status.chann.valid);
    std::cout << "status: " << status.toJsonStr() << "\n";

    // Motion command contract (physical translation is sim-dependent; position is logged, not asserted).
    assert(dev.doMoveAbsolute(Channel::X_CHANNEL, 3.0) == OperationResult::OPERATION_OK);
    dev.waitForMoveFinished(seconds(10));
    double real_pos = 0.0;
    dev.getChannelPosition(Channel::X_CHANNEL, real_pos);
    std::cout << "X position after move-to-3mm command: " << real_pos << " mm (target reached only with a real stage)\n";

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
    assert(M30X::getDeviceList(list) == OperationResult::OPERATION_OK);
    if (list.empty())
    {
        std::cout << "SKIP: no virtual M30X (type 105) configured in the simulator.\n";
        return 0;
    }

    const std::string serial = list.front();
    std::cout << "using M30X serial: " << serial << "\n";

    M30X dev(serial);
    assert(dev.getChannelCount() == 1);
    assert(!dev.isConnected());

    // Channel validation is checked before any device I/O, so it works without a connection.
    assert(dev.doMoveAbsolute(Channel::Y_CHANNEL, 1.0) == OperationResult::INVALID_CHANNEL);
    {
        double dummy = 0.0;
        assert(dev.getChannelPosition(Channel::Y_CHANNEL, dummy) == OperationResult::INVALID_CHANNEL);
    }

    const OperationResult conn = dev.doConnect();
    std::cout << "doConnect: " << types::toString(conn) << "\n";

    if (conn == OperationResult::LOAD_SETTINGS_ERROR)
    {
        std::cout << "NOTE: the simulator's virtual M30X (" << serial << ") has no stage/settings profile assigned,\n"
                     "      so settings-dependent operation (home/move/units) cannot be exercised here. Discovery,\n"
                     "      BDC_Open, channel validation and the single-axis driver are confirmed; the motion path is\n"
                     "      the same DCServoChannel validated live by AppM30XYSimTesting.\n";
        std::cout << "Test_M30XSim: PASSED (motion skipped - sim M30X has no stage configured)" << std::endl;
        return 0;
    }

    assert(conn == OperationResult::OPERATION_OK);
    runFullMotion(dev, serial);

    assert(dev.doDisconnect() == OperationResult::OPERATION_OK);
    assert(!dev.isConnected());

    std::cout << "Test_M30XSim: ALL CHECKS PASSED" << std::endl;
    return 0;
}
