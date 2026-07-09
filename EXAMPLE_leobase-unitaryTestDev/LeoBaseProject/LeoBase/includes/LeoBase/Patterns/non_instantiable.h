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
 * @brief A utility class to prevent instantiation while allowing inheritance.
 *
 * This class serves as a base class to make derived classes non-instantiable.
 */
class LEOBASE_EXPORT NonInstantiable
{
protected:

    /**
     * @brief Deleted constructor.
     *
     * Prevents direct instantiation of the base class.
     */
    NonInstantiable() = delete;

    /**
     * @brief Default destructor.
     *
     * Ensures derived classes can be destructed properly.
     */
    ~NonInstantiable() = default;

    /**
     * @brief Default move constructor.
     * @param other Another NonInstantiable instance to move from.
     */
    NonInstantiable(NonInstantiable&& other) noexcept = default;

    /**
     * @brief Default move assignment operator.
     * @param other Another NonInstantiable instance to move from.
     * @return Reference to this object.
     */
    NonInstantiable& operator=(NonInstantiable&& other) noexcept = default;

public:

    /**
     * @brief Deleted copy constructor.
     *
     * Prevents copying of instances of derived classes.
     */
    NonInstantiable(const NonInstantiable&) = delete;

    /**
     * @brief Deleted copy assignment operator.
     *
     * Prevents assignment of instances of derived classes.
     */
    NonInstantiable& operator=(const NonInstantiable&) = delete;
};

// ---------------------------------------------------------------------------------------------------------------------

LEOBASE_MODULE_END
