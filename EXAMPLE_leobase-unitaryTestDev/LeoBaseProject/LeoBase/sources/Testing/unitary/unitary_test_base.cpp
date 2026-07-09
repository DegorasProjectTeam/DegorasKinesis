/*
 *      Copyright(C) Milethos Technologies SLU. 2025
 *      All rights reserved. Reproduction in whole or in
 *      part is prohibited without the written consent of
 *      the copyright holder.
 */

// C++ INCLUDES
#include <thread>

// LIBRARY INCLUDES
#include "LeoBase/Testing/unitary/unitary_test_base.h"

// LEOBASE MACROS
LEOBASE_MODULE_BEGIN(testing)

// ---------------------------------------------------------------------------------------------------------------------

UnitaryTestBase::UnitaryTestBase(const std::string &name):
    testName_(name),
    result_(true),
    forceStreamData_(false),
    stopOnSubtestFail_(false),
    currentCheckN_(0)
{}

bool UnitaryTestBase::forceFail()
{
    std::cout << std::setw(27) << " ";
    std::cout << "- Force test fail" << std::endl;
    std::cout << std::flush;
    this->updateCheckResults(false);
    return false;
}

bool UnitaryTestBase::forcePass()
{
    std::cout << std::setw(27) << " ";
    std::cout << "- Force test pass" << std::endl;
    std::cout << std::flush;
    this->updateCheckResults(true);
    return true;
}

bool UnitaryTestBase::expectTrue(bool result)
{
    std::cout << std::setw(27) << " ";
    std::cout << "- Expecting true result " << std::endl;
    std::cout << std::flush;
    this->updateCheckResults(result);
    return result;
}

bool UnitaryTestBase::expectFalse(bool result)
{
    std::cout << std::setw(27) << " ";
    std::cout << "- Expecting false result" << std::endl;
    std::cout << std::flush;
    this->updateCheckResults(!result);
    return !result;
}

bool UnitaryTestBase::expectEQ(const std::string &str1, const std::string &str2)
{
    std::cout << std::setw(27) << " ";
    std::cout << "- Comparing strings equality" << std::endl;
    std::cout << std::flush;
    bool result = (str1 == str2);
    this->updateCheckResults(result, str1, str2);
    return result;
}

bool UnitaryTestBase::expectNE(const std::string &str1, const std::string &str2)
{
    std::cout << std::setw(27) << " ";
    std::cout << "- Comparing strings inequality" << std::endl;
    std::cout << std::flush;
    bool result = (str1 != str2);
    this->updateCheckResults(result, str1, str2);
    return result;
}

bool UnitaryTestBase::expectEQ(const char *str1, const char *str2)
{
    std::cout << std::setw(27) << " ";
    std::cout << "- Comparing chars equality" << std::endl;
    std::cout << std::flush;
    bool result = (std::string(str1) == std::string(str2));
    this->updateCheckResults(result, str1, str2);
    return result;
}

bool UnitaryTestBase::expectNE(const char *str1, const char *str2)
{
    std::cout << std::setw(27) << " ";
    std::cout << "- Comparing chars inequality" << std::endl;
    std::cout << std::flush;
    bool result = (std::string(str1) != std::string(str2));
    this->updateCheckResults(result, str1, str2);
    return result;
}

void UnitaryTestBase::sleepUs(long long us) const
{
    std::cout << std::setw(27) << " ";
    std::cout << "- Waiting for " << us << " us" << std::endl;
    std::cout << std::flush;
    std::this_thread::sleep_for(std::chrono::microseconds(us));
}

UnitaryTestBase::~UnitaryTestBase(){}

void UnitaryTestBase::setForceStreamData(bool enable)
{
    this->forceStreamData_ = enable;
}

void UnitaryTestBase::setStopOnSubtestFail(bool enable)
{
    this->stopOnSubtestFail_ = enable;
}

/*
bool UnitaryTestBase::checkAnyThrow(std::function<void()> statement){
    try {
        statement(); //Gather the function
        return false;
    } catch (...) {
        return true;
    }
}

bool UnitaryTestBase::checkNoThrow(std::function<void()> statement){
    try {
        statement();
        return true;
    } catch (...) {
        return false;
    }
}
*/



// ---------------------------------------------------------------------------------------------------------------------

LEOBASE_MODULE_END
