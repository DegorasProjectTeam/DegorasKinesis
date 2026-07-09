/*
 *      Copyright(C) Milethos Technologies SLU. 2025
 *      All rights reserved. Reproduction in whole or in
 *      part is prohibited without the written consent of
 *      the copyright holder.
 */

#pragma once

// C++ INCLUDES
#include <vector>

// LEOBASE INCLUDES
#include "LeoBase/Aliases/Global"

// LEOBASE MACROS
LEOBASE_MODULE_BEGIN(helpers)
BEGIN_NAMESPACE(containers)

// ---------------------------------------------------------------------------------------------------------------------

template <typename Container>
std::vector<typename Container::key_type> getMapKeys(const Container& container)
{
    std::vector<typename Container::key_type> keys;
    for(auto it = container.begin(), end = container.end();
        it != end; it = container.equal_range(it->first).second)
    {
        keys.push_back(it->first);
    }
    return keys;
}

// ---------------------------------------------------------------------------------------------------------------------

END_NAMESPACE(containers)
LEOBASE_MODULE_END

// ---------------------------------------------------------------------------------------------------------------------
