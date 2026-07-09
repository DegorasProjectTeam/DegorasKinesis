/*
 *      Copyright(C) Milethos Technologies SLU. 2025
 *      All rights reserved. Reproduction in whole or in
 *      part is prohibited without the written consent of
 *      the copyright holder.
 */

// C++ INCLUDES
#include <sstream>

// LEOBASE INCLUDES
#include "LeoBase/Exceptions/base_exception.h"

// LEOBASE MACROS
LEOBASE_MODULE_BEGIN(exceptions)

// ---------------------------------------------------------------------------------------------------------------------

BaseException::BaseException(const std::string &project, const std::string &module, const std::string &scope, const std::string &msg)
{
    std::ostringstream oss;
    oss << "[" << project << "," << module << "," << scope << "] " << msg;
    this->fullMessage_ = oss.str();
}

const char *BaseException::what() const noexcept
{
    return this->fullMessage_.c_str();
}



// ---------------------------------------------------------------------------------------------------------------------

LEOBASE_MODULE_END
