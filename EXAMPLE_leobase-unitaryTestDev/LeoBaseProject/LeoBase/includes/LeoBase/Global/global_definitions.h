/*
 *      Copyright(C) Milethos Technologies SLU. 2026
 *      All rights reserved. Reproduction in whole or in
 *      part is prohibited without the written consent of
 *      the copyright holder.
 */

#pragma once

// LEOBASE INCLUDES
#include "LeoBase/Global/global_macros.h"

// LEOBASE MACROS
LEOBASE_BEGIN_NAMESPACE

// ---------------------------------------------------------------------------------------------------------------------

#if defined(__MINGW32__) || defined(_MSC_VER)
#define MKGMTIME _mkgmtime
#else
#define MKGMTIME timegm
#endif

#if defined(WINDOWS) || defined(_WIN32)
#define GetCurrentDir _getcwd
#else
#define GetCurrentDir getcwd
#endif

// ---------------------------------------------------------------------------------------------------------------------

/// Name of the LeoBase project.
constexpr const char* kProjN = "LeoBase";

BEGIN_NAMESPACE(callbacks)
constexpr const char* kModN = "Callbacks";
LEOBASE_END_NAMESPACE

BEGIN_NAMESPACE(communication)
constexpr const char* kModN = "Communication";
LEOBASE_END_NAMESPACE

BEGIN_NAMESPACE(containers)
constexpr const char* kModN = "Containers";
LEOBASE_END_NAMESPACE

BEGIN_NAMESPACE(crypto)
constexpr const char* kModN = "Crypto";
LEOBASE_END_NAMESPACE

BEGIN_NAMESPACE(exceptions)
constexpr const char* kModN = "Exceptions";
LEOBASE_END_NAMESPACE

BEGIN_NAMESPACE(global)
constexpr const char* kModN = "Global";
LEOBASE_END_NAMESPACE

BEGIN_NAMESPACE(helpers)
constexpr const char* kModN = "Helpers";
LEOBASE_END_NAMESPACE

BEGIN_NAMESPACE(logger)
constexpr const char* kModN = "Logger";
LEOBASE_END_NAMESPACE

BEGIN_NAMESPACE(messaging)
constexpr const char* kModN = "Messaging";
LEOBASE_END_NAMESPACE

BEGIN_NAMESPACE(module)
constexpr const char* kModN = "Module";
LEOBASE_END_NAMESPACE

BEGIN_NAMESPACE(network)
constexpr const char* kModN = "Network";
LEOBASE_END_NAMESPACE

BEGIN_NAMESPACE(patterns)
constexpr const char* kModN = "Patterns";
LEOBASE_END_NAMESPACE

BEGIN_NAMESPACE(system)
constexpr const char* kModN = "System";
LEOBASE_END_NAMESPACE

BEGIN_NAMESPACE(tasks)
constexpr const char* kModN = "Tasks";
LEOBASE_END_NAMESPACE

BEGIN_NAMESPACE(testing)
constexpr const char* kModN = "Testing";
LEOBASE_END_NAMESPACE

BEGIN_NAMESPACE(timing)
constexpr const char* kModN = "Timing";
LEOBASE_END_NAMESPACE

// ---------------------------------------------------------------------------------------------------------------------

constexpr const char* kDefaultFilename = "LeobaseDefaultFilename";

// ---------------------------------------------------------------------------------------------------------------------

// TODO: Namespace o no, se mantiene en global? se saca fuera? Se crea una carpeta modular "Definitions"?

BEGIN_NAMESPACE(colors)
constexpr const char* kAnsiOrangeFg1 = "\033[38;2;255;128;0m";
constexpr const char* kAnsiReset = "\033[0m";
LEOBASE_END_NAMESPACE

// ---------------------------------------------------------------------------------------------------------------------


LEOBASE_END_NAMESPACE
