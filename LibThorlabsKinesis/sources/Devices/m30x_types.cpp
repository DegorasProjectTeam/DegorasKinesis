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
#include "LibThorlabsKinesis/Devices/m30x_types.h"


// NAMESPACES
namespace thorlabs
{
namespace types
{

// ---------------------------------------------------------------------------------------------------------------------

M30XChannelStatus::M30XChannelStatus() :
    valid(false),
    flags(),
    pos_raw(0),
    pos_mm(0.0)
{}

std::string M30XChannelStatus::toJsonStr() const
{
    std::ostringstream ss;
    ss << "{"
       << "\"valid\": " << (this->valid ? "true" : "false") << ","
       << "\"pos_raw\": " << this->pos_raw << ","
       << "\"pos_mm\": " << this->pos_mm << ","
       << "\"flags\": " << this->flags.toJsonStr()
       << "}";
    return ss.str();
}

// ---------------------------------------------------------------------------------------------------------------------

M30XDeviceStatus::M30XDeviceStatus() :
    serial_no(),
    connected(false),
    chann()
{}

std::string M30XDeviceStatus::toJsonStr() const
{
    std::ostringstream ss;
    ss << "{"
       << "\"serial_no\": \"" << this->serial_no << "\","
       << "\"connected\": " << (this->connected ? "true" : "false") << ","
       << "\"chann\": " << this->chann.toJsonStr()
       << "}";
    return ss.str();
}

// ---------------------------------------------------------------------------------------------------------------------

}} // END NAMESPACES

// ---------------------------------------------------------------------------------------------------------------------
