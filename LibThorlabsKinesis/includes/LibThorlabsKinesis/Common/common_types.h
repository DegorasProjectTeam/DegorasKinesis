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
#include <cstdint>
#include <string>
#include <vector>

// PROJECT INCLUDES
#include "LibThorlabsKinesis/libthorlabskinesis_global.h"


// NAMESPACES
namespace thorlabs
{
namespace types
{

// ---------------------------------------------------------------------------------------------------------------------
// TYPE ALIASES

using ThorlabsSN = std::string;                  ///< Thorlabs device serial number.
using ThorlabsSNList = std::vector<ThorlabsSN>;  ///< List of Thorlabs serial numbers.

// ---------------------------------------------------------------------------------------------------------------------
// ENUMERATIONS

/**
 * @brief High-level, hardware-agnostic result of a library operation.
 * @note Every enumerator has an explicit, stable value so it is safe to log or compare across versions.
 *       When the category is THORLABS_INTERNAL_ERROR, the raw vendor code is preserved in DeviceError::kinesis_code.
 */
enum class OperationResult : std::uint8_t
{
    OPERATION_OK            = 0,   ///< Operation completed successfully.
    NOT_CONNECTED           = 1,   ///< The device is not connected.
    DEVICE_NOT_FOUND        = 2,   ///< The requested serial number was not found during discovery.
    ALREADY_CONNECTED       = 3,   ///< This object already owns an open connection (idempotent re-connect).
    SERIAL_IN_USE           = 4,   ///< Another live object or process already owns this serial number.
    LOAD_SETTINGS_ERROR     = 5,   ///< Loading device or named settings failed.
    START_POLLING_ERROR     = 6,   ///< Starting the SDK status polling failed.
    OPERATION_TIMEOUT       = 7,   ///< A bounded wait expired before its condition was met.
    THORLABS_INTERNAL_ERROR = 8,   ///< A Thorlabs/Kinesis call failed; raw code is in DeviceError::kinesis_code.
    WORKER_ALREADY_RUNNING  = 9,   ///< The status-polling worker was already running.
    WORKER_NOT_RUNNING      = 10,  ///< The status-polling worker was not running.
    WORKER_START_ERROR      = 11,  ///< The status-polling worker thread could not be started.
    READ_FAILED             = 12,  ///< A cache read was stale or comms were lost; the value is NOT valid.
    INVALID_CHANNEL         = 13   ///< The requested channel does not exist on this device.
};

/**
 * @brief Human-readable name for an OperationResult.
 * @param r The result code to stringify.
 * @return The enumerator name, or "UNKNOWN_OPERATION_RESULT" for an unrecognised value.
 */
LIBTHORLABSKINESIS_EXPORT std::string toString(OperationResult r);

/// Motor axis / channel identifier (1-based, matching Kinesis channel numbering).
enum class Channel : std::uint8_t
{
    X_CHANNEL = 1,   ///< First / X axis.
    Y_CHANNEL = 2    ///< Second / Y axis.
};

/// Direction of travel for a jog or move.
enum class TravelDirection : std::uint8_t
{
    UNDEFINED = 0x00,   ///< Undefined / unset direction.
    FORWARDS  = 0x01,   ///< Move in the forward direction.
    REVERSE   = 0x02    ///< Move in the reverse direction.
};

/// Jogging mode.
enum class JogMode : std::uint8_t
{
    UNDEFINED   = 0x00,   ///< Undefined / unset.
    CONTINUOUS  = 0x01,   ///< Continuous jog while triggered.
    SINGLE_STEP = 0x02    ///< Single fixed-size step per jog request.
};

/// How a motion is stopped.
enum class StopMode : std::uint8_t
{
    UNDEFINED = 0x00,   ///< Undefined / unset.
    IMMEDIATE = 0x01,   ///< Stop abruptly.
    PROFILED  = 0x02    ///< Stop using the configured deceleration profile.
};

/// Physical quantity selector for unit conversion.
enum class PhysicalUnit : std::uint8_t
{
    DISTANCE     = 0,   ///< Distance (mm).
    VELOCITY     = 1,   ///< Velocity (mm/s).
    ACCELERATION = 2    ///< Acceleration (mm/s^2).
};

// ---------------------------------------------------------------------------------------------------------------------
// DATA STRUCTURES

/// Velocity profile in physical units.
struct LIBTHORLABSKINESIS_EXPORT VelocityProfile
{
    VelocityProfile();

    double min;   ///< Minimum velocity (mm/s).
    double max;   ///< Maximum velocity (mm/s).
    double acc;   ///< Acceleration (mm/s^2).
};

/// Parameters describing a jog operation.
struct LIBTHORLABSKINESIS_EXPORT JogParameters
{
    JogParameters();

    /// @brief Serialise the parameters to a JSON string.
    std::string toJsonStr() const;

    JogMode mode;                  ///< Jogging mode.
    double step_size;              ///< Step size in millimetres (single-step mode only).
    VelocityProfile vel_profile;   ///< Velocity profile in physical units.
    StopMode stop_mode;            ///< Stop mode applied at the end of a jog.
};

/**
 * @brief Self-contained error returned by adapter-level calls.
 * @note Returned by value and complete on its own: it carries both the high-level @ref OperationResult category
 *       and the raw Thorlabs/Kinesis code. The library keeps NO shared or per-instance "last error" state, so a
 *       returned DeviceError can never be aliased or overwritten by a concurrent operation.
 */
struct LIBTHORLABSKINESIS_EXPORT DeviceError
{
    OperationResult category = OperationResult::OPERATION_OK;   ///< High-level result category.
    short kinesis_code = 0;                                     ///< Raw Thorlabs/Kinesis return code (FT_*/BDC_*).
    std::string context;                                        ///< Human context, e.g. "BDC_Home(ch=1)".

    /// @brief True when the operation succeeded.
    bool ok() const { return this->category == OperationResult::OPERATION_OK; }

    /// @brief Human-readable form: "<category>: kinesis=<code> (<context>)".
    std::string toString() const;
};

// ---------------------------------------------------------------------------------------------------------------------
// SDK NUMERIC ADAPTERS
// Map library enums to the raw numeric values the Kinesis C API expects. These are pure casts with no vendor-header
// dependency, so they belong in the device-agnostic layer; the MOT_* struct conversions live in the Layer-2 adapter.

LIBTHORLABSKINESIS_EXPORT short toType(TravelDirection direction);

LIBTHORLABSKINESIS_EXPORT short toType(Channel channel);

LIBTHORLABSKINESIS_EXPORT short toType(JogMode jog_mode);

LIBTHORLABSKINESIS_EXPORT short toType(StopMode stop_mode);

LIBTHORLABSKINESIS_EXPORT int toType(PhysicalUnit unit);

// ---------------------------------------------------------------------------------------------------------------------

}} // END NAMESPACES

// ---------------------------------------------------------------------------------------------------------------------
