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
#include <chrono>
#include <iostream>
#include <string>

// PROJECT INCLUDES (module aggregators)
#include <LibDegorasKinesis/Modules/IntStepper>
#include <LibDegorasKinesis/Modules/Devices>


// Example: connect to a single K10CR2/M rotation stage, enable and home it, rotate the axis, read the angular
// position and decoded status, then stop and disconnect. Works against the Kinesis Simulator (virtual K10CR2 with
// serial 55000002) or real hardware. All positions are in DEGREES.

using namespace dpkin;
using namespace dpkin::kinesis;
using dpkin::types::Channel;
using dpkin::types::OperationResult;
using dpkin::types::StopMode;

namespace
{
bool step(const std::string& name, OperationResult result)
{
    std::cout << "  " << name << ": " << types::toString(result) << "\n";
    return result == OperationResult::OPERATION_OK;
}
}

int main()
{
    using namespace std::chrono;

    // Connect to the Kinesis Simulator (a no-op against real hardware).
    KinesisSimulatorSession sim;
    std::cout << "Kinesis simulator: " << toString(sim.result()) << "\n";

    // 1) Discover the K10CR2 controller(s).
    types::ThorlabsSNList serials;
    if (!step("getDeviceList", K10CR2::getDeviceList(serials)) || serials.empty())
    {
        std::cout << "No K10CR2 device found (expected simulator serial 55000002).\n";
        return 1;
    }
    std::cout << "Connecting to " << serials.front() << "\n";

    // 2) Open + initialise (load settings, start polling, arm the freshness timer). No device I/O in the constructor.
    K10CR2 dev(serials.front());
    const OperationResult conn = dev.doConnect();
    if (!step("doConnect", conn))
    {
        if (conn == OperationResult::LOAD_SETTINGS_ERROR)
        {
            // Known simulator limitation, not a library error: discovery + open worked, only settings are absent.
            std::cout << "NOTE: the simulated K10CR2 has no stage/settings profile assigned, so unit-dependent\n"
                         "      operation (home/move/position in degrees) cannot be exercised. Discovery and open\n"
                         "      succeeded; assign a stage in the simulator (or use real hardware) for full motion.\n";
            return 0;
        }
        return 1;
    }

    // 3) Enable, home (establishes the zero datum), and rotate to a safe absolute angle.
    const bool ok = step("doEnable(X)", dev.doEnable(Channel::X_CHANNEL, true))
                 && step("doHome(X)", dev.doHome(Channel::X_CHANNEL))
                 && step("waitForHomed(X)", dev.waitForHomed(seconds(60)))
                 && step("doMoveAbsolute(X, 10 deg)", dev.doMoveAbsolute(Channel::X_CHANNEL, 10.0))
                 && step("waitForMoveFinished(X)", dev.waitForMoveFinished(seconds(30)));

    // 4) Read back the angular position and the decoded status.
    double angle_deg = 0.0;
    dev.getChannelPosition(Channel::X_CHANNEL, angle_deg);
    std::cout << "X angular position: " << angle_deg << " deg\n";

    types::K10CR2DeviceStatus status;
    if (dev.getDeviceStatus(status) == OperationResult::OPERATION_OK)
        std::cout << "status: " << status.toJsonStr() << "\n";

    // 5) Stop and disconnect cleanly (the destructor would also disconnect).
    step("doStop(X)", dev.doStop(Channel::X_CHANNEL, StopMode::PROFILED));
    step("doDisconnect", dev.doDisconnect());

    std::cout << (ok ? "Done." : "Finished with errors.") << "\n";
    return ok ? 0 : 1;
}
