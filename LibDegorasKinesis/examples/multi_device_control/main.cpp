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
#include <mutex>
#include <string>
#include <thread>
#include <vector>

// PROJECT INCLUDES (module aggregators)
#include <LibDegorasKinesis/Modules/DCServo>
#include <LibDegorasKinesis/Modules/Devices>


// Example: drive several devices CONCURRENTLY, one worker thread per device. The library's per-serial locking keeps
// operations on distinct devices independent, so no cross-device coordination is needed. Works against the Kinesis
// Simulator or real hardware; the M30X path degrades gracefully if its (simulated) device has no stage profile.

using namespace dpkin;
using namespace dpkin::kinesis;
using dpkin::types::Channel;
using dpkin::types::OperationResult;

namespace
{
std::mutex g_io;   // serialises console output from the worker threads

void say(const std::string& text)
{
    const std::lock_guard<std::mutex> lock(g_io);
    std::cout << text << "\n";
}

void driveM30XY(const std::string& serial)
{
    using namespace std::chrono;
    M30XY dev(serial);
    if (dev.doConnect() != OperationResult::OPERATION_OK)
    {
        say("[M30XY " + serial + "] connect failed");
        return;
    }
    dev.doEnableChannels(true);
    dev.doHomeAll();
    dev.waitForHomed(Channel::X_CHANNEL, seconds(60));

    for (double target : {2.0, 6.0, 1.0})
    {
        dev.doMoveAbsolute(Channel::X_CHANNEL, target);
        dev.waitForMoveFinished(Channel::X_CHANNEL, seconds(30));
        double real_pos = 0.0;
        dev.getChannelPosition(Channel::X_CHANNEL, real_pos);
        say("[M30XY " + serial + "] X commanded " + std::to_string(target) + " mm (read "
            + std::to_string(real_pos) + ")");
    }

    dev.doDisconnect();
    say("[M30XY " + serial + "] done");
}

void driveM30X(const std::string& serial)
{
    using namespace std::chrono;
    M30X dev(serial);
    const OperationResult conn = dev.doConnect();
    if (conn == OperationResult::LOAD_SETTINGS_ERROR)
    {
        say("[M30X " + serial + "] no stage/settings profile assigned; skipping motion (simulator limitation).");
        return;
    }
    if (conn != OperationResult::OPERATION_OK)
    {
        say("[M30X " + serial + "] connect: " + types::toString(conn));
        return;
    }
    dev.doEnable(Channel::X_CHANNEL, true);
    dev.doHome(Channel::X_CHANNEL);
    dev.waitForHomed(seconds(60));
    dev.doMoveAbsolute(Channel::X_CHANNEL, 3.0);
    dev.waitForMoveFinished(seconds(30));
    double real_pos = 0.0;
    dev.getChannelPosition(Channel::X_CHANNEL, real_pos);
    say("[M30X " + serial + "] X commanded 3 mm (read " + std::to_string(real_pos) + ")");
    dev.doDisconnect();
    say("[M30X " + serial + "] done");
}
}

int main()
{
    // A single, process-wide simulator session covers every device driven below.
    KinesisSimulatorSession sim;
    say(std::string("Kinesis simulator: ") + toString(sim.result()));

    types::ThorlabsSNList m30xy;
    types::ThorlabsSNList m30x;
    M30XY::getDeviceList(m30xy);
    M30X::getDeviceList(m30x);

    if (m30xy.empty() && m30x.empty())
    {
        say("No devices found. Start the Kinesis Simulator (or connect hardware) and retry.");
        return 1;
    }

    // One worker thread per discovered device; they run concurrently.
    std::vector<std::thread> workers;
    if (!m30xy.empty()) workers.emplace_back(driveM30XY, m30xy.front());
    if (!m30x.empty())  workers.emplace_back(driveM30X, m30x.front());
    for (std::thread& worker : workers)
        worker.join();

    say("All device threads finished.");
    return 0;
}
