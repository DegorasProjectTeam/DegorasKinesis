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
#include <utility>
#include <typeindex>

// LEOBASE INCLUDES
#include "LeoBase/Aliases/Global"

// LEOBASE MACROS
LEOBASE_MODULE_BEGIN(callbacks)

// ---------------------------------------------------------------------------------------------------------------------

/// Alias for hashed callback identifiers.
using CallbackId = std::size_t;

/// Alias for pair of stored callbacks.
using CallbackPair = std::pair<std::any, std::type_index>;

/// Alias for the callback map.
using CallbackMap = std::unordered_map<CallbackId, CallbackPair>;

// ---------------------------------------------------------------------------------------------------------------------

LEOBASE_MODULE_END
