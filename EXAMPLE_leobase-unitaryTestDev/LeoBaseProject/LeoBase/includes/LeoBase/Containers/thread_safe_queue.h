/*
 *      Copyright(C) Milethos Technologies SLU. 2025
 *      All rights reserved. Reproduction in whole or in
 *      part is prohibited without the written consent of
 *      the copyright holder.
 *
 *      Author: Javier Pardo
 *      Summary: Thread Safe Queue (Updated)
 */

#pragma once

// C++ INCLUDES
#include <algorithm>
#include <queue>
#include <optional>
#include <mutex>
#include <condition_variable>
#include <atomic>

// LEOBASE INCLUDES
#include "LeoBase/Patterns/non_copyable.h"
#include "LeoBase/Patterns/non_movable.h"

// LEOBASE MACROS
LEOBASE_MODULE_BEGIN(containers)

/**
 * @brief Thread-safe unbounded queue supporting multiple consumers and producers.
 *
 * This class provides a thread-safe queue using a `std::deque` container. It supports blocking
 * and non-blocking operations for adding and retrieving elements. Designed for use in
 * multi-threaded applications where synchronization is required.
 *
 * @tparam T The type of elements stored in the queue.
 */
template<typename T>
class ThreadSafeQueue : public patterns::NonCopyable,
                        public patterns::NonMovable
{
public:

    /**
     * @brief Default constructor.
     */
    ThreadSafeQueue() :
        size_(0),
        stopReq_(false)
    {}

    /**
     * @brief Attempts to pop an element without blocking.
     * @return An optional containing the popped element if available, or `std::nullopt` if the queue is empty.
     */
    std::optional<T> tryPop()
    {
        std::lock_guard<std::mutex> lk(this->lock_);
        if (this->queue_.empty()) return std::nullopt;

        T value = std::move(this->queue_.front());
        this->queue_.pop();
        --this->size_;
        return value;
    }

    /**
     * @brief Attempts to pop an element into the provided reference without blocking.
     * @param[out] value The popped element.
     * @return `true` if an element was dequeued, `false` if the queue was empty.
     */
    bool tryPop(T& value)
    {
        std::lock_guard<std::mutex> lk(this->lock_);
        if (this->queue_.empty()) return false;

        value = std::move(this->queue_.front());
        this->queue_.pop();
        --this->size_;
        return true;
    }

    /**
     * @brief Blocking method to pop an element from the queue.
     *
     * This method will block until an element becomes available or if `stop()` is called.
     * If the queue is stopped and no element is available, it returns `std::nullopt`.
     *
     * @return The popped element as `std::optional<T>`, or `std::nullopt` if the queue is empty/stopped.
     */
    std::optional<T> waitAndPop()
    {
        std::unique_lock<std::mutex> lk(this->lock_);
        this->condVar_.wait(lk, [this] { return this->stopReq_ || !this->queue_.empty(); });

        if (this->stopReq_ && this->queue_.empty())
            return std::nullopt;

        T value = std::move(this->queue_.front());
        this->queue_.pop();
        --this->size_;
        return value;
    }

    /**
     * @brief Blocking method to pop an element into a provided reference.
     *
     * This method blocks until an element is available or if `stop()` is called.
     * If the queue is stopped and no element is available, it does nothing.
     *
     * @param[out] value Reference where the popped element will be stored.
     * @return `true` if an element was successfully popped, `false` if stopped and no element exists.
     */
    bool waitAndPop(T& value)
    {
        std::unique_lock<std::mutex> lk(this->lock_);
        this->condVar_.wait(lk, [this] { return this->stopReq_ || !this->queue_.empty(); });

        if (this->stopReq_ && this->queue_.empty())
            return false;

        value = std::move(this->queue_.front());
        this->queue_.pop_front();
        --this->size_;
        return true;
    }

    /**
     * @brief Adds an element to the back of the queue if not stopped.
     * @param element The element to add.
     */
    void push(const T& element)
    {
        {
            std::lock_guard<std::mutex> lk(this->lock_);
            if (this->stopReq_)
                return;
            this->queue_.push(element);
            ++this->size_;
        }
        this->condVar_.notify_one();
    }

    /**
     * @brief Adds an element to the queue using move semantics if not stopped.
     * @param element The element to add.
     */
    void push(T&& element)
    {
        {
            std::lock_guard<std::mutex> lk(this->lock_);
            if (this->stopReq_)
                return;
            this->queue_.push(std::move(element));
            ++this->size_;
        }
        this->condVar_.notify_one();
    }

    /**
     * @brief Removes all elements from the queue that match the provided value.
     *
     * This function iterates through the queue and removes all elements that are equal to the given `element`.
     * It maintains thread safety while performing the operation.
     *
     * @param element The value to compare against for removal.
     */
    void removeAll(const T& element)
    {
        std::lock_guard<std::mutex> lk(this->lock_);
        if (this->queue_.empty()) return;

        // Create a new queue that will hold only the elements we want to keep.
        std::queue<T> new_queue;

        // Move all elements that are NOT equal to `element`
        while (!this->queue_.empty())
        {
            if (this->queue_.front() != element)
                new_queue.push(std::move(this->queue_.front()));

            this->queue_.pop();
        }

        // Swap old queue with the new queue (efficient replacement)
        std::swap(this->queue_, new_queue);

        // Update size_
        this->size_ = this->queue_.size();
    }

    /**
     * @brief Stops the queue, waking all waiting consumers.
     *
     * Once stopped, `waitAndPop()` will return `std::nullopt` if the queue is empty.
     */
    void stop()
    {
        this->stopReq_.store(true, std::memory_order_relaxed);
        this->condVar_.notify_all();
    }

    /**
     * @brief Checks whether the queue is empty.
     * @return `true` if the queue is empty, `false` otherwise.
     */
    bool empty() const noexcept
    {
        return this->size_ == 0;
    }

    /**
     * @brief Retrieves the current size of the queue.
     * @return The number of elements in the queue.
     */
    size_t size() const noexcept
    {
        return this->size_;
    }

    /**
     * @brief Clears all elements from the queue and reset the stop state.
     */
    void clear()
    {
        std::lock_guard<std::mutex> lk(this->lock_);
        std::queue<T> empty;
        std::swap(this->queue_, empty);
        this->size_ = 0;
        this->stopReq_ = false;
        this->condVar_.notify_all();
    }

private:

    std::mutex lock_;                 ///< Mutex for protecting shared data.
    std::condition_variable condVar_; ///< Condition variable for blocking operations.
    std::queue<T> queue_;             ///< Internal queue storage.
    std::atomic<size_t> size_;        ///< Atomic size tracking.
    std::atomic<bool> stopReq_;       ///< Flag to signal stop condition.
};

// ---------------------------------------------------------------------------------------------------------------------

LEOBASE_MODULE_END
