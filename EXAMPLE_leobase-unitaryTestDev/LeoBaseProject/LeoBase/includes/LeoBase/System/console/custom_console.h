/*
 *      Copyright(C) Milethos Technologies SLU. 2025
 *      All rights reserved. Reproduction in whole or in
 *      part is prohibited without the written consent of
 *      the copyright holder.
 */

// TODO ADD UNIX PART!!!!

// ---------
#pragma once
// ---------

// C++ INCLUDES
#include <Windows.h>
#include <atomic>
#include <functional>
#include <condition_variable>
#include <future>

// LEOBASE INCLUDES
#include "LeoBase/Aliases/Global"

// LEOBASE MACROS
LEOBASE_MODULE_BEGIN(system)
BEGIN_NAMESPACE(console)

// ---------------------------------------------------------------------------------------------------------------------

class LEOBASE_EXPORT CustomConsole
{
public:

    /// Exit callback alias.
    using ExitConsoleCallback = std::function<void()>;

    /**
     * @brief Access to the singleton instance
     */
    static CustomConsole& getInstance();

    /**
     * @brief Configure the console settings.
     *
     * This function allows you to configure various settings for the console, such as applying a Ctrl handler,
     * hiding the cursor, and disable the input processing.
     *
     * @param ctrl_hndlr If true, a Ctrl handler will be applied to capture Ctrl events.
     * @param hide_cursor If true, the cursor will be hidden in the console.
     * @param allow_in If false, the input processing will be disabled.
     *
     * @note Enabling Ctrl handler allows you to capture Ctrl events like Ctrl+C or Ctrl+Break.
     * @note Hiding the cursor can be useful for creating a cleaner console interface.
     */
    void configureConsole(bool ctrl_hndlr = false, bool hide_cursor = false, bool allow_in = true);

    /**
     * @brief Setter function for the exit callback.
     */
    void setExitCallback(const ExitConsoleCallback& exit_callback);

    /**
     * @brief Restore Console Configuration.
     *
     * This function restores the console configuration to its original state, including input mode and cursor settings.
     * Call this function when you no longer need the custom console settings applied by `CustomConsole`.
     *
     * @note Use this function to clean up and restore the console settings before exiting your application.
     */
    void restoreConsole();

    /**
     * @brief Wait for Console Close Signal.
     *
     * This function blocks the calling thread until a console close signal is received, typically triggered by
     * events like Ctrl+C, Ctrl+Break, or Ctrl+Close. It is useful for waiting until the user chooses to exit
     * the application gracefully.
     */
    void waitForClose();

    void restoreCloseFlag()
    {
        this->close_flag_ = false;
    }

    /**
     * @brief Wait for a future task to complete or a close signal.
     *
     * @tparam T The type of the future's result.
     * @param future A `std::shared_future<T>` representing the asynchronous task to monitor.
     */
    template <typename T>
    void waitForClose(std::shared_future<T> future)
    {
        std::unique_lock<std::mutex> lock(this->cv_mtx_);

        auto task_ready = [&]
        {
            return future.valid() &&
                   future.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
        };

        while (!this->close_flag_.load() && !task_ready())
            this->close_cv_.wait_for(lock, std::chrono::milliseconds(20));

        this->close_flag_.store(true);
    }

    /**
     * @brief Get Console Close Status.
     *
     * @return Returns true if a console close signal has been received, indicating that the application should
     * prepare for exit. Returns false otherwise.
     */
    bool closeStatus();

    /**
     * @brief Custom Console Control Handler.
     *
     * This virtual function can be overridden by subclasses of `CustomConsole` to implement a custom control handler
     * for console events. When subclassing `CustomConsole` and providing your own implementation of this function, you
     * can define how your application should respond to specific console control events, such as Ctrl+C or Ctrl+Break.
     *
     * @param dw_ctrl_t The type of control event received.
     * @return Returns TRUE if the control event is handled by the custom handler, or FALSE if it's not handled.
     *
     * @note When you override this function, you gain control over how your application responds to console
     * control events, allowing you to implement custom behavior for events like Ctrl+C.
     */
    virtual BOOL WINAPI consoleCtrlHandler(DWORD dw_ctrl_t);

    /**
     * @brief Virtual destructor. Restores the default console configuration.
     */
    virtual ~CustomConsole();

    // Deleted constructors and assignment operators.
    CustomConsole(const CustomConsole &) = delete;
    CustomConsole(CustomConsole&&) = delete;
    CustomConsole& operator=(const CustomConsole &) = delete;
    CustomConsole& operator=(CustomConsole&&) = delete;

private:

    // Private constructor.
    CustomConsole();

    // Static console handler function trick.
    static BOOL WINAPI StaticConsoleCtrlHandler(DWORD dwCtrlType);

    // Configuration.
    ExitConsoleCallback exit_callback_;
    std::condition_variable close_cv_;
    std::atomic_bool close_flag_;
    std::mutex mtx_;
    std::mutex cv_mtx_;

    // Console realted members.
    HANDLE handle_stdin_;
    HANDLE handle_stdout_;
    DWORD orig_in_mode_;
    CONSOLE_CURSOR_INFO orig_cursor_info_;
};

// ---------------------------------------------------------------------------------------------------------------------

END_NAMESPACE(console)
LEOBASE_MODULE_END
