/*
 *      Copyright(C) Milethos Technologies SLU. 2025
 *      All rights reserved. Reproduction in whole or in
 *      part is prohibited without the written consent of
 *      the copyright holder.
 */

// C++ INCLUDES
#include <iostream>
#include <sstream>
#include <fstream>

// LEOBASE INCLUDES
#include "LeoBase/Testing/unitary/unitary_test_summary.h"
#include "LeoBase/Timing/utils/time_utils.h"
#include "LeoBase/Helpers/strings_utils.h"
#include "LeoBase/Helpers/containers_utils.h"
#include "LeoBase/System/filesystem/filesystem_utils.h"

// LEOBASE MACROS
LEOBASE_MODULE_BEGIN(testing)

// ---------------------------------------------------------------------------------------------------------------------

using namespace timing::utils;
using namespace timing::types;

UnitaryTestSummary::UnitaryTestSummary() :
    nPass_(0),
    nFail_(0)
{}

void UnitaryTestSummary::setSessionName(const std::string &name)
{
    this->session_ = name;
}

void UnitaryTestSummary::addLog(UnitaryTestLog&& log)
{
    this->testLogs_.insert(std::pair<std::string, UnitaryTestLog>(log.getModuleName(), std::move(log)));
    log.getResult() ? this->nPass_++ : this->nFail_++;
}

void UnitaryTestSummary::clear()
{
    this->testLogs_.clear();
}

void UnitaryTestSummary::makeSummary(bool force_show) const
{
    // Separators.
    std::string sep1 = helpers::strings::fill("=", 120) + "\n";
    std::string sep2 = helpers::strings::fill("-", 120) + "\n";

    // Auxiliar containers.
    std::stringstream final_stringstream;
    std::vector<std::string> keys = helpers::containers::getMapKeys(this->testLogs_);
    std::stringstream title;
    std::stringstream border;
    std::string s_name = "= Session:  " + this->session_;
    std::string date = "= Datetime: " + currentDatetimeIso8601(TimeResolution::MILLISECONDS);
    std::string keys_str = "= Modules:  " + helpers::strings::join(keys, " | ");

    // Prepare the filename.
    std::string date_file = timepointToString(SysClock::now(), "%Y%m%dT%H%M%S", TimeResolution::SECONDS);
    std::string filename = date_file + "_" + this->session_ + ".leoutsum";
    helpers::strings::replaceInPlace(filename, " ", "");
    system::files::sanitizeFilenameInPlace(filename);
    if (!this->outputPath_.empty()) {
        filename = this->outputPath_ + "/" + filename;
    }
    std::string file = "= File:     " + filename;

    // Auxiliar containers.
    std::stringstream test;
    std::stringstream pass;
    std::stringstream fail;
    std::stringstream all_pass;
    std::string all_pass_color = this->nFail_ == 0 ? "\033[038;2;0;210;000m" : "\033[038;2;210;000;000m";
    std::string all_pass_str = this->nFail_ == 0 ? "YES" : "NO ";

    test << "= Tests:    " << "\033[038;2;000;140;255m" << std::to_string(this->testLogs_.size()) << "\033[38;2;255;128;0m";
    pass << "= Passed:   " << "\033[038;2;000;210;000m" << std::to_string(this->nPass_) << "\033[38;2;255;128;0m";
    fail << "= Failed:   " << "\033[038;2;210;000;000m" << std::to_string(this->nFail_) << "\033[38;2;255;128;0m";
    all_pass << "= All Pass: " << all_pass_color << all_pass_str << "\033[38;2;255;128;0m";

    std::string pass_str = pass.str();

    // Get the keys.
    keys = helpers::containers::getMapKeys(this->testLogs_);

    // Generate the header.
    title << "\033[38;2;255;128;0m";
    title << sep1;
    title << "=" << helpers::strings::centerText("COMPLETE UNIT TEST SESSION SUMMARY", 118) << "=\n";
    title << sep1;
    title << s_name << std::string(119 - s_name.length(), ' ') << "=\n";
    title << date << std::string(119 - date.length(), ' ') << "=\n";
    title << keys_str << std::string(119 - keys_str.length(), ' ') << "=\n";
    title << file << std::string(119 - file.length(), ' ') << "=\n";
    title << test.str() << std::string(156 - test.str().length(), ' ') << "=\n";
    title << pass.str() << std::string(156 - pass.str().length(), ' ') << "=\n";
    title << fail.str() << std::string(156 - fail.str().length(), ' ') << "=\n";
    title << all_pass.str() << std::string(154 - fail.str().length(), ' ') << "=\n";
    title << sep1;
    title << "\n";

    border.fill('=');
    border.width(80);
    border << "\n";

    // Show the title.
    final_stringstream<<title.str();

    // Iterate over the multimap in order of keys
    for (auto it = this->testLogs_.begin(); it != this->testLogs_.end();)
    {
        // Auxiliar containers.
        std::string c_module = it->first;
        auto range = this->testLogs_.equal_range(c_module);
        std::stringstream sep_mod;

        // Prepare the separator string.
        sep_mod << "\033[38;2;255;128;0m";
        sep_mod << sep2;
        sep_mod << "-- TESTS RESULTS FOR MODULE: " << c_module << "\n";
        sep_mod << sep2;

        final_stringstream << sep_mod.str();

        // Process all elements with the same key.
        for (auto range_it = range.first; range_it != range.second; ++range_it)
        {
            final_stringstream << range_it->second.makeLog(force_show) << std::endl;
        }

        // Finish the section.
        final_stringstream << "\033[38;2;255;128;0m";
        final_stringstream << sep2 << "\n";

        // Move the iterator to the next unique key
        it = range.second;
    }

    // Finish and restore the default color.
    final_stringstream << sep1;
    final_stringstream << "\x1b[0m";

    //std::cerr<<final_stringstream.str();

    //Write to ".leoutsum" logfile TODO: JSON IMPLEMENTATION FOR LOGS
    /*
    std::ofstream outFile(filename);

    if (outFile.is_open())
    {
        outFile << final_stringstream.str();
        outFile.close();
        std::cout << "\n[INFO] Summary saved to: " << filename << std::endl;
    }
    else
    {
        std::cerr << "\n[ERROR] Could not create log file: " << filename << std::endl;
    }
*/

    std::string fullContent = final_stringstream.str();

    std::cerr << fullContent;

    if(this->writeLog_)
    {
        writeSumary(filename, fullContent);
    }
}

void UnitaryTestSummary::writeSumary(const std::string& filename, const std::string& content) const
{
    std::ofstream outFile(filename);

    if (outFile.is_open())
    {
        outFile << content;
        outFile.close();
        std::cout << "\n[INFO] Summary saved to: " << filename << std::endl;
    }
    else
    {
        std::cerr << "\n[ERROR] Could not create log file: " << filename << std::endl;
    }
}

void UnitaryTestSummary::setOutputPath(const std::string& path) {
    this->outputPath_ = path;

    if (!this->outputPath_.empty() && this->outputPath_.back() != '/' && this->outputPath_.back() != '\\') {
        this->outputPath_ += "/";
    }
}

void UnitaryTestSummary::setWriteLog(bool enable)
{
    this->writeLog_=enable;
}


}} // END NAMESPACES.
// =====================================================================================================================
