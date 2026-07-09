/*
 *      Copyright(C) Milethos Technologies SLU. 2025
 *      All rights reserved. Reproduction in whole or in
 *      part is prohibited without the written consent of
 *      the copyright holder.
 */

#pragma once

// C++ INCLUDES
#include <map>

// LEOBASE INCLUDES
#include "LeoBase/Aliases/Global"
#include "LeoBase/Patterns/non_copyable.h"
#include "LeoBase/Patterns/non_movable.h"
#include "LeoBase/Testing/unitary/unitary_test_base.h"
#include "LeoBase/Testing/unitary/unitary_test_summary.h"

// LEOBASE MACROS
LEOBASE_MODULE_BEGIN(testing)

/// Alias for a dictionary of UnitaryTestBase class.
using UnitarryTestDict = std::multimap<std::string, std::pair<std::string, UnitaryTestBase*>>;

class LEOBASE_EXPORT UnitaryTest : public patterns::NonCopyable,
                                   public patterns::NonMovable
{

public:

    static UnitaryTest& instance();

    void setSessionName(std::string&& session);

    void registerTest(std::string&& module, std::string&& submodule, UnitaryTestBase* test);

    bool runTests();

    void clear();

    void setForceShowResults(bool enable);


private:

    // Constructor.
    UnitaryTest();

    // Members.
    UnitarryTestDict test_dict_;
    UnitaryTestSummary summary_;
    std::string session_;

    // Configuration.
    bool force_show_results_;
};

// ---------------------------------------------------------------------------------------------------------------------

LEOBASE_MODULE_END
