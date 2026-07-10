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
#include <cstdlib>
#include <iostream>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>

// PROJECT INCLUDES (module aggregators)
#include <LibDegorasKinesis/Modules/IntStepper>
#include <LibDegorasKinesis/Modules/Devices>


using namespace dpkin;
using namespace dpkin::kinesis;
using dpkin::types::Channel;
using dpkin::types::OperationResult;
using dpkin::types::StopMode;
using dpkin::types::TravelDirection;

// ---------------------------------------------------------------------------------------------------------------------
// Continuous monitoring tool for the K10CR2/M rotation stage. It connects to the simulator's virtual K10CR2, prints
// the live decoded status on every poll, and runs a scripted command sequence (home, jog FWD/REV, move-relative,
// move-to) so the commanded motion is visible in the Kinesis Simulator window. After the scripted part it keeps
// monitoring for the rest of the requested duration, so the stage can also be driven MANUALLY and watched here.
//
// If a stage/settings profile is loaded the angle is shown in DEGREES and the scripted moves are in degrees; if not
// (the simulator default), the tool warns, shows raw motor COUNTS, and issues the scripted moves in device units.
//
// Usage: Test_K10CR2Monitor [seconds]   (default 30). This is an observational tool, not an assert-based test.
// ---------------------------------------------------------------------------------------------------------------------

namespace
{

std::mutex g_print;

void line(const std::string& text)
{
    const std::lock_guard<std::mutex> lock(g_print);
    std::cout << text << std::endl;
}

std::string axisDesc(const types::K10CR2ChannelStatus& s, bool real_units)
{
    std::ostringstream ss;
    const auto& f = s.flags;
    if (real_units) ss << s.real_pos << "deg ";
    else            ss << s.pos_raw << "cnt ";
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

    types::ThorlabsSNList list;
    if (K10CR2::getDeviceList(list) != OperationResult::OPERATION_OK || list.empty())
    {
        line("No K10CR2 found. Open the Kinesis Simulator with a virtual K10CR2 (type 55) and retry.");
        return 0;
    }

    K10CR2 dev(list.front());
    line("Connecting to K10CR2 " + list.front() + " ...");
    const OperationResult conn = dev.doConnect();
    if (conn != OperationResult::OPERATION_OK)
    {
        line(std::string("Connect failed: ") + types::toString(conn));
        return 0;
    }
    dev.doEnable(Channel::X_CHANNEL, true);

    const bool real_units = dev.hasRealUnits();
    if (!real_units)
        line("WARNING: no stage/settings profile loaded - showing raw device units (counts) and commanding motion in\n"
             "         device units. Assign a stage in the simulator (or use real hardware) for degree-based motion.");

    // Live status print on every poll (runs on the worker thread; output is serialised by g_print).
    dev.setNewStatusCb([real_units](OperationResult r, const types::K10CR2DeviceStatus& s)
    {
        if (r != OperationResult::OPERATION_OK)
            return;
        line("[status]  axis: " + axisDesc(s.chann, real_units));
    });
    dev.startStatusPolling();

    line("================================================================================");
    line("Monitoring for " + std::to_string(duration_s) + " s. Rotate the stage MANUALLY in the");
    line("simulator at any time to see the position evolve below. A scripted command");
    line("sequence runs first so you can watch the commanded motions in the simulator.");
    line("================================================================================");

    const auto issue = [&](const std::string& name, OperationResult r)
    {
        line(">>> " + name + "  ->  " + types::toString(r));
    };

    // Scripted demonstration sequence. Homing, jogging and stopping are unit-independent; the relative/absolute
    // moves use real-world degrees when a profile is loaded, otherwise raw device units.
    issue("home", dev.doHome(Channel::X_CHANNEL));
    std::this_thread::sleep_for(std::chrono::seconds(3));

    issue("jog FORWARD", dev.doJog(Channel::X_CHANNEL, TravelDirection::FORWARDS));
    std::this_thread::sleep_for(std::chrono::seconds(2));
    issue("stop", dev.doStop(Channel::X_CHANNEL, StopMode::PROFILED));
    std::this_thread::sleep_for(std::chrono::seconds(1));

    issue("jog REVERSE", dev.doJog(Channel::X_CHANNEL, TravelDirection::REVERSE));
    std::this_thread::sleep_for(std::chrono::seconds(2));
    issue("stop", dev.doStop(Channel::X_CHANNEL, StopMode::PROFILED));
    std::this_thread::sleep_for(std::chrono::seconds(1));

    if (real_units)
    {
        issue("move relative +5 deg", dev.doMoveRelative(Channel::X_CHANNEL, 5.0));
        std::this_thread::sleep_for(std::chrono::seconds(2));
        issue("move absolute 10 deg", dev.doMoveAbsolute(Channel::X_CHANNEL, 10.0));
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
    else
    {
        issue("move relative +200000 counts", dev.doMoveRelativeDeviceUnits(Channel::X_CHANNEL, 200000));
        std::this_thread::sleep_for(std::chrono::seconds(2));
        issue("move absolute 400000 counts", dev.doMoveAbsoluteDeviceUnits(Channel::X_CHANNEL, 400000));
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }

    line("--- Scripted sequence done. Free monitoring continues; drive the stage manually now. ---");

    const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(duration_s);
    while (std::chrono::steady_clock::now() < deadline)
        std::this_thread::sleep_for(std::chrono::milliseconds(200));

    dev.stopStatusPolling();
    dev.doDisconnect();
    line("Test_K10CR2Monitor: finished.");
    return 0;
}
