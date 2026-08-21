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

// PROJECT INCLUDES
#include "DegorasKinesis/Devices/m30x_types.h"
#include "DegorasKinesis/Helpers/json_utils.h"


// NAMESPACES
namespace dpkin
{
namespace types
{

// ---------------------------------------------------------------------------------------------------------------------

M30XChannelStatus::M30XChannelStatus() :
    valid(false),
    flags(),
    pos_raw(0),
    real_pos(0.0)
{}

std::string M30XChannelStatus::toJsonStr(bool pretty) const
{
    std::ostringstream ss;
    ss << "{"
       << "\"valid\": " << (this->valid ? "true" : "false") << ","
       << "\"pos_raw\": " << this->pos_raw << ","
       << "\"real_pos\": " << this->real_pos << ","
       << "\"flags\": " << this->flags.toJsonStr()
       << "}";
    return pretty ? json::prettify(ss.str()) : ss.str();
}

M30XChannelStatus M30XChannelStatus::fromJsonStr(const std::string& json_str)
{
    M30XChannelStatus status;
    status.valid = json::getBool(json_str, "valid");
    status.pos_raw = json::getInt(json_str, "pos_raw");
    status.real_pos = json::getDouble(json_str, "real_pos");
    status.flags = kinesis::MotorStatusFlags::fromJsonStr(json::getObject(json_str, "flags"));
    return status;
}

// ---------------------------------------------------------------------------------------------------------------------

M30XDeviceStatus::M30XDeviceStatus() :
    serial_no(),
    connected(false),
    chann()
{}

std::string M30XDeviceStatus::toJsonStr(bool pretty) const
{
    std::ostringstream ss;
    ss << "{"
       << "\"serial_no\": \"" << this->serial_no << "\","
       << "\"connected\": " << (this->connected ? "true" : "false") << ","
       << "\"chann\": " << this->chann.toJsonStr()
       << "}";
    return pretty ? json::prettify(ss.str()) : ss.str();
}

M30XDeviceStatus M30XDeviceStatus::fromJsonStr(const std::string& json_str)
{
    M30XDeviceStatus status;
    status.serial_no = json::getString(json_str, "serial_no");
    status.connected = json::getBool(json_str, "connected");
    status.chann = M30XChannelStatus::fromJsonStr(json::getObject(json_str, "chann"));
    return status;
}

// ---------------------------------------------------------------------------------------------------------------------

}} // END NAMESPACES

// ---------------------------------------------------------------------------------------------------------------------
