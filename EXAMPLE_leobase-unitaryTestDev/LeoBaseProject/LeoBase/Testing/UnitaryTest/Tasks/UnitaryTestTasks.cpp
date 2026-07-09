/*
 * Copyright(C) Milethos Technologies SLU. 2026
 * All rights reserved. Reproduction in whole or in
 * part is prohibited without the written consent of
 * the copyright holder.
*/


//Mock Task Class for testing purposes

#include <atomic>
#include <chrono>
#include <thread>

#include <LeoBase/Aliases/Tasks>
#include <LeoBase/Aliases/Testing>
#include "LeoBase/Tasks/task.h"
#include "LeoBase/Testing/unitary/unitary_test_macros.h"

using namespace leobase;
using namespace leobase::testing;

class MockTask : public leobase::tasks::Task
{
public:
    // This will simulate a continuous background loop
    void step() override
    {
        // Loop until the base class signals stopThread_
        while (!this->stopThread_)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
    }
};

class QuickMockTask : public leobase::tasks::Task
{
public:
    // This simulates a task that finishes almost immediately
    void step() override
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
};

// --- Constructor & Initial State ---
UT_DECLARE_TEST(Task_Constructor_IsNotRunning);

// --- Task::run() Tests ---
UT_DECLARE_TEST(Task_Run_SetsRunningStateToTrue);
UT_DECLARE_TEST(Task_Run_MultipleCallsDoNotAlterState);

// --- Task::stop() Tests ---
UT_DECLARE_TEST(Task_Stop_WithJoinTrue_SetsRunningToFalse);
UT_DECLARE_TEST(Task_Stop_WithJoinFalse_EventuallySetsRunningToFalse);
UT_DECLARE_TEST(Task_Stop_WhenAlreadyStopped_RemainsFalse);

// --- Task::join() Tests ---
UT_DECLARE_TEST(Task_Join_WhenRunning_SetsRunningToFalse);
UT_DECLARE_TEST(Task_Join_WhenNotRunning_DoesNothing);


// =======================================================================
// CONSTRUCTOR & INITIAL STATE
// =======================================================================

/**
 * @brief Tests that a newly instantiated task is in a stopped state.
 */
UT_DEFINE_TEST(Task_Constructor_IsNotRunning)
{
    MockTask task;
    UT_EXPECTED_FALSE(task.isRunning());
}

// =======================================================================
// RUN() TESTS
// =======================================================================

/**
 * @brief Tests that calling run() correctly flags the task as running.
 */
UT_DEFINE_TEST(Task_Run_SetsRunningStateToTrue)
{
    MockTask task;
    task.run();
    
    UT_EXPECTED_TRUE(task.isRunning());
    
    // Cleanup
    task.stop(true); 
}

/**
 * @brief Tests that calling run() on an already running task maintains the running state 
 * and doesn't break the internal logic.
 */
UT_DEFINE_TEST(Task_Run_MultipleCallsDoNotAlterState)
{
    MockTask task;
    task.run();
    UT_EXPECTED_TRUE(task.isRunning());
    
    // Second call
    task.run();
    UT_EXPECTED_TRUE(task.isRunning());
    
    // Cleanup
    task.stop(true);
}


// =======================================================================
// STOP() TESTS
// =======================================================================

/**
 * @brief Tests that stop(true) blocks and safely transitions the state to false.
 */
UT_DEFINE_TEST(Task_Stop_WithJoinTrue_SetsRunningToFalse)
{
    MockTask task;
    task.run();
    UT_EXPECTED_TRUE(task.isRunning());
    
    task.stop(true); // Blocks until thread exits
    
    UT_EXPECTED_FALSE(task.isRunning());
}

/**
 * @brief Tests that stop(false) detaches the thread and eventually sets isRunning to false.
 */
UT_DEFINE_TEST(Task_Stop_WithJoinFalse_EventuallySetsRunningToFalse)
{
    MockTask task;
    task.run();
    UT_EXPECTED_TRUE(task.isRunning());
    
    task.stop(false); // Does not block
    
    // Allow the detached cleanup thread a moment to process t.join() and set isRunning_ = false
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    
    UT_EXPECTED_FALSE(task.isRunning());
}

/**
 * @brief Tests that calling stop() on a task that isn't running is perfectly safe.
 */
UT_DEFINE_TEST(Task_Stop_WhenAlreadyStopped_RemainsFalse)
{
    MockTask task;
    UT_EXPECTED_FALSE(task.isRunning());
    
    task.stop(true);
    
    UT_EXPECTED_FALSE(task.isRunning());
}


// =======================================================================
// JOIN() TESTS
// =======================================================================

/**
 * @brief Tests that join() waits for the thread and updates the running state to false.
 */
UT_DEFINE_TEST(Task_Join_WhenRunning_SetsRunningToFalse)
{
    QuickMockTask task; // Using the quick task so it finishes naturally fast
    task.run();
    UT_EXPECTED_TRUE(task.isRunning());
    
    // Note: Since step() doesn't loop in QuickMockTask, it will finish quickly.
    // join() should process this and set isRunning_ to false.
    task.join();
    
    UT_EXPECTED_FALSE(task.isRunning());
}

/**
 * @brief Tests that calling join() on a non-running task is a safe no-op.
 */
UT_DEFINE_TEST(Task_Join_WhenNotRunning_DoesNothing)
{
    MockTask task;
    UT_EXPECTED_FALSE(task.isRunning());
    
    task.join();
    
    UT_EXPECTED_FALSE(task.isRunning());
}


UT_START_SESSION("LeoBase Task Unitary Tests")

UT_REGISTER_TEST(Tasks, State, Task_Constructor_IsNotRunning);

UT_REGISTER_TEST(Tasks, Run, Task_Run_SetsRunningStateToTrue);
UT_REGISTER_TEST(Tasks, Run, Task_Run_MultipleCallsDoNotAlterState);

UT_REGISTER_TEST(Tasks, Stop, Task_Stop_WithJoinTrue_SetsRunningToFalse);
UT_REGISTER_TEST(Tasks, Stop, Task_Stop_WithJoinFalse_EventuallySetsRunningToFalse);
UT_REGISTER_TEST(Tasks, Stop, Task_Stop_WhenAlreadyStopped_RemainsFalse);

UT_REGISTER_TEST(Tasks, Join, Task_Join_WhenRunning_SetsRunningToFalse);
UT_REGISTER_TEST(Tasks, Join, Task_Join_WhenNotRunning_DoesNothing);

UT_RUN_TESTS();

UT_FINISH_SESSION() //This is somthing to be aware
