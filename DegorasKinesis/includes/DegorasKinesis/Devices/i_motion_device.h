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

#pragma once

// C++ INCLUDES
#include <string>
#include <vector>

// PROJECT INCLUDES
#include "DegorasKinesis/Global/degoraskinesis_export.h"
#include "DegorasKinesis/Common/common_types.h"
#include "DegorasKinesis/Kinesis/motor_status.h"


// NAMESPACES
namespace dpkin
{

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Connection configuration for a motion device.
 * @note @ref settings holds the per-channel Kinesis named-settings profile; an empty entry (or a missing one) loads
 *       the device's default settings for that channel. Size it to the device's channel count (1 for M30X, 2 for
 *       M30XY); a per-channel vector keeps the generic surface free of any fixed channel count.
 */
struct DeviceConfig
{
    std::vector<std::string> settings;   ///< Per-channel named settings; "" / missing -> default settings.
    int poll_rate_ms = 500;              ///< SDK status-polling rate.
};

/**
 * @brief Hardware-agnostic, motion-oriented device interface for library consumers.
 * @note Consume by pointer or reference only (it is polymorphic; never copy or slice it). Channel-indexed: an axis
 *       is identified by (device, Channel). A future Kinesis module implements this interface with its own internal
 *       adapter, reusing the generic layer unchanged.
 * @warning Status reads return READ_FAILED (not a misleading zero) when the device's last-message timer indicates
 *          the cached value is stale / communications were lost.
 */
class DEGORASKINESIS_EXPORT IMotionDevice
{
public:

    virtual ~IMotionDevice() = default;

    // THE DOCUMENTED RULE, NOW ENFORCED BY THE COMPILER. The note above says "never copy or slice it", and until
    // these lines that was a request: declaring only a destructor leaves the copy operations implicitly available,
    // so `IMotionDevice& a = dev; SomeBase b = a;` sliced an open device handle in two without a diagnostic. The
    // concrete devices already delete all four (see m30xy.h), but a base class must not depend on its derivatives
    // to enforce its own contract -- nothing stops a future device from forgetting.
    //
    // Deleting rather than protecting: there is no case in this library for copying a device, and a deleted
    // function gives a clearer error at the call site than an inaccessible one.
    IMotionDevice(const IMotionDevice&) = delete;
    IMotionDevice& operator=(const IMotionDevice&) = delete;
    IMotionDevice(IMotionDevice&&) = delete;
    IMotionDevice& operator=(IMotionDevice&&) = delete;

protected:

    // Declaring any of the above suppresses the implicit default constructor, so the derived devices stop
    // compiling until it is put back. PROTECTED rather than public: this is an abstract interface, and only a
    // derived class has any business constructing the base subobject.
    IMotionDevice() = default;

public:

    /// @brief Serial number of the controller this object targets.
    virtual std::string getSerialNo() const = 0;

    /// @brief Number of motion channels (axes) the device exposes.
    virtual short getChannelCount() const = 0;

    /// @brief Whether the device currently has an open connection.
    virtual bool isConnected() const = 0;

    /**
     * @brief Open and initialise the device. Idempotent for the object that owns the connection.
     * @param cfg Optional per-channel named settings and polling rate.
     * @return OPERATION_OK on success; ALREADY_CONNECTED if this object already owns it; SERIAL_IN_USE if another
     *         object holds it; DEVICE_NOT_FOUND / LOAD_SETTINGS_ERROR / START_POLLING_ERROR on failure.
     */
    virtual types::OperationResult doConnect(const DeviceConfig& cfg = DeviceConfig{}) = 0;

    /// @brief Stop, close and release the device. Returns NOT_CONNECTED if it was not connected.
    virtual types::OperationResult doDisconnect() = 0;

    /// @brief Enable (energise) or disable a channel. Disabling stops the channel first.
    virtual types::OperationResult doEnable(types::Channel ch, bool enable) = 0;

    /// @brief Begin homing a channel. Non-blocking; use a wait helper to block until homed.
    virtual types::OperationResult doHome(types::Channel ch) = 0;

    /// @brief Stop a channel, immediately or using its deceleration profile.
    virtual types::OperationResult doStop(types::Channel ch, types::StopMode mode) = 0;

    /// @brief Begin jogging a channel in the given direction (per its configured jog parameters).
    virtual types::OperationResult doJog(types::Channel ch, types::TravelDirection direction) = 0;

    /// @brief Move a channel to an absolute position, in real-world units (mm for linear stages, degrees for
    ///        rotation stages), relative to the home datum. Non-blocking.
    ///
    /// @warning OPERATION_OK MEANS "THE SDK ACCEPTED THE COMMAND", NOT "THE MOVE HAPPENED", and the two really do
    ///          come apart. Measured against the Kinesis Simulator: a move commanded while the axis was still
    ///          homing returned OPERATION_OK here while the device logged
    ///          "Motor Exception: Simulator is already in motion" and never moved. The Thorlabs SDK does not
    ///          report that rejection back to the caller, so this layer cannot either.
    ///
    ///          To know that a move actually took place, wait for the axis to settle (waitForMoveFinished) and
    ///          then read the position back (getChannelPosition). And make sure the axis is idle before
    ///          commanding: a device that is homing or moving silently discards the command.
    virtual types::OperationResult doMoveAbsolute(types::Channel ch, double real_pos) = 0;

    /// @brief Move a channel by a relative distance, in real-world units (mm for linear stages, degrees for
    ///        rotation stages). Non-blocking.
    virtual types::OperationResult doMoveRelative(types::Channel ch, double real_pos) = 0;

    /// @brief Configure a channel's velocity profile (physical units).
    virtual types::OperationResult doConfigureVelocity(types::Channel ch, const types::VelocityProfile& profile) = 0;

    /// @brief Configure a channel's jog parameters.
    virtual types::OperationResult doConfigureJog(types::Channel ch, const types::JogParameters& params) = 0;

    /// @brief Read a channel's position, in real-world units (mm for linear stages, degrees for rotation stages).
    ///        @return READ_FAILED if the cached value is stale.
    virtual types::OperationResult getChannelPosition(types::Channel ch, double& real_pos) = 0;

    /// @brief Read a channel's decoded status flags. @return READ_FAILED if the cached value is stale.
    virtual types::OperationResult getChannelFlags(types::Channel ch, kinesis::MotorStatusFlags& flags) = 0;
};

// ---------------------------------------------------------------------------------------------------------------------

} // END NAMESPACES

// ---------------------------------------------------------------------------------------------------------------------
