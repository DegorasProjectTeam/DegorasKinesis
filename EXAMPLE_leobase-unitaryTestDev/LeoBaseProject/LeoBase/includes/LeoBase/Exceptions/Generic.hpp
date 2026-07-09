/*
 *      Copyright(C) Milethos Technologies SLU. 2019
 *      All rights reserved. Reproduction in whole or in
 *      part is prohibited without the written consent of
 *      the copyright holder.
 *
 *      Author:
 */

#ifndef __LEOBASE_ERROR_HANDLING_GENERIC_HPP__
#define __LEOBASE_ERROR_HANDLING_GENERIC_HPP__ 1

#include <stdexcept>
#include <string>
#include <sstream>
#include <limits>
#include <vector>

namespace LeoBase
{

    class NotImplementedException : public std::runtime_error
    {
    public:

        NotImplementedException()
            : std::runtime_error("Not implemented")
        {
        }

        explicit NotImplementedException(std::string message)
            : std::runtime_error(buildErrorMessage(std::move(message)))
        {
        };

    private:
        std::string buildErrorMessage(std::string message)
        {
            return "Not implemented: " + message;
        }
    };

    class UninitializedException : public std::runtime_error
    {
    public:
        UninitializedException()
            : std::runtime_error("Not initialized")
        {
        }

        explicit UninitializedException(std::string message)
            : std::runtime_error(buildErrorMessage(std::move(message)))
        {
        };

    private:
        std::string buildErrorMessage(std::string message)
        {
            return "Not initialized: " + message;
        }
    };

}

#endif

