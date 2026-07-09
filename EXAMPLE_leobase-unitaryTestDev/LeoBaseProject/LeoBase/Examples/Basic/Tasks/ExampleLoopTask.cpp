/*
 *      Copyright(C) Milethos Technologies SLU. 2025
 *      All rights reserved. Reproduction in whole or in
 *      part is prohibited without the written consent of
 *      the copyright holder.
 */

// C++ INCLUDES
#include <iostream>
#include <mutex>

// LEOBASE INCLUDES
#include <LeoBase/Aliases/Tasks>
#include <LeoBase/Aliases/Timing>
#include <LeoBase/Aliases/System>
#include <LeoBase/Aliases/Exceptions>

// ---------------------------------------------------------------------------------------------------------------------

// LeoBase namespaces.
using namespace leobase::tasks;
using namespace leobase::timing;
using namespace leobase::timing::types;
using namespace leobase::timing::utils;

// ---------------------------------------------------------------------------------------------------------------------

// Global mutex to synchronize console output.
std::mutex kGMutex;

class SimpleLoopTask : public LoopTask
{
public:

    explicit SimpleLoopTask(unsigned id, long long interval_ms = 1000) :
        LoopTask(interval_ms),
        id_(id)
    {}

    ~SimpleLoopTask() = default;

    void step() override
    {
        std::lock_guard<std::mutex> lock(kGMutex);
        std::string now_dt = currentDatetimeIso8601();
        std::cout << now_dt + " - [LoopTask " << id_ << "] Executing at interval: "
                  << LoopTask::getPeriod() << " ms" << std::endl;
    }

private:

    unsigned id_;
};

class ComplexTestTask : public LoopTask
{
public:

    explicit ComplexTestTask(unsigned id, long long interval_ms = 1000, long long execution_time_ms = 500) :
        LoopTask(interval_ms),
        id_(id),
        executionTime_(std::chrono::milliseconds(execution_time_ms))
    {}

    ComplexTestTask(const ComplexTestTask&) = delete;
    ComplexTestTask& operator=(const ComplexTestTask&) = delete;

    ~ComplexTestTask() override = default;

    void step() override
    {
        {
            std::lock_guard<std::mutex> lock(kGMutex);
            std::cout << "[LoopTask " << id_ << "] Task started." << std::endl;
        }

        while (true)
        {
            std::unique_lock<std::mutex> lock(this->mtx_);
            if (LoopTask::condVar_.wait_for(lock, this->executionTime_, [this]()
                                            {return LoopTask::stopThread_.load() || LoopTask::isPaused_.load();}))
            {
                break;
            }

            {
                std::lock_guard<std::mutex> lock(kGMutex);
                std::string now_dt = currentDatetimeIso8601();
                std::cout <<  now_dt + + " - [LoopTask " << id_ << "] Executing complex step..." << std::endl;
            }
        }

        {
            std::lock_guard<std::mutex> lock(kGMutex);
            std::string now_dt = currentDatetimeIso8601();
            std::cout <<  now_dt + " - [LoopTask " << id_ << "] Task ended safely." << std::endl;
        }
    }

private:
    unsigned id_;
    std::chrono::milliseconds executionTime_;  ///< Fixed execution time for the simulated task.
    std::mutex mtx_;
};

class ExceptionTestTask : public LoopTask
{
public:

    explicit ExceptionTestTask(unsigned id) :
        LoopTask(100ll),
        id_(id),
        executionTime_(std::chrono::milliseconds(500))
    {}

    ExceptionTestTask(const ExceptionTestTask&) = delete;
    ExceptionTestTask& operator=(const ExceptionTestTask&) = delete;

    ~ExceptionTestTask() override = default;

    void step() override
    {
        {
            std::lock_guard<std::mutex> lock(kGMutex);
            std::cout << "[LoopTask " << id_ << "] Task started." << std::endl;
        }

        std::this_thread::sleep_for(this->executionTime_);

        {
            std::lock_guard<std::mutex> lock(kGMutex);
            std::string now_dt = currentDatetimeIso8601();
            std::cout <<  now_dt + " - [LoopTask " << id_ << "] Task ended safely." << std::endl;
        }
    }

    void loop()
    {
        LoopTask::loop();
    }

private:
    unsigned id_;
    std::chrono::milliseconds executionTime_;  ///< Fixed execution time for the simulated task.
    std::mutex mtx_;
};

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Main entry point of the program `ExampleLoopTask`.
 */
