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

// ---------------------------------------------------------------------------------------------------------------------

// LeoBase namespaces.
using namespace leobase::tasks;
using namespace leobase::timing;
using namespace leobase::timing::types;

// ---------------------------------------------------------------------------------------------------------------------

// Global mutex to synchronize console output.
std::mutex kGMutex;

class TestTask : public Task
{
public:

    explicit TestTask(unsigned id, long long wait_ms = 1000) :
        Task(),
        id_(id),
        waitMs_(Ms(wait_ms))
    {}

    ~TestTask() = default;

    void step() override
    {
        {
            std::lock_guard<std::mutex> lock(kGMutex);
            std::cout << "[Task " << id_ << "] Task started." << std::endl;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(this->waitMs_));

        {
            std::lock_guard<std::mutex> lock(kGMutex);
            std::cout << "[Task " << id_ << "] Task ended." << std::endl;
        }
    }

private:

    unsigned id_;
    Ms waitMs_;
};

class ComplexTestTask : public Task
{
public:

    explicit ComplexTestTask(unsigned id, long long wait_ms = 500) :
        Task(),
        id_(id),
        waitMs_(std::chrono::milliseconds(wait_ms))
    {}

    ComplexTestTask(const ComplexTestTask&) = delete;
    ComplexTestTask& operator=(const ComplexTestTask&) = delete;

    ~ComplexTestTask() override = default;

    void step() override
    {
        {
            std::lock_guard<std::mutex> lock(kGMutex);
            std::cout << "[Task " << id_ << "] Task started." << std::endl;
        }

        while (!Task::stopThread_)
        {
            std::unique_lock<std::mutex> lock(this->mtx_);
            if (Task::condVar_.wait_for(lock, this->waitMs_, [this]() { return Task::stopThread_.load(); }))
                break;

            {
                std::lock_guard<std::mutex> lock(kGMutex);
                std::cout << "[Task " << id_ << "] Executing..." << std::endl;
            }
        }

        {
            std::lock_guard<std::mutex> lock(kGMutex);
            std::cout << "[Task " << id_ << "] Task ended." << std::endl;
        }
    }

private:
    unsigned id_;
    std::chrono::milliseconds waitMs_;
    std::mutex mtx_;
};

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Main entry point of the program `ExampleTask`.
 */
int main()
{
    // Initial log.
    std::cout << "===============================================" << std::endl;
    std::cout << "== Example Task" << std::endl;
    std::cout << "===============================================" << std::endl;
    std::cout << std::endl;
    std::cout << "Starting example..." << std::endl;
    std::cout << std::endl;

    // ----------------------------------------------------------------------------------------------------
    // Execute a Single Task.

    std::cout << "[INFO] Running a single TestTask..." << std::endl;
    TestTask test_task1(1);
    test_task1.run();
    test_task1.stop();
    std::cout << "[SUCCESS] Single task executed successfully." << std::endl << std::endl;

    // ----------------------------------------------------------------------------------------------------
    // Running Multiple Tasks Concurrently.

    std::cout << "[INFO] Running multiple tasks in parallel..." << std::endl;
    std::vector<std::shared_ptr<TestTask>> tasks;

    for (int i = 2; i <= 5; ++i)
    {
        tasks.push_back(std::make_shared<TestTask>(i));
        tasks.back()->run();
    }

    for (auto& task : tasks)
    {
        task->stop();
    }

    std::cout << "[SUCCESS] Multiple tasks executed and stopped successfully." << std::endl << std::endl;

    // ----------------------------------------------------------------------------------------------------
    // Testing Repeated Start/Stop Calls.

    std::cout << "[INFO] Testing multiple start/stop calls on the same task..." << std::endl;
    TestTask test_task9(9);
    test_task9.run();
    test_task9.stop();
    test_task9.run();
    test_task9.stop();
    std::cout << "[SUCCESS] Multiple start/stop calls handled correctly." << std::endl << std::endl;

    // ----------------------------------------------------------------------------------------------------
    // Run a complex task with periodic execution and safely stop it.

    std::cout << "[INFO] Running a complex task that executes periodically..." << std::endl;

    ComplexTestTask periodic_task1(10, 200);
    ComplexTestTask periodic_task2(11, 400);
    periodic_task1.run();
    periodic_task2.run();

    std::this_thread::sleep_for(std::chrono::seconds(3));

    std::cout << "[INFO] Stopping periodic complex task 1 safely..." << std::endl;
    periodic_task1.stop();

    std::this_thread::sleep_for(std::chrono::seconds(1));

    std::cout << "[INFO] Stopping periodic complex task 2 safely..." << std::endl;
    periodic_task2.stop();

    std::cout << "[SUCCESS] Periodic complex tasks stopped safely." << std::endl << std::endl;

    //----------------------------------------------------------------------------------------------------
    // Final log.
    std::cout << std::endl << "Example finished!" << std::endl;
    std::cout << std::endl;
    leobase::system::utils::userInputPause("Press <ENTER> to exit...");

    // Final return.
    return 0;
}

// ---------------------------------------------------------------------------------------------------------------------
