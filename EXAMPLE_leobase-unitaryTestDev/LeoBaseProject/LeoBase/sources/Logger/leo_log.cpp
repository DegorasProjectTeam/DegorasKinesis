/*
 *      Copyright(C) Milethos Technologies SLU. 2026
 *      All rights reserved. Reproduction in whole or in
 *      part is prohibited without the written consent of
 *      the copyright holder.
 */

// C++ INCLUDES
#include <sstream>
#include <iostream>
#include <cstring>
#include <sys/time.h>
#include <chrono>
#include <fstream>

// LEOBASE INCLUDES
#include "LeoBase/Logger/leo_log.h"
#include "LeoBase/Logger/utils/logger_utils.h"
#include "LeoBase/System/filesystem/filesystem_utils.h"
#include "LeoBase/System/utils/system_utils.h"
#include "LeoBase/Timing/utils/time_utils.h"

// LEOBASE MACROS
LEOBASE_MODULE_BEGIN(logger)

// ---------------------------------------------------------------------------------------------------------------------

LeoLog* LeoLog::instance()
{
    // Initialize static members
    static std::shared_ptr<LeoLog> instance = nullptr;
    static std::once_flag init_flag;

    std::call_once(init_flag, []()
        {
            instance.reset(new LeoLog());
        });

    return instance.get();
}

LeoLog::LeoLog()
    : stopped_(false)
    , running_(false)
    , dir_("NONE")
{

}

LeoLog::~LeoLog()
{
    this->stop();
}
    
void LeoLog::setDir(const std::string dir)
{
    this->dir_ = dir;
}

void LeoLog::init(const LoggerId& loggerId, std::string filename, LogLevel level)
{
    if (this->running_)
        return;

    this->filename_    = filename;
    this->loggerId_    = loggerId;
    this->loggerLevel_ = level;

    // Start task for process entries.
    Task::run();

    // Wait until the log file has been openned.
    {
        std::unique_lock<std::mutex> lock(this->logTaskFileMutex_);
        this->logTaskFileCondVar_.wait(lock, [&]()
            {
                return (this->running_ || this->stopped_ );
            });
    }
}

void LeoLog::stop()
{
    if (this->stopped_)
        return;

    this->stopped_ = true;

    this->dequeueFile_.stop();

    this->logTaskFileCondVar_.notify_all();

    // TODO: Safe sleep due to dequeue stop.
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    Task::stop(true);
}

void LeoLog::appendLogger(const LoggerId& loggerId, LogLevel level)
{
    if (this->stopped_)
        return;

    {
        std::unique_lock<std::mutex> lock(logMutex_);

        auto it = this->appendedLoggerMap_.find(loggerId);
        if (it == this->appendedLoggerMap_.end())
        {
            this->appendedLoggerMap_.emplace(loggerId, level);
        }
    }
}

void LeoLog::removeLogger(const LoggerId& loggerId)
{
    if (this->stopped_)
        return;

    {
        std::unique_lock<std::mutex> lock(logMutex_);

        auto it = this->appendedLoggerMap_.find(loggerId);
        if (it != this->appendedLoggerMap_.end())
        {
            this->appendedLoggerMap_.erase(it);
        }
    }
}

void LeoLog::setLogLevel(const LoggerId& loggerId, const LogLevel& level)
{
    if (this->stopped_)
        return;

    {
        std::unique_lock<std::mutex> lock(logMutex_);

        if (this->loggerId_ == loggerId)
        {
            this->loggerLevel_ = level;
        }
        else
        {
            auto it = this->appendedLoggerMap_.find(loggerId);
            if (it != this->appendedLoggerMap_.end())
            {
                it->second = level;
            }
        }
    }
}

LogLevel LeoLog::getLogLevel(const LoggerId& loggerId)
{
    if (this->stopped_)
        return LogLevel::LOGLEVEL_UNKNOWN;

    {
        std::unique_lock<std::mutex> lock(logMutex_);

        if (this->loggerId_ != loggerId)
        {
            auto it = this->appendedLoggerMap_.find(loggerId);
            if (it != this->appendedLoggerMap_.end())
            {
                return it->second;
            }
        }

        return this->loggerLevel_;
    }
}

