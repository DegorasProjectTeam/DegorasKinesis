/*
 *      Copyright(C) Milethos Technologies SLU. 2025
 *      All rights reserved. Reproduction in whole or in
 *      part is prohibited without the written consent of
 *      the copyright holder.
 */

#pragma once

// C++ INCLUDES
#include <any>
#include <string>

// LEOBASE INCLUDES
#include "LeoBase/Aliases/Global"

// LEOBASE MACROS
LEOBASE_MODULE_BEGIN(containers)

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Template equality comparator for std::any that compares stored values of a given type.
 * @tparam T The type stored inside `std::any`.
 */
template <typename T>
struct LEOBASE_EXPORT AnyTypeEqual
{
    /**
     * @brief Compares two `std::any` objects for equality.
     * @param a The first `std::any` object.
     * @param b The second `std::any` object.
     * @return `true` if both objects store the same type `T` and their values are equal, otherwise `false`.
     * @throws std::bad_any_cast if `a` or `b` does not contain type `T`.
     */
    bool operator()(const std::any& a, const std::any& b) const
    {
        if constexpr (std::is_same_v<T, const char*>)
            return std::string(std::any_cast<const char*>(a)) == std::string(std::any_cast<const char*>(b));
        else
            return std::any_cast<T>(a) == std::any_cast<T>(b);
    }
};

/**
 * @brief Custom equality comparator for std::any that checks both type and value.
 *
 * This struct ensures that two `std::any` objects are considered equal if:
 * 1. They have the same type.
 * 2. Their stored values are equal.
 *
 * Floating-point types are **not supported** due to precision inconsistencies that
 * may lead to incorrect equality results.
 *
 * @warning Floating-point types (`float`, `double`, `long double`) are NOT
 * supported for equality comparison due to their inherent precision issues.
 */
struct LEOBASE_EXPORT AnyEqual
{
    /**
     * @brief Compares two `std::any` objects for equality.
     * @param a The first `std::any` object.
     * @param b The second `std::any` object.
     * @return `true` if both `a` and `b` have the same type and value, otherwise `false`.
     * @throws std::runtime_error if the type is not supported.
     */
    bool operator()(const std::any& a, const std::any& b) const;
};

// ---------------------------------------------------------------------------------------------------------------------

LEOBASE_MODULE_END
