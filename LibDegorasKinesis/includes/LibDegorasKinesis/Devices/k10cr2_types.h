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

#pragma once

// C++ INCLUDES
#include <string>

// PROJECT INCLUDES
#include "LibDegorasKinesis/Global/libdegoraskinesis_export.h"
#include "LibDegorasKinesis/Common/common_types.h"
#include "LibDegorasKinesis/Kinesis/motor_status.h"


// NAMESPACES
namespace dpkin
{
namespace types
{

// ---------------------------------------------------------------------------------------------------------------------

/// @brief Status of the single K10CR2 axis: decoded flags plus angular position. @c valid is false when the read failed.
struct LIBDEGORASKINESIS_EXPORT K10CR2ChannelStatus
{
    K10CR2ChannelStatus();

    /// @brief Serialise to a JSON object string (pretty-printed when @p pretty is true).
    std::string toJsonStr(bool pretty = false) const;

    /// @brief Parse from a JSON string produced by toJsonStr(); missing fields keep their defaults.
    static K10CR2ChannelStatus fromJsonStr(const std::string& json);

    bool valid;                        ///< false => read failed (READ_FAILED); other fields are not meaningful.
    kinesis::MotorStatusFlags flags;   ///< Decoded Kinesis motor status flags.
    int pos_raw;                       ///< Position in device units.
    double real_pos;                   ///< Angular position in degrees.
};

/// @brief Status of a K10CR2 device (its single rotation axis).
struct LIBDEGORASKINESIS_EXPORT K10CR2DeviceStatus
{
    K10CR2DeviceStatus();

    /// @brief Serialise to a JSON object string (pretty-printed when @p pretty is true).
    std::string toJsonStr(bool pretty = false) const;

    /// @brief Parse from a JSON string produced by toJsonStr(); missing fields keep their defaults.
    static K10CR2DeviceStatus fromJsonStr(const std::string& json);

    std::string serial_no;         ///< Controller serial number.
    bool connected;                ///< Whether the device was connected when the status was taken.
    K10CR2ChannelStatus chann;     ///< Axis status.
};

// ---------------------------------------------------------------------------------------------------------------------

}} // END NAMESPACES

// ---------------------------------------------------------------------------------------------------------------------
