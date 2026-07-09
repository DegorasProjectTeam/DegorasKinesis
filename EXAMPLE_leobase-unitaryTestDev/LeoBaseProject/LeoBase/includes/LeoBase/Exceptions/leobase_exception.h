/*
 *      Copyright(C) Milethos Technologies SLU. 2025
 *      All rights reserved. Reproduction in whole or in
 *      part is prohibited without the written consent of
 *      the copyright holder.
 */

#pragma once

// C++ INCLUDES
#include <string>

// LEOBASE INCLUDES
#include "LeoBase/Aliases/Global"
#include "LeoBase/Exceptions/base_exception.h"

// LEOBASE MACROS
LEOBASE_MODULE_BEGIN(exceptions)

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Specialized exception for LeoBase.
 *
 * This exception automatically sets the project name as "LeoBase" for all exceptions
 * occurring within the LeoBase library.
 */
class LEOBASE_EXPORT LeoBaseException : public BaseException
{

public:

    /**
     * @brief Constructs a LeoBase-specific exception.
     * @param module The name of the module where the error occurred.
     * @param scope The specific scope or related class where the error occurred.
     * @param msg The actual exception message.
     */
    explicit LeoBaseException(const std::string& module,
                              const std::string& scope,
                              const std::string& msg);

    // TODO operador ==
};

// ---------------------------------------------------------------------------------------------------------------------

LEOBASE_MODULE_END
