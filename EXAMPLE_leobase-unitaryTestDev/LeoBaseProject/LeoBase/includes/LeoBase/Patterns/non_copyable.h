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
 * @brief A utility class to disable copy operations for derived classes.
 *
 * This class is intended to be used as a base class to prevent copying
 * of derived objects by deleting the copy constructor and copy assignment operator.
 */
class LEOBASE_EXPORT NonCopyable
{
protected:

    /**
     * @brief Default constructor.
     *
     * The constructor is protected to allow derived classes to construct
     * but prevent instantiation of NonCopyable itself.
     */
    NonCopyable() = default;

    /**
     * @brief Default destructor.
     *
     * The destructor is protected to ensure only derived classes can destruct
     * but prevent deletion through base class pointers.
     */
    ~NonCopyable() = default;

    /**
     * @brief Move constructor.
     * @param other Another NonCopyable instance to move from.
     */
    NonCopyable(NonCopyable&& other) noexcept = default;

    /**
     * @brief Move assignment operator.
     * @param other Another NonCopyable instance to move from.
     * @return Reference to this object.
     */
    NonCopyable& operator=(NonCopyable&& other) noexcept = default;

public:

    /**
     * @brief Deleted copy constructor.
     *
     * Prevents copying of instances of derived classes.
     *
     * @param other Another instance of NonCopyable (not allowed).
     */
    NonCopyable(const NonCopyable&) = delete;

    /**
     * @brief Deleted copy assignment operator.
     *
     * Prevents assignment of instances of derived classes.
     *
     * @param other Another instance of NonCopyable (not allowed).
     * @return NonCopyable& Reference to the instance (not allowed).
     */
    NonCopyable& operator=(const NonCopyable&) = delete;
};

// ---------------------------------------------------------------------------------------------------------------------

LEOBASE_MODULE_END
