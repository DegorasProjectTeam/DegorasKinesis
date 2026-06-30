/*
 *  LibThorlabsKinesis - An extensible C++ library for controlling Thorlabs Kinesis motion devices.
 *
 *  Copyright (C) 2018-2026 Degoras Project Team
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

#pragma once

// C++ INCLUDES
#include <string>

// PROJECT INCLUDES
#include "LibThorlabsKinesis/libthorlabskinesis_global.h"
#include "LibThorlabsKinesis/Common/common_types.h"
#include "LibThorlabsKinesis/DCServo/dcservo_status.h"


// NAMESPACES
namespace thorlabs
{
namespace types
{

// ---------------------------------------------------------------------------------------------------------------------

/// @brief Status of a single M30XY axis: decoded flags plus position. @c valid is false when the read failed.
struct LIBTHORLABSKINESIS_EXPORT M30XYChannelStatus
{
    M30XYChannelStatus();
    explicit M30XYChannelStatus(Channel ch);

    /// @brief Serialise to a JSON object string.
    std::string toJsonStr() const;

    Channel channel;                     ///< Axis this status describes.
    bool valid;                          ///< false => read failed (READ_FAILED); other fields are not meaningful.
    dcservo::DCServoStatusFlags flags;   ///< Decoded DC Servo status flags.
    int pos_raw;                         ///< Position in device units.
    double pos_mm;                       ///< Position in millimetres.
};

/// @brief Aggregate status of an M30XY device (both axes).
struct LIBTHORLABSKINESIS_EXPORT M30XYDeviceStatus
{
    M30XYDeviceStatus();

    /// @brief Serialise to a JSON object string.
    std::string toJsonStr() const;

    std::string serial_no;        ///< Controller serial number.
    bool connected;               ///< Whether the device was connected when the status was taken.
    M30XYChannelStatus chann_x;   ///< X-axis status.
    M30XYChannelStatus chann_y;   ///< Y-axis status.
};

// ---------------------------------------------------------------------------------------------------------------------

}} // END NAMESPACES

// ---------------------------------------------------------------------------------------------------------------------
