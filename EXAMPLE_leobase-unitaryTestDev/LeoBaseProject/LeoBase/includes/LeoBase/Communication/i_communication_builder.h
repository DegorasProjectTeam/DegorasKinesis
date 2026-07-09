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
#include "LeoBase/Containers/any_unordered_map.h"

// LEOBASE MACROS
LEOBASE_MODULE_BEGIN(communication)

// ---------------------------------------------------------------------------------------------------------------------

class LEOBASE_EXPORT ICommunicationBuilder : public patterns::NonCopyable
{

public:

    virtual ~ICommunicationBuilder() = default;

    virtual bool start() = 0;

    virtual bool stop() = 0;

    virtual bool produceAll() = 0;

    virtual void waitServer() = 0;

    virtual void configureCallbacks(const std::any& moduleId, const containers::AnyUnorderedMap& actionCallbacks) = 0;

    /**
     * @brief Creates a shared pointer to a derived type of ICommunicationBuilder.
     * @tparam Derived The derived class type that inherits from ICommunicationBuilder.
     * @tparam Args Parameter pack for the constructor arguments.
     * @param args Arguments to forward to the constructor of the derived type.
     * @return A shared pointer to the created instance.
     */
    template <typename Derived, typename... Args>
    static std::shared_ptr<Derived> makeShared(Args&&... args)
    {
        static_assert(std::is_base_of<ICommunicationBuilder, Derived>::value,
                      "[LeoBase,Module,ICommunicationBuilder::makeShared] "
                      "Derived must inherit from ICommunicationBuilder.");
        return std::make_shared<Derived>(std::forward<Args>(args)...);
    }

    /**
     * @brief Creates a unique pointer to a derived type of ICommunicationBuilder.
     * @tparam Derived The derived class type that inherits from ICommunicationBuilder.
     * @tparam Args Parameter pack for the constructor arguments.
     * @param args Arguments to forward to the constructor of the derived type.
     * @return A unique pointer to the created instance.
     */
    template <typename Derived, typename... Args>
    static std::unique_ptr<Derived> makeUnique(Args&&... args)
    {
        static_assert(std::is_base_of<ICommunicationBuilder, Derived>::value,
                      "[LeoBase,Module,ICommunicationBuilder::makeUnique] "
                      "Derived must inherit from ICommunicationBuilder.");
        return std::make_unique<Derived>(std::forward<Args>(args)...);
    }

};

/// Shared pointer type alias for IParams.
using ICommunicationBuilderPtr = std::shared_ptr<ICommunicationBuilder>;

/// Unique pointer type alias for ICommunicationBuilder.
using ICommunicationBuilderUniquePtr = std::unique_ptr<ICommunicationBuilder>;

/// Weak pointer type alias for ICommunicationBuilder.
using ICommunicationBuilderWeakPtr = std::weak_ptr<ICommunicationBuilder>;

// ---------------------------------------------------------------------------------------------------------------------

LEOBASE_MODULE_END
