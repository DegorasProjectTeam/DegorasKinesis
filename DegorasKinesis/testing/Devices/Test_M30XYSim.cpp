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

// ASSERTIONS ARE THE POINT OF A TEST, so NDEBUG must not be allowed to delete them. A Release build defines it and
// assert() then expands to NOTHING AT ALL -- the whole expression, not just the abort, so any call written inside an
// assert stops happening too. Measured on this suite before the fix: objdump found ZERO references to assert in all
// nine Release test objects, i.e. every one of the 195 checks was dead, and the suite reported success without
// evaluating any of them. After the fix the same objects carry 197 references and the suite still passes 9/9 in both
// configurations -- so nothing had been failing behind the dead layer here; it simply was not being checked.
//
// BOTH LINES ARE REQUIRED, in this order, above the first #include. The bare #undef is not enough: if anything has
// already pulled in <cassert> while NDEBUG was defined, assert is already expanded away and stays dead. Including
// the header again re-arms it, because assert.h undefines and redefines the macro on every inclusion and <cassert>
// deliberately has no include guard.
#undef NDEBUG
#include <cassert>

// C++ INCLUDES
#include <atomic>
#include <cassert>
#include <chrono>
#include <iostream>

// PROJECT INCLUDES
#include "DegorasKinesis/Kinesis/kinesis_simulator.h"
#include "DegorasKinesis/Devices/m30xy.h"
#include "DegorasKinesis/Helpers/wait_for.h"


using namespace dpkin;
using namespace dpkin::kinesis;
using dpkin::types::Channel;
using dpkin::types::OperationResult;
using dpkin::types::StopMode;

// ---------------------------------------------------------------------------------------------------------------------
// LIVE simulator self-check for the M30XY device (milestone M5). Requires the Thorlabs Kinesis Simulator running with
// a virtual M30XY (type 101). Self-skips (exit 0) when absent.
//
// SCOPE: validates the full SDK-integration CONTRACT through the real vendor DLL (discovery, connect, idempotent
// reconnect, enable, home command, status read + decode of both channels, async status callback, disconnect). It does
// NOT assert physical position, because the simulator's virtual stages cannot be assigned a travel range; the commanded
// position is logged instead. On real hardware the logged position should reach the commanded target.
// ---------------------------------------------------------------------------------------------------------------------

// Usage: Test_M30XYSim [serial]   (serial defaults to the first discovered M30XY, e.g. sim 101000002).
int main(int argc, char** argv)
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

    // Target the serial given on the command line (validated against the M30XY type id), else the first discovered.
    std::string serial;
    if (argc > 1)
    {
        serial = argv[1];
        if (!M30XY::isCompatibleSerial(serial))
        {
            std::cout << "SKIP: serial '" << serial << "' is not an M30XY (type 101).\n";
            return 0;
        }
    }
    else if (!list.empty())
    {
        serial = list.front();
    }
    else
    {
        std::cout << "SKIP: no virtual M30XY (type 101) configured in the simulator.\n";
        return 0;
    }
    std::cout << "using M30XY serial: " << serial << "\n";

    M30XY dev(serial);
    assert(dev.getChannelCount() == 2);
    assert(!dev.isConnected());

    // Lifecycle contract.
    assert(dev.doConnect() == OperationResult::OPERATION_OK);
    assert(dev.isConnected());
    assert(dev.doConnect() == OperationResult::ALREADY_CONNECTED);

    // Command contract: enable + home both axes succeed.
    assert(dev.doEnableChannels(true) == OperationResult::OPERATION_OK);
    assert(dev.doHomeAll() == OperationResult::OPERATION_OK);

    // Status read + decode contract: both channels report a valid, decoded status.
    types::M30XYDeviceStatus status;
    assert(dev.getDeviceStatus(status) == OperationResult::OPERATION_OK);
    assert(status.connected);
    assert(status.chann_x.valid && status.chann_y.valid);
    std::cout << "homed flags (sim-dependent): X=" << status.chann_x.flags.homed
              << " Y=" << status.chann_y.flags.homed << "\n";

    // Motion command contract (physical translation is sim-dependent; position is logged, not asserted).
    assert(dev.doMoveAbsolute(Channel::X_CHANNEL, 5.0) == OperationResult::OPERATION_OK);
    dev.waitForMoveFinished(Channel::X_CHANNEL, seconds(10));
    double x_mm = 0.0;
    dev.getChannelPosition(Channel::X_CHANNEL, x_mm);
    std::cout << "X position after move-to-5mm command: " << x_mm << " mm (target reached only with a real stage)\n";
    assert(dev.doStopAll(StopMode::PROFILED) == OperationResult::OPERATION_OK);

    // Async status callback contract: a callback arrives with a valid status while polling.
    std::atomic<int> cb_ok{0};
    dev.setNewStatusCb([&](OperationResult r, const types::M30XYDeviceStatus& s)
    {
        if (r == OperationResult::OPERATION_OK && s.chann_x.valid)
            ++cb_ok;
    });
    assert(dev.startStatusPolling() == OperationResult::OPERATION_OK);
    assert(dev.isStatusPollingRunning());
    assert(waitForCondition([&]{ return cb_ok.load() >= 1; }, seconds(5), milliseconds(50))
           == OperationResult::OPERATION_OK);
    assert(dev.stopStatusPolling() == OperationResult::OPERATION_OK);
    assert(!dev.isStatusPollingRunning());

    // Teardown contract.
    assert(dev.doDisconnect() == OperationResult::OPERATION_OK);
    assert(!dev.isConnected());

    std::cout << "Test_M30XYSim: ALL CHECKS PASSED" << std::endl;
    return 0;
}
