/*
 *      Copyright(C) Milethos Technologies SLU. 2025
 *      All rights reserved. Reproduction in whole or in
 *      part is prohibited without the written consent of
 *      the copyright holder.
 */

// C++ INCLUDES

// LEOBASE INCLUDES
#include "LeoBase/System/console/custom_console.h"

// LEOBASE MACROS
LEOBASE_MODULE_BEGIN(system)
BEGIN_NAMESPACE(console)

// ---------------------------------------------------------------------------------------------------------------------

CustomConsole::CustomConsole() :
    exit_callback_(nullptr),
    close_flag_(false),
    handle_stdin_(GetStdHandle(STD_INPUT_HANDLE)),
    handle_stdout_(GetStdHandle(STD_OUTPUT_HANDLE))
{}

BOOL WINAPI CustomConsole::StaticConsoleCtrlHandler(DWORD dwCtrlType)
{
    return CustomConsole::getInstance().consoleCtrlHandler(dwCtrlType);
}

CustomConsole &CustomConsole::getInstance()
{
    // Guaranteed to be destroyed, instantiated on first use.
    static CustomConsole instance;
    return instance;
}

void CustomConsole::configureConsole(bool apply_ctrl_handler, bool hide_cursor, bool allow_in)
{
    // Safe lock.
    std::lock_guard<std::mutex> lock(this->mtx_);

    GetConsoleCursorInfo(handle_stdout_, &this->orig_cursor_info_);
    GetConsoleMode(handle_stdin_, &this->orig_in_mode_);

    // Add the console control handler.
    // Note: The handler will work async.
    if(apply_ctrl_handler)
        SetConsoleCtrlHandler(StaticConsoleCtrlHandler, TRUE);

    // Disable the input proccesing.
    if(!allow_in)
    {
        DWORD mode = this->orig_in_mode_ & ~static_cast<DWORD>(ENABLE_LINE_INPUT);
        SetConsoleMode(this->handle_stdin_, mode);
    }

    // Hide the console cursor.
    if(hide_cursor)
    {
        CONSOLE_CURSOR_INFO cursorInfo = orig_cursor_info_;
        cursorInfo.bVisible = false;
        SetConsoleCursorInfo(this->handle_stdout_, &cursorInfo);
    }
}

void CustomConsole::setExitCallback(const ExitConsoleCallback &exit_callback)
{
    // Safe lock.
    std::lock_guard<std::mutex> lock(this->mtx_);

    this->exit_callback_ = exit_callback;
}

CustomConsole::~CustomConsole()
{
    this->restoreConsole();
}

void CustomConsole::restoreConsole()
{
    // Safe lock.
    std::lock_guard<std::mutex> lock(this->mtx_);

    // Restore original input and output modes
    SetConsoleMode(this->handle_stdin_, this->orig_in_mode_);
    SetConsoleCursorInfo(this->handle_stdout_, &this->orig_cursor_info_);
}

BOOL CustomConsole::consoleCtrlHandler(DWORD dw_ctrl_t)
{
    // Safe lock.
    std::lock_guard<std::mutex> lock(this->mtx_);

    WSADATA wsa_data;
    WSAStartup(MAKEWORD(2,2), &wsa_data);

    if (dw_ctrl_t == CTRL_C_EVENT || dw_ctrl_t == CTRL_BREAK_EVENT || dw_ctrl_t == CTRL_CLOSE_EVENT)
    {
        // Update the closing flag.
        CustomConsole::close_flag_ = true;

        // Call the exit callback
        if(this->exit_callback_)
            this->exit_callback_();

        // Notify with the cv and return.
        this->close_cv_.notify_all();

        return TRUE;
    }
    return FALSE;
}

void CustomConsole::waitForClose()
{
    // Safe lock.
    std::unique_lock<std::mutex> lock(this->cv_mtx_);
    this->close_cv_.wait(lock, [this]{ return this->close_flag_.load(); });
}

bool CustomConsole::closeStatus()
{
    return CustomConsole::close_flag_;
}

// ---------------------------------------------------------------------------------------------------------------------

END_NAMESPACE(console)
LEOBASE_MODULE_END
