/*
 *  DegorasKinesis - An extensible C++ library for controlling Thorlabs Kinesis motion devices.
 *
 *  Developed as free software by and for the Spanish Navy Observatory SLR station (SFEL) in San Fernando.
 *
 *  Copyright (C) 2024-2026 Degoras Project Team
 *                          < Ángel Vera Herrera, avera@roa.es - angelvh.engr@gmail.com >
 *                          < Jesús Relinque Madroñal, jrelinque@roa.es >
 *
 *  This program is free software: you can redistribute it and/or modify it under the terms of the GNU General
 *  Public License as published by the Free Software Foundation, either version 3 of the License, or (at your
 *  option) any later version.
 *
 *  This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the
 *  implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 *  for more details.
 *
 *  You should have received a copy of the GNU General Public License along with this program. If not, see
 *  <https://www.gnu.org/licenses/>.
 *
 *  SPDX-License-Identifier: GPL-3.0-or-later
 */

// C++ INCLUDES
#include <sstream>
#include <vector>

// PROJECT INCLUDES
#include "DegorasKinesis/Kinesis/motor_status.h"
#include "DegorasKinesis/Helpers/json_utils.h"


// NAMESPACES
namespace dpkin
{
namespace kinesis
{

// ---------------------------------------------------------------------------------------------------------------------
// Kinesis MOT status-word bit masks (shared across Kinesis motion modules; mirror the Kinesis header documentation).

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

MotorStatusFlags decodeMotorStatus(const std::bitset<32>& bits)
{
    const unsigned long raw = bits.to_ulong();

    MotorStatusFlags f;
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

std::string MotorStatusFlags::toJsonStr(bool pretty) const
{
    const auto b = [](bool v) { return v ? "true" : "false"; };

    std::ostringstream ss;
    ss << "{"
       << "\"limit_cw\": " << b(this->limit_cw) << ","
       << "\"limit_ccw\": " << b(this->limit_ccw) << ","
       << "\"sw_limit_cw\": " << b(this->sw_limit_cw) << ","
       << "\"sw_limit_ccw\": " << b(this->sw_limit_ccw) << ","
       << "\"moving_cw\": " << b(this->moving_cw) << ","
       << "\"moving_ccw\": " << b(this->moving_ccw) << ","
       << "\"jogging_cw\": " << b(this->jogging_cw) << ","
       << "\"jogging_ccw\": " << b(this->jogging_ccw) << ","
       << "\"motor_connected\": " << b(this->motor_connected) << ","
       << "\"homing\": " << b(this->homing) << ","
       << "\"homed\": " << b(this->homed) << ","
       << "\"active\": " << b(this->active) << ","
       << "\"enabled\": " << b(this->enabled) << ","
       << "\"digital_in\": [";
    for (std::size_t i = 0; i < this->digital_in.size(); ++i)
    {
        ss << b(this->digital_in[i]);
        if (i + 1 < this->digital_in.size())
            ss << ",";
    }
    ss << "]}";
    return pretty ? json::prettify(ss.str()) : ss.str();
}

MotorStatusFlags MotorStatusFlags::fromJsonStr(const std::string& json_str)
{
    MotorStatusFlags f;
    f.limit_cw        = json::getBool(json_str, "limit_cw");
    f.limit_ccw       = json::getBool(json_str, "limit_ccw");
    f.sw_limit_cw     = json::getBool(json_str, "sw_limit_cw");
    f.sw_limit_ccw    = json::getBool(json_str, "sw_limit_ccw");
    f.moving_cw       = json::getBool(json_str, "moving_cw");
    f.moving_ccw      = json::getBool(json_str, "moving_ccw");
    f.jogging_cw      = json::getBool(json_str, "jogging_cw");
    f.jogging_ccw     = json::getBool(json_str, "jogging_ccw");
    f.motor_connected = json::getBool(json_str, "motor_connected");
    f.homing          = json::getBool(json_str, "homing");
    f.homed           = json::getBool(json_str, "homed");
    f.active          = json::getBool(json_str, "active");
    f.enabled         = json::getBool(json_str, "enabled");

    const std::vector<bool> din = json::getBoolArray(json_str, "digital_in");
    for (std::size_t i = 0; i < f.digital_in.size(); ++i)
        f.digital_in[i] = (i < din.size()) ? din[i] : false;

    return f;
}

// ---------------------------------------------------------------------------------------------------------------------

bool MotorStatusFlags::isMoving() const
{
    return this->moving_cw || this->moving_ccw;
}

bool MotorStatusFlags::isJogging() const
{
    return this->jogging_cw || this->jogging_ccw;
}

}} // END NAMESPACES

// ---------------------------------------------------------------------------------------------------------------------
