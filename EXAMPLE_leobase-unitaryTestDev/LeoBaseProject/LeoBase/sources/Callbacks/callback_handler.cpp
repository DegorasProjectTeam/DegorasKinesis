/*
 *      Copyright(C) Milethos Technologies SLU. 2025
 *      All rights reserved. Reproduction in whole or in
 *      part is prohibited without the written consent of
 *      the copyright holder.
 */

// LEOBASE INCLUDES
#include "LeoBase/Callbacks/callback_handler.h"

// LEOBASE MACROS
LEOBASE_MODULE_BEGIN(callbacks)

// ---------------------------------------------------------------------------------------------------------------------

void CallbackHandler::clearCallbacks()
{
    std::lock_guard<std::mutex> lock(this->mtx_);
    this->callbackMap_.clear();
}

// ---------------------------------------------------------------------------------------------------------------------

LEOBASE_MODULE_END
