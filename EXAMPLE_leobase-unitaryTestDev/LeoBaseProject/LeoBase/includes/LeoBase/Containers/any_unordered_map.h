/*
 *      Copyright(C) Milethos Technologies SLU. 2025
 *      All rights reserved. Reproduction in whole or in
 *      part is prohibited without the written consent of
 *      the copyright holder.
 */

#pragma once

// C++ INCLUDES
#include <unordered_map>
#include <any>
#include <optional>

// LEOBASE INCLUDES
#include "LeoBase/Aliases/Global"
#include "LeoBase/Containers/any_hasher.h"
#include "LeoBase/Containers/any_equal.h"
#include "LeoBase/Exceptions/leobase_exception.h"

// LEOBASE MACROS
LEOBASE_MODULE_BEGIN(containers)

// ---------------------------------------------------------------------------------------------------------------------

using AnyUnorderedMap = std::unordered_map<std::any, std::any, AnyHasher, AnyEqual>;

/**
 * @brief Utility function to retrieve a value from `AnyUnorderedMap`.
 * @tparam T The expected type of the value.
 * @param map The `AnyUnorderedMap` instance.
 * @param key The key whose value needs to be retrieved.
 * @return The casted value if it exists and is of the correct type.
 * @throws std::runtime_error if the key does not exist or type mismatch occurs.
 */
template <typename T>
T getValue(const AnyUnorderedMap& map, const std::any& key)
{
    auto it = map.find(key);
    if (it == map.end())
        throw exceptions::LeoBaseException(
            kModN, "getValue", "Key not found in AnyUnorderedMap.");
    try
    {
        return std::any_cast<T>(it->second);
    }
    catch (const std::bad_any_cast&)
    {
        throw exceptions::LeoBaseException(
            kModN, "getValue", "Type mismatch when retrieving value.");
    }
}

/**
 * @brief Utility function to safely retrieve a value from `AnyUnorderedMap` without exceptions.
 * @tparam T The expected type of the value.
 * @param map The `AnyUnorderedMap` instance.
 * @param key The key whose value needs to be retrieved.
 * @return `std::optional<T>` containing the value if found and correctly cast, otherwise `std::nullopt`.
 */
template <typename T>
std::optional<T> tryGetValue(const AnyUnorderedMap& map, const std::any& key)
{
    auto it = map.find(key);
    if (it == map.end())
        return std::nullopt;
    try
    {
        return std::any_cast<T>(it->second);
    }
    catch (const std::bad_any_cast&)
    {
        return std::nullopt;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

LEOBASE_MODULE_END
