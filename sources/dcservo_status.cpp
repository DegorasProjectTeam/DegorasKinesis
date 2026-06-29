/*
 *  Copyright (C) 2018-2026 Degoras Project Team
 *
 *  This file is part of a small-scale research or utility tool built atop
 *  the Degoras Project infrastructure, released under the MIT License.
 *
 *  SPDX-License-Identifier: MIT
 *
 *  See the LICENSE file in the root directory for full license details.
 */

// PROJECT INCLUDES
#include "dcservo_status.h"


// NAMESPACES
namespace thorlabs
{
namespace dcservo
{

// ---------------------------------------------------------------------------------------------------------------------
// Benchtop DC Servo status-word bit masks (module-specific; mirror the Kinesis header documentation).

namespace
{
constexpr unsigned long kLimitCw        = 0x00000001UL;
constexpr unsigned long kLimitCcw       = 0x00000002UL;
constexpr unsigned long kSwLimitCw      = 0x00000004UL;
constexpr unsigned long kSwLimitCcw     = 0x00000008UL;
constexpr unsigned long kMovingCw       = 0x00000010UL;
constexpr unsigned long kMovingCcw      = 0x00000020UL;
constexpr unsigned long kJoggingCw      = 0x00000040UL;
constexpr unsigned long kJoggingCcw     = 0x00000080UL;
constexpr unsigned long kMotorConnected = 0x00000100UL;
constexpr unsigned long kHoming         = 0x00000200UL;
constexpr unsigned long kHomed          = 0x00000400UL;
constexpr unsigned long kActive         = 0x20000000UL;
constexpr unsigned long kEnabled        = 0x80000000UL;
constexpr unsigned long kDigitalIn[6] =
{
    0x00100000UL, 0x00200000UL, 0x00400000UL, 0x00800000UL, 0x01000000UL, 0x02000000UL
};
} // namespace

// ---------------------------------------------------------------------------------------------------------------------

DCServoStatusFlags decodeDCServoStatus(const std::bitset<32>& bits)
{
    const unsigned long raw = bits.to_ulong();

    DCServoStatusFlags f;
    f.limit_cw        = (raw & kLimitCw)        != 0;
    f.limit_ccw       = (raw & kLimitCcw)       != 0;
    f.sw_limit_cw     = (raw & kSwLimitCw)      != 0;
    f.sw_limit_ccw    = (raw & kSwLimitCcw)     != 0;
    f.moving_cw       = (raw & kMovingCw)       != 0;
    f.moving_ccw      = (raw & kMovingCcw)      != 0;
    f.jogging_cw      = (raw & kJoggingCw)      != 0;
    f.jogging_ccw     = (raw & kJoggingCcw)     != 0;
    f.motor_connected = (raw & kMotorConnected) != 0;
    f.homing          = (raw & kHoming)         != 0;
    f.homed           = (raw & kHomed)          != 0;
    f.active          = (raw & kActive)         != 0;
    f.enabled         = (raw & kEnabled)        != 0;

    for (std::size_t i = 0; i < f.digital_in.size(); ++i)
        f.digital_in[i] = (raw & kDigitalIn[i]) != 0;

    return f;
}

// ---------------------------------------------------------------------------------------------------------------------

}} // END NAMESPACES

// ---------------------------------------------------------------------------------------------------------------------
