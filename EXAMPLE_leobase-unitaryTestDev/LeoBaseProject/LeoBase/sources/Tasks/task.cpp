/*
 *      Copyright(C) Milethos Technologies SLU. 2025
 *      All rights reserved. Reproduction in whole or in
 *      part is prohibited without the written consent of
 *      the copyright holder.
 */

// C++ INCLUDES
#include <iostream>
#include <memory>

// LEOBASE INCLUDES
#include "LeoBase/Tasks/task.h"

// LEOBASE MACROS
LEOBASE_MODULE_BEGIN(tasks)

// ---------------------------------------------------------------------------------------------------------------------

Task::Task():
    thread_(std::thread()),
    isRunning_(false),
    stopThread_(false)
{}

Task::~Task()
{
    this->stop();
}

void Task::run()
{
    std::lock_guard<std::mutex> lock(this->mutex_);

    if (!this->isRunning_)
    {
        this->stopThread_ = false;
        this->isRunning_ = true;
        this->thread_ = std::thread([this]() { this->step(); });
    }
}

void Task::stop(bool join)
{
    std::lock_guard<std::mutex> lock(this->mutex_);

    if (this->isRunning_)
    {
        this->stopThread_ = true;
        this->condVar_.notify_all();

        if (this->thread_.joinable() && join)
        {
            this->thread_.join();
            this->isRunning_ = false;
        }
        else if (this->thread_.joinable())
        {
            std::thread([this, t = std::move(this->thread_)]() mutable
                        {
                            t.join();
                            this->isRunning_ = false;
                            this->stopThread_ = false;
                        }).detach();
        }
    }
}

void Task::join()
{
    std::lock_guard<std::mutex> lock(this->mutex_);

    if (this->thread_.joinable())
    {
        this->thread_.join();
        this->isRunning_ = false;
    }
    else
    {
        while (this->isRunning_)
        {
            std::this_thread::yield();
        }
    }
}

bool Task::isRunning() const
{
    return this->isRunning_;
}

// ---------------------------------------------------------------------------------------------------------------------

LEOBASE_MODULE_END
