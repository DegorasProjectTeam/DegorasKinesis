/*
 *  DegorasKinesis - An extensible C++ library for controlling Thorlabs Kinesis motion devices.
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
#include <cstdlib>
#include <iostream>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>

// PROJECT INCLUDES (module aggregators)
#include <DegorasKinesis/Modules/DCServo>
#include <DegorasKinesis/Modules/Devices>


using namespace dpkin;
using namespace dpkin::kinesis;
using dpkin::types::Channel;
using dpkin::types::OperationResult;
using dpkin::types::StopMode;
using dpkin::types::TravelDirection;

// ---------------------------------------------------------------------------------------------------------------------
// Continuous monitoring tool for the M30XY. It connects to the simulator's virtual M30XY, prints the live decoded
// status on every poll, and runs a scripted command sequence (home, jog FWD/REV, move-relative, move-to) so the
// commanded motion is visible in the Kinesis Simulator window. After the scripted part it keeps monitoring for the
// rest of the requested duration, so the stage can also be driven MANUALLY in the simulator and watched here.
//
// Usage: Test_M30XYMonitor [seconds] [serial]   (defaults: 30 s; first discovered M30XY). Observational tool.
// ---------------------------------------------------------------------------------------------------------------------

namespace
{

std::mutex g_print;

void line(const std::string& text)
{
    const std::lock_guard<std::mutex> lock(g_print);
    std::cout << text << std::endl;
}

std::string axisDesc(const types::M30XYChannelStatus& s)
{
    std::ostringstream ss;
    const auto& f = s.flags;
    ss << s.real_pos << "mm ";
    if (f.isMoving())       ss << "MOVING";
    else if (f.isJogging()) ss << "JOGGING";
    else if (f.homing)      ss << "homing";
    else if (f.homed)       ss << "homed";
    else                    ss << "idle";
    if (f.limit_cw || f.limit_ccw) ss << " LIMIT";
    return ss.str();
}

} // namespace

int main(int argc, char** argv)
{
    const int duration_s = (argc > 1 && std::atoi(argv[1]) > 0) ? std::atoi(argv[1]) : 30;

    KinesisSimulatorSession sim;
    line(std::string("simulator session: ") + toString(sim.result()));

    // Target the serial given as the 2nd argument (validated against the M30XY type id), else the first discovered.
    std::string serial;
    if (argc > 2)
    {
        serial = argv[2];
        if (!M30XY::isCompatibleSerial(serial))
        {
            line("Serial '" + serial + "' is not an M30XY (type 101).");
            return 0;
        }
    }
    else
    {
        types::ThorlabsSNList list;
        if (M30XY::getDeviceList(list) != OperationResult::OPERATION_OK || list.empty())
        {
            line("No M30XY found. Open the Kinesis Simulator with a virtual M30XY (type 101), or pass a serial as "
                 "the 2nd argument.");
            return 0;
        }
        serial = list.front();
    }

    M30XY dev(serial);
    line("Connecting to M30XY " + serial + " ...");
    if (dev.doConnect() != OperationResult::OPERATION_OK)
    {
        line("Connect failed.");
        return 0;
    }
    dev.doEnableChannels(true);

    // Live status print on every poll (runs on the worker thread; output is serialised by g_print).
    dev.setNewStatusCb([](OperationResult r, const types::M30XYDeviceStatus& s)
    {
        if (r != OperationResult::OPERATION_OK)
            return;
        line("[status]  X: " + axisDesc(s.chann_x) + "   |   Y: " + axisDesc(s.chann_y));
    });
    dev.startStatusPolling();

    line("================================================================================");
    line("Monitoring for " + std::to_string(duration_s) + " s. Move the stage MANUALLY in the");
    line("simulator at any time to see the position evolve below. A scripted command");
    line("sequence runs first so you can watch the commanded motions in the simulator.");
    line("================================================================================");

    const auto issue = [&](const std::string& name, OperationResult r)
    {
        line(">>> " + name + "  ->  " + types::toString(r));
    };

    // Scripted demonstration sequence (each command is followed by an observation window).
    issue("home all", dev.doHomeAll());
    std::this_thread::sleep_for(std::chrono::seconds(3));

    issue("jog X FORWARD", dev.doJog(Channel::X_CHANNEL, TravelDirection::FORWARDS));
    std::this_thread::sleep_for(std::chrono::seconds(2));
    issue("stop X", dev.doStop(Channel::X_CHANNEL, StopMode::PROFILED));
    std::this_thread::sleep_for(std::chrono::seconds(1));

    issue("jog X REVERSE", dev.doJog(Channel::X_CHANNEL, TravelDirection::REVERSE));
    std::this_thread::sleep_for(std::chrono::seconds(2));
    issue("stop X", dev.doStop(Channel::X_CHANNEL, StopMode::PROFILED));
    std::this_thread::sleep_for(std::chrono::seconds(1));

    issue("move relative X +2 mm", dev.doMoveRelative(Channel::X_CHANNEL, 2.0));
    std::this_thread::sleep_for(std::chrono::seconds(2));

    issue("move absolute X 5 mm", dev.doMoveAbsolute(Channel::X_CHANNEL, 5.0));
    std::this_thread::sleep_for(std::chrono::seconds(2));

    line("--- Scripted sequence done. Free monitoring continues; drive the stage manually now. ---");

    const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(duration_s);
    while (std::chrono::steady_clock::now() < deadline)
        std::this_thread::sleep_for(std::chrono::milliseconds(200));

    dev.stopStatusPolling();
    dev.doDisconnect();
    line("Test_M30XYMonitor: finished.");
    return 0;
}
