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
#include <LibDegorasKinesis/Modules/DCServo>
#include <LibDegorasKinesis/Modules/Devices>


// Example: connect to a single M30XY dual-axis (X/Y) benchtop DC-servo stage, enable and home both axes, move each
// axis, read the positions and decoded status, then stop and disconnect. Works against the Kinesis Simulator
// (virtual M30XY, type 101) or real hardware. All positions are in MILLIMETRES.

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

// Usage: Example_M30XY_control [serial]   (serial defaults to the first discovered M30XY, e.g. sim 101000002).
int main(int argc, char** argv)
{
    using namespace std::chrono;

    // Connect to the Kinesis Simulator (a no-op against real hardware).
    KinesisSimulatorSession sim;
    std::cout << "Kinesis simulator: " << toString(sim.result()) << "\n";

    // 1) Resolve the target serial: the command-line one (validated against the M30XY type id), else the first
    //    discovered M30XY.
    std::string serial;
    if (argc > 1)
    {
        serial = argv[1];
        if (!M30XY::isCompatibleSerial(serial))
        {
            std::cout << "Serial '" << serial << "' is not an M30XY (Thorlabs type 101).\n";
            return 1;
        }
    }
    else
    {
        types::ThorlabsSNList serials;
        if (!step("getDeviceList", M30XY::getDeviceList(serials)) || serials.empty())
        {
            std::cout << "No M30XY found. Pass a serial to target a specific unit, e.g. Example_M30XY_control 101000002\n";
            return 1;
        }
        serial = serials.front();
    }
    std::cout << "Connecting to " << serial << "\n";

    // 2) Open + initialise both axes. No device I/O in the constructor.
    M30XY dev(serial);
    const OperationResult conn = dev.doConnect();
    if (!step("doConnect", conn))
    {
        if (conn == OperationResult::LOAD_SETTINGS_ERROR)
        {
            std::cout << "NOTE: the simulated M30XY has no stage/settings profile assigned, so unit-dependent\n"
                         "      operation (home/move/position in mm) cannot be exercised. Discovery and open\n"
                         "      succeeded; assign a stage in the simulator (or use real hardware) for full motion.\n";
            return 0;
        }
        return 1;
    }

    // 3) Enable both axes, then establish a known idle state before homing. The Kinesis Simulator keeps each
    //    device's motion state across connections (and a profiled stop is still decelerating when the previous run
    //    closed), so a fresh connection can find an axis "in motion", which makes Home fail; a stop + short settle
    //    avoids that. Home both, then move each to a safe absolute position.
    bool ok = step("doEnableChannels", dev.doEnableChannels(true));
    dev.doStopAll(StopMode::IMMEDIATE);
    dev.waitForMoveFinished(Channel::X_CHANNEL, seconds(5));
    dev.waitForMoveFinished(Channel::Y_CHANNEL, seconds(5));
    ok = ok && step("doHomeAll", dev.doHomeAll())
            && step("waitForHomed(X)", dev.waitForHomed(Channel::X_CHANNEL, seconds(60)))
                 && step("waitForHomed(Y)", dev.waitForHomed(Channel::Y_CHANNEL, seconds(60)))
                 && step("doMoveAbsolute(X, 5 mm)", dev.doMoveAbsolute(Channel::X_CHANNEL, 5.0))
                 && step("waitForMoveFinished(X)", dev.waitForMoveFinished(Channel::X_CHANNEL, seconds(30)))
                 && step("doMoveAbsolute(Y, 3 mm)", dev.doMoveAbsolute(Channel::Y_CHANNEL, 3.0))
                 && step("waitForMoveFinished(Y)", dev.waitForMoveFinished(Channel::Y_CHANNEL, seconds(30)));

    // 4) Read back both positions and the aggregate decoded status.
    double x_mm = 0.0, y_mm = 0.0;
    dev.getChannelPosition(Channel::X_CHANNEL, x_mm);
    dev.getChannelPosition(Channel::Y_CHANNEL, y_mm);
    std::cout << "position: X=" << x_mm << " mm  Y=" << y_mm << " mm\n";

    types::M30XYDeviceStatus status;
    if (dev.getDeviceStatus(status) == OperationResult::OPERATION_OK)
        std::cout << "status: " << status.toJsonStr() << "\n";

    // 5) Stop both axes and disconnect cleanly (the destructor would also disconnect).
    step("doStopAll", dev.doStopAll(StopMode::PROFILED));
    step("doDisconnect", dev.doDisconnect());

    std::cout << (ok ? "Done." : "Finished with errors.") << "\n";
    return ok ? 0 : 1;
}
