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
LEOBASE_MODULE_BEGIN(module)

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Interface for value passing in modules.
 *
 * This abstract class serves as a base for value objects
 * that will be used to store and transfer values within modules.
 */
class LEOBASE_EXPORT IValues : public patterns::NonCopyable
{

public:

    /**
     * @brief Default constructor.
     */
    IValues() = default;

    /**
     * @brief Virtual destructor to allow cleanup in derived classes.
     */
    virtual ~IValues() = default;

    /**
     * @brief Creates a shared pointer to a derived type of IValues.
     * @tparam Derived The derived class type that inherits from IValues.
     * @tparam Args Parameter pack for the constructor arguments.
     * @param args Arguments to forward to the constructor of the derived type.
     * @return A shared pointer to the created instance.
     */
    template <typename Derived, typename... Args>
    static std::shared_ptr<Derived> makeShared(Args&&... args)
    {
        static_assert(std::is_base_of<IValues, Derived>::value,
                      "[LeoBase,Module,IValues::makeShared] Derived must inherit from IValues.");
        return std::make_shared<Derived>(std::forward<Args>(args)...);
    }

    /**
     * @brief Creates a unique pointer to a derived type of IValues.
     * @tparam Derived The derived class type that inherits from IValues.
     * @tparam Args Parameter pack for the constructor arguments.
     * @param args Arguments to forward to the constructor of the derived type.
     * @return A unique pointer to the created instance.
     */
    template <typename Derived, typename... Args>
    static std::unique_ptr<Derived> makeUnique(Args&&... args)
    {
        static_assert(std::is_base_of<IValues, Derived>::value,
                      "[LeoBase,Module,IValues::makeUnique] Derived must inherit from IValues.");
        return std::make_unique<Derived>(std::forward<Args>(args)...);
    }

};

/// Shared pointer type alias SS IValues.
using IValuesPtr = std::shared_ptr<IValues>;

/// Unique pointer type alias for IValues.
using IValuesUniquePtr = std::unique_ptr<IValues>;

/// Weak pointer type alias for IValues.
using IValuesEWeakPtr = std::weak_ptr<IValues>;

// ---------------------------------------------------------------------------------------------------------------------

LEOBASE_MODULE_END
