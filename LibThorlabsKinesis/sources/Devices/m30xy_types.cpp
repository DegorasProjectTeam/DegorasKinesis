/*
 *  LibThorlabsKinesis - An extensible C++ library for controlling Thorlabs Kinesis motion devices.
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
#include "LibThorlabsKinesis/Devices/m30xy_types.h"
#include "LibThorlabsKinesis/Helpers/json_utils.h"


// NAMESPACES
namespace thorlabs
{
namespace types
{

// ---------------------------------------------------------------------------------------------------------------------

M30XYChannelStatus::M30XYChannelStatus() :
    channel(Channel::X_CHANNEL),
    valid(false),
    flags(),
    pos_raw(0),
    pos_mm(0.0)
{}

M30XYChannelStatus::M30XYChannelStatus(Channel ch) :
    M30XYChannelStatus()
{
    this->channel = ch;
}

std::string M30XYChannelStatus::toJsonStr(bool pretty) const
{
    std::ostringstream ss;
    ss << "{"
       << "\"channel\": " << static_cast<int>(this->channel) << ","
       << "\"valid\": " << (this->valid ? "true" : "false") << ","
       << "\"pos_raw\": " << this->pos_raw << ","
       << "\"pos_mm\": " << this->pos_mm << ","
       << "\"flags\": " << this->flags.toJsonStr()
       << "}";
    return pretty ? json::prettify(ss.str()) : ss.str();
}

M30XYChannelStatus M30XYChannelStatus::fromJsonStr(const std::string& json_str)
{
    M30XYChannelStatus status;
    status.channel = static_cast<Channel>(json::getInt(json_str, "channel", static_cast<int>(Channel::X_CHANNEL)));
    status.valid = json::getBool(json_str, "valid");
    status.pos_raw = json::getInt(json_str, "pos_raw");
    status.pos_mm = json::getDouble(json_str, "pos_mm");
    status.flags = dcservo::DCServoStatusFlags::fromJsonStr(json::getObject(json_str, "flags"));
    return status;
}

// ---------------------------------------------------------------------------------------------------------------------

M30XYDeviceStatus::M30XYDeviceStatus() :
    serial_no(),
    connected(false),
    chann_x(Channel::X_CHANNEL),
    chann_y(Channel::Y_CHANNEL)
{}

std::string M30XYDeviceStatus::toJsonStr(bool pretty) const
{
    std::ostringstream ss;
    ss << "{"
       << "\"serial_no\": \"" << this->serial_no << "\","
       << "\"connected\": " << (this->connected ? "true" : "false") << ","
       << "\"chann_x\": " << this->chann_x.toJsonStr() << ","
       << "\"chann_y\": " << this->chann_y.toJsonStr()
       << "}";
    return pretty ? json::prettify(ss.str()) : ss.str();
}

M30XYDeviceStatus M30XYDeviceStatus::fromJsonStr(const std::string& json_str)
{
    M30XYDeviceStatus status;
    status.serial_no = json::getString(json_str, "serial_no");
    status.connected = json::getBool(json_str, "connected");
    status.chann_x = M30XYChannelStatus::fromJsonStr(json::getObject(json_str, "chann_x"));
    status.chann_y = M30XYChannelStatus::fromJsonStr(json::getObject(json_str, "chann_y"));
    return status;
}

// ---------------------------------------------------------------------------------------------------------------------

}} // END NAMESPACES

// ---------------------------------------------------------------------------------------------------------------------
