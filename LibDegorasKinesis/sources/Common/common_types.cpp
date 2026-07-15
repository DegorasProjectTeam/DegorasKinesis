/*
 *  LibDegorasKinesis - An extensible C++ library for controlling Thorlabs Kinesis motion devices.
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

// PROJECT INCLUDES
#include "LibDegorasKinesis/Common/common_types.h"
#include "LibDegorasKinesis/Helpers/json_utils.h"


// NAMESPACES
namespace dpkin
{
namespace types
{

// ---------------------------------------------------------------------------------------------------------------------
// SDK NUMERIC ADAPTERS

short toType(Channel channel)
{
    return static_cast<short>(channel);
}

short toType(TravelDirection direction)
{
    return static_cast<short>(direction);
}

short toType(JogMode jog_mode)
{
    return static_cast<short>(jog_mode);
}

short toType(StopMode stop_mode)
{
    return static_cast<short>(stop_mode);
}

int toType(PhysicalUnit unit)
{
    return static_cast<int>(unit);
}

// ---------------------------------------------------------------------------------------------------------------------
// STRINGIFICATION

std::string toString(OperationResult r)
{
    switch (r)
    {
        case OperationResult::OPERATION_OK:            return "OPERATION_OK";
        case OperationResult::NOT_CONNECTED:           return "NOT_CONNECTED";
        case OperationResult::DEVICE_NOT_FOUND:        return "DEVICE_NOT_FOUND";
        case OperationResult::ALREADY_CONNECTED:       return "ALREADY_CONNECTED";
        case OperationResult::SERIAL_IN_USE:           return "SERIAL_IN_USE";
        case OperationResult::LOAD_SETTINGS_ERROR:     return "LOAD_SETTINGS_ERROR";
        case OperationResult::START_POLLING_ERROR:     return "START_POLLING_ERROR";
        case OperationResult::OPERATION_TIMEOUT:       return "OPERATION_TIMEOUT";
        case OperationResult::THORLABS_INTERNAL_ERROR: return "THORLABS_INTERNAL_ERROR";
        case OperationResult::WORKER_ALREADY_RUNNING:  return "WORKER_ALREADY_RUNNING";
        case OperationResult::WORKER_NOT_RUNNING:      return "WORKER_NOT_RUNNING";
        case OperationResult::WORKER_START_ERROR:      return "WORKER_START_ERROR";
        case OperationResult::READ_FAILED:             return "READ_FAILED";
        case OperationResult::INVALID_CHANNEL:         return "INVALID_CHANNEL";
    }
    return "UNKNOWN_OPERATION_RESULT";
}

// ---------------------------------------------------------------------------------------------------------------------
// DATA STRUCTURES

VelocityProfile::VelocityProfile() :
    min(0),
    max(0),
    acc(0)
{}

JogParameters::JogParameters() :
    mode(JogMode::UNDEFINED),
    step_size(0),
    vel_profile(VelocityProfile()),
    stop_mode(StopMode::UNDEFINED)
{}

std::string JogParameters::toJsonStr(bool pretty) const
{
    std::ostringstream ss;
    ss << "{";
    ss << "\"mode\": " << static_cast<int>(this->mode) << ",";
    ss << "\"step_size\": " << this->step_size << ",";
    ss << "\"vel_profile\": {";
    ss << "\"min\": " << this->vel_profile.min << ",";
    ss << "\"acc\": " << this->vel_profile.acc << ",";
    ss << "\"max\": " << this->vel_profile.max;
    ss << "},";
    ss << "\"stop_mode\": " << static_cast<int>(this->stop_mode);
    ss << "}";
    return pretty ? json::prettify(ss.str()) : ss.str();
}

JogParameters JogParameters::fromJsonStr(const std::string& json_str)
{
    JogParameters params;
    params.mode = static_cast<JogMode>(json::getInt(json_str, "mode", static_cast<int>(JogMode::UNDEFINED)));
    params.step_size = json::getDouble(json_str, "step_size");
    const std::string vel = json::getObject(json_str, "vel_profile");
    params.vel_profile.min = json::getDouble(vel, "min");
    params.vel_profile.acc = json::getDouble(vel, "acc");
    params.vel_profile.max = json::getDouble(vel, "max");
    params.stop_mode = static_cast<StopMode>(json::getInt(json_str, "stop_mode", static_cast<int>(StopMode::UNDEFINED)));
    return params;
}

std::string DeviceError::toString() const
{
    std::ostringstream ss;
    ss << dpkin::types::toString(this->category)
       << ": kinesis=" << this->kinesis_code
       << " (" << this->context << ")";
    return ss.str();
}

// ---------------------------------------------------------------------------------------------------------------------

bool DeviceError::ok() const
{
    return this->category == OperationResult::OPERATION_OK;
}

}} // END NAMESPACES

// ---------------------------------------------------------------------------------------------------------------------
