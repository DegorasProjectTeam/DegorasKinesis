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
#include <chrono>
#include <iostream>
#include <string>

// PROJECT INCLUDES
#include "LibThorlabsKinesis/DCServo/kinesis_simulator.h"
#include "LibThorlabsKinesis/Devices/m30xy.h"


// Idiomatic example of driving an M30XY through the public library API. Works against the Kinesis Simulator (a
// virtual M30XY) or real hardware. This is documentation, not a test: it prints progress and returns non-zero on
// the first failure. The M30X single-axis device is used the same way, with one channel.

using namespace thorlabs;
using thorlabs::types::Channel;
using thorlabs::types::OperationResult;

namespace
{
// Print the step, and on failure print the result and signal the caller to abort.
bool ok(const std::string& step, OperationResult r)
{
    std::cout << "  " << step << ": " << types::toString(r) << "\n";
    return r == OperationResult::OPERATION_OK;
}
}

int main()
{
    using namespace std::chrono;

    // The simulator session is optional: against real hardware it simply reports SIMULATOR_NOT_RUNNING and the
    // RAII guard does nothing. Its destructor releases the simulator connection on every exit path.
    KinesisSimulatorSession sim;
    std::cout << "Kinesis simulator: " << toString(sim.result()) << "\n";

    // Discover.
    types::ThorlabsSNList serials;
    if (!ok("getDeviceList", M30XY::getDeviceList(serials)) || serials.empty())
    {
        std::cout << "No M30XY device found.\n";
        return 1;
    }
    std::cout << "Found M30XY: " << serials.front() << "\n";

    // Connect (RAII: the destructor disconnects if we forget).
    M30XY dev(serials.front());
    if (!ok("doConnect", dev.doConnect()))
        return 1;

    // Subscribe to asynchronous status updates (delivered on a background thread).
    dev.setNewStatusCb([](OperationResult r, const types::M30XYDeviceStatus& status)
    {
        if (r == OperationResult::OPERATION_OK)
            std::cout << "    [status] " << status.toJsonStr() << "\n";
    });
    dev.startStatusPolling();

    // Enable, home, and move both axes.
    bool good = ok("doEnableChannels", dev.doEnableChannels(true))
             && ok("doHomeAll", dev.doHomeAll())
             && ok("waitForHomed(X)", dev.waitForHomed(Channel::X_CHANNEL, seconds(60)))
             && ok("waitForHomed(Y)", dev.waitForHomed(Channel::Y_CHANNEL, seconds(60)))
             && ok("doMoveAbsolute(X, 5mm)", dev.doMoveAbsolute(Channel::X_CHANNEL, 5.0))
             && ok("waitForMoveFinished(X)", dev.waitForMoveFinished(Channel::X_CHANNEL, seconds(30)))
             && ok("doMoveAbsolute(Y, 3mm)", dev.doMoveAbsolute(Channel::Y_CHANNEL, 3.0))
             && ok("waitForMoveFinished(Y)", dev.waitForMoveFinished(Channel::Y_CHANNEL, seconds(30)));

    if (good)
    {
        double x_mm = 0.0;
        double y_mm = 0.0;
        dev.getChannelPosition(Channel::X_CHANNEL, x_mm);
        dev.getChannelPosition(Channel::Y_CHANNEL, y_mm);
        std::cout << "Final position: X=" << x_mm << " mm, Y=" << y_mm << " mm\n";
    }

    dev.stopStatusPolling();
    ok("doDisconnect", dev.doDisconnect());

    std::cout << (good ? "Example completed." : "Example finished with errors.") << "\n";
    return good ? 0 : 1;
}
