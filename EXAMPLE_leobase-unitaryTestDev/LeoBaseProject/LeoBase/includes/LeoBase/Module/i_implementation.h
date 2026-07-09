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
 * @brief Interface for implementation classes.
 *
 * This class serves as a base interface for module implementations.
 * Derived classes should provide specific implementation logic.
 */
class LEOBASE_EXPORT IImplementation : public patterns::NonCopyable
{

public:

    /**
     * @brief Default constructor.
     */
    IImplementation() = default;

    /**
     * @brief Virtual destructor to allow proper cleanup in derived classes.
     */
    virtual ~IImplementation() = default;

    /**
     * @brief Creates a shared pointer to a derived type of IImplementation.
     * @tparam Derived The derived class type that inherits from IImplementation.
     * @tparam Args Parameter pack for the constructor arguments.
     * @param args Arguments to forward to the constructor of the derived type.
     * @return A shared pointer to the created instance.
     */
    template <typename Derived, typename... Args>
    static std::shared_ptr<Derived> makeShared(Args&&... args)
    {
        static_assert(std::is_base_of<IImplementation, Derived>::value,
                      "[LeoBase,Module,IImplementation::makeShared] Derived must inherit from IImplementation.");
        return std::make_shared<Derived>(std::forward<Args>(args)...);
    }

    /**
     * @brief Creates a unique pointer to a derived type of IImplementation.
     * @tparam Derived The derived class type that inherits from IImplementation.
     * @tparam Args Parameter pack for the constructor arguments.
     * @param args Arguments to forward to the constructor of the derived type.
     * @return A unique pointer to the created instance.
     */
    template <typename Derived, typename... Args>
    static std::unique_ptr<Derived> makeUnique(Args&&... args)
    {
        static_assert(std::is_base_of<IImplementation, Derived>::value,
                      "[LeoBase,Module,IImplementation::makeUnique] Derived must inherit from IImplementation.");
        return std::make_unique<Derived>(std::forward<Args>(args)...);
    }

};

/// Shared pointer type alias for IImplementation.
using IImplementationPtr = std::shared_ptr<IImplementation>;

/// Unique pointer type alias for IImplementation.
using IImplementationUniquePtr = std::unique_ptr<IImplementation>;

/// Weak pointer type alias for IImplementation.
using IImplementationWeakPtr = std::weak_ptr<IImplementation>;

// ---------------------------------------------------------------------------------------------------------------------

LEOBASE_MODULE_END
