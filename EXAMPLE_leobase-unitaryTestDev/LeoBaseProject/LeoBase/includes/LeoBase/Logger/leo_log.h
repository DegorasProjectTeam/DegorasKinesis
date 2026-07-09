/*
 *      Copyright(C) Milethos Technologies SLU. 2025
 *      All rights reserved. Reproduction in whole or in
 *      part is prohibited without the written consent of
 *      the copyright holder.
 */

#pragma once

// C++ INCLUDES
#include <atomic>
#include <mutex>
#include <sstream>
#include <iomanip>
#include <condition_variable>

// LEOBASE INCLUDES
#include "LeoBase/Aliases/Global"

#include "LeoBase/Logger/types/logger_types.h"
#include "LeoBase/Tasks/task.h"
#include "LeoBase/Containers/thread_safe_deque.h"

// LEOBASE MACROS
LEOBASE_MODULE_BEGIN(logger)

// ---------------------------------------------------------------------------------------------------------------------

// TODO:
// - Use colors for show text in console.
// - Close current file and open a new one if its produced a day change in the system.
// - Show text in console using a independient thread.
// - If app fails, store text in file if queue is filled.

//! \note This Log should be initialized by main thread at the beginnig.
class LEOBASE_EXPORT LeoLog : public tasks::Task
{
public:

    static LeoLog* instance();
    ~LeoLog();

    void setDir(const std::string dir);

    void init(const LoggerId& loggerId, std::string filename = "", LogLevel level = LogLevel::LOGLEVEL_DEBUG);
    void stop();

    void appendLogger(const LoggerId& loggerId, LogLevel level = LogLevel::LOGLEVEL_DEBUG);
    void removeLogger(const LoggerId& loggerId);
    void setLogLevel(const LoggerId& loggerId, const LogLevel& level);
    LogLevel getLogLevel(const LoggerId& loggerId);

    #define LEOLOG_FUNC_DEF(func, level) \
        template <typename... Types> \
        void func(const LoggerId& loggerId, Types&&... msgs) \
        { \
            std::stringstream str; \
            str << std::setprecision(15); \
            logToOstream(str, std::forward<Types>(msgs)...); \
            log(loggerId, level, str.str()); \
        }

    LEOLOG_FUNC_DEF(critical, LogLevel::LOGLEVEL_CRITICAL)
    LEOLOG_FUNC_DEF(error,    LogLevel::LOGLEVEL_ERROR)
    LEOLOG_FUNC_DEF(warning,  LogLevel::LOGLEVEL_WARNING)
    LEOLOG_FUNC_DEF(info,     LogLevel::LOGLEVEL_INFO)
    LEOLOG_FUNC_DEF(debug,    LogLevel::LOGLEVEL_DEBUG)

private:
    // Private constructor.
    LeoLog();

    template <typename... Types>
    void logToOstream(std::ostream& out, Types&&... msgs)
    {
        // unrolls into a list of 'out << msg'. Initializer list is used to make it compile. 0, as ostream is not copyable.
        (void)std::initializer_list<int>{ (out << std::forward<Types>(msgs), 0)... };
    }

    void log(const LoggerId& loggerId, const LogLevel& level, const std::string& entry);

    void logToConsole(const LogLevel& level, const std::string& entry);
    void logToFile(const std::string& entry);

    void step() override;

    std::string getCurrentTime(bool forFile = false, bool withMillis = true);

private:
    std::atomic_bool                        stopped_;
    std::atomic_bool                        running_;

    std::string                              dir_;
    std::string                              filename_;
    std::string                              loggerId_;
    LogLevel                                 loggerLevel_;

    std::mutex                               logTaskFileMutex_;
    std::condition_variable                  logTaskFileCondVar_;
    containers::ThreadSafeDeque<std::string> dequeueFile_;

    std::mutex                               logConsoleMutex_;
    std::mutex                               logMutex_;

    AppendedLoggerMap                        appendedLoggerMap_;
};

// ---------------------------------------------------------------------------------------------------------------------

LEOBASE_MODULE_END
