/*
 *      Copyright(C) Milethos Technologies SLU. 2025
 *      All rights reserved. Reproduction in whole or in
 *      part is prohibited without the written consent of
 *      the copyright holder.
 */

#pragma once

// C++ INCLUDES
#include <atomic>

// LEOBASE INCLUDES
#include "LeoBase/Aliases/Global"

// LEOBASE MACROS
LEOBASE_MODULE_BEGIN(patterns)

// ---------------------------------------------------------------------------------------------------------------------

class LEOBASE_EXPORT CancellationToken
{
public:

    CancellationToken();

    void cancel();

    bool isCanceled() const;

    void throwIfCanceled() const;

private:

    std::atomic_bool cancelled_;
};

// ---------------------------------------------------------------------------------------------------------------------

LEOBASE_MODULE_END
