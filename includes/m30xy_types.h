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

#pragma once

// C++ INCLUDES
#include <string>

// PROJECT INCLUDES
#include "libthorlabskinesis_global.h"
#include "common_types.h"
#include "dcservo_status.h"


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
