/*
 *      Copyright(C) Milethos Technologies SLU. 2025
 *      All rights reserved. Reproduction in whole or in
 *      part is prohibited without the written consent of
 *      the copyright holder.
 */

// C++ INCLUDES
#include <future>
#include <iostream>

// LEOBASE INCLUDES
#include "LeoBase/Testing/unitary/unitary_test.h"
#include "LeoBase/Helpers/strings_utils.h"
#include "LeoBase/Timing/utils/time_utils.h"
#include "LeoBase/System/console/console_utils.h"

// LEOBASE MACROS
LEOBASE_MODULE_BEGIN(testing)

// ---------------------------------------------------------------------------------------------------------------------

using namespace timing::types;
using namespace timing::utils;

UnitaryTest &UnitaryTest::instance()
{
    static UnitaryTest uTest;
    return uTest;
}

void UnitaryTest::setSessionName(std::string &&session)
{
    this->session_ = std::move(session);
    this->summary_.setSessionName(this->session_);
}

void UnitaryTest::registerTest(std::string &&module, std::string &&submodule, UnitaryTestBase* test)
{
    std::pair<std::string, UnitaryTestBase*> submodule_pair = std::make_pair(std::move(submodule), test);
    std::pair<std::string, std::pair<std::string, UnitaryTestBase*>> multi_pair =
        std::make_pair(std::move(module), std::move(submodule_pair));
    this->test_dict_.insert(std::move(multi_pair));
}

void UnitaryTest::clear()
{
    this->test_dict_.clear();
    this->summary_.clear();
}

bool UnitaryTest::runTests()
{
    // Enable ANSI colors.
    system::console::consoleEnableAnsiColors();

    // Final result.
    bool final_res = true;

    // Separator.
    std::string sep = helpers::strings::fill("=", 120) + "\n";

    // Log.
    std::cout<<"\033[38;2;255;128;0m"<<sep<<"=                                              ";
    std::cout<<"EXECUTING UNIT TEST SESSION                                             =\n";
    std::cout<<"\033[38;2;255;128;0m"<<sep;
    std::string now_str = currentDatetimeIso8601(TimeResolution::MILLISECONDS);
    std::cout<<"\033[38;2;255;128;0m"<<"["<<now_str<<"]"<<"\033[038;2;0;210;0m";
    std::cout<<" Starting the session: "<<this->session_<<"\033[38;2;255;128;0m"<<std::endl;

    // TODO: Poner opcion de paralelizacion de tests.
    // TODO: Poner posibilidad de cancelar.

    // Iterate over the multimap in order of keys
    for (auto it = this->test_dict_.begin(); it != this->test_dict_.end();)
    {
        // Auxiliar containers.
        std::string c_module = it->first;
        auto range = this->test_dict_.equal_range(c_module);

        // Process all elements with the same key
        for (auto range_it = range.first; range_it != range.second; ++range_it)
        {
            // Auxiliar containers.
            std::vector<std::pair<unsigned, bool>> results;
            std::string submodule = range_it->second.first;
            UnitaryTestBase* test = range_it->second.second;
            std::string det_ex;
            long long elapsed = 0;
            bool result;

            // Get current time.
            std::string now_str = currentDatetimeIso8601(TimeResolution::MILLISECONDS);

            // Log.
            std::cout<<colors::kAnsiOrangeFg1<<"["<<now_str<<"] "<<"\033[038;2;0;140;255m"
                      <<c_module << " | " << submodule << " | " << test->testName_<<std::endl;

            // Async execution.
            auto future = std::async(std::launch::async, [test, &result, this]()
            {
                // Configure the base test.
                test->setForceStreamData(this->force_show_results_);
                // Start time.
                auto start = std::chrono::steady_clock::now();
                // Run the test.
                test->runTest();
                // End time.
                auto stop = std::chrono::steady_clock::now();
                // Get the elapsed time.
                auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
                // Store the result.
                result = test->result_;
                return static_cast<long long>(duration.count());
            });

            // Wait for the asynchronous task to complete.
            future.wait();

            // Get the elapsed time.
            try{elapsed = future.get();}
            catch (const std::exception& e)
            {
                result = false;
                det_ex = e.what();
            }

            // Update final result.
            if(final_res && !result)
                final_res = false;

            // Instantiate the test log and store.
            UnitaryTestLog t_log(c_module, submodule, test->testName_, det_ex, result,
                              now_str, elapsed, test->checkResultsV_);
            this->summary_.addLog(std::move(t_log));

            // TODO Seguramente podamos escapar de los test con stopOnSubtestFail_
        }

        // Move the iterator to the next unique key
        it = range.second;
    }

    // Log.
    now_str = currentDatetimeIso8601(TimeResolution::MILLISECONDS);
    std::cout<<"\033[38;2;255;128;0m"<<"["<<now_str<<"]";
    std::cout<<"\033[038;2;0;210;0m"<<" All registerted unit tests executed!"<<std::endl;
    std::cout<<"\033[38;2;255;128;0m"<<sep<<std::endl;

    // Make the summary.
    this->summary_.makeSummary(this->force_show_results_);

    // Return the final result.
    return final_res;
}

void UnitaryTest::setForceShowResults(bool enable)
{
    this->force_show_results_ = enable;
}

UnitaryTest::UnitaryTest() :
    force_show_results_(false)
{}

// ---------------------------------------------------------------------------------------------------------------------

LEOBASE_MODULE_END
