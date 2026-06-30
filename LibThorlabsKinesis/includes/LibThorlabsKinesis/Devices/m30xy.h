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
#include <array>
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
#include "LibThorlabsKinesis/Devices/m30xy_types.h"
#include "LibThorlabsKinesis/Common/status_poller.h"


// NAMESPACES
namespace thorlabs
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
class LIBTHORLABSKINESIS_EXPORT M30XY final : public IMotionDevice
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

    // -- Dual-axis conveniences (fan-out / aggregation; not on the interface) --
    types::OperationResult doEnableChannels(bool enable);   ///< Enable/disable both axes (first-error-wins).
    types::OperationResult doHomeAll();                     ///< Home both axes (first-error-wins).
    types::OperationResult doStopAll(types::StopMode mode); ///< Stop both axes (first-error-wins).
    types::OperationResult getChannelStatus(types::Channel ch, types::M30XYChannelStatus& status);
    types::OperationResult getDeviceStatus(types::M30XYDeviceStatus& status);
    types::OperationResult waitForHomed(types::Channel ch, std::chrono::milliseconds timeout);
    types::OperationResult waitForMoveFinished(types::Channel ch, std::chrono::milliseconds timeout);

    /// @warning A registered callback only fires while startStatusPolling() is active.
    types::OperationResult setNewStatusCb(NewStatusCb cb);
    types::OperationResult startStatusPolling();
    types::OperationResult stopStatusPolling();
    bool isStatusPollingRunning() const;

    /// @brief Enumerate connected M30XY controller serial numbers.
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
