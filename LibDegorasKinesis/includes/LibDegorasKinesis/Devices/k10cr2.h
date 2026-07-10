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
#include <chrono>
#include <functional>
#include <memory>
#include <mutex>
#include <string>

// PROJECT INCLUDES
#include "LibDegorasKinesis/Global/libdegoraskinesis_export.h"
#include "LibDegorasKinesis/Common/common_types.h"
#include "LibDegorasKinesis/IntStepper/intstepper_controller.h"
#include "LibDegorasKinesis/Kinesis/motor_status.h"
#include "LibDegorasKinesis/Devices/i_motion_device.h"
#include "LibDegorasKinesis/Devices/k10cr2_types.h"
#include "LibDegorasKinesis/Helpers/status_poller.h"


// NAMESPACES
namespace dpkin
{

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Driver for the Thorlabs K10CR2/M motorized rotation stage (integrated stepper).
 * @details A device personality composed of ONE @ref intstepper::IntStepperController adapter. The K10CR2/M is a
 *          single-axis rotation mount whose user-facing units are DEGREES: absolute moves are angles measured from
 *          the home datum, over a 360-degree continuous travel range. Homing (against the internal Hall-effect limit
 *          switch) establishes the zero datum and must be performed before absolute positioning is meaningful.
 *
 *          Being single-axis, the channel-indexed @ref IMotionDevice methods accept only Channel::X_CHANNEL (the sole
 *          rotation axis); any other channel returns OperationResult::INVALID_CHANNEL.
 *
 * Ownership & lifetime mirror M30X: no device I/O in the constructor, deterministic and bounded shutdown,
 * non-copyable and non-movable.
 */
class LIBDEGORASKINESIS_EXPORT K10CR2 final : public IMotionDevice
{
public:

    /// Callback delivering each poll's (result, status). Invoked on the worker thread, holding no lock.
    using NewStatusCb = std::function<void(types::OperationResult, const types::K10CR2DeviceStatus&)>;

    explicit K10CR2(const std::string& serial_no);
    ~K10CR2() override;

    K10CR2(const K10CR2&) = delete;
    K10CR2& operator=(const K10CR2&) = delete;
    K10CR2(K10CR2&&) = delete;
    K10CR2& operator=(K10CR2&&) = delete;

    /// @name IMotionDevice interface
    /// Behaviour is documented on IMotionDevice. This single-axis device accepts only Channel::X_CHANNEL; any other
    /// channel returns OperationResult::INVALID_CHANNEL. Positions and distances are in DEGREES.
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
    /// @brief Whether a stage/settings profile is loaded, i.e. whether real-world-unit (degrees) operations are
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
    /// @brief Read the device status (its single rotation axis).
    types::OperationResult getDeviceStatus(types::K10CR2DeviceStatus& status);
    /// @brief Block until the axis reports homed, or @p timeout elapses (OPERATION_TIMEOUT).
    types::OperationResult waitForHomed(std::chrono::milliseconds timeout);
    /// @brief Block until the axis stops moving, or @p timeout elapses (OPERATION_TIMEOUT).
    types::OperationResult waitForMoveFinished(std::chrono::milliseconds timeout);

    /// @brief Register the status callback. @warning It only fires while startStatusPolling() is active.
    types::OperationResult setNewStatusCb(NewStatusCb cb);
    types::OperationResult startStatusPolling();   ///< Start the background worker that drives the status callback.
    types::OperationResult stopStatusPolling();    ///< Stop the background status worker (bounded; never blocks).
    bool isStatusPollingRunning() const;           ///< Whether the status worker is currently running.

    /// @brief Enumerate the serial numbers of connected K10CR2 controllers.
    static types::OperationResult getDeviceList(types::ThorlabsSNList& list);

private:

    /// @brief OPERATION_OK if @p ch is this device's only axis, otherwise INVALID_CHANNEL.
    types::OperationResult checkChannel(types::Channel ch) const;
    types::OperationResult fillChannelStatus(types::K10CR2ChannelStatus& status);

    std::string serial_no_;
    int poll_rate_ms_;
    bool i_own_open_;                                  ///< True if THIS object opened the connection (vs aliasing it).
    bool units_ready_;                                 ///< True if a stage/settings profile loaded (real-world units OK).
    intstepper::IntStepperController ctrl_;
    mutable std::mutex cb_mtx_;
    NewStatusCb cb_;
    StatusPoller<types::K10CR2DeviceStatus> poller_;   ///< Declared last -> destroyed first (worker stopped before ctrl_).
};

using K10CR2Ptr = std::shared_ptr<K10CR2>;

// ---------------------------------------------------------------------------------------------------------------------

} // END NAMESPACES

// ---------------------------------------------------------------------------------------------------------------------
