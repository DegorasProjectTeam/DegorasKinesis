/*
 *      Copyright(C) Milethos Technologies SLU. 2025
 *      All rights reserved. Reproduction in whole or in
 *      part is prohibited without the written consent of
 *      the copyright holder.
 */

#pragma once

// C++ INCLUDES
#include <exception>
#include <string>

// LEOBASE INCLUDES
#include "LeoBase/Aliases/Global"

// LEOBASE MACROS
LEOBASE_MODULE_BEGIN(exceptions)

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Base exception class for all exceptions in LeoBase and other projects.
 *
 * This class serves as the foundation for hierarchical exception handling,
 * enforcing a structured message format: `[Project,Module,Scope] exception_msg`
 */
class LEOBASE_EXPORT BaseException : public std::exception
{

public:

    /**
     * @brief Constructs an exception with structured context.
     * @param project The name of the project.
     * @param module The name of the module where the error occurred.
     * @param scope The specific scope or related class where the error occurred.
     * @param msg The actual exception message.
     */
    explicit BaseException(const std::string& project,
                           const std::string& module,
                           const std::string& scope,
                           const std::string& msg);

    /**
     * @brief Retrieves the formatted exception message.
     * @return The detailed exception message.
     */
    const char* what() const noexcept override;

protected:

    std::string fullMessage_;
};

// ---------------------------------------------------------------------------------------------------------------------

LEOBASE_MODULE_END
