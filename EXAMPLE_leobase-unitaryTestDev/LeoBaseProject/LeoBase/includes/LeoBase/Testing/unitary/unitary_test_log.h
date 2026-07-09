/*
 *      Copyright(C) Milethos Technologies SLU. 2025
 *      All rights reserved. Reproduction in whole or in
 *      part is prohibited without the written consent of
 *      the copyright holder.
 */

#pragma once

// C++ INCLUDES
#include <string>
#include <vector>

// LEOBASE INCLUDES
#include "LeoBase/Aliases/Global"
#include "LeoBase/Patterns/non_copyable.h"
#include "LeoBase/Timing/types/base_time_types.h"

// LEOBASE MACROS
LEOBASE_MODULE_BEGIN(testing)

// ---------------------------------------------------------------------------------------------------------------------

using UnitTestResult = std::vector<std::tuple<unsigned, bool, std::string>>;

struct LEOBASE_EXPORT UnitaryTestLog : public patterns::NonCopyable
{

public:

    UnitaryTestLog(const std::string& module,
                const std::string& submodule,
                const std::string& test,
                const std::string& detEx,
                bool passed,
                const std::string& tp_str,
                long long elapsed_us,
                const UnitTestResult& results);

    std::string makeLog(bool force_show = false) const;

    const std::string& getModuleName() const;

    const std::string& getSubmoduleName() const;

    bool getResult() const;

private:

    std::string formatResult() const;

    std::string module_;
    std::string submodule_;
    std::string test_;
    std::string tpStr_;
    std::string detEx_;
    bool passed_;
    timing::types::Us elapsed_;
    UnitTestResult results_;
};

// ---------------------------------------------------------------------------------------------------------------------

LEOBASE_MODULE_END
