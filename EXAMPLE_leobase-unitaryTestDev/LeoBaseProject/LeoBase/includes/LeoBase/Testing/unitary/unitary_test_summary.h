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
#include "LeoBase/Testing/unitary/unitary_test_log.h"

// LEOBASE MACROS
LEOBASE_MODULE_BEGIN(testing)

// ---------------------------------------------------------------------------------------------------------------------

using UnitaryTestLogMap = std::multimap<std::string, UnitaryTestLog>;

class LEOBASE_EXPORT UnitaryTestSummary : public patterns::NonCopyable
{

public:

    UnitaryTestSummary();

    void setSessionName(const std::string& name);

    void addLog(UnitaryTestLog&& log);

    void clear();

    void makeSummary(bool forceShow = true) const;

    void writeSumary(const std::string& filename, const std::string& content) const;

    void setOutputPath(const std::string& path);

    void setWriteLog(bool enable);

private:

    UnitaryTestLogMap testLogs_;
    std::string session_;
    unsigned nPass_;
    unsigned nFail_;
    std::string outputPath_ = "";
    bool writeLog_ = true;
};

// ---------------------------------------------------------------------------------------------------------------------

LEOBASE_MODULE_END
