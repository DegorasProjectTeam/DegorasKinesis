/*
 *      Copyright(C) Milethos Technologies SLU. 2025
 *      All rights reserved. Reproduction in whole or in
 *      part is prohibited without the written consent of
 *      the copyright holder.
 */

// C++ INCLUDES
#include <iomanip>
#include <iostream>
#include <sstream>

// LEOBASE INCLUDES
#include "LeoBase/Testing/unitary/unitary_test_log.h"

// LEOBASE MACROS
LEOBASE_MODULE_BEGIN(testing)

// ---------------------------------------------------------------------------------------------------------------------

// Namespaces
using namespace timing::types;

UnitaryTestLog::UnitaryTestLog(const std::string& module,
                         const std::string& submodule,
                         const std::string& test,
                         const std::string& det_ex,
                         bool passed,
                         const std::string& tp_str,
                         long long elapsed_us,
                         const UnitTestResult& results) :
    module_(module),
    submodule_(submodule),
    test_(test),
    tpStr_(tp_str),
    detEx_(det_ex),
    passed_(passed),
    elapsed_(elapsed_us),
    results_(results)
{}

std::string UnitaryTestLog::makeLog(bool force_show) const
{
    // Stream.
    std::stringstream stream;

    // Update colors.
    stream << (this->passed_ ? "\033[038;2;0;210;0m" : "\033[038;2;210;000;000m");

    // Log the result.
    stream << "[" << this->tpStr_ << "] ";
    stream << this->module_ << " | " << this->submodule_ << " | " << this->test_ << " ";
    std::string aux_str = formatResult();
    size_t dot_w = 70 - this->test_.size() - this->module_.size() - this->submodule_.size() - aux_str.size();
    stream << std::left << std::setw(static_cast<int>(dot_w)) << std::setfill('.') << "" << aux_str;

    // Add the elapsed time.
    stream << " [" << this->elapsed_.count() << "us]";

    // Check if we have detailed error.
    if(!this->detEx_.empty())
        stream << " [Except: " << this->detEx_ << "]";

    // Put all the checks.
    if(!this->passed_ || force_show)
    {
        stream << "\n";
        stream << "   Subtest  -  Result  -  Data\n";
        for (const auto& check : this->results_)
        {
            const auto& [num, result, message] = check;

            size_t padd = 3 - std::to_string(num).length();
            stream << (result ? "\033[038;2;0;210;0m" : "\033[038;2;200;000;000m");

            stream << "      " << num << std::string(padd, ' ') << "   -   ";
            if (result)
            {
                stream << "PASS";
                if(force_show && !message.empty())
                    stream << " - " << message;
            }
            else
            {
                stream << "FAIL";
                if (!message.empty())
                    stream << "   -  " << message;
            }
            stream << "\n";
        }
    }

    // Restore the default color
    stream << "\x1b[0m";

    // Return the stream.
    return stream.str();
}

std::string UnitaryTestLog::formatResult() const
{
    std::stringstream result;
    result << std::left << std::setw(4) << std::setfill('.')
           << (this->passed_ ? " PASS" : " FAIL");
    return result.str();
}

const std::string &UnitaryTestLog::getModuleName() const
{
    return this->module_;
}

const std::string &UnitaryTestLog::getSubmoduleName() const
{
    return this->submodule_;
}

bool UnitaryTestLog::getResult() const
{
    return this->passed_;
}

// ---------------------------------------------------------------------------------------------------------------------

LEOBASE_MODULE_END
