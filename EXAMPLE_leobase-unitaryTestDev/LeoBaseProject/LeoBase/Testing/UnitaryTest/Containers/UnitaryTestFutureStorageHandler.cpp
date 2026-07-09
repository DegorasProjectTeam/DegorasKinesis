/*
 * Copyright(C) Milethos Technologies SLU. 2026
 * All rights reserved. Reproduction in whole or in
 * part is prohibited without the written consent of
 * the copyright holder.
*/

#include <future>
#include <atomic>
#include <chrono>
#include <thread>

// LeoBase INCLUDES
#include <LeoBase/Aliases/Containers>
#include <LeoBase/Aliases/Testing>

#include "LeoBase/Containers/future_storage_handler.h"
#include "LeoBase/Testing/unitary/unitary_test_macros.h"

using namespace leobase;
using namespace leobase::testing;
using namespace leobase::containers;

// =======================================================================
// DECLARATIONS
// =======================================================================

UT_DECLARE_TEST(StartAndStop_DoesNotCrash);
UT_DECLARE_TEST(Add_WhenNotStarted_IgnoresFuture);
UT_DECLARE_TEST(WaitAll_BlocksUntilFuturesComplete);
UT_DECLARE_TEST(Stop_BlocksUntilFuturesComplete);
UT_DECLARE_TEST(Step_CleansUpReadyFuturesBackground);

// =======================================================================
// DEFINITIONS
// =======================================================================

/**
 * @brief Checks that the basic start and stop lifecycle works without interruptions.
 */
UT_DEFINE_TEST(StartAndStop_DoesNotCrash)
{
    FutureStorageHandler<std::future<void>> handler;
    
    UT_EXPECT_NO_THROW(handler.start());
    UT_EXPECT_NO_THROW(handler.stop());
}

/**
 * @brief If a future is added before starting or after stopping, it should be ignored without failing.
 */
UT_DEFINE_TEST(Add_WhenNotStarted_IgnoresFuture)
{
    FutureStorageHandler<std::future<void>> handler;
    std::atomic<bool> executed = false;

    // Create a future that executes immediately
    auto fut = std::async(std::launch::async, [&](){
        executed = true;
    });

    // Add without calling start()
    handler.add(std::move(fut));

    // If we wait a moment, the future finishes on its own (because std::async launches it),
    // but we want to ensure the handler does not crash when calling waitAll or stop.
    UT_EXPECT_NO_THROW(handler.waitAll());
    UT_EXPECT_NO_THROW(handler.stop());
    
    // Wait to prevent the destroyed std::async from blocking or corrupting memory
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    UT_EXPECTED_TRUE(executed.load());
}

/**
 * @brief Checks that waitAll() blocks execution until pending futures complete.
 */
UT_DEFINE_TEST(WaitAll_BlocksUntilFuturesComplete)
{
    FutureStorageHandler<std::future<void>> handler;
    handler.start();

    std::atomic<bool> taskFinished = false;

    auto fut = std::async(std::launch::async, [&](){
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        taskFinished = true;
    });

    handler.add(std::move(fut));

    // At this point, taskFinished should be false
    UT_EXPECTED_FALSE(taskFinished.load());

    // waitAll should block until the 200ms sleep_for finishes
    handler.waitAll();

    // After waitAll, the task MUST be finished
    UT_EXPECTED_TRUE(taskFinished.load());

    handler.stop();
}

/**
 * @brief Checks that the destructor / stop() block until all in-flight tasks complete.
 */
UT_DEFINE_TEST(Stop_BlocksUntilFuturesComplete)
{
    std::atomic<bool> taskFinished = false;

    {
        FutureStorageHandler<std::future<void>> handler;
        handler.start();

        auto fut = std::async(std::launch::async, [&](){
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            taskFinished = true;
        });

        handler.add(std::move(fut));
        
        // Call stop manually (it would also be called in the destructor).
        handler.stop(); 
        
        // Stop should have waited for it to finish
        UT_EXPECTED_TRUE(taskFinished.load());
    }
}

/**
 * @brief Evaluates that the background LoopTask (the step method) cleans up finished futures.
 * Since we cannot see the size of the internal map, we verify there are no unexpected blocks.
 */
UT_DEFINE_TEST(Step_CleansUpReadyFuturesBackground)
{
    FutureStorageHandler<std::future<void>> handler;
    handler.start();

    // Launch a very fast task
    auto fut = std::async(std::launch::async, [](){
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    });

    handler.add(std::move(fut));

    // Wait longer than the task duration AND the internal LoopTask loop (assuming 1000ms or similar).
    // If step() is working, it will silently clean up the future.
    std::this_thread::sleep_for(std::chrono::milliseconds(1500));

    // Calling waitAll now should be practically instantaneous, since the map should be empty.
    auto startWait = std::chrono::steady_clock::now();
    handler.waitAll();
    auto endWait = std::chrono::steady_clock::now();

    auto waitDuration = std::chrono::duration_cast<std::chrono::milliseconds>(endWait - startWait).count();
    
    // Assume it won't take more than 50ms to process an empty or already completed container
    UT_EXPECTED_TRUE(waitDuration < 50);

    handler.stop();
}

// =======================================================================
// SESSION
// =======================================================================

UT_START_SESSION("LeoBase FutureStorageHandler Unitary Tests")

UT_REGISTER_TEST(Containers, FutureStorageHandler, StartAndStop_DoesNotCrash);
UT_REGISTER_TEST(Containers, FutureStorageHandler, Add_WhenNotStarted_IgnoresFuture);
UT_REGISTER_TEST(Containers, FutureStorageHandler, WaitAll_BlocksUntilFuturesComplete);
UT_REGISTER_TEST(Containers, FutureStorageHandler, Stop_BlocksUntilFuturesComplete);
UT_REGISTER_TEST(Containers, FutureStorageHandler, Step_CleansUpReadyFuturesBackground);

UT_RUN_TESTS();

UT_FINISH_SESSION()
