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
#include <chrono>
#include <functional>
#include <memory>
#include <mutex>
#include <string>

// PROJECT INCLUDES
#include "LibThorlabsKinesis/libthorlabskinesis_global.h"
#include "LibThorlabsKinesis/Common/common_types.h"
#include "LibThorlabsKinesis/DCServo/dcservo_channel.h"
#include "LibThorlabsKinesis/DCServo/dcservo_status.h"
#include "LibThorlabsKinesis/Devices/i_motion_device.h"
#include "LibThorlabsKinesis/Devices/m30x_types.h"
#include "LibThorlabsKinesis/Common/status_poller.h"


// NAMESPACES
namespace thorlabs
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
class LIBTHORLABSKINESIS_EXPORT M30X final : public IMotionDevice
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

    // -- IMotionDevice --
    std::string getSerialNo() const override;
    short getChannelCount() const override;
    bool isConnected() const override;
    types::OperationResult doConnect(const DeviceConfig& cfg = DeviceConfig{}) override;
    types::OperationResult doDisconnect() override;
    types::OperationResult doEnable(types::Channel ch, bool enable) override;
    types::OperationResult doHome(types::Channel ch) override;
    types::OperationResult doStop(types::Channel ch, types::StopMode mode) override;
    types::OperationResult doJog(types::Channel ch, types::TravelDirection direction) override;
    types::OperationResult doMoveAbsolute(types::Channel ch, double pos_mm) override;
    types::OperationResult doMoveRelative(types::Channel ch, double pos_mm) override;
    types::OperationResult doConfigureVelocity(types::Channel ch, const types::VelocityProfile& profile) override;
    types::OperationResult doConfigureJog(types::Channel ch, const types::JogParameters& params) override;
    types::OperationResult getChannelPosition(types::Channel ch, double& pos_mm) override;
    types::OperationResult getChannelFlags(types::Channel ch, dcservo::DCServoStatusFlags& flags) override;

    // -- Conveniences --
    types::OperationResult getDeviceStatus(types::M30XDeviceStatus& status);
    types::OperationResult waitForHomed(std::chrono::milliseconds timeout);
    types::OperationResult waitForMoveFinished(std::chrono::milliseconds timeout);

    /// @warning A registered callback only fires while startStatusPolling() is active.
    types::OperationResult setNewStatusCb(NewStatusCb cb);
    types::OperationResult startStatusPolling();
    types::OperationResult stopStatusPolling();
    bool isStatusPollingRunning() const;

    /// @brief Enumerate connected M30X controller serial numbers.
    static types::OperationResult getDeviceList(types::ThorlabsSNList& list);

private:

    /// @brief OPERATION_OK if @p ch is this device's only axis, otherwise INVALID_CHANNEL.
    types::OperationResult checkChannel(types::Channel ch) const;
    types::OperationResult fillChannelStatus(types::M30XChannelStatus& status);

    std::string serial_no_;
    int poll_rate_ms_;
    dcservo::DCServoChannel chan_;
    mutable std::mutex cb_mtx_;
    NewStatusCb cb_;
    StatusPoller<types::M30XDeviceStatus> poller_;   ///< Declared last -> destroyed first (worker stopped before chan_).
};

using M30XPtr = std::shared_ptr<M30X>;

// ---------------------------------------------------------------------------------------------------------------------

} // END NAMESPACES

// ---------------------------------------------------------------------------------------------------------------------
