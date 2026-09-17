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
#include <chrono>
#include <functional>
#include <memory>
#include <mutex>
#include <string>

// PROJECT INCLUDES
#include "DegorasKinesis/Global/degoraskinesis_export.h"
#include "DegorasKinesis/Common/common_types.h"
#include "DegorasKinesis/DCServo/dcservo_channel.h"
#include "DegorasKinesis/Kinesis/motor_status.h"
#include "DegorasKinesis/Devices/i_motion_device.h"
#include "DegorasKinesis/Devices/m30x_types.h"
#include "DegorasKinesis/Helpers/status_poller.h"


// NAMESPACES
namespace dpkin
{

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Driver for the Thorlabs M30X single-axis benchtop DC servo stage.
 * @details A device personality composed of ONE @ref dcservo::DCServoChannel adapter, demonstrating that the
 *          channel adapter and generic infrastructure compose for an arbitrary axis count without a base class.
 *          The channel-indexed @ref IMotionDevice methods accept only Channel::X_CHANNEL; any other channel returns
 *          OperationResult::INVALID_CHANNEL.
 *
 * Ownership & lifetime mirror M30XY: no device I/O in the constructor, deterministic and bounded shutdown,
 * non-copyable and non-movable.
 */
class DEGORASKINESIS_EXPORT M30X final : public IMotionDevice
{
public:

    /// Callback delivering each poll's (result, status). Invoked on the worker thread, holding no lock.
    using NewStatusCb = std::function<void(types::OperationResult, const types::M30XDeviceStatus&)>;

    explicit M30X(const std::string& serial_no);
    ~M30X() override;

    M30X(const M30X&) = delete;
    M30X& operator=(const M30X&) = delete;
    M30X(M30X&&) = delete;
    M30X& operator=(M30X&&) = delete;

    /// @name IMotionDevice interface
    /// Behaviour is documented on IMotionDevice. This single-axis device accepts only Channel::X_CHANNEL; any
    /// other channel returns OperationResult::INVALID_CHANNEL.
    /// @{
    std::string getSerialNo() const override;
    short getChannelCount() const override;
    bool isConnected() const override;
    types::OperationResult doConnect(const DeviceConfig& cfg = DeviceConfig{}) override;
    types::OperationResult doDisconnect() override;
    types::OperationResult doEnable(types::Channel ch, bool enable) override;
    types::OperationResult doHome(types::Channel ch) override;
    types::OperationResult doStop(types::Channel ch, types::StopMode mode) override;
    types::OperationResult doJog(types::Channel ch, types::TravelDirection direction) override;
    types::OperationResult doMoveAbsolute(types::Channel ch, double real_pos) override;
    types::OperationResult doMoveRelative(types::Channel ch, double real_pos) override;
    types::OperationResult doConfigureVelocity(types::Channel ch, const types::VelocityProfile& profile) override;
    types::OperationResult doConfigureJog(types::Channel ch, const types::JogParameters& params) override;
    types::OperationResult getChannelPosition(types::Channel ch, double& real_pos) override;
    types::OperationResult getChannelFlags(types::Channel ch, kinesis::MotorStatusFlags& flags) override;
    /// @}

    // -- Device-unit control (usable even without a loaded stage/settings profile) --
    /// @brief Whether a stage/settings profile is loaded, i.e. whether real-world-unit (mm) operations are
    ///        available. When false (e.g. a simulated device with no profile assigned) the real-world-unit methods
    ///        (doMoveAbsolute/doMoveRelative/getChannelPosition) return OperationResult::LOAD_SETTINGS_ERROR; drive
    ///        and read the stage with the device-unit methods below (raw motor counts) instead.
    bool hasRealUnits() const;
    /// @brief Absolute move to a raw device-unit (motor count) position. Works without a profile. Non-blocking.
    types::OperationResult doMoveAbsoluteDeviceUnits(types::Channel ch, int device_units);
    /// @brief Relative move by a raw device-unit (motor count) displacement. Works without a profile. Non-blocking.
    types::OperationResult doMoveRelativeDeviceUnits(types::Channel ch, int device_units);
    /// @brief Read the position in raw device units (motor counts). Works without a profile.
    types::OperationResult getChannelPositionDeviceUnits(types::Channel ch, int& device_units);

    // -- Conveniences --
    /// @brief Read the device status (its single axis).
    types::OperationResult getDeviceStatus(types::M30XDeviceStatus& status);
    /// @brief Block until the axis reports homed, or @p timeout elapses (OPERATION_TIMEOUT).
    types::OperationResult waitForHomed(std::chrono::milliseconds timeout);
    /// @brief Block until the axis stops moving, or @p timeout elapses (OPERATION_TIMEOUT).
    types::OperationResult waitForMoveFinished(std::chrono::milliseconds timeout);

    /// @brief Register the status callback. @warning It only fires while startStatusPolling() is active.
    types::OperationResult setNewStatusCb(NewStatusCb cb);
    types::OperationResult startStatusPolling();   ///< Start the background worker that drives the status callback.
    types::OperationResult stopStatusPolling();    ///< Stop the background status worker (bounded; never blocks).
    bool isStatusPollingRunning() const;           ///< Whether the status worker is currently running.

    /// @brief Enumerate the serial numbers of connected M30X controllers.
    static types::OperationResult getDeviceList(types::ThorlabsSNList& list);

    /// @brief Whether @p serial belongs to a M30X (Thorlabs type id 105). Pure prefix check;
    ///        contacts no device. Useful to validate a user-supplied serial before doConnect().
    static bool isCompatibleSerial(const types::ThorlabsSN& serial);

private:

    /// @brief OPERATION_OK if @p ch is this device's only axis, otherwise INVALID_CHANNEL.
    /// @brief Validate a channel id against this single-axis device. Static: it reads no member state.
    static types::OperationResult checkChannel(types::Channel ch);
    types::OperationResult fillChannelStatus(types::M30XChannelStatus& status);

    std::string serial_no_;
    int poll_rate_ms_;
    bool i_own_open_;                                ///< True if THIS object opened the connection (vs aliasing it).
    bool units_ready_;                               ///< True if a stage/settings profile loaded (real-world units OK).
    dcservo::DCServoChannel chan_;
    mutable std::mutex cb_mtx_;
    NewStatusCb cb_;
    StatusPoller<types::M30XDeviceStatus> poller_;   ///< Declared last -> destroyed first (worker stopped before chan_).
};

using M30XPtr = std::shared_ptr<M30X>;

// ---------------------------------------------------------------------------------------------------------------------

} // END NAMESPACES

// ---------------------------------------------------------------------------------------------------------------------
