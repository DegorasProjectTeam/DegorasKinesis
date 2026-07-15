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
#include <array>
#include <bitset>
#include <string>

// PROJECT INCLUDES
#include "LibDegorasKinesis/Global/libdegoraskinesis_export.h"


// NAMESPACES
namespace dpkin
{
namespace kinesis
{

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Decoded Kinesis motor status word.
 * @note This is the standard Kinesis "MOT" 32-bit status word, whose bit layout is shared across Kinesis motion
 *       modules (e.g. Benchtop DC Servo and Integrated Stepper decode identically), so this type and its decoder are
 *       reused by every Layer-2 adapter rather than being redefined per module.
 */
struct LIBDEGORASKINESIS_EXPORT MotorStatusFlags
{
    bool limit_cw = false;          ///< CW hardware limit switch active (0x00000001).
    bool limit_ccw = false;         ///< CCW hardware limit switch active (0x00000002).
    bool sw_limit_cw = false;       ///< CW software limit reached (0x00000004).
    bool sw_limit_ccw = false;      ///< CCW software limit reached (0x00000008).
    bool moving_cw = false;         ///< Shaft moving clockwise (0x00000010).
    bool moving_ccw = false;        ///< Shaft moving counter-clockwise (0x00000020).
    bool jogging_cw = false;        ///< Shaft jogging clockwise (0x00000040).
    bool jogging_ccw = false;       ///< Shaft jogging counter-clockwise (0x00000080).
    bool motor_connected = false;   ///< Motor detected/connected (0x00000100).
    bool homing = false;            ///< Axis currently homing (0x00000200, bit 9).
    bool homed = false;             ///< Axis has completed homing (0x00000400, bit 10).
    bool active = false;            ///< Device/axis active (0x20000000, bit 29).
    bool enabled = false;           ///< Channel enabled (0x80000000, bit 31).
    std::array<bool, 6> digital_in{};   ///< Digital inputs 1-6 (0x00100000 .. 0x02000000, bits 20-25).

    /// @brief True if the shaft is moving in either direction.
    bool isMoving() const;

    /// @brief True if the shaft is jogging in either direction.
    bool isJogging() const;

    /// @brief Serialise the flags and digital inputs to a JSON object string (pretty-printed when @p pretty is true).
    std::string toJsonStr(bool pretty = false) const;

    /// @brief Parse flags from a JSON string produced by toJsonStr(); missing fields default to false.
    static MotorStatusFlags fromJsonStr(const std::string& json);
};

/**
 * @brief Decode a raw Kinesis 32-bit motor status word into named flags.
 * @param bits The status word as returned by a module's GetStatusBits (e.g. BDC_GetStatusBits, ISC_GetStatusBits).
 * @return The decoded flags.
 * @note Pure function with no vendor-SDK dependency, so it is unit-testable without hardware.
 */
LIBDEGORASKINESIS_EXPORT MotorStatusFlags decodeMotorStatus(const std::bitset<32>& bits);

// ---------------------------------------------------------------------------------------------------------------------

}} // END NAMESPACES

// ---------------------------------------------------------------------------------------------------------------------
