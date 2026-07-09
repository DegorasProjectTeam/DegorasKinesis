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

/**
 * @brief Exception class for canceled operations.
 *
 * This exception represents an operation that was
 * explicitly canceled before its completion.
 */
class LEOBASE_EXPORT CanceledException : public exceptions::BaseException
{

public:

    explicit CanceledException(const std::string& project,
                               const std::string& module,
                               const std::string& scope);

    /**
     * @brief Retrieves the exception message.
     * @return A C-string representing the error message.
     */
    const char* what() const noexcept override;
};

// ---------------------------------------------------------------------------------------------------------------------

LEOBASE_MODULE_END
