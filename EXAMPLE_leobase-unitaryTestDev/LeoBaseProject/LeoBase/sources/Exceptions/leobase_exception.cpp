/*
 *      Copyright(C) Milethos Technologies SLU. 2025
 *      All rights reserved. Reproduction in whole or in
 *      part is prohibited without the written consent of
 *      the copyright holder.
 */

// LEOBASE INCLUDES
#include "LeoBase/Exceptions/leobase_exception.h"

// LEOBASE MACROS

LEOBASE_MODULE_BEGIN(exceptions)

// ---------------------------------------------------------------------------------------------------------------------

LeoBaseException::LeoBaseException(const std::string &module, const std::string &scope, const std::string &msg)
    : BaseException("LeoBase", module, scope, msg)
{}

// ---------------------------------------------------------------------------------------------------------------------

LEOBASE_MODULE_END

