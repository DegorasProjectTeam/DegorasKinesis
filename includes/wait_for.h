/*
 *  Copyright (C) 2018-2026 Degoras Project Team
 *
 *  This file is part of a small-scale research or utility tool built atop
 *  the Degoras Project infrastructure, released under the MIT License.
 *
 *  SPDX-License-Identifier: MIT
 *
 *  See the LICENSE file in the root directory for full license details.
 */

#pragma once

// C++ INCLUDES
#include <chrono>
#include <thread>

// PROJECT INCLUDES
#include "common_types.h"


// NAMESPACES
namespace thorlabs
{

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Poll a predicate until it becomes true or a timeout elapses.
 * @tparam Pred Callable returning something convertible to bool; true means the condition is met.
 * @param pred The condition to test. Evaluated immediately, then once per @p interval.
 * @param timeout Maximum time to wait for @p pred to become true.
 * @param interval Delay between successive evaluations of @p pred.
 * @return OperationResult::OPERATION_OK if @p pred became true within @p timeout, otherwise
 *         OperationResult::OPERATION_TIMEOUT.
 * @note Generic deadline-poll helper that replaces the per-device, copy-pasted waitForHoming* loops. It uses a
 *       steady clock, so it is unaffected by wall-clock adjustments. It does not throw.
 */
template <class Pred>
types::OperationResult waitForCondition(Pred pred,
                                        std::chrono::milliseconds timeout,
                                        std::chrono::milliseconds interval = std::chrono::milliseconds(50))
{
    const std::chrono::steady_clock::time_point deadline = std::chrono::steady_clock::now() + timeout;
    while (!pred())
    {
        if (std::chrono::steady_clock::now() >= deadline)
            return types::OperationResult::OPERATION_TIMEOUT;
        std::this_thread::sleep_for(interval);
    }
    return types::OperationResult::OPERATION_OK;
}

// ---------------------------------------------------------------------------------------------------------------------

} // END NAMESPACES

// ---------------------------------------------------------------------------------------------------------------------
