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
#include <chrono>
#include <functional>
#include <memory>
#include <mutex>
#include <string>

// PROJECT INCLUDES
#include "LibDegorasKinesis/Global/libdegoraskinesis_export.h"
#include "LibDegorasKinesis/Common/common_types.h"
#include "LibDegorasKinesis/DCServo/dcservo_channel.h"
#include "LibDegorasKinesis/Kinesis/motor_status.h"
#include "LibDegorasKinesis/Devices/i_motion_device.h"
#include "LibDegorasKinesis/Devices/m30xy_types.h"
#include "LibDegorasKinesis/Helpers/status_poller.h"


// NAMESPACES
namespace dpkin
{

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Driver for the Thorlabs M30XY dual-axis (X/Y) benchtop DC servo stage.
 * @details A device personality composed of two @ref dcservo::DCServoChannel adapters (X and Y). It adds only the
 *          dual-axis fan-out/aggregation on top of the per-axis logic that lives once in the adapter; there is no
 *          shared device base class.
 *
 * Ownership & lifetime: the constructor performs no device I/O. The destructor stops the status worker (bounded) and
 * disconnects deterministically. Non-copyable and non-movable.
 *
 * @warning Destroying the device from inside its own status callback, or while a status read is wedged in a blocking
 *          SDK call on dead hardware, is best-effort: shutdown is bounded (it never hangs) but the wedged worker is
 *          detached and leaks. Under normal operation and against the simulator this never occurs.
 */
class LIBDEGORASKINESIS_EXPORT M30XY final : public IMotionDevice
{
public:

    /// Callback delivering each poll's (result, aggregate status). Invoked on the worker thread, holding no lock.
    using NewStatusCb = std::function<void(types::OperationResult, const types::M30XYDeviceStatus&)>;

    explicit M30XY(const std::string& serial_no);
    ~M30XY() override;

    M30XY(const M30XY&) = delete;
    M30XY& operator=(const M30XY&) = delete;
    M30XY(M30XY&&) = delete;
    M30XY& operator=(M30XY&&) = delete;

    /// @name IMotionDevice interface
    /// Behaviour is documented on IMotionDevice; M30XY dispatches each call to the addressed channel (X or Y).
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

    // -- Dual-axis conveniences (fan-out / aggregation; not on the interface) --
    types::OperationResult doEnableChannels(bool enable);   ///< Enable/disable both axes (first-error-wins).
    types::OperationResult doHomeAll();                     ///< Home both axes (first-error-wins).
    types::OperationResult doStopAll(types::StopMode mode); ///< Stop both axes (first-error-wins).

    /// @brief Read the full decoded status of one axis.
    types::OperationResult getChannelStatus(types::Channel ch, types::M30XYChannelStatus& status);
    /// @brief Read the aggregate status of both axes.
    types::OperationResult getDeviceStatus(types::M30XYDeviceStatus& status);
    /// @brief Block until the axis reports homed, or @p timeout elapses (OPERATION_TIMEOUT).
    types::OperationResult waitForHomed(types::Channel ch, std::chrono::milliseconds timeout);
    /// @brief Block until the axis stops moving, or @p timeout elapses (OPERATION_TIMEOUT).
    types::OperationResult waitForMoveFinished(types::Channel ch, std::chrono::milliseconds timeout);

    /// @brief Register the status callback. @warning It only fires while startStatusPolling() is active.
    types::OperationResult setNewStatusCb(NewStatusCb cb);
    types::OperationResult startStatusPolling();   ///< Start the background worker that drives the status callback.
    types::OperationResult stopStatusPolling();    ///< Stop the background status worker (bounded; never blocks).
    bool isStatusPollingRunning() const;           ///< Whether the status worker is currently running.

    /// @brief Enumerate the serial numbers of connected M30XY controllers.
    static types::OperationResult getDeviceList(types::ThorlabsSNList& list);

private:

    dcservo::DCServoChannel& channelFor(types::Channel ch);

    std::string serial_no_;
    int poll_rate_ms_;
    bool i_own_open_;                                  ///< True if THIS object opened the connection (vs aliasing it).
    std::array<dcservo::DCServoChannel, 2> chans_;     ///< [0] = X, [1] = Y.
    mutable std::mutex cb_mtx_;                        ///< Guards cb_.
    NewStatusCb cb_;                                   ///< Consumer status callback.
    StatusPoller<types::M30XYDeviceStatus> poller_;    ///< Declared last -> destroyed first (worker stopped before chans_).
};

using M30XYPtr = std::shared_ptr<M30XY>;

// ---------------------------------------------------------------------------------------------------------------------

} // END NAMESPACES

// ---------------------------------------------------------------------------------------------------------------------
