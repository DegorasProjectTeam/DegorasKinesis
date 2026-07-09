/*
 *      Copyright(C) Milethos Technologies SLU. 2025
 *      All rights reserved. Reproduction in whole or in
 *      part is prohibited without the written consent of
 *      the copyright holder.
 */

#pragma once

// C++ INCLUDES
#include <any>
#include <functional>
#include <string>
#include <type_traits>
#include <cstring>

// LEOBASE INCLUDES
#include "LeoBase/Aliases/Global"

// LEOBASE MACROS
LEOBASE_MODULE_BEGIN(containers)

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Template hasher for std::any that hashes the stored value of a given type.
 * @tparam T The type stored inside `std::any`.
 */
template <typename T>
struct AnyTypeHasher
{
    /**
     * @brief Computes the hash of a given `std::any` object.
     * @param key The `std::any` object to be hashed.
     * @return A `std::size_t` hash value.
     * @throws std::bad_any_cast if `key` does not contain type `T`.
     */
    std::size_t operator()(const std::any& key) const
    {
        if constexpr (std::is_same_v<T, const char*>)
            return std::hash<std::string>{}(std::string(std::any_cast<const char*>(key)));
        return std::hash<T>{}(std::any_cast<T>(key));
    }
};

/**
 * @brief Custom hasher for std::any that hashes both type and value.
 *
 * This class provides a hashing mechanism for `std::any` objects, ensuring
 * that both the type and value contribute to the hash. It supports various
 * integer types and `std::string`, but does NOT support floating-point types
 * due to precision inconsistencies that may lead to unreliable behavior.
 *
 * @warning Floating-point types (`float`, `double`, `long double`) are NOT
 * supported as keys in unordered maps due to their precision uncertainty,
 * which can cause incorrect hashing and unpredictable lookups.
 */
struct LEOBASE_EXPORT AnyHasher
{
    /**
     * @brief Computes the hash of a given `std::any` key.
     * @param key The `std::any` object to be hashed.
     * @return A `std::size_t` representing the hash value.
     * @throws std::runtime_error if the type is not supported.
     */
    std::size_t operator()(const std::any& key) const;
};

// ---------------------------------------------------------------------------------------------------------------------

LEOBASE_MODULE_END