void LeoLog::log(const LoggerId& loggerId, const LogLevel& level, const std::string& entry)
{
    if (this->stopped_)
        return;

    // Get log level.
    std::string levelStr = logLevelToString(level);

    // Get current process pid.
    std::string current_pid = std::to_string(system::utils::getCurrentPID());

    // Get current thread id.
    std::ostringstream thread_id_cast;
    thread_id_cast << std::hex << std::this_thread::get_id();
    const auto current_thread_id = thread_id_cast.str();

    std::string msg;
    {
        std::unique_lock<std::mutex> lock(logMutex_);

        if ((this->loggerId_ == loggerId) && (this->loggerLevel_ >= level))
        {
            msg = "[" + this->getCurrentTime() + "][" + current_pid + "][" + current_thread_id + "][" + this->loggerId_ + "!" + levelStr + "] " + entry;
        }
        else
        {
            auto it = this->appendedLoggerMap_.find(loggerId);
            if (it == this->appendedLoggerMap_.end())
            {
                return;
            }

            if (it->second >= level)
            {
                msg = "[" + this->getCurrentTime() + "][" + current_pid + "][" + current_thread_id + "][" + this->loggerId_ + "|" + it->first + "!" + levelStr + "] " + entry;
            }
            else
            {
                return;
            }
        }
    }

    logToConsole(level, msg);
    logToFile(msg);
}

void LeoLog::logToConsole(const LogLevel& /*level*/, const std::string& entry)
{
    if (this->stopped_)
        return;

    // TODO: CHANGE BY COMPATIBILITY COLOR IN WINDOWS AND LINUX.
    // LogColor color = logLevelToColor(level);

    std::unique_lock<std::mutex> lock(logConsoleMutex_);
    std::cout << /*LogColourReset << color <<*/ entry /*<< LogColourReset*/ << std::endl;
}

void LeoLog::logToFile(const std::string& entry)
{
    if (this->stopped_)
        return;

    this->dequeueFile_.pushBack(entry);
}

void LeoLog::step()
{
    // If filename or logger id are empty, use PID for name.
    std::string name;
    if (this->filename_.empty() && this->loggerId_.empty())
    {
        name = std::to_string(system::utils::getCurrentPID());
    }
    else
    {
        name = (!this->filename_.empty()) ? this->filename_ : this->loggerId_;
    }

    auto date_str = this->getCurrentTime(true, false);

    std::string filename = (this->dir_ == "NONE" ? system::files::getTempDir() : this->dir_ ) + "/log_" + name + "_" + date_str + ".log";

    std::ofstream ofs_file(filename, std::ios::out | std::ios::app);
    if (ofs_file.fail())
    {
        throw std::runtime_error(std::string("Failed to open logFile: " + filename + " Error: ").append(strerror(errno)));
    }

    {
        // We notify that logger is up.
        std::unique_lock<std::mutex> lock(this->logTaskFileMutex_);
        this->running_ = true;
    }

    this->logTaskFileCondVar_.notify_all();

    std::string log_str;
    while (!this->stopped_)
    {
        if (!this->dequeueFile_.waitAndPopFront(log_str))
            break;

        if (this->stopped_)
            break;

        ofs_file << log_str << std::endl;
    }

    // Close file if stopped.
    ofs_file.close();
}

std::string LeoLog::getCurrentTime(bool forFile, bool withMillis)
{
    std::stringstream time_str;
    if (forFile)
    {
        // Time is in UTC by default.
        time_str << timing::utils::timepointToString(std::chrono::system_clock::now(),
                                                     "%Y%m%d_%H-%M-%S", timing::types::TimeResolution::SECONDS);
    }
    else
    {
        // Current datetime to ISO8601 use by defaults:
        // Resolution: Milliseconds
        // UTC: true
        // ClockType: SYSTEM
        if (withMillis)
        {
            time_str << timing::utils::currentDatetimeIso8601();
        }
        else
        {
            time_str << timing::utils::currentDatetimeIso8601(timing::types::TimeResolution::SECONDS);
        }
    }

    return time_str.str();
}

// ---------------------------------------------------------------------------------------------------------------------

LEOBASE_MODULE_END
