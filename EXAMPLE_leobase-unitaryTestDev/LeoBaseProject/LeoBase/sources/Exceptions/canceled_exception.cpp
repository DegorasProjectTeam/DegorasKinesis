/*
 *      Copyright(C) Milethos Technologies SLU. 2025
 *      All rights reserved. Reproduction in whole or in
 *      part is prohibited without the written consent of
 *      the copyright holder.
 */

#include "LeoBase/Exceptions/canceled_exception.h"

// LEOBASE MACROS
LEOBASE_MODULE_BEGIN(exceptions)

CanceledException::CanceledException(const std::string &project,
                                     const std::string &module,
                                     const std::string &scope) :
    BaseException(project, module, scope,
                  "Action was canceled.")
{}

const char* CanceledException::what() const noexcept
{
        return BaseException::what();
}

// ---------------------------------------------------------------------------------------------------------------------

LEOBASE_MODULE_END

