/*
 *      Copyright(C) Milethos Technologies SLU. 2025
 *      All rights reserved. Reproduction in whole or in
 *      part is prohibited without the written consent of
 *      the copyright holder.
 */

#pragma once

// LEOBASE INCLUDES
#include "LeoBase/Aliases/Global"

// LEOBASE MACROS
LEOBASE_MODULE_BEGIN(timing)

// ---------------------------------------------------------------------------------------------------------------------

// CONSTANTS
constexpr long double kPosixEpochToJulian = 2440587.5L;
constexpr long long kNsPerSecond = 1'000'000'000LL;
constexpr long long kSecsPerDayLL = 86'400LL;
constexpr long double kSecsPerDayL = 86'400.0L;
constexpr double kSecsPerDay = 86'400.0;
constexpr long long kWin32EpochToPosixEpoch = -11'644'473'600LL;
constexpr long double kNsPerMs = 1'000'000.0L;
constexpr long double kNsPerUs = 1'000.0L;
constexpr long double kNsPerS = 1'000'000'000.0L;

// ---------------------------------------------------------------------------------------------------------------------

LEOBASE_MODULE_END
