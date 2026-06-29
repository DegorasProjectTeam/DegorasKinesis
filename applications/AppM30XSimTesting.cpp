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
#include <chrono>
#include <cmath>
#include <iostream>

// PROJECT INCLUDES
#include "kinesis_simulator.h"
#include "m30x.h"
#include "wait_for.h"


using namespace thorlabs;
using thorlabs::types::Channel;
using thorlabs::types::OperationResult;
using thorlabs::types::StopMode;

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
    assert(dev.waitForHomed(seconds(60)) == OperationResult::OPERATION_OK);

    types::M30XDeviceStatus status;
    assert(dev.getDeviceStatus(status) == OperationResult::OPERATION_OK);
    assert(status.connected && status.chann.valid && status.chann.flags.homed);
    std::cout << "status: " << status.toJsonStr() << "\n";

    assert(dev.doMoveAbsolute(Channel::X_CHANNEL, 3.0) == OperationResult::OPERATION_OK);
    const OperationResult reached = waitForCondition([&]()
    {
        double mm = 0.0;
        return dev.getChannelPosition(Channel::X_CHANNEL, mm) == OperationResult::OPERATION_OK
               && std::abs(mm - 3.0) < 0.2;
    }, seconds(30), milliseconds(100));
    double pos_mm = -1.0;
    dev.getChannelPosition(Channel::X_CHANNEL, pos_mm);
    std::cout << "position after move: " << pos_mm << " mm (reached=" << types::toString(reached) << ")\n";
    assert(reached == OperationResult::OPERATION_OK);
    assert(dev.waitForMoveFinished(seconds(10)) == OperationResult::OPERATION_OK);

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
        std::cout << "AppM30XSimTesting: PASSED (motion skipped - sim M30X has no stage configured)" << std::endl;
        return 0;
    }

    assert(conn == OperationResult::OPERATION_OK);
    runFullMotion(dev, serial);

    assert(dev.doDisconnect() == OperationResult::OPERATION_OK);
    assert(!dev.isConnected());

    std::cout << "AppM30XSimTesting: ALL CHECKS PASSED" << std::endl;
    return 0;
}
