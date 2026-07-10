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

// PROJECT INCLUDES
#include "LibDegorasKinesis/Kinesis/kinesis_simulator.h"
#include "LibDegorasKinesis/Devices/m30xy.h"


// ---------------------------------------------------------------------------------------------------------------------
// HARDWARE / SIMULATOR integration test for the M30XY.
//
// SAFETY: this drives a physical stage. It is GATED behind the --i-have-hardware flag and performs only small,
// conservative RELATIVE moves, always homing first, always stopping at the end, and bounding every wait. Before
// running on real optics, verify the stage is clear to move over the commanded range. DO NOT run unattended.
// Without the flag it explains itself and exits 0, so it is safe to include in an automated build.
// ---------------------------------------------------------------------------------------------------------------------

using namespace dpkin;
using namespace dpkin::kinesis;
using dpkin::types::Channel;
using dpkin::types::OperationResult;
using dpkin::types::StopMode;

namespace
{
constexpr double kSmallRelativeMm = 1.0;   ///< Conservative relative move for a hardware smoke test.

bool require(const std::string& step, OperationResult r)
{
    std::cout << "  " << step << ": " << types::toString(r) << "\n";
    return r == OperationResult::OPERATION_OK;
}
}

int main(int argc, char** argv)
{
    using namespace std::chrono;

    const bool authorised = (argc > 1 && std::string(argv[1]) == "--i-have-hardware");
    if (!authorised)
    {
        std::cout << "Test_M30XYHardware: refusing to move hardware without explicit authorisation.\n"
                     "Re-run with --i-have-hardware ONLY when a stage is connected and clear to move.\n";
        return 0;
    }

    KinesisSimulatorSession sim;
    std::cout << "Kinesis simulator: " << toString(sim.result()) << " (real hardware reports SIMULATOR_NOT_RUNNING)\n";

    types::ThorlabsSNList serials;
    if (require("getDeviceList", M30XY::getDeviceList(serials)) == false || serials.empty())
    {
        std::cout << "No M30XY device found.\n";
        return 1;
    }

    M30XY dev(serials.front());
    if (!require("doConnect", dev.doConnect()))
        return 1;

    // Always stop both axes on the way out, even on an early failure.
    struct StopGuard
    {
        M30XY& d;
        ~StopGuard() { d.doStopAll(StopMode::PROFILED); d.doDisconnect(); }
    } guard{dev};

    bool good = require("doEnableChannels", dev.doEnableChannels(true))
             && require("doHomeAll", dev.doHomeAll())
             && require("waitForHomed(X)", dev.waitForHomed(Channel::X_CHANNEL, seconds(120)))
             && require("waitForHomed(Y)", dev.waitForHomed(Channel::Y_CHANNEL, seconds(120)));

    if (good)
    {
        // Conservative relative moves only.
        good = require("doMoveRelative(X, +1mm)", dev.doMoveRelative(Channel::X_CHANNEL, kSmallRelativeMm))
            && require("waitForMoveFinished(X)", dev.waitForMoveFinished(Channel::X_CHANNEL, seconds(30)))
            && require("doMoveRelative(X, -1mm)", dev.doMoveRelative(Channel::X_CHANNEL, -kSmallRelativeMm))
            && require("waitForMoveFinished(X)", dev.waitForMoveFinished(Channel::X_CHANNEL, seconds(30)));
    }

    std::cout << (good ? "Test_M30XYHardware: PASSED" : "Test_M30XYHardware: FAILED") << "\n";
    return good ? 0 : 1;
}
