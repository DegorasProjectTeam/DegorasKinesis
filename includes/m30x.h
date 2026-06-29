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
#include <chrono>
#include <functional>
#include <memory>
#include <mutex>
#include <string>

// PROJECT INCLUDES
#include "libthorlabskinesis_global.h"
#include "common_types.h"
#include "dcservo_channel.h"
#include "dcservo_status.h"
#include "i_motion_device.h"
#include "m30x_types.h"
#include "status_poller.h"


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
