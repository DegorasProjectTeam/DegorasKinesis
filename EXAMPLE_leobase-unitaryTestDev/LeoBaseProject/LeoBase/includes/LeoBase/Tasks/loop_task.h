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
#include <functional>

// LEOBASE INCLUDES
#include "LeoBase/Aliases/Global"
#include "LeoBase/Patterns/non_copyable.h"
#include "LeoBase/Patterns/non_movable.h"

// LEOBASE MACROS
LEOBASE_MODULE_BEGIN(tasks)

// ---------------------------------------------------------------------------------------------------------------------


// TODO IMPORTANTE:
// SI EL TIEMPO DE EJECUCION > PERIODO EMPEZARÁ A IR A DESTIEMPO. LAUNCH DEBERÍA PODER REALIZAR LAS LLAMADAS
// A STEP DE MANERA ASÍNCRONA. EN CASO DE SER SÍNCRONO DEBERÍA LANZARSE EXCEPCIÓN SI LAS TAREAS ENCOMENDADAS
// TARDAN MÁS QUE EL PERIODO, O AL MENOS PERMITIR QUE EL COMPORTAMIENTO SEA CONFIGURABLE (SALTARSE EL SIGUIENTE
// STEP, LANZARLO A CONTINUACION, LANZAR EXCEPCION, ETC. TAL VEZ LA SOLUCION ES LLAMAR SIEMPRE EN ASYNC Y SI SE PRODUCE ENCOLAMIENTO O LLAMADAS MULTIPLES
// SEA PROBLEMA DEL USUARIO POR NO HABERLO COMPROBADO.

/// Alias for the predicate of a loop task.
using LoopTaskPredicate = std::function<bool(void)>;

/**
 *  @brief A base class for creating periodic tasks that execute in a loop.
 *
 *  This class runs an independent thread executing a user-defined `step()`
 *  function at a fixed interval.
 *
 *  @note This class supports starting and stopping execution.
 */
class LEOBASE_EXPORT LoopTask : public patterns::NonCopyable,
                                public patterns::NonMovable
{
public:

    /**
     *  @brief Constructs a LoopTask instance.
     *  @param interval_msecs Time between iterations in milliseconds.
     */
    explicit LoopTask(long long interval_msecs);

    /**
     *  @brief Destroys the LoopTask instance.
     *
     *  Ensures proper cleanup by stopping execution if it is running.
     */
    virtual ~LoopTask();

    /**
     *  @brief Sets a new cycle time for the loop task.
     *  @param interval_msecs The new interval in milliseconds.
     */
    void setPeriod(long long interval_msecs);

    /**
     *  @brief Gets the current loop period.
     *  @return The period in milliseconds.
     */
    long long getPeriod() const;

    /**
     *  @brief Starts task execution in a new thread.
     *
     *  If the task is already running, this function does nothing.
     */
    void run();

    /**
     *  @brief Stop and start the loop thread.
     */
    virtual void resetTime();

    /**
     *  @brief Stops the task execution.
     *
     *  Signals the task to stop and optionally joins the thread to ensure proper cleanup.
     *
     *  @param join If true, the method blocks until the thread finishes execution.
     */
    virtual void stop(bool join = true);

    /**
     *  @brief Waits for the task to complete execution.
     *
     *  Blocks the calling thread until the task's execution thread has finished.
     */
    virtual void join();

    /**
     *  @brief Implements the user-defined task.
     *
     *  This is a pure virtual function that must be overridden in derived classes
     *  to define the behavior of the task.
     */
    virtual void step() = 0;

    //! \brief This method pauses the thread excution
    void pause();

    //! \brief This method restarts the thread execution
    void play();

    /**
     *  @brief Checks if the task is currently running.
     *
     *  @return True if the task is active, false otherwise.
     */
    bool isRunning() const;

protected:

    // Internal loop function for running the task periodically.
    void loop();

    std::mutex mutex_;                      ///< Mutex for thread safety.
    std::condition_variable condVar_;       ///< Condition variable to manage execution.
    std::atomic<bool> isRunning_;           ///< Flag to indicate running state.
    std::atomic<bool> stopThread_;          ///< Flag to request stop.
    std::atomic<bool> isPaused_;            ///< Flag to indicate pause state.
    std::chrono::milliseconds period_;      ///< Execution period.
    std::shared_ptr<std::thread> thread_;   ///< Thread for execution.
};

// ---------------------------------------------------------------------------------------------------------------------

LEOBASE_MODULE_END
