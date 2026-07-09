/*
 * Copyright(C) Milethos Technologies SLU. 2026
 * All rights reserved. Reproduction in whole or in
 * part is prohibited without the written consent of
 * the copyright holder.
*/

// C++ INCLUDES
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <functional>


// LEOBSE INCLUDES

#include <LeoBase/Aliases/Tasks>
#include <LeoBase/Aliases/Testing>
#include "LeoBase/Tasks/loop_task.h"
#include "LeoBase/Testing/unitary/unitary_test_macros.h"
#include <LeoBase/Aliases/Exceptions>


using namespace leobase;
using namespace leobase::testing;
using namespace leobase::exceptions;

class MockLoopTask : public leobase::tasks::LoopTask
{
public:
    MockLoopTask(long long interval_ms = 50) : LoopTask(interval_ms) {}

    std::atomic<int> executionCount{0};
    std::atomic<int> simulatedDelayMs{0};

    void runSynchronouslyForTesting()
    {
        this->isRunning_ = true; 
        this->loop(); 
    }


protected:
    void step() override
    {
        executionCount++;
        
        if (simulatedDelayMs > 0)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(simulatedDelayMs.load()));
        }
    }
};


// --- Configuration Tests ---
UT_DECLARE_TEST(LoopTask_SetGetPeriod_WorksCorrectly);

// --- Execution & Timing Tests ---
UT_DECLARE_TEST(LoopTask_Run_ExecutesPeriodically);
UT_DECLARE_TEST(LoopTask_PausePlay_StopsAndResumesExecution);
UT_DECLARE_TEST(LoopTask_ResetTime_RestartsExecution);

// --- Stop & Join Tests ---
UT_DECLARE_TEST(LoopTask_StopJoinTrue_StopsSafely);
UT_DECLARE_TEST(LoopTask_StopJoinFalse_EventuallyStops);

// --- Exception / Timeout Tests ---
UT_DECLARE_TEST(LoopTask_StepExceedsPeriod_ThrowsException);


// =======================================================================
// CONFIGURATION
// =======================================================================

/**
 * @brief Tests getter and setter for the period.
 */
UT_DEFINE_TEST(LoopTask_SetGetPeriod_WorksCorrectly)
{
    MockLoopTask task(100);
    UT_EXPECTED_TRUE(task.getPeriod() == 100);

    task.setPeriod(250);
    UT_EXPECTED_TRUE(task.getPeriod() == 250);
}

// =======================================================================
// EXECUTION & TIMING
// =======================================================================

/**
 * @brief Tests that the task runs and executes multiple times within an expected timeframe.
 */
UT_DEFINE_TEST(LoopTask_Run_ExecutesPeriodically)
{
    MockLoopTask task(20); // 20ms period
    task.run();
    
    // Wait for roughly 3-4 periods
    std::this_thread::sleep_for(std::chrono::milliseconds(75));
    
    task.stop(true);
    
    // We expect it ran at least 3 times
    UT_EXPECTED_TRUE(task.executionCount >= 3);
}

/**
 * @brief Tests the pause and play functionality.
 */
UT_DEFINE_TEST(LoopTask_PausePlay_StopsAndResumesExecution)
{
    MockLoopTask task(20);
    task.run();
    
    // Let it run for a bit
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    task.pause();
    
    int countAfterPause = task.executionCount.load();
    
    // Wait while paused. Count should NOT increase.
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    UT_EXPECTED_TRUE(task.executionCount.load() == countAfterPause);
    
    // Resume
    task.play();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    
    task.stop(true);
    
    // It should have executed more times after play()
    UT_EXPECTED_TRUE(task.executionCount.load() > countAfterPause);
}

/**
 * @brief Tests that resetTime() correctly stops and restarts the task.
 */
UT_DEFINE_TEST(LoopTask_ResetTime_RestartsExecution)
{
    MockLoopTask task(20);
    task.run();
    
    std::this_thread::sleep_for(std::chrono::milliseconds(30));
    task.resetTime(); // Stops and runs again
    
    std::this_thread::sleep_for(std::chrono::milliseconds(30));
    UT_EXPECTED_TRUE(task.isRunning()); // Should still be running
    
    task.stop(true);
}

// =======================================================================
// STOP & JOIN
// =======================================================================

/**
 * @brief Tests safe synchronous stopping.
 */
UT_DEFINE_TEST(LoopTask_StopJoinTrue_StopsSafely)
{
    MockLoopTask task(20);
    task.run();
    std::this_thread::sleep_for(std::chrono::milliseconds(30));
    
    task.stop(true);
    
    int finalCount = task.executionCount.load();
    std::this_thread::sleep_for(std::chrono::milliseconds(50)); // Wait extra time
    
    // Count should not have increased after stop
    UT_EXPECTED_TRUE(task.executionCount.load() == finalCount);
    UT_EXPECTED_FALSE(task.isRunning());
}
/**
 * @brief Tests asynchronous stopping.
 */
UT_DEFINE_TEST(LoopTask_StopJoinFalse_EventuallyStops)
{
    MockLoopTask task(20);
    task.run();
    std::this_thread::sleep_for(std::chrono::milliseconds(30));
    
    task.stop(false); // Detach stop
    
    // Give the detached thread time to process the join and set isRunning_ to false
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    
    UT_EXPECTED_FALSE(task.isRunning());
}

// =======================================================================
// EXCEPTIONS / TIMEOUTS
// =======================================================================

/**
 * @brief Tests that if a step takes longer than the period, an exception is thrown.
 * Note: Because the exception is thrown inside a std::thread, it might call std::terminate 
 * depending on your thread architecture. We need to catch it if it bubbles up, or at least 
 * verify the task stopped itself as designed in your code.
 */


UT_DEFINE_TEST(LoopTask_StepExceedsPeriod_ThrowsException)
{
    MockLoopTask task(20); // 20ms period
    task.simulatedDelayMs = 50; // Delay > Period

    // When executed synchronously, the task.loop() code blocks here,
    // pauses for 50ms, detects the timeout, and throws the exception
    // in this SAME thread, allowing the macro to catch it.

    UT_EXPECT_THROW(task.runSynchronouslyForTesting(), exceptions::LeoBaseException);
}
UT_START_SESSION("LeoBase LoopTask Unitary Tests")

// Configuration
UT_REGISTER_TEST(LoopTasks, Config, LoopTask_SetGetPeriod_WorksCorrectly);

// Execution
UT_REGISTER_TEST(LoopTasks, Execution, LoopTask_Run_ExecutesPeriodically);
UT_REGISTER_TEST(LoopTasks, Execution, LoopTask_PausePlay_StopsAndResumesExecution);
UT_REGISTER_TEST(LoopTasks, Execution, LoopTask_ResetTime_RestartsExecution);

// Stop Lifecycle
UT_REGISTER_TEST(LoopTasks, Stop, LoopTask_StopJoinTrue_StopsSafely);
UT_REGISTER_TEST(LoopTasks, Stop, LoopTask_StopJoinFalse_EventuallyStops);

// Robustness
//Fix
UT_REGISTER_TEST(LoopTasks, Robustness, LoopTask_StepExceedsPeriod_ThrowsException);

UT_RUN_TESTS();

UT_FINISH_SESSION()
