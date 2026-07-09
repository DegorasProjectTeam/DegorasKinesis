/*
 *      Copyright(C) Milethos Technologies SLU. 2025
 *      All rights reserved. Reproduction in whole or in
 *      part is prohibited without the written consent of
 *      the copyright holder.
 */

// LEOBASE INCLUDES
#include "LeoBase/Tasks/loop_task.h"
#include "LeoBase/Exceptions/leobase_exception.h"
#include "LeoBase/Global/global_definitions.h"

// LEOBASE MACROS
LEOBASE_MODULE_BEGIN(tasks)

// ---------------------------------------------------------------------------------------------------------------------

LoopTask::LoopTask(long long interval_msecs) :
    isRunning_(false),
    stopThread_(false),
    isPaused_(false),
    period_(std::chrono::milliseconds(interval_msecs)),
    thread_(nullptr)
{}

LoopTask::~LoopTask()
{
    this->stop();
};

void LoopTask::setPeriod(long long interval_msecs)
{
    std::unique_lock<std::mutex> lock(this->mutex_);
    this->period_ = std::chrono::milliseconds(interval_msecs);
}

long long LoopTask::getPeriod() const
{
    return this->period_.count();
}

void LoopTask::run()
{
    std::lock_guard<std::mutex> lock(this->mutex_);

    if (!this->isRunning_)
    {
        this->stopThread_ = false;
        this->isRunning_ = true;
        this->thread_ = std::make_shared<std::thread>(&LoopTask::loop, this);
    };
};

void LoopTask::stop(bool join)
{
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!isRunning_) return;
        stopThread_ = true;
    }

    condVar_.notify_all();

    if (join)
    {
        if (thread_ && thread_->joinable())
        {
            thread_->join();
        }
        isRunning_ = false;
    }
    else
    {
        if (thread_ && thread_->joinable())
        {
            std::thread temp_thread = std::move(*thread_);
            thread_.reset();
            std::thread([this, t = std::move(temp_thread)]() mutable
                        {
                            t.join();
                            isRunning_ = false;
                            stopThread_ = false;
                        }).detach();
        }
    }
}

void LoopTask::join()
{
    std::lock_guard<std::mutex> lock(this->mutex_);

    if (this->thread_ && this->thread_->joinable())
    {
        this->thread_->join();
        this->isRunning_ = false;
    }
    else
    {
        while (this->isRunning_)
        {
            std::this_thread::yield();
        }
    }
};

void LoopTask::resetTime()
{
    this->stop(true);
    this->run();
};

bool LoopTask::isRunning() const
{
    return this->isRunning_;
}


void LoopTask::pause()
{
    {
        std::lock_guard<std::mutex> lock(mutex_);
        this->isPaused_ = true;
    }
    this->condVar_.notify_all();
}

void LoopTask::play()
{
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!isPaused_)
            return;
        isPaused_ = false;
    }

    this->condVar_.notify_all();
}

void LoopTask::loop()
{
    const std::chrono::microseconds period_us(this->period_);

    while (true)
    {
        auto start_time = std::chrono::steady_clock::now();

        {
            std::unique_lock<std::mutex> lock(this->mutex_);
            this->condVar_.wait(lock, [this]
                { return !this->isPaused_ || this->stopThread_; });

            if (this->stopThread_)
                break;
        }

        auto step_start_time = std::chrono::steady_clock::now();

        this->step();

        auto elapsed_time = std::chrono::steady_clock::now() - start_time;
        auto step_elapsed_time = std::chrono::steady_clock::now() - step_start_time;

        if (step_elapsed_time > period_us && !this->stopThread_ && !this->isPaused_)
        {
            this->stop(true);
            const std::string error_str = "Step execution time exceeded the configured period.";
            throw exceptions::LeoBaseException(kModN, "LoopTask::loop", error_str);
        }

        if (elapsed_time < period_us && !this->stopThread_)
        {
            auto remaining_time = period_us - elapsed_time;
            std::unique_lock<std::mutex> lock(mutex_);
            condVar_.wait_for(lock, remaining_time, [this] { return this->stopThread_ || this->isPaused_; });
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

LEOBASE_MODULE_END
