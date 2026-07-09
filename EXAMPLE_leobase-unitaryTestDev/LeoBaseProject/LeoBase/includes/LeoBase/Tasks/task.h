/*
 *      Copyright(C) Milethos Technologies SLU. 2025
 *      All rights reserved. Reproduction in whole or in
 *      part is prohibited without the written consent of
 *      the copyright holder.
 */

#pragma once

// C++ INCLUDES
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>

// LEOBASE INCLUDES
#include "LeoBase/Aliases/Global"
#include "LeoBase/Patterns/non_copyable.h"
#include "LeoBase/Patterns/non_movable.h"

// LEOBASE MACROS
LEOBASE_MODULE_BEGIN(tasks)

// ---------------------------------------------------------------------------------------------------------------------

/**
 *  @brief Base class for creating tasks that run on independent threads.
 *
 *  This class provides a framework for executing user-defined tasks asynchronously
 *  within a dedicated thread. It supports starting, stopping, and joining the execution
 *  while ensuring thread safety.
 *
 *  @note This class is non-copyable and non-movable to prevent unintended ownership issues.
 */
class LEOBASE_EXPORT Task: public patterns::NonCopyable,
                           public patterns::NonMovable
{
public:

    /**
     *  @brief Constructs a new Task instance but does not start execution.
     */
    explicit Task();

    /**
     *  @brief Destroys the Task instance. Stops execution if the task is running.
     */
    virtual ~Task();

    /**
     *  @brief Starts task execution in a new thread.
     *
     *  If a task is already running, this method has no effect.
     */
    void run();

    /**
     *  @brief Stops the task execution.
     *
     *  Signals the task to stop and optionally joins the thread to ensure proper cleanup.
     *
     *  @param join If true, the method blocks until the thread finishes execution.
     */
    void stop(bool join = true);

    /**
     *  @brief Waits for the task to complete execution.
     *
     *  Blocks the calling thread until the task's execution thread has finished.
     */
    void join();

    /**
     *  @brief Implements the user-defined task.
     *
     *  This is a pure virtual function that must be overridden in derived classes
     *  to define the behavior of the task.
     */
    virtual void step() = 0;

    /**
     *  @brief Checks if the task is currently running.
     *
     *  @return True if the task is active, false otherwise.
     */
    bool isRunning() const;

protected:

    std::mutex mutex_;                    ///< Mutex for synchronizing access to shared resources.
    std::thread thread_; ///< Thread executing the task.
    std::atomic<bool> isRunning_;         ///< Flag indicating whether the task is running.
    std::atomic<bool> stopThread_;        ///< Flag indicating a stop request for the task.
    std::condition_variable condVar_;     ///< Condition variable for signaling events within the task.
};

// ---------------------------------------------------------------------------------------------------------------------

LEOBASE_MODULE_END
