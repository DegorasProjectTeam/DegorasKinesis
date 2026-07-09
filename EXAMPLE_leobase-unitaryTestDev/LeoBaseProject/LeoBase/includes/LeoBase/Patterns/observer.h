/*
 *      Copyright(C) Milethos Technologies SLU. 2025
 *      All rights reserved. Reproduction in whole or in
 *      part is prohibited without the written consent of
 *      the copyright holder.
 */

#pragma once

// LEOBASE INCLUDES
#include "LeoBase/Aliases/Global"

// LEOBASE MACROS
LEOBASE_MODULE_BEGIN(patterns)

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Template class that defines an observer interface.
 *
 * The Observer pattern allows objects (observers) to receive updates from
 * a subject they are subscribed to when the subject's state changes.
 * It is commonly used in event-driven architectures and systems requiring
 * notification mechanisms.
 *
 * @tparam T Type of the data to be observed.
 */
template <class T>
class Observer
{
public:

    /**
     * @brief Virtual destructor to allow proper cleanup of derived classes.
     *
     * Ensures that derived class destructors are invoked correctly when deleting
     * an observer through a base class pointer.
     */
    virtual ~Observer() = default;

    /**
     * @brief Receives updates from the observed subject by reference.
     *
     * This method is called by the subject when new data is available.
     * The data is passed by constant reference to avoid unnecessary copying.
     *
     * @param to_update A constant reference to the updated data of type `T`.
     * @return True if the update was successfully processed, false otherwise.
     */
    virtual bool update(const T& to_update) noexcept = 0;
};

// ---------------------------------------------------------------------------------------------------------------------

LEOBASE_MODULE_END