int main()
{
    // Initial log.
    std::cout << "===============================================" << std::endl;
    std::cout << "== Example ExampleLoopTask" << std::endl;
    std::cout << "===============================================" << std::endl;
    std::cout << std::endl;
    std::cout << "Starting example..." << std::endl;
    std::cout << std::endl;

    // ----------------------------------------------------------------------------------------------------
    // Execute a Single Loop Task.

    std::cout << "[INFO] Running a single SimpleLoopTask..." << std::endl;
    SimpleLoopTask loop_task1(1, 500);
    loop_task1.run();

    std::this_thread::sleep_for(std::chrono::seconds(3));
    loop_task1.stop();

    std::cout << "[SUCCESS] Single loop task executed and stopped successfully." << std::endl << std::endl;

    // ----------------------------------------------------------------------------------------------------
    // Running Multiple Loop Tasks Concurrently.

    std::cout << "[INFO] Running multiple loop tasks in parallel..." << std::endl;
    std::vector<std::shared_ptr<SimpleLoopTask>> loop_tasks;

    for (int i = 2; i <= 5; ++i)
    {
        loop_tasks.push_back(std::make_shared<SimpleLoopTask>(i, 800));
        loop_tasks.back()->run();
    }

    std::this_thread::sleep_for(std::chrono::seconds(3));

    for (auto& task : loop_tasks)
    {
        task->stop();
    }

    std::cout << "[SUCCESS] Multiple loop tasks executed and stopped successfully." << std::endl << std::endl;

    // ----------------------------------------------------------------------------------------------------
    // Testing Repeated Start/Stop Calls.

    std::cout << "[INFO] Testing multiple start/stop calls on the same loop task..." << std::endl;
    SimpleLoopTask loop_task9(9, 1000);
    loop_task9.run();
    std::this_thread::sleep_for(std::chrono::seconds(2));
    loop_task9.stop();
    std::this_thread::sleep_for(std::chrono::seconds(1));
    loop_task9.run();
    std::this_thread::sleep_for(std::chrono::seconds(2));
    loop_task9.stop();

    std::cout << "[SUCCESS] Multiple start/stop calls handled correctly." << std::endl << std::endl;

    std::cout << "[INFO] Running a complex task that executes periodically..." << std::endl;

    // ----------------------------------------------------------------------------------------------------
    // Testing play and pause.

    std::cout << "[INFO] Testing play/pause functionality in LoopTask..." << std::endl;

    // Create a loop task with a 1-second execution interval.
    SimpleLoopTask playpause_task(10, 1000);
    playpause_task.run();

    std::this_thread::sleep_for(std::chrono::seconds(3));

    // Pause the task.
    std::cout << "[INFO] Pausing the loop task..." << std::endl;
    playpause_task.pause();

    std::this_thread::sleep_for(std::chrono::seconds(3));

    // Resume the task.
    std::cout << "[INFO] Resuming the loop task..." << std::endl;
    playpause_task.play();

    std::this_thread::sleep_for(std::chrono::seconds(3));

    // Stop the task safely.
    std::cout << "[INFO] Stopping the loop task safely..." << std::endl;
    playpause_task.stop();

    std::cout << "[SUCCESS] Play/Pause test completed." << std::endl << std::endl;

    // ----------------------------------------------------------------------------------------------------
    // Testing play and pause in complex.

    std::cout << "[INFO] Testing play/pause functionality in complex LoopTask..." << std::endl;

    // Create a loop task with a 1-second execution interval.
    ComplexTestTask complex_playpause_task(11, 1000, 600);
    complex_playpause_task.run();

    std::this_thread::sleep_for(std::chrono::seconds(3));

    // Pause the task.
    std::cout << "[INFO] Pausing the complex loop task..." << std::endl;
    complex_playpause_task.pause();

    std::this_thread::sleep_for(std::chrono::seconds(3));

    // Resume the task.
    std::cout << "[INFO] Resuming the complex loop task..." << std::endl;
    complex_playpause_task.play();

    std::this_thread::sleep_for(std::chrono::seconds(3));

    // Stop the task safely.
    std::cout << "[INFO] Stopping the complex loop task safely..." << std::endl;
    complex_playpause_task.stop();

    std::cout << "[SUCCESS] Play/Pause test completed." << std::endl << std::endl;

    // ----------------------------------------------------------------------------------------------------
    // Complex tasks.

    ComplexTestTask periodic_task1(12, 800, 500);
    ComplexTestTask periodic_task2(13, 200, 500);
    periodic_task1.run();
    periodic_task2.run();

    std::this_thread::sleep_for(std::chrono::seconds(3));

    std::cout << "[INFO] Stopping periodic complex task 1 safely..." << std::endl;
    periodic_task1.stop();

    std::this_thread::sleep_for(std::chrono::seconds(2));

    std::cout << "[INFO] Stopping periodic complex task 2 safely..." << std::endl;
    periodic_task2.stop();

    std::cout << "[SUCCESS] Periodic complex tasks stopped safely." << std::endl << std::endl;

    // ----------------------------------------------------------------------------------------------------
    // Testing Exception Handling.

    std::cout << "[INFO] Testing exception handling when step execution exceeds period..." << std::endl;

    try
    {
        ExceptionTestTask exception_task(14);
        exception_task.loop();
    }
    catch (const leobase::exceptions::LeoBaseException& e)
    {
        std::cout << "[EXCEPTION] Exception caught: " << e.what() << std::endl;
        std::cout << "[SUCCESS] Exception handling test completed successfully." << std::endl;
    }

    std::cout << std::endl << "[INFO] Continuing execution after exception test..." << std::endl << std::endl;

    //----------------------------------------------------------------------------------------------------
    // Final log.
    std::cout << std::endl << "Example finished!" << std::endl;
    std::cout << std::endl;
    leobase::system::utils::userInputPause("Press <ENTER> to exit...");

    // Final return.
    return 0;
}

// ---------------------------------------------------------------------------------------------------------------------
