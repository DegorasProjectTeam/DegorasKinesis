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
#include <atomic>
#include <bitset>
#include <cassert>
#include <chrono>
#include <iostream>
#include <thread>

// PROJECT INCLUDES
#include "DegorasKinesis/Kinesis/kinesis_simulator.h"
#include "DegorasKinesis/DCServo/dcservo_channel.h"
#include "DegorasKinesis/Kinesis/kinesis_discovery.h"
#include "DegorasKinesis/Devices/m30xy.h"


using namespace dpkin;
using namespace dpkin::kinesis;
using dpkin::types::Channel;
using dpkin::types::OperationResult;

// ---------------------------------------------------------------------------------------------------------------------
// M-VAL: per-serial concurrency and same-serial aliasing validation. Requires the Kinesis Simulator. Self-skips when
// absent. Validates two properties of the locking model:
//   (1) Aliasing: a second object opening an already-open serial gets SERIAL_IN_USE (distinct from a same-object
//       reconnect's ALREADY_CONNECTED).
//   (2) Per-serial concurrency safety: two devices on DISTINCT serials, hammered from two threads at once, run to
//       completion with no crash / deadlock / corruption (the assumption that BDC_* is safe across distinct serials,
//       guarded by per-serial mutexes). Reads on the second device use the channel adapter directly so the check does
//       not depend on that device having a settings profile.
// ---------------------------------------------------------------------------------------------------------------------

int main()
{
    using namespace std::chrono;

    KinesisSimulatorSession sim;
    std::cout << "simulator session: " << toString(sim.result()) << "\n";
    if (!sim.usable())
    {
        std::cout << "SKIP: Kinesis simulator not available.\n";
        return 0;
    }

    types::ThorlabsSNList xy;
    assert(M30XY::getDeviceList(xy) == OperationResult::OPERATION_OK);
    if (xy.empty())
    {
        std::cout << "SKIP: no virtual M30XY (type 101) in the simulator.\n";
        return 0;
    }

    // (1) Aliasing on the same serial.
    {
        M30XY owner(xy.front());
        assert(owner.doConnect() == OperationResult::OPERATION_OK);
        assert(owner.doConnect() == OperationResult::ALREADY_CONNECTED);   // same object, idempotent

        M30XY alias(xy.front());
        assert(alias.doConnect() == OperationResult::SERIAL_IN_USE);       // different object, serial already owned

        assert(owner.doDisconnect() == OperationResult::OPERATION_OK);
        std::cout << "aliasing: ALREADY_CONNECTED vs SERIAL_IN_USE distinguished -> OK\n";
    }

    // (2) Per-serial concurrency. Pick a SECOND, distinct serial (any DC servo type) for the other thread.
    types::ThorlabsSNList m30x;
    kinesis::enumerateByTypeId(105, m30x);
    std::string second;
    for (const std::string& s : m30x)
        if (s != xy.front()) { second = s; break; }

    if (second.empty())
    {
        std::cout << "NOTE: only one distinct device present; cross-device concurrency not exercised.\n";
        std::cout << "Test_Concurrency: ALL CHECKS PASSED (aliasing)\n";
        return 0;
    }

    M30XY dev_a(xy.front());
    assert(dev_a.doConnect() == OperationResult::OPERATION_OK);

    // Drive the second serial at the adapter level (settings-independent): open + poll + read.
    dcservo::DCServoChannel chan_b(second, Channel::X_CHANNEL);
    assert(chan_b.open().ok());
    chan_b.startPolling(500);
    chan_b.enableFreshnessTimer(2000);

    std::atomic<long> a_iters{0};
    std::atomic<long> b_iters{0};
    std::atomic<bool> stop{false};

    std::thread ta([&]()
    {
        while (!stop.load())
        {
            types::M30XYDeviceStatus s;
            dev_a.getDeviceStatus(s);   // exercises serialMtx(serial_a)
            ++a_iters;
        }
    });
    std::thread tb([&]()
    {
        while (!stop.load())
        {
            std::bitset<32> bits;
            chan_b.readStatusBits(bits);   // exercises serialMtx(serial_b)
            ++b_iters;
        }
    });

    std::this_thread::sleep_for(seconds(2));
    stop.store(true);
    ta.join();
    tb.join();

    std::cout << "concurrent iterations: A(" << xy.front() << ")=" << a_iters.load()
              << "  B(" << second << ")=" << b_iters.load() << "\n";

    // Both threads made real, overlapping progress and nothing crashed/hung/corrupted.
    assert(a_iters.load() > 100 && b_iters.load() > 100);

    chan_b.stopPolling();
    chan_b.close();
    assert(dev_a.doDisconnect() == OperationResult::OPERATION_OK);

    std::cout << "Test_Concurrency: ALL CHECKS PASSED (aliasing + two-serial concurrency)" << std::endl;
    return 0;
}
