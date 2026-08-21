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

// C++ INCLUDES
#include <bitset>
#include <string>
#include <utility>

// PROJECT INCLUDES
#include "DegorasKinesis/Devices/m30xy.h"
#include "DegorasKinesis/Kinesis/kinesis_discovery.h"
#include "DegorasKinesis/Helpers/wait_for.h"


// NAMESPACES
namespace dpkin
{

using namespace dpkin::types;

namespace
{
constexpr int kM30XYThorlabsID = 101;   ///< Kinesis device type id for the M30XY.
constexpr int kChannelCount = 2;        ///< X and Y.
} // namespace

// ---------------------------------------------------------------------------------------------------------------------

M30XY::M30XY(const std::string& serial_no) :
    serial_no_(serial_no),
    poll_rate_ms_(500),
    i_own_open_(false),
    units_ready_(false),
    chans_{ dcservo::DCServoChannel(serial_no, Channel::X_CHANNEL),
            dcservo::DCServoChannel(serial_no, Channel::Y_CHANNEL) }
{}

M30XY::~M30XY()
{
    static_cast<void>(this->stopStatusPolling());
    if (this->isConnected())
        static_cast<void>(this->doDisconnect());
}

dcservo::DCServoChannel& M30XY::channelFor(Channel ch)
{
    return (ch == Channel::X_CHANNEL) ? this->chans_[0] : this->chans_[1];
}

// -- Identity / state -------------------------------------------------------------------------------------------------

std::string M30XY::getSerialNo() const
{
    return this->serial_no_;
}

short M30XY::getChannelCount() const
{
    return kChannelCount;
}

bool M30XY::isConnected() const
{
    return this->chans_[0].isConnected();   // controller-level (channel-independent).
}

// -- Connection lifecycle ---------------------------------------------------------------------------------------------

OperationResult M30XY::doConnect(const DeviceConfig& cfg)
{
    // Same object reconnecting is idempotent (ALREADY_CONNECTED); a different object on an open serial gets
    // SERIAL_IN_USE. Short-circuited BEFORE enumeration: an already-open device disappears from the device list,
    // so re-enumerating it would wrongly read DEVICE_NOT_FOUND.
    if (this->isConnected())
        return this->i_own_open_ ? OperationResult::ALREADY_CONNECTED : OperationResult::SERIAL_IN_USE;

    // Controller open (build device list + BDC_Open) on one channel; FT_DeviceNotFound -> DEVICE_NOT_FOUND.
    DeviceError err = this->chans_[0].open();
    if (!err.ok())
        return err.category;

    this->poll_rate_ms_ = cfg.poll_rate_ms;
    const int freshness_ms = cfg.poll_rate_ms * 5;   // stale only after several missed polls.

    // Partial-init cleanup (lambdas in a member function may touch private members through this).
    const auto rollback = [this]()
    {
        this->chans_[0].stopPolling();
        this->chans_[1].stopPolling();
        this->chans_[0].close();
    };

    // LoadSettings failure is NON-FATAL (see K10CR2/M30X): each axis still works in device units; only
    // real-world-unit (mm) conversion needs a loaded stage/settings profile.
    this->units_ready_ = true;
    for (std::size_t i = 0; i < this->chans_.size(); ++i)
    {
        const std::string settings = (i < cfg.settings.size()) ? cfg.settings[i] : std::string();

        if (!this->chans_[i].loadSettings(settings).ok())
            this->units_ready_ = false;

        err = this->chans_[i].startPolling(this->poll_rate_ms_);
        if (!err.ok())
        {
            rollback();
            return err.category;
        }

        this->chans_[i].enableFreshnessTimer(freshness_ms);
        this->chans_[i].clearMessageQueue();
    }

    this->i_own_open_ = true;
    return OperationResult::OPERATION_OK;
}

OperationResult M30XY::doDisconnect()
{
    static_cast<void>(this->stopStatusPolling());

    const bool was_connected = this->isConnected();
    OperationResult first_error = OperationResult::OPERATION_OK;

    if (was_connected)
    {
        const OperationResult stop_result = this->doStopAll(StopMode::PROFILED);
        if (stop_result != OperationResult::OPERATION_OK)
            first_error = stop_result;
    }

    // Best-effort teardown regardless of connection state: release the SDK's per-serial state (drop recovery).
    this->chans_[0].stopPolling();
    this->chans_[1].stopPolling();
    const DeviceError close_err = this->chans_[0].close();
    if (!close_err.ok() && first_error == OperationResult::OPERATION_OK)
        first_error = close_err.category;

    this->i_own_open_ = false;

    if (!was_connected && first_error == OperationResult::OPERATION_OK)
        return OperationResult::NOT_CONNECTED;
    return first_error;
}

// -- Motion -----------------------------------------------------------------------------------------------------------

OperationResult M30XY::doEnable(Channel ch, bool enable)
{
    // Disabling frees the motor; stop it first (mirrors the proof of concept).
    if (!enable)
    {
        const OperationResult stop_result = this->doStop(ch, StopMode::PROFILED);
        if (stop_result != OperationResult::OPERATION_OK)
            return stop_result;
    }
    return this->channelFor(ch).enable(enable).category;
}

OperationResult M30XY::doEnableChannels(bool enable)
{
    const OperationResult r = this->doEnable(Channel::X_CHANNEL, enable);
    if (r != OperationResult::OPERATION_OK)
        return r;
    return this->doEnable(Channel::Y_CHANNEL, enable);
}

OperationResult M30XY::doHome(Channel ch)
{
    return this->channelFor(ch).home().category;
}

OperationResult M30XY::doHomeAll()
{
    const OperationResult r = this->doHome(Channel::X_CHANNEL);
    if (r != OperationResult::OPERATION_OK)
        return r;
    return this->doHome(Channel::Y_CHANNEL);
}

OperationResult M30XY::doStop(Channel ch, StopMode mode)
{
    return this->channelFor(ch).stop(mode).category;
}

OperationResult M30XY::doStopAll(StopMode mode)
{
    const OperationResult r = this->doStop(Channel::X_CHANNEL, mode);
    if (r != OperationResult::OPERATION_OK)
        return r;
    return this->doStop(Channel::Y_CHANNEL, mode);
}

OperationResult M30XY::doJog(Channel ch, TravelDirection direction)
{
    return this->channelFor(ch).jog(direction).category;
}

OperationResult M30XY::doMoveAbsolute(Channel ch, double real_pos)
{
    if (!this->units_ready_)
        return OperationResult::LOAD_SETTINGS_ERROR;
    dcservo::DCServoChannel& c = this->channelFor(ch);
    int target_dev = 0;
    const DeviceError conv = c.realToDevice(PhysicalUnit::DISTANCE, real_pos, target_dev);
    if (!conv.ok())
        return conv.category;
    return c.moveAbsolute(target_dev).category;
}

OperationResult M30XY::doMoveRelative(Channel ch, double real_pos)
{
    if (!this->units_ready_)
        return OperationResult::LOAD_SETTINGS_ERROR;
    dcservo::DCServoChannel& c = this->channelFor(ch);
    int delta_dev = 0;
    const DeviceError conv = c.realToDevice(PhysicalUnit::DISTANCE, real_pos, delta_dev);
    if (!conv.ok())
        return conv.category;
    return c.moveRelative(delta_dev).category;
}

OperationResult M30XY::doConfigureVelocity(Channel ch, const VelocityProfile& profile)
{
    return this->channelFor(ch).setVelocity(profile).category;
}

OperationResult M30XY::doConfigureJog(Channel ch, const JogParameters& params)
{
    return this->channelFor(ch).setJog(params).category;
}

// -- Reads ------------------------------------------------------------------------------------------------------------

OperationResult M30XY::getChannelPosition(Channel ch, double& real_pos)
{
    real_pos = 0.0;
    if (!this->units_ready_)
        return OperationResult::LOAD_SETTINGS_ERROR;
    dcservo::DCServoChannel& c = this->channelFor(ch);

    int pos_raw = 0;
    DeviceError err = c.readPosition(pos_raw);
    if (!err.ok())
        return err.category;

    err = c.deviceToReal(PhysicalUnit::DISTANCE, pos_raw, real_pos);
    return err.category;
}

OperationResult M30XY::getChannelFlags(Channel ch, kinesis::MotorStatusFlags& flags)
{
    flags = kinesis::MotorStatusFlags{};

    std::bitset<32> bits;
    const DeviceError err = this->channelFor(ch).readStatusBits(bits);
    if (!err.ok())
        return err.category;

    flags = kinesis::decodeMotorStatus(bits);
    return OperationResult::OPERATION_OK;
}

// -- Device-unit control (no stage/settings profile required) ---------------------------------------------------------

bool M30XY::hasRealUnits() const
{
    return this->units_ready_;
}

OperationResult M30XY::doMoveAbsoluteDeviceUnits(Channel ch, int device_units)
{
    return this->channelFor(ch).moveAbsolute(device_units).category;
}

OperationResult M30XY::doMoveRelativeDeviceUnits(Channel ch, int device_units)
{
    return this->channelFor(ch).moveRelative(device_units).category;
}

OperationResult M30XY::getChannelPositionDeviceUnits(Channel ch, int& device_units)
{
    device_units = 0;
    return this->channelFor(ch).readPosition(device_units).category;
}

OperationResult M30XY::getChannelStatus(Channel ch, M30XYChannelStatus& status)
{
    status = M30XYChannelStatus(ch);
    dcservo::DCServoChannel& c = this->channelFor(ch);

    std::bitset<32> bits;
    DeviceError err = c.readStatusBits(bits);
    if (!err.ok())
        return err.category;   // status.valid stays false.

    int pos_raw = 0;
    err = c.readPosition(pos_raw);
    if (!err.ok())
        return err.category;

    status.flags = kinesis::decodeMotorStatus(bits);
    status.pos_raw = pos_raw;

    // Real-world position only when a profile is loaded; otherwise report device units only (real_pos stays 0).
    if (this->units_ready_)
    {
        double real_pos = 0.0;
        err = c.deviceToReal(PhysicalUnit::DISTANCE, pos_raw, real_pos);
        if (!err.ok())
            return err.category;
        status.real_pos = real_pos;
    }

    status.valid = true;
    return OperationResult::OPERATION_OK;
}

OperationResult M30XY::getDeviceStatus(M30XYDeviceStatus& status)
{
    status = M30XYDeviceStatus();
    status.serial_no = this->serial_no_;
    status.connected = this->isConnected();
    if (!status.connected)
        return OperationResult::NOT_CONNECTED;

    const OperationResult rx = this->getChannelStatus(Channel::X_CHANNEL, status.chann_x);
    if (rx != OperationResult::OPERATION_OK)
        return rx;
    return this->getChannelStatus(Channel::Y_CHANNEL, status.chann_y);
}

// -- Waits ------------------------------------------------------------------------------------------------------------

OperationResult M30XY::waitForHomed(Channel ch, std::chrono::milliseconds timeout)
{
    return waitForCondition([this, ch]()
    {
        kinesis::MotorStatusFlags flags;
        return this->getChannelFlags(ch, flags) == OperationResult::OPERATION_OK && flags.homed && !flags.homing;
    }, timeout);
}

OperationResult M30XY::waitForMoveFinished(Channel ch, std::chrono::milliseconds timeout)
{
    // Two phases so we never report "finished" before the move is even picked up: first wait (briefly, best-effort)
    // for motion to START, then wait for it to SETTLE. A move that is already complete simply settles immediately.
    const std::chrono::milliseconds start_window(1000);
    waitForCondition([this, ch]()
    {
        kinesis::MotorStatusFlags flags;
        return this->getChannelFlags(ch, flags) == OperationResult::OPERATION_OK && flags.isMoving();
    }, start_window);

    return waitForCondition([this, ch]()
    {
        kinesis::MotorStatusFlags flags;
        return this->getChannelFlags(ch, flags) == OperationResult::OPERATION_OK && !flags.isMoving();
    }, timeout);
}

// -- Status polling ---------------------------------------------------------------------------------------------------

OperationResult M30XY::setNewStatusCb(NewStatusCb cb)
{
    const std::lock_guard<std::mutex> lock(this->cb_mtx_);
    this->cb_ = std::move(cb);
    return OperationResult::OPERATION_OK;
}

OperationResult M30XY::startStatusPolling()
{
    auto producer = [this](M30XYDeviceStatus& status) { return this->getDeviceStatus(status); };

    auto sink = [this](OperationResult result, const M30XYDeviceStatus& status)
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

OperationResult M30XY::stopStatusPolling()
{
    return this->poller_.stop();
}

bool M30XY::isStatusPollingRunning() const
{
    return this->poller_.isRunning();
}

// -- Discovery --------------------------------------------------------------------------------------------------------

OperationResult M30XY::getDeviceList(ThorlabsSNList& list)
{
    return kinesis::enumerateByTypeId(kM30XYThorlabsID, list);
}

bool M30XY::isCompatibleSerial(const ThorlabsSN& serial)
{
    return kinesis::serialMatchesTypeId(serial, kM30XYThorlabsID);
}

// ---------------------------------------------------------------------------------------------------------------------

} // END NAMESPACES

// ---------------------------------------------------------------------------------------------------------------------
