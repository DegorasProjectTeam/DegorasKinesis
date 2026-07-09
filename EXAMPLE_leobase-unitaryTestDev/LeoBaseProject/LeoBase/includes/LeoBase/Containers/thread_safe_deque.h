/*
 *      Copyright(C) Milethos Technologies SLU. 2025
 *      All rights reserved. Reproduction in whole or in
 *      part is prohibited without the written consent of
 *      the copyright holder.
 */

#pragma once

// C++ INCLUDES
#include <deque>
#include <mutex>
#include <condition_variable>
#include <algorithm>
#include <optional>
#include <atomic>

// LEOBASE INCLUDES
#include "LeoBase/Aliases/Global"
#include "LeoBase/Patterns/non_copyable.h"
#include "LeoBase/Patterns/non_movable.h"

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
class ThreadSafeDeque: public patterns::NonCopyable,
                       public patterns::NonMovable
{
public:

    /**
     * @brief Basic constructor.
     */
    ThreadSafeDeque() :
        size_(0),
        stopReq_(false)
    {}

    /**
     * @brief Attempt to pop front an element.
     * @return An optional containing the popped element if successful, or `std::nullopt` if empty.
     */
    std::optional<T> tryPopFront()
    {
        std::lock_guard<std::mutex> lk(this->mtx_);

        if (this->deque_.empty())
           return std::nullopt;

        T value = std::move(this->deque_.front());
        this->deque_.pop_front();
        --this->size_;

        return value;
    }

    /**
     * @brief Attempt to pop front an element into the provided reference.
     * @param[out] value The popped element.
     * @return `true` if an element was dequeued, `false` if the deque was empty.
     */
    bool tryPopFront(T& value)
    {
        std::lock_guard<std::mutex> lk(this->mtx_);

        if (this->deque_.empty())
           return false;

        value = std::move(this->deque_.front());
        this->deque_.pop_front();
        --this->size_;

        return true;
    }

    /**
     * @brief Attempt to pop back an element.
     * @return An optional containing the popped element if successful, or `std::nullopt` if empty.
     */
    std::optional<T> tryPopBack()
    {
        std::lock_guard<std::mutex> lk(this->mtx_);

        if (this->deque_.empty())
            return std::nullopt;

        T value = std::move(this->deque_.back());
        this->deque_.pop_back();
        --this->size_;

        return value;
    }

    /**
     * @brief Attempt to pop back an element into the provided reference.
     * @param[out] value The popped element.
     * @return `true` if an element was dequeued, `false` if the deque was empty.
     */
    bool tryPopBack(T& value)
    {
        std::lock_guard<std::mutex> lk(this->mtx_);

        if (this->deque_.empty())
            return false;

        value = std::move(this->deque_.back());
        this->deque_.pop_back();
        --this->size_;

        return true;
    }

    /**
     * @brief Blocking method to pop front an element.
     *
     * This method will block until an element becomes available or if `stop()` is called.
     * If the deque is stopped and no element is available, it returns `std::nullopt`.
     *
     * @return The popped element as `std::optional<T>`, or `std::nullopt` if the deque is empty/stopped.
     */
    std::optional<T> waitAndPopFront()
    {
        if (this->stopReq_)
            return std::nullopt;

        std::unique_lock<std::mutex> lk(this->mtx_);

        this->wakeCv_.wait(lk, [this]
        {
            return this->stopReq_.load(std::memory_order_acquire) || !this->deque_.empty();
        });

        if (this->stopReq_.load(std::memory_order_acquire))
            return std::nullopt;

        T value = std::move(deque_.front());
        this->deque_.pop_front();
        --size_;

        return value;
    }
     
    /**
     * @brief Blocking method to pop front an element into a provided reference.
     *
     * This method blocks until an element is available or if `stop()` is called.
     * If the deque is stopped and no element is available, it does nothing.
     *
     * @param[out] value Reference where the popped element will be stored.
     * @return `true` if an element was successfully popped, `false` if stopped and no element exists.
     */
    bool waitAndPopFront(T& value)
    {
        if (this->stopReq_)
            return false;

        std::unique_lock<std::mutex> lk(this->mtx_);

        this->wakeCv_.wait(lk, [this]
        {
            return this->stopReq_.load(std::memory_order_acquire) || !this->deque_.empty();
        });

        if (this->stopReq_.load(std::memory_order_acquire))
            return false;

        value = std::move(this->deque_.front());
        this->deque_.pop_front();
        --this->size_;

        return true;
    }

    /**
     * @brief Blocking method to pop back an element.
     *
     * This method will block until an element becomes available or if `stop()` is called.
     * If the deque is stopped and no element is available, it returns `std::nullopt`.
     *
     * @return The popped element as `std::optional<T>`, or `std::nullopt` if the deque is empty/stopped.
     */
    std::optional<T> waitAndPopBack()
    {
        if (this->stopReq_)
            return std::nullopt;

        std::unique_lock<std::mutex> lk(this->mtx_);

        this->wakeCv_.wait(lk, [this]
        {
            return this->stopReq_.load(std::memory_order_acquire) || !this->deque_.empty();
        });

        if (this->stopReq_.load(std::memory_order_acquire))
            return std::nullopt;

        T value = std::move(deque_.back());
        this->deque_.pop_back();
        --size_;

        return value;
    }

    /**
     * @brief Blocking method to pop back an element into a provided reference.
     *
     * This method blocks until an element is available or if `stop()` is called.
     * If the deque is stopped and no element is available, it does nothing.
     *
     * @param[out] value Reference where the popped element will be stored.
     * @return `true` if an element was successfully popped, `false` if stopped and no element exists.
     */
    bool waitAndPopBack(T& value)
    {
        if (this->stopReq_)
            return false;

        std::unique_lock<std::mutex> lk(this->mtx_);

        this->wakeCv_.wait(lk, [this]
        {
            return this->stopReq_.load(std::memory_order_acquire) || !this->deque_.empty();
        });

        if (this->stopReq_.load(std::memory_order_acquire))
            return false;

        value = std::move(this->deque_.back());
        this->deque_.pop_back();
        --this->size_;

        return true;
    }

    /**
     * @brief Adds an element to the back of the deque if not stopped.
     * @param element The element to add.
     */
    void pushBack(const T& element)
    {
        if (this->stopReq_)
            return;

        {
            std::lock_guard<std::mutex> lk(this->mtx_);

            this->deque_.push_back(element);
            ++this->size_;
        }

        this->wakeCv_.notify_one();
    }

    /**
     * @brief Adds an element to the back of the deque if not stopped using move semantics.
     * @param element The element to add.
     */
    void pushBack(T&& element)
    {
        if (this->stopReq_)
            return;

        {
            std::lock_guard<std::mutex> lk(this->mtx_);

            this->deque_.push_back(std::move(element));
            ++this->size_;
        }

        this->wakeCv_.notify_one();
    }

    /**
     * @brief Adds an element to the front of the deque if not stopped.
     * @param element The element to add.
     */
    void pushFront(const T& element)
    {
        if (this->stopReq_)
            return;

        {
            std::lock_guard<std::mutex> lk(this->mtx_);

            this->deque_.push_front(element);
            ++this->size_;
        }

        this->wakeCv_.notify_one();
    }

    /**
     * @brief Adds an element to the front of the deque if not stopped using move semantics.
     * @param element The element to add.
     */
    void pushFront(T&& element)
    {
        if (this->stopReq_)
            return;

        {
            std::lock_guard<std::mutex> lk(this->mtx_);

            this->deque_.push_front(std::move(element));
            ++this->size_;
        }

        this->wakeCv_.notify_one();
    }

    /**
     * @brief Removes all elements from the deque that match the provided value.
     *
     * This function iterates through the deque and removes all elements that are equal to the given `element`.
     * It maintains thread safety while performing the operation.
     *
     * @param element The value to compare against for removal.
     */
    void removeAll(const T& element)
    {
        std::lock_guard<std::mutex> lk(this->mtx_);

        if (this->deque_.empty())
            return;

        this->deque_.erase(
            std::stable_partition(this->deque_.begin(), this->deque_.end(),
                                  [&element](const T& e) { return e != element; }),
            this->deque_.end());

        this->size_ = this->deque_.size();
    }

    /**
     * @brief Stops the deque, waking all waiting consumers.
     *
     * Once stopped, `waitAndPop()` will return `std::nullopt` if the deque is empty.
     */
    void stop()
    {
        {
            std::lock_guard<std::mutex> lk(this->mtx_);

            this->stopReq_ = true;
            this->deque_.clear();
            this->size_ = 0;
        }

        this->wakeCv_.notify_all();
    }

    /**
     * @brief Checks whether the deque is empty.
     * @return `true` if the deque is empty, `false` otherwise.
     */
    bool empty() const noexcept
    {
        return this->size_ == 0;
    }

    /**
     * @brief Retrieves the current size of the deque.
     * @return The number of elements in the deque.
     */
    size_t size() const noexcept
    {
        return this->size_;
    }

    /**
     * @brief Clears all elements from the deque.
     */
    void clear()
    {
        std::lock_guard<std::mutex> lk(mtx_);

        this->deque_.clear();
        this->size_ = 0;
        this->wakeCv_.notify_all();
    }

    void restart()
    {
        this->stopReq_ = false;
        this->wakeCv_.notify_all();
    }

private:
    std::mutex              mtx_;      ///< Mutex for protecting shared data.
    std::condition_variable wakeCv_;   ///< Condition variable for blocking operations.
    std::deque<T>           deque_;    ///< Internal deque storage.
    std::atomic<size_t>     size_;     ///< Atomic size tracking.
    std::atomic_bool        stopReq_;  ///< Flag to signal stop condition.

};

// ---------------------------------------------------------------------------------------------------------------------

LEOBASE_MODULE_END
