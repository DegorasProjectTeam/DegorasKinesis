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


// Example: discover, connect, initialise, home, move, read position/status, then stop and disconnect an M30X
// single-axis benchtop DC-servo stage. Works against the Kinesis Simulator (virtual M30X, type 105) or real hardware.
//
// Units: with a stage/settings profile loaded the stage is driven in MILLIMETRES; without one (the simulator ships
// the M30X with no profile) hasRealUnits() is false and the demo drives it in raw device units (motor counts).

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

// Usage: Example_M30X_control [serial]   (serial defaults to the first discovered M30X).
int main(int argc, char** argv)
{
    using namespace std::chrono;

    KinesisSimulatorSession sim;
    std::cout << "Kinesis simulator: " << toString(sim.result()) << "\n";

    // Resolve the target serial: the command-line one (validated against the M30X type id), else the first discovered.
    std::string serial;
    if (argc > 1)
    {
        serial = argv[1];
        if (!M30X::isCompatibleSerial(serial))
        {
            std::cout << "Serial '" << serial << "' is not an M30X (Thorlabs type 105).\n";
            return 1;
        }
    }
    else
    {
        types::ThorlabsSNList serials;
        if (!step("getDeviceList", M30X::getDeviceList(serials)) || serials.empty())
        {
            std::cout << "No M30X found. Pass a serial to target a specific unit, e.g. Example_M30X_control 105000002\n";
            return 1;
        }
        serial = serials.front();
    }
    std::cout << "Connecting to " << serial << "\n";

    M30X dev(serial);                     // no device I/O in the constructor
    if (!step("doConnect", dev.doConnect()))
        return 1;

    // Enable, then establish a known idle state before homing. The Kinesis Simulator keeps each device's motion
    // state across connections (and a profiled stop is still decelerating when the previous run closed), so a fresh
    // connection can find the device "in motion", which makes Home fail; a stop + short settle avoids that.
    bool ok = step("doEnable(X)", dev.doEnable(Channel::X_CHANNEL, true));
    dev.doStop(Channel::X_CHANNEL, StopMode::IMMEDIATE);
    dev.waitForMoveFinished(seconds(5));
    ok = ok && step("doHome(X)", dev.doHome(Channel::X_CHANNEL))
            && step("waitForHomed", dev.waitForHomed(seconds(60)));

    // Move + read, in whichever unit is available.
    if (dev.hasRealUnits())
    {
        ok = ok && step("doMoveAbsolute(X, 5 mm)", dev.doMoveAbsolute(Channel::X_CHANNEL, 5.0))
                && step("waitForMoveFinished", dev.waitForMoveFinished(seconds(30)));
        double pos_mm = 0.0;
        dev.getChannelPosition(Channel::X_CHANNEL, pos_mm);
        std::cout << "X position: " << pos_mm << " mm\n";
    }
    else
    {
        std::cout << "WARNING: no stage/settings profile is loaded, so real-world millimetres are unavailable.\n"
                     "         Operating in device units (motor counts) - assign a stage in the simulator, or use\n"
                     "         real hardware, for mm-based motion.\n";
        ok = ok && step("doMoveRelativeDeviceUnits(X, +100000)",
                        dev.doMoveRelativeDeviceUnits(Channel::X_CHANNEL, 100000))
                && step("waitForMoveFinished", dev.waitForMoveFinished(seconds(30)));
        int counts = 0;
        dev.getChannelPositionDeviceUnits(Channel::X_CHANNEL, counts);
        std::cout << "X position: " << counts << " device units (counts)\n";
    }

    types::M30XDeviceStatus status;
    if (dev.getDeviceStatus(status) == OperationResult::OPERATION_OK)
        std::cout << "status: " << status.toJsonStr() << "\n";

    step("doStop(X)", dev.doStop(Channel::X_CHANNEL, StopMode::PROFILED));
    step("doDisconnect", dev.doDisconnect());
    std::cout << (ok ? "Done." : "Finished with errors.") << "\n";
    return ok ? 0 : 1;
}
