/*
 *      Copyright(C) Milethos Technologies SLU. 2025
 *      All rights reserved. Reproduction in whole or in
 *      part is prohibited without the written consent of
 *      the copyright holder.
 */

#pragma once

// C++ INCLUDES
#include <mutex>
#include <future>
#include <atomic>

// LEOBASE INCLUDES
#include "LeoBase/Aliases/Global"
#include "LeoBase/Tasks/loop_task.h"
#include "LeoBase/Crypto/leo_uuid_generator.h"

// LEOBASE MACROS
LEOBASE_MODULE_BEGIN(containers)

/**
 * @brief Thread-safe unbounded deque supporting multiple consumers and producers.
 *
 * This class provides a thread-safe deque using a `std::deque` container. It supports blocking
 * and non-blocking operations for adding and retrieving elements. Designed for use in
 * multi-threaded applications where synchronization is required.
 *
 * @tparam T The type of elements stored in the deque.
 */
template<typename T>
class FutureStorageHandler : public tasks::LoopTask
{
public:

    /**
     * @brief Constructor.
     */
    FutureStorageHandler()
        : LoopTask(1000)
        , started_(false)
        , stopped_(false)
    {}

    /**
     * @brief Destructor.
     */
    ~FutureStorageHandler()
    {
        this->stop();
    }

    void stop()
    {
        if (!this->started_)
            return;

        this->started_ = false;

        if (this->stopped_)
            return;

        this->stopped_ = true;

        LoopTask::stop();

        {
            std::unique_lock lock(this->futuresMutex_);

            // Wait until each future is finished before exit.
            for (auto& it : this->futures_)
            {
                it.second.wait();
            }

            this->futures_.clear();
        }
    }

    void start()
    {
        if (this->started_)
            return;

        LoopTask::run();

        this->started_ = true;
    }

    void add(T future)
    {
        if (!this->started_ || this->stopped_)
            return;

        std::unique_lock lock(this->futuresMutex_);

        auto uuid = crypto::LeoUUIDGenerator::getInstance().generateUUIDv4();
        this->futures_.emplace(uuid.toRFC4122String(), std::move(future));
    }

    void waitAll()
    {
        if (!this->started_ || this->stopped_)
            return;

        std::unique_lock lock(this->futuresMutex_);

        // Wait until each future is finished before exit.
        for (auto& it : this->futures_)
        {
            it.second.wait();
        }

        this->futures_.clear();
    }

private:

    void step() override
    {
        if (!this->started_ || this->stopped_)
            return;

        std::unique_lock lock(this->futuresMutex_);

        for (auto it = this->futures_.begin() ; it != this->futures_.end() ; )
        {
            if (it->second.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
            {
                // Erase and move to next valid iterator.
                it = this->futures_.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }

private:
    std::atomic<bool> started_;
    std::atomic<bool> stopped_;

    std::mutex                         futuresMutex_;
    std::unordered_map<std::string, T> futures_;

};

// ---------------------------------------------------------------------------------------------------------------------

LEOBASE_MODULE_END
