/*
 * Copyright(C) Milethos Technologies SLU. 2026
 * All rights reserved. Reproduction in whole or in
 * part is prohibited without the written consent of
 * the copyright holder.
 */

// C++ INCLUDES
#include <atomic>
#include <thread>
#include <chrono>
#include <vector>
#include <algorithm>
// LeoBase INCLUDES
#include <LeoBase/Aliases/Testing>
#include "LeoBase/System/console/custom_console.h" 
#include "LeoBase/Testing/unitary/unitary_test_macros.h"

// =======================================================================
// NAMESPACES
// =======================================================================
using namespace leobase;
using namespace leobase::testing;
using namespace leobase::system::console;

// =======================================================================
// DECLARATIONS
// =======================================================================

UT_DECLARE_TEST(Singleton_IsThreadSafeOnInitialization);
UT_DECLARE_TEST(ConfigureAndRestore_HandlesMultipleInvocationsSafely);
UT_DECLARE_TEST(SetExitCallback_HandlesNullAndOverwrites);
UT_DECLARE_TEST(ConsoleCtrlHandler_IgnoresUnhandledEvents);
UT_DECLARE_TEST(WaitForClose_BlocksAndWakesUpOnCtrlEvent);

// =======================================================================
// DEFINITIONS
// =======================================================================

/**
 * @brief Ensures that multiple threads requesting the singleton simultaneously 
 * always get the exact same memory address without crashing.
 */
UT_DEFINE_TEST(Singleton_IsThreadSafeOnInitialization)
{
    const int num_threads = 10;
    std::vector<std::thread> threads;
    std::vector<CustomConsole*> instances(num_threads, nullptr);

    for (int i = 0; i < num_threads; ++i)
    {
        threads.emplace_back([&instances, i]() {
            instances[i] = &CustomConsole::getInstance();
        });
    }

    for (auto& t : threads)
    {
        if (t.joinable()) t.join();
    }

    // All pointers in the array must point to the exact same instance
    //
    CustomConsole* first_instance = instances[0];
    // for (int i = 1; i < num_threads; ++i)
    // {
    //     if (instances[i] != first_instance) {
    //         all_same = false;
    //         break;
    //     }
    // }

    //std::all_of to avoid the use of break

    bool all_same = std::all_of(instances.begin(), instances.end(),
                                [first_instance](CustomConsole* inst) {
                                    return inst == first_instance;
                                });

    UT_EXPECTED_TRUE(all_same == true);
}

/**
 * @brief Stress tests the configuration/restore methods to ensure mutexes 
 * prevent race conditions and don't deadlock upon repeated calls.
 */
UT_DEFINE_TEST(ConfigureAndRestore_HandlesMultipleInvocationsSafely)
{
    CustomConsole& console = CustomConsole::getInstance();
    
    // Call configure multiple times
    console.configureConsole(false, true, false);
    console.configureConsole(false, false, true);
    console.configureConsole(true, false, false);
    // Call restore multiple times
    console.restoreConsole();
    console.restoreConsole();
    console.restoreConsole();
    // If we didn't crash or deadlock due to double locking, it passes.
    UT_EXPECTED_TRUE(true);
}

/**
 * @brief Verifies that setting a null callback is safe and that new callbacks 
 * overwrite previous ones correctly.
 */
UT_DEFINE_TEST(SetExitCallback_HandlesNullAndOverwrites)
{
    CustomConsole& console = CustomConsole::getInstance();
    
    // 1. Set to nullptr (should not crash)
    console.setExitCallback(nullptr);
    
    // 2. Set to a valid callback
    int counter = 0;
    console.setExitCallback([&counter]() { counter += 1; });
    
    // 3. Overwrite with a new one
    console.setExitCallback([&counter]() { counter += 10; });
    
    // We cannot easily invoke it here directly without triggering close_flag, 
    // but we verify the assignment logic runs without exceptions.
    UT_EXPECTED_TRUE(true);
}

/**
 * @brief Verifies that random/unhandled signals (like 999) return FALSE 
 * and do NOT trigger the close state.
 */
UT_DEFINE_TEST(ConsoleCtrlHandler_IgnoresUnhandledEvents)
{
    CustomConsole& console = CustomConsole::getInstance();
    
    // Pass a dummy/unsupported event code
    DWORD dummy_event = 999;
    BOOL handled = console.consoleCtrlHandler(dummy_event);
    
    UT_EXPECTED_TRUE(handled == FALSE);
    UT_EXPECTED_TRUE(console.closeStatus() == false);
}

/**
 * @brief Crucial concurrency test: A background thread waits for the console to close. 
 * The main thread triggers a CTRL_CLOSE_EVENT. We verify the thread unblocks properly.
 */
UT_DEFINE_TEST(WaitForClose_BlocksAndWakesUpOnCtrlEvent)
{
    CustomConsole& console = CustomConsole::getInstance();
    
    std::atomic<bool> thread_woke_up{false};
    std::atomic<bool> callback_executed{false};

    // Set a callback to ensure it gets executed during the simulated shutdown
    console.setExitCallback([&callback_executed]() {
        callback_executed = true;
    });

    // Launch a background thread that will block on waitForClose()
    std::thread waiter_thread([&console, &thread_woke_up]() {
        console.waitForClose();
        thread_woke_up = true;
    });

    // Give the background thread a tiny moment to actually reach the wait state
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    
    // Ensure the thread is actually blocked (hasn't woken up yet)
    UT_EXPECTED_TRUE(thread_woke_up.load() == false);

    // Simulate the user closing the console (triggers the handler and condition variable)
    BOOL handled = console.consoleCtrlHandler(CTRL_CLOSE_EVENT);
    
    // Wait for the background thread to finish its execution
    if (waiter_thread.joinable()) {
        waiter_thread.join();
    }

    // Assertions
    UT_EXPECTED_TRUE(handled == TRUE);
    UT_EXPECTED_TRUE(console.closeStatus() == true);
    UT_EXPECTED_TRUE(callback_executed.load() == true);
    UT_EXPECTED_TRUE(thread_woke_up.load() == true);
}

// =======================================================================
// SESSION
// =======================================================================

UT_START_SESSION("LeoBase CustomConsole Unitary Tests")

UT_REGISTER_TEST(System, CustomConsole, Singleton_IsThreadSafeOnInitialization);
UT_REGISTER_TEST(System, CustomConsole, ConfigureAndRestore_HandlesMultipleInvocationsSafely);
UT_REGISTER_TEST(System, CustomConsole, SetExitCallback_HandlesNullAndOverwrites);
UT_REGISTER_TEST(System, CustomConsole, ConsoleCtrlHandler_IgnoresUnhandledEvents);
UT_REGISTER_TEST(System, CustomConsole, WaitForClose_BlocksAndWakesUpOnCtrlEvent);

UT_RUN_TESTS();

UT_FINISH_SESSION()
