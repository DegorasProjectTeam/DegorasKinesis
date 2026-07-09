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
 * @brief A utility class to disable both copy and move operations for derived classes.
 *
 * This class is intended to be used as a base class to prevent copying
 * and moving of derived objects by deleting the copy constructor, copy assignment,
 * move constructor, and move assignment operator.
 */
class LEOBASE_EXPORT NonMovable
{
protected:

    /**
     * @brief Default constructor.
     *
     * The constructor is protected to allow derived classes to construct
     * but prevent instantiation of NonMovable itself.
     */
    NonMovable() = default;

    /**
     * @brief Default destructor.
     *
     * The destructor is protected to ensure only derived classes can destruct
     * but prevent deletion through base class pointers.
     */
    ~NonMovable() = default;

public:

    /**
     * @brief Deleted copy constructor.
     *
     * Prevents copying of instances of derived classes.
     *
     * @param other Another instance of NonMovable (not allowed).
     */
    NonMovable(const NonMovable&) = delete;

    /**
     * @brief Deleted copy assignment operator.
     *
     * Prevents assignment of instances of derived classes.
     *
     * @param other Another instance of NonMovable (not allowed).
     * @return NonMovable& Reference to the instance (not allowed).
     */
    NonMovable& operator=(const NonMovable&) = delete;

    /**
     * @brief Deleted move constructor.
     *
     * Prevents moving of instances of derived classes.
     *
     * @param other Another instance of NonMovable (not allowed).
     */
    NonMovable(NonMovable&&) = delete;

    /**
     * @brief Deleted move assignment operator.
     *
     * Prevents move assignment of instances of derived classes.
     *
     * @param other Another instance of NonMovable (not allowed).
     * @return NonMovable& Reference to the instance (not allowed).
     */
    NonMovable& operator=(NonMovable&&) = delete;
};

// ---------------------------------------------------------------------------------------------------------------------

LEOBASE_MODULE_END
