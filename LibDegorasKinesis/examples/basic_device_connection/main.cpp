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


// Example: connect to a single M30XY, enable and home it, move an axis, read the position, and disconnect.
// Works against the Kinesis Simulator or real hardware.

using namespace dpkin;
using namespace dpkin::kinesis;
using dpkin::types::Channel;
using dpkin::types::OperationResult;

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

    KinesisSimulatorSession sim;
    std::cout << "Kinesis simulator: " << toString(sim.result()) << "\n";

    types::ThorlabsSNList serials;
    if (!step("getDeviceList", M30XY::getDeviceList(serials)) || serials.empty())
    {
        std::cout << "No M30XY device found.\n";
        return 1;
    }
    std::cout << "Connecting to " << serials.front() << "\n";

    M30XY dev(serials.front());   // no device I/O in the constructor
    if (!step("doConnect", dev.doConnect()))
        return 1;

    const bool ok = step("doEnableChannels", dev.doEnableChannels(true))
                 && step("doHomeAll", dev.doHomeAll())
                 && step("waitForHomed(X)", dev.waitForHomed(Channel::X_CHANNEL, seconds(60)))
                 && step("doMoveAbsolute(X, 5 mm)", dev.doMoveAbsolute(Channel::X_CHANNEL, 5.0))
                 && step("waitForMoveFinished(X)", dev.waitForMoveFinished(Channel::X_CHANNEL, seconds(30)));

    double real_pos = 0.0;
    dev.getChannelPosition(Channel::X_CHANNEL, real_pos);
    std::cout << "X position: " << real_pos << " mm\n";

    step("doDisconnect", dev.doDisconnect());   // the destructor would also do this
    std::cout << (ok ? "Done." : "Finished with errors.") << "\n";
    return ok ? 0 : 1;
}
