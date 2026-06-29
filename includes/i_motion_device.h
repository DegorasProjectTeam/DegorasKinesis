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
#include <vector>

// PROJECT INCLUDES
#include "libthorlabskinesis_global.h"
#include "common_types.h"
#include "dcservo_status.h"


// NAMESPACES
namespace thorlabs
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
class LIBTHORLABSKINESIS_EXPORT IMotionDevice
{
public:

    virtual ~IMotionDevice() = default;

    virtual std::string getSerialNo() const = 0;
    virtual short getChannelCount() const = 0;
    virtual bool isConnected() const = 0;

    virtual types::OperationResult doConnect(const DeviceConfig& cfg = DeviceConfig{}) = 0;
    virtual types::OperationResult doDisconnect() = 0;

    virtual types::OperationResult doEnable(types::Channel ch, bool enable) = 0;
    virtual types::OperationResult doHome(types::Channel ch) = 0;
    virtual types::OperationResult doStop(types::Channel ch, types::StopMode mode) = 0;
    virtual types::OperationResult doJog(types::Channel ch, types::TravelDirection direction) = 0;
    virtual types::OperationResult doMoveAbsolute(types::Channel ch, double pos_mm) = 0;
    virtual types::OperationResult doMoveRelative(types::Channel ch, double pos_mm) = 0;
    virtual types::OperationResult doConfigureVelocity(types::Channel ch, const types::VelocityProfile& profile) = 0;
    virtual types::OperationResult doConfigureJog(types::Channel ch, const types::JogParameters& params) = 0;

    virtual types::OperationResult getChannelPosition(types::Channel ch, double& pos_mm) = 0;
    virtual types::OperationResult getChannelFlags(types::Channel ch, dcservo::DCServoStatusFlags& flags) = 0;
};

// ---------------------------------------------------------------------------------------------------------------------

} // END NAMESPACES

// ---------------------------------------------------------------------------------------------------------------------
