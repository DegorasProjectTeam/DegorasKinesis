/*
 *      Copyright(C) Milethos Technologies SLU. 2025
 *      All rights reserved. Reproduction in whole or in
 *      part is prohibited without the written consent of
 *      the copyright holder.
 */

// LEOBASE INCLUDES
#include "LeoBase/Global/global_definitions.h"
#include "LeoBase/Patterns/cancellation_token.h"
#include "LeoBase/Exceptions/canceled_exception.h"

// LEOBASE MACROS
LEOBASE_MODULE_BEGIN(patterns)

// ---------------------------------------------------------------------------------------------------------------------

CancellationToken::CancellationToken() :
    cancelled_(false)
{}

void CancellationToken::cancel()
{
    this->cancelled_ = true;
}

bool CancellationToken::isCanceled() const
{
    return this->cancelled_;
}

void CancellationToken::throwIfCanceled() const
{
    if (this->cancelled_)
        throw exceptions::CanceledException(kProjN,kModN,
            "CancellationToken::throwIfCanceled");
}

// ---------------------------------------------------------------------------------------------------------------------

LEOBASE_MODULE_END
