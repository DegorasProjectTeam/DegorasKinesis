/*
 *      Copyright(C) Milethos Technologies SLU. 2025
 *      All rights reserved. Reproduction in whole or in
 *      part is prohibited without the written consent of
 *      the copyright holder.
 */

#pragma once

// C++ INCLUDES
#include <memory>

// LEOBASE INCLUDES
#include "LeoBase/Aliases/Global"
#include "LeoBase/Patterns/non_copyable.h"

// LEOBASE MACROS
LEOBASE_MODULE_BEGIN(messaging)

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief The base class for event objects within the module.
 *
 * This interface serves as the foundation for all event-driven
 * operations in the system. Derived classes should implement
 * specific event behaviors.
 */
class LEOBASE_EXPORT IEvent : public patterns::NonCopyable
{
public:

    /**
     * @brief Default constructor.
     */
    IEvent() = default;

    /**
     * @brief Virtual destructor to ensure proper cleanup of derived classes.
     */
    virtual ~IEvent() = default;
};

/// Shared pointer type alias for IEventPtr.
using IEventPtr = std::shared_ptr<IEvent>;

// ---------------------------------------------------------------------------------------------------------------------

LEOBASE_MODULE_END
