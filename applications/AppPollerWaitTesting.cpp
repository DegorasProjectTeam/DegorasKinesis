/*
 *  Copyright (C) 2018-2026 Degoras Project Team
 *
 *  This file is part of a small-scale research or utility tool built atop
 *  the Degoras Project infrastructure, released under the MIT License.
 *
 *  SPDX-License-Identifier: MIT
 *
 *  See the LICENSE file in the root directory for full license details.
 */

// C++ INCLUDES
#include <atomic>
#include <cassert>
#include <chrono>
#include <iostream>
#include <thread>

// PROJECT INCLUDES
#include "status_poller.h"
#include "wait_for.h"


using namespace thorlabs;
using thorlabs::types::OperationResult;

// ---------------------------------------------------------------------------------------------------------------------
// No-hardware self-check for the Layer-1 generic infrastructure (milestone M2): StatusPoller + waitForCondition.
// ---------------------------------------------------------------------------------------------------------------------

namespace
{

struct FakeStatus { int v = 0; };

long long elapsedMsSince(std::chrono::steady_clock::time_point t0)
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - t0).count();
}

void testWaitForCondition()
{
    // Already-true predicate returns OK immediately.
    assert(waitForCondition([]{ return true; }, std::chrono::milliseconds(100)) == OperationResult::OPERATION_OK);

    // Predicate that flips true after a few polls.
    int n = 0;
    OperationResult r = waitForCondition([&]{ return ++n >= 3; },
                                         std::chrono::milliseconds(1000), std::chrono::milliseconds(5));
    assert(r == OperationResult::OPERATION_OK);

    // Never-true predicate times out, and at least ~timeout elapses.
    const std::chrono::steady_clock::time_point t0 = std::chrono::steady_clock::now();
    r = waitForCondition([]{ return false; }, std::chrono::milliseconds(120), std::chrono::milliseconds(10));
    assert(r == OperationResult::OPERATION_TIMEOUT);
    assert(elapsedMsSince(t0) >= 100);
}

void testPollerProduceAndDeliver()
{
    StatusPoller<FakeStatus> poller;
    std::atomic<int> produced{0};
    std::atomic<int> delivered{0};
    std::atomic<int> last_v{-1};
    std::atomic<int> last_res{-1};

    auto producer = [&](FakeStatus& s) { s.v = ++produced; return OperationResult::OPERATION_OK; };
    auto sink = [&](OperationResult res, const FakeStatus& s)
    {
        last_res = static_cast<int>(res);
        last_v = s.v;
        ++delivered;
    };

    assert(poller.start(producer, sink, std::chrono::milliseconds(10)) == OperationResult::OPERATION_OK);
    assert(poller.isRunning());
    assert(poller.start(producer, sink, std::chrono::milliseconds(10)) == OperationResult::WORKER_ALREADY_RUNNING);

    std::this_thread::sleep_for(std::chrono::milliseconds(80));

    assert(poller.stop() == OperationResult::OPERATION_OK);
    assert(!poller.isRunning());
    assert(poller.stop() == OperationResult::WORKER_NOT_RUNNING);

    assert(produced.load() >= 1);
    assert(delivered.load() >= 1);
    assert(last_res.load() == static_cast<int>(OperationResult::OPERATION_OK));
    assert(last_v.load() >= 1);
}

void testPollerDeliversNonOk()
{
    StatusPoller<FakeStatus> poller;
    std::atomic<int> last_res{-1};

    auto producer = [&](FakeStatus&) { return OperationResult::READ_FAILED; };
    auto sink = [&](OperationResult res, const FakeStatus&) { last_res = static_cast<int>(res); };

    assert(poller.start(producer, sink, std::chrono::milliseconds(5)) == OperationResult::OPERATION_OK);
    std::this_thread::sleep_for(std::chrono::milliseconds(40));
    poller.stop();

    // Non-OK results must be delivered to the sink, not silently dropped.
    assert(last_res.load() == static_cast<int>(OperationResult::READ_FAILED));
}

void testPromptCancel()
{
    StatusPoller<FakeStatus> poller;
    auto producer = [&](FakeStatus&) { return OperationResult::OPERATION_OK; };
    auto sink = [&](OperationResult, const FakeStatus&) {};

    // Long interval: stop() must wake the worker via the condition variable, not wait the interval out.
    assert(poller.start(producer, sink, std::chrono::seconds(5)) == OperationResult::OPERATION_OK);
    std::this_thread::sleep_for(std::chrono::milliseconds(20));

    const std::chrono::steady_clock::time_point t0 = std::chrono::steady_clock::now();
    assert(poller.stop() == OperationResult::OPERATION_OK);
    assert(elapsedMsSince(t0) < 1000);
}

void testBoundedJoinDetach()
{
    StatusPoller<FakeStatus> poller;

    // Producer ignores stop and blocks, simulating a wedged SDK call on dead hardware.
    auto producer = [&](FakeStatus&) { std::this_thread::sleep_for(std::chrono::milliseconds(1500));
                                       return OperationResult::OPERATION_OK; };
    auto sink = [&](OperationResult, const FakeStatus&) {};

    assert(poller.start(producer, sink, std::chrono::milliseconds(10),
                        std::chrono::milliseconds(150)) == OperationResult::OPERATION_OK);
    std::this_thread::sleep_for(std::chrono::milliseconds(20));   // ensure the worker is inside the blocking producer

    const std::chrono::steady_clock::time_point t0 = std::chrono::steady_clock::now();
    const OperationResult r = poller.stop();
    const long long elapsed = elapsedMsSince(t0);

    assert(r == OperationResult::OPERATION_TIMEOUT);   // had to detach
    assert(elapsed < 1000);                            // did NOT block for the full 1.5s
    assert(!poller.isRunning());
}

void testSelfStopFromCallback()
{
    StatusPoller<FakeStatus> poller;
    std::atomic<bool> stopped_from_cb{false};

    auto producer = [&](FakeStatus&) { return OperationResult::OPERATION_OK; };
    auto sink = [&](OperationResult, const FakeStatus&)
    {
        // Simulates an owner destroying the device from inside its own callback: stop() runs on the worker thread
        // and must detach (not self-join), so this must not deadlock.
        if (!stopped_from_cb.exchange(true))
            poller.stop();
    };

    assert(poller.start(producer, sink, std::chrono::milliseconds(10)) == OperationResult::OPERATION_OK);

    const OperationResult ok = waitForCondition([&]{ return stopped_from_cb.load() && !poller.isRunning(); },
                                                std::chrono::milliseconds(1000), std::chrono::milliseconds(5));
    assert(ok == OperationResult::OPERATION_OK);   // completed without hanging
}

} // namespace

int main()
{
    testWaitForCondition();
    testPollerProduceAndDeliver();
    testPollerDeliversNonOk();
    testPromptCancel();
    testBoundedJoinDetach();
    testSelfStopFromCallback();

    std::cout << "AppPollerWaitTesting: ALL CHECKS PASSED" << std::endl;
    return 0;
}
