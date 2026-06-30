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
#include <atomic>
#include <cassert>
#include <chrono>
#include <cmath>
#include <iostream>

// PROJECT INCLUDES
#include "LibThorlabsKinesis/DCServo/kinesis_simulator.h"
#include "LibThorlabsKinesis/Devices/m30xy.h"
#include "LibThorlabsKinesis/Common/wait_for.h"


using namespace thorlabs;
using thorlabs::types::Channel;
using thorlabs::types::OperationResult;
using thorlabs::types::StopMode;

// ---------------------------------------------------------------------------------------------------------------------
// LIVE simulator self-check for the M30XY device (milestone M5). Requires the Thorlabs Kinesis Simulator running with
// a virtual M30XY (type 101) configured. If the simulator/device is absent, the test SKIPS (exit 0) so it never fails
// on machines without it. NOT a real-hardware test (no optics) - see AppM30XYHardwareTesting for that.
// ---------------------------------------------------------------------------------------------------------------------

int main()
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
    assert(M30XY::getDeviceList(list) == OperationResult::OPERATION_OK);
    if (list.empty())
    {
        std::cout << "SKIP: no virtual M30XY (type 101) configured in the simulator.\n";
        return 0;
    }

    const std::string serial = list.front();
    std::cout << "using M30XY serial: " << serial << "\n";

    M30XY dev(serial);
    assert(dev.getChannelCount() == 2);
    assert(!dev.isConnected());

    // Connect (idempotent).
    assert(dev.doConnect() == OperationResult::OPERATION_OK);
    assert(dev.isConnected());
    assert(dev.doConnect() == OperationResult::ALREADY_CONNECTED);

    // Enable and home both axes.
    assert(dev.doEnableChannels(true) == OperationResult::OPERATION_OK);
    assert(dev.doHomeAll() == OperationResult::OPERATION_OK);
    assert(dev.waitForHomed(Channel::X_CHANNEL, seconds(60)) == OperationResult::OPERATION_OK);
    assert(dev.waitForHomed(Channel::Y_CHANNEL, seconds(60)) == OperationResult::OPERATION_OK);

    // Aggregate status: both axes valid and homed.
    types::M30XYDeviceStatus status;
    assert(dev.getDeviceStatus(status) == OperationResult::OPERATION_OK);
    assert(status.connected);
    assert(status.chann_x.valid && status.chann_y.valid);
    assert(status.chann_x.flags.homed && status.chann_y.flags.homed);
    std::cout << "status: " << status.toJsonStr() << "\n";

    // Move X to 5 mm and verify it reaches the target.
    assert(dev.doMoveAbsolute(Channel::X_CHANNEL, 5.0) == OperationResult::OPERATION_OK);
    const OperationResult reached = waitForCondition([&]()
    {
        double mm = 0.0;
        return dev.getChannelPosition(Channel::X_CHANNEL, mm) == OperationResult::OPERATION_OK
               && std::abs(mm - 5.0) < 0.2;
    }, seconds(30), milliseconds(100));

    double x_mm = -1.0;
    dev.getChannelPosition(Channel::X_CHANNEL, x_mm);
    std::cout << "X position after move: " << x_mm << " mm (reached=" << types::toString(reached) << ")\n";
    assert(reached == OperationResult::OPERATION_OK);
    assert(dev.waitForMoveFinished(Channel::X_CHANNEL, seconds(10)) == OperationResult::OPERATION_OK);

    assert(dev.doStopAll(StopMode::PROFILED) == OperationResult::OPERATION_OK);

    // Status callback fires while polling, carrying a valid status.
    std::atomic<int> cb_count{0};
    std::atomic<int> cb_ok{0};
    dev.setNewStatusCb([&](OperationResult r, const types::M30XYDeviceStatus& s)
    {
        ++cb_count;
        if (r == OperationResult::OPERATION_OK && s.chann_x.valid)
            ++cb_ok;
    });
    assert(dev.startStatusPolling() == OperationResult::OPERATION_OK);
    assert(dev.isStatusPollingRunning());
    assert(waitForCondition([&]{ return cb_ok.load() >= 1; }, seconds(5), milliseconds(50))
           == OperationResult::OPERATION_OK);
    assert(dev.stopStatusPolling() == OperationResult::OPERATION_OK);
    assert(!dev.isStatusPollingRunning());
    std::cout << "callback invocations: " << cb_count.load() << " (ok=" << cb_ok.load() << ")\n";

    // Disconnect.
    assert(dev.doDisconnect() == OperationResult::OPERATION_OK);
    assert(!dev.isConnected());

    std::cout << "AppM30XYSimTesting: ALL CHECKS PASSED" << std::endl;
    return 0;
}
