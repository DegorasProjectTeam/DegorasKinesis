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

// C++ INCLUDES
#include <bitset>
#include <string>
#include <utility>

// PROJECT INCLUDES
#include "LibThorlabsKinesis/Devices/m30x.h"
#include "LibThorlabsKinesis/DCServo/dcservo_discovery.h"
#include "LibThorlabsKinesis/Common/wait_for.h"


// NAMESPACES
namespace thorlabs
{

using namespace thorlabs::types;

namespace
{
constexpr int kM30XThorlabsID = 105;   ///< Kinesis device type id for the M30X.
} // namespace

// ---------------------------------------------------------------------------------------------------------------------

M30X::M30X(const std::string& serial_no) :
    serial_no_(serial_no),
    poll_rate_ms_(500),
    chan_(serial_no, Channel::X_CHANNEL)
{}

M30X::~M30X()
{
    static_cast<void>(this->stopStatusPolling());
    if (this->isConnected())
        static_cast<void>(this->doDisconnect());
}

OperationResult M30X::checkChannel(Channel ch) const
{
    return (ch == Channel::X_CHANNEL) ? OperationResult::OPERATION_OK : OperationResult::INVALID_CHANNEL;
}

// -- Identity / state -------------------------------------------------------------------------------------------------

std::string M30X::getSerialNo() const
{
    return this->serial_no_;
}

short M30X::getChannelCount() const
{
    return 1;
}

bool M30X::isConnected() const
{
    return this->chan_.isConnected();
}

// -- Connection lifecycle ---------------------------------------------------------------------------------------------

OperationResult M30X::doConnect(const DeviceConfig& cfg)
{
    if (this->isConnected())
        return OperationResult::ALREADY_CONNECTED;

    DeviceError err = this->chan_.open();
    if (!err.ok())
        return err.category;

    this->poll_rate_ms_ = cfg.poll_rate_ms;
    const int freshness_ms = cfg.poll_rate_ms * 5;

    const auto rollback = [this]()
    {
        this->chan_.stopPolling();
        this->chan_.close();
    };

    const std::string settings = cfg.settings.empty() ? std::string() : cfg.settings.front();

    err = this->chan_.loadSettings(settings);
    if (!err.ok())
    {
        rollback();
        return err.category;
    }

    err = this->chan_.startPolling(this->poll_rate_ms_);
    if (!err.ok())
    {
        rollback();
        return err.category;
    }

    this->chan_.enableFreshnessTimer(freshness_ms);
    this->chan_.clearMessageQueue();

    return OperationResult::OPERATION_OK;
}

OperationResult M30X::doDisconnect()
{
    static_cast<void>(this->stopStatusPolling());

    const bool was_connected = this->isConnected();
    OperationResult first_error = OperationResult::OPERATION_OK;

    if (was_connected)
    {
        const OperationResult stop_result = this->doStop(Channel::X_CHANNEL, StopMode::PROFILED);
        if (stop_result != OperationResult::OPERATION_OK)
            first_error = stop_result;
    }

    this->chan_.stopPolling();
    const DeviceError close_err = this->chan_.close();
    if (!close_err.ok() && first_error == OperationResult::OPERATION_OK)
        first_error = close_err.category;

    if (!was_connected && first_error == OperationResult::OPERATION_OK)
        return OperationResult::NOT_CONNECTED;
    return first_error;
}

// -- Motion -----------------------------------------------------------------------------------------------------------

OperationResult M30X::doEnable(Channel ch, bool enable)
{
    const OperationResult chk = this->checkChannel(ch);
    if (chk != OperationResult::OPERATION_OK)
        return chk;

    if (!enable)
    {
        const OperationResult stop_result = this->doStop(ch, StopMode::PROFILED);
        if (stop_result != OperationResult::OPERATION_OK)
            return stop_result;
    }
    return this->chan_.enable(enable).category;
}

OperationResult M30X::doHome(Channel ch)
{
    const OperationResult chk = this->checkChannel(ch);
    if (chk != OperationResult::OPERATION_OK)
        return chk;
    return this->chan_.home().category;
}

OperationResult M30X::doStop(Channel ch, StopMode mode)
{
    const OperationResult chk = this->checkChannel(ch);
    if (chk != OperationResult::OPERATION_OK)
        return chk;
    return this->chan_.stop(mode).category;
}

OperationResult M30X::doJog(Channel ch, TravelDirection direction)
{
    const OperationResult chk = this->checkChannel(ch);
    if (chk != OperationResult::OPERATION_OK)
        return chk;
    return this->chan_.jog(direction).category;
}

OperationResult M30X::doMoveAbsolute(Channel ch, double pos_mm)
{
    const OperationResult chk = this->checkChannel(ch);
    if (chk != OperationResult::OPERATION_OK)
        return chk;

    int target_dev = 0;
    const DeviceError conv = this->chan_.realToDevice(PhysicalUnit::DISTANCE, pos_mm, target_dev);
    if (!conv.ok())
        return conv.category;
    return this->chan_.moveAbsolute(target_dev).category;
}

OperationResult M30X::doMoveRelative(Channel ch, double pos_mm)
{
    const OperationResult chk = this->checkChannel(ch);
    if (chk != OperationResult::OPERATION_OK)
        return chk;

    int delta_dev = 0;
    const DeviceError conv = this->chan_.realToDevice(PhysicalUnit::DISTANCE, pos_mm, delta_dev);
    if (!conv.ok())
        return conv.category;
    return this->chan_.moveRelative(delta_dev).category;
}

OperationResult M30X::doConfigureVelocity(Channel ch, const VelocityProfile& profile)
{
    const OperationResult chk = this->checkChannel(ch);
    if (chk != OperationResult::OPERATION_OK)
        return chk;
    return this->chan_.setVelocity(profile).category;
}

OperationResult M30X::doConfigureJog(Channel ch, const JogParameters& params)
{
    const OperationResult chk = this->checkChannel(ch);
    if (chk != OperationResult::OPERATION_OK)
        return chk;
    return this->chan_.setJog(params).category;
}

// -- Reads ------------------------------------------------------------------------------------------------------------

OperationResult M30X::getChannelPosition(Channel ch, double& pos_mm)
{
    pos_mm = 0.0;
    const OperationResult chk = this->checkChannel(ch);
    if (chk != OperationResult::OPERATION_OK)
        return chk;

    int pos_raw = 0;
    DeviceError err = this->chan_.readPosition(pos_raw);
    if (!err.ok())
        return err.category;

    err = this->chan_.deviceToReal(PhysicalUnit::DISTANCE, pos_raw, pos_mm);
    return err.category;
}

OperationResult M30X::getChannelFlags(Channel ch, dcservo::DCServoStatusFlags& flags)
{
    flags = dcservo::DCServoStatusFlags{};
    const OperationResult chk = this->checkChannel(ch);
    if (chk != OperationResult::OPERATION_OK)
        return chk;

    std::bitset<32> bits;
    const DeviceError err = this->chan_.readStatusBits(bits);
    if (!err.ok())
        return err.category;

    flags = dcservo::decodeDCServoStatus(bits);
    return OperationResult::OPERATION_OK;
}

OperationResult M30X::fillChannelStatus(M30XChannelStatus& status)
{
    status = M30XChannelStatus();

    std::bitset<32> bits;
    DeviceError err = this->chan_.readStatusBits(bits);
    if (!err.ok())
        return err.category;

    int pos_raw = 0;
    err = this->chan_.readPosition(pos_raw);
    if (!err.ok())
        return err.category;

    double pos_mm = 0.0;
    err = this->chan_.deviceToReal(PhysicalUnit::DISTANCE, pos_raw, pos_mm);
    if (!err.ok())
        return err.category;

    status.flags = dcservo::decodeDCServoStatus(bits);
    status.pos_raw = pos_raw;
    status.pos_mm = pos_mm;
    status.valid = true;
    return OperationResult::OPERATION_OK;
}

OperationResult M30X::getDeviceStatus(M30XDeviceStatus& status)
{
    status = M30XDeviceStatus();
    status.serial_no = this->serial_no_;
    status.connected = this->isConnected();
    if (!status.connected)
        return OperationResult::NOT_CONNECTED;

    return this->fillChannelStatus(status.chann);
}

// -- Waits ------------------------------------------------------------------------------------------------------------

OperationResult M30X::waitForHomed(std::chrono::milliseconds timeout)
{
    return waitForCondition([this]()
    {
        dcservo::DCServoStatusFlags flags;
        return this->getChannelFlags(Channel::X_CHANNEL, flags) == OperationResult::OPERATION_OK
               && flags.homed && !flags.homing;
    }, timeout);
}

OperationResult M30X::waitForMoveFinished(std::chrono::milliseconds timeout)
{
    // Two phases so we never report "finished" before the move is picked up: wait (briefly, best-effort) for motion
    // to START, then wait for it to SETTLE. A move that is already complete settles immediately.
    const std::chrono::milliseconds start_window(1000);
    waitForCondition([this]()
    {
        dcservo::DCServoStatusFlags flags;
        return this->getChannelFlags(Channel::X_CHANNEL, flags) == OperationResult::OPERATION_OK && flags.isMoving();
    }, start_window);

    return waitForCondition([this]()
    {
        dcservo::DCServoStatusFlags flags;
        return this->getChannelFlags(Channel::X_CHANNEL, flags) == OperationResult::OPERATION_OK && !flags.isMoving();
    }, timeout);
}

// -- Status polling ---------------------------------------------------------------------------------------------------

OperationResult M30X::setNewStatusCb(NewStatusCb cb)
{
    const std::lock_guard<std::mutex> lock(this->cb_mtx_);
    this->cb_ = std::move(cb);
    return OperationResult::OPERATION_OK;
}

OperationResult M30X::startStatusPolling()
{
    auto producer = [this](M30XDeviceStatus& status) { return this->getDeviceStatus(status); };

    auto sink = [this](OperationResult result, const M30XDeviceStatus& status)
    {
        NewStatusCb cb;
        {
            const std::lock_guard<std::mutex> lock(this->cb_mtx_);
            cb = this->cb_;
        }
        if (cb)
            cb(result, status);
    };

    return this->poller_.start(producer, sink, std::chrono::milliseconds(this->poll_rate_ms_));
}

OperationResult M30X::stopStatusPolling()
{
    return this->poller_.stop();
}

bool M30X::isStatusPollingRunning() const
{
    return this->poller_.isRunning();
}

// -- Discovery --------------------------------------------------------------------------------------------------------

OperationResult M30X::getDeviceList(ThorlabsSNList& list)
{
    return dcservo::enumerateByTypeId(kM30XThorlabsID, list);
}

// ---------------------------------------------------------------------------------------------------------------------

} // END NAMESPACES

// ---------------------------------------------------------------------------------------------------------------------
