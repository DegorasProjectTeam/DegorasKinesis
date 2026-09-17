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
#include "DegorasKinesis/Devices/k10cr2.h"
#include "DegorasKinesis/Kinesis/kinesis_discovery.h"
#include "DegorasKinesis/Helpers/wait_for.h"


// NAMESPACES
namespace dpkin
{

using namespace dpkin::types;

namespace
{
constexpr int kK10CR2ThorlabsID = 55;   ///< Kinesis device type id for the K10CR2/M (matches serial prefix 55).
} // namespace

// ---------------------------------------------------------------------------------------------------------------------

K10CR2::K10CR2(const std::string& serial_no) :
    serial_no_(serial_no),
    poll_rate_ms_(500),
    i_own_open_(false),
    units_ready_(false),
    ctrl_(serial_no)
{}

K10CR2::~K10CR2()
{
    static_cast<void>(this->stopStatusPolling());
    if (this->isConnected())
        static_cast<void>(this->doDisconnect());
}

OperationResult K10CR2::checkChannel(Channel ch)
{
    return (ch == Channel::X_CHANNEL) ? OperationResult::OPERATION_OK : OperationResult::INVALID_CHANNEL;
}

// -- Identity / state -------------------------------------------------------------------------------------------------

std::string K10CR2::getSerialNo() const
{
    return this->serial_no_;
}

short K10CR2::getChannelCount() const
{
    return 1;
}

bool K10CR2::isConnected() const
{
    return this->ctrl_.isConnected();
}

// -- Connection lifecycle ---------------------------------------------------------------------------------------------

OperationResult K10CR2::doConnect(const DeviceConfig& cfg)
{
    // Same object reconnecting is idempotent; a different object on an open serial gets SERIAL_IN_USE.
    if (this->isConnected())
        return this->i_own_open_ ? OperationResult::ALREADY_CONNECTED : OperationResult::SERIAL_IN_USE;

    DeviceError err = this->ctrl_.open();
    if (!err.ok())
        return err.category;

    this->poll_rate_ms_ = cfg.poll_rate_ms;
    const int freshness_ms = cfg.poll_rate_ms * 5;

    const auto rollback = [this]()
    {
        this->ctrl_.stopPolling();
        this->ctrl_.close();
    };

    const std::string settings = cfg.settings.empty() ? std::string() : cfg.settings.front();

    // LoadSettings failure is NON-FATAL: an integrated stepper still homes, moves and reports status in device
    // units; only real-world-unit (degrees) conversion needs a loaded stage/settings profile. Track availability so
    // the real-world-unit methods refuse cleanly and callers can fall back to the device-unit methods.
    this->units_ready_ = this->ctrl_.loadSettings(settings).ok();

    err = this->ctrl_.startPolling(this->poll_rate_ms_);
    if (!err.ok())
    {
        rollback();
        return err.category;
    }

    this->ctrl_.enableFreshnessTimer(freshness_ms);
    this->ctrl_.clearMessageQueue();

    this->i_own_open_ = true;
    return OperationResult::OPERATION_OK;
}

OperationResult K10CR2::doDisconnect()
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

    this->ctrl_.stopPolling();
    const DeviceError close_err = this->ctrl_.close();
    if (!close_err.ok() && first_error == OperationResult::OPERATION_OK)
        first_error = close_err.category;

    this->i_own_open_ = false;

    if (!was_connected && first_error == OperationResult::OPERATION_OK)
        return OperationResult::NOT_CONNECTED;
    return first_error;
}

// -- Motion -----------------------------------------------------------------------------------------------------------

OperationResult K10CR2::doEnable(Channel ch, bool enable)
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
    return this->ctrl_.enable(enable).category;
}

OperationResult K10CR2::doHome(Channel ch)
{
    const OperationResult chk = this->checkChannel(ch);
    if (chk != OperationResult::OPERATION_OK)
        return chk;
    return this->ctrl_.home().category;
}

OperationResult K10CR2::doStop(Channel ch, StopMode mode)
{
    const OperationResult chk = this->checkChannel(ch);
    if (chk != OperationResult::OPERATION_OK)
        return chk;
    return this->ctrl_.stop(mode).category;
}

OperationResult K10CR2::doJog(Channel ch, TravelDirection direction)
{
    const OperationResult chk = this->checkChannel(ch);
    if (chk != OperationResult::OPERATION_OK)
        return chk;
    return this->ctrl_.jog(direction).category;
}

OperationResult K10CR2::doMoveAbsolute(Channel ch, double real_pos)
{
    const OperationResult chk = this->checkChannel(ch);
    if (chk != OperationResult::OPERATION_OK)
        return chk;

    if (!this->units_ready_)
        return OperationResult::LOAD_SETTINGS_ERROR;

    int target_dev = 0;
    const DeviceError conv = this->ctrl_.realToDevice(PhysicalUnit::DISTANCE, real_pos, target_dev);
    if (!conv.ok())
        return conv.category;
    return this->ctrl_.moveAbsolute(target_dev).category;
}

OperationResult K10CR2::doMoveRelative(Channel ch, double real_pos)
{
    const OperationResult chk = this->checkChannel(ch);
    if (chk != OperationResult::OPERATION_OK)
        return chk;

    if (!this->units_ready_)
        return OperationResult::LOAD_SETTINGS_ERROR;

    int delta_dev = 0;
    const DeviceError conv = this->ctrl_.realToDevice(PhysicalUnit::DISTANCE, real_pos, delta_dev);
    if (!conv.ok())
        return conv.category;
    return this->ctrl_.moveRelative(delta_dev).category;
}

OperationResult K10CR2::doConfigureVelocity(Channel ch, const VelocityProfile& profile)
{
    const OperationResult chk = this->checkChannel(ch);
    if (chk != OperationResult::OPERATION_OK)
        return chk;
    return this->ctrl_.setVelocity(profile).category;
}

OperationResult K10CR2::doConfigureJog(Channel ch, const JogParameters& params)
{
    const OperationResult chk = this->checkChannel(ch);
    if (chk != OperationResult::OPERATION_OK)
        return chk;
    return this->ctrl_.setJog(params).category;
}

// -- Reads ------------------------------------------------------------------------------------------------------------

OperationResult K10CR2::getChannelPosition(Channel ch, double& real_pos)
{
    real_pos = 0.0;
    const OperationResult chk = this->checkChannel(ch);
    if (chk != OperationResult::OPERATION_OK)
        return chk;

    if (!this->units_ready_)
        return OperationResult::LOAD_SETTINGS_ERROR;

    int pos_raw = 0;
    DeviceError err = this->ctrl_.readPosition(pos_raw);
    if (!err.ok())
        return err.category;

    err = this->ctrl_.deviceToReal(PhysicalUnit::DISTANCE, pos_raw, real_pos);
    return err.category;
}

OperationResult K10CR2::getChannelFlags(Channel ch, kinesis::MotorStatusFlags& flags)
{
    flags = kinesis::MotorStatusFlags{};
    const OperationResult chk = this->checkChannel(ch);
    if (chk != OperationResult::OPERATION_OK)
        return chk;

    std::bitset<32> bits;
    const DeviceError err = this->ctrl_.readStatusBits(bits);
    if (!err.ok())
        return err.category;

    flags = kinesis::decodeMotorStatus(bits);
    return OperationResult::OPERATION_OK;
}

// -- Device-unit control (no stage/settings profile required) ---------------------------------------------------------

bool K10CR2::hasRealUnits() const
{
    return this->units_ready_;
}

OperationResult K10CR2::doMoveAbsoluteDeviceUnits(Channel ch, int device_units)
{
    const OperationResult chk = this->checkChannel(ch);
    if (chk != OperationResult::OPERATION_OK)
        return chk;
    return this->ctrl_.moveAbsolute(device_units).category;
}

OperationResult K10CR2::doMoveRelativeDeviceUnits(Channel ch, int device_units)
{
    const OperationResult chk = this->checkChannel(ch);
    if (chk != OperationResult::OPERATION_OK)
        return chk;
    return this->ctrl_.moveRelative(device_units).category;
}

OperationResult K10CR2::getChannelPositionDeviceUnits(Channel ch, int& device_units)
{
    device_units = 0;
    const OperationResult chk = this->checkChannel(ch);
    if (chk != OperationResult::OPERATION_OK)
        return chk;
    return this->ctrl_.readPosition(device_units).category;
}

OperationResult K10CR2::fillChannelStatus(K10CR2ChannelStatus& status)
{
    status = K10CR2ChannelStatus();

    std::bitset<32> bits;
    DeviceError err = this->ctrl_.readStatusBits(bits);
    if (!err.ok())
        return err.category;

    int pos_raw = 0;
    err = this->ctrl_.readPosition(pos_raw);
    if (!err.ok())
        return err.category;

    status.flags = kinesis::decodeMotorStatus(bits);
    status.pos_raw = pos_raw;

    // Real-world position only when a profile is loaded; otherwise report device units only (real_pos stays 0).
    if (this->units_ready_)
    {
        double real_pos = 0.0;
        err = this->ctrl_.deviceToReal(PhysicalUnit::DISTANCE, pos_raw, real_pos);
        if (!err.ok())
            return err.category;
        status.real_pos = real_pos;
    }

    status.valid = true;
    return OperationResult::OPERATION_OK;
}

OperationResult K10CR2::getDeviceStatus(K10CR2DeviceStatus& status)
{
    status = K10CR2DeviceStatus();
    status.serial_no = this->serial_no_;
    status.connected = this->isConnected();
    if (!status.connected)
        return OperationResult::NOT_CONNECTED;

    return this->fillChannelStatus(status.chann);
}

// -- Waits ------------------------------------------------------------------------------------------------------------

OperationResult K10CR2::waitForHomed(std::chrono::milliseconds timeout)
{
    // TWO PHASES, for the same reason waitForMoveFinished has them, and for a failure measured against the
    // Kinesis Simulator: the homed flag is still set from the previous home and the homing flag takes around
    // 800 ms to rise, so a one-phase `homed && !homing` wait is already true when called and returns without
    // waiting at all. The move that follows then lands mid-home and is rejected by the device -- a rejection
    // the SDK does not report back, so nothing fails loudly.
    //
    // The first phase is BEST-EFFORT and its result is deliberately ignored: on the simulator a home can finish
    // faster than the SDK cache refreshes, so the rise is never observable and this window simply expires.
    const types::Timeout start_window(std::chrono::milliseconds(1000));
    waitForCondition([this]()
    {
        kinesis::MotorStatusFlags flags;
        return this->getChannelFlags(Channel::X_CHANNEL, flags) == OperationResult::OPERATION_OK && flags.homing;
    }, start_window);

    return waitForCondition([this]()
    {
        kinesis::MotorStatusFlags flags;
        return this->getChannelFlags(Channel::X_CHANNEL, flags) == OperationResult::OPERATION_OK
               && flags.homed && !flags.homing;
    }, types::Timeout(timeout));
}

OperationResult K10CR2::waitForMoveFinished(std::chrono::milliseconds timeout)
{
    // Two phases so we never report "finished" before the move is picked up: wait (briefly, best-effort) for motion
    // to START, then wait for it to SETTLE. A move that is already complete settles immediately.
    const types::Timeout start_window(std::chrono::milliseconds(1000));
    waitForCondition([this]()
    {
        kinesis::MotorStatusFlags flags;
        return this->getChannelFlags(Channel::X_CHANNEL, flags) == OperationResult::OPERATION_OK && flags.isMoving();
    }, start_window);

    return waitForCondition([this]()
    {
        kinesis::MotorStatusFlags flags;
        return this->getChannelFlags(Channel::X_CHANNEL, flags) == OperationResult::OPERATION_OK && !flags.isMoving();
    }, types::Timeout(timeout));
}

// -- Status polling ---------------------------------------------------------------------------------------------------

OperationResult K10CR2::setNewStatusCb(NewStatusCb cb)
{
    const std::lock_guard<std::mutex> lock(this->cb_mtx_);
    this->cb_ = std::move(cb);
    return OperationResult::OPERATION_OK;
}

OperationResult K10CR2::startStatusPolling()
{
    auto producer = [this](K10CR2DeviceStatus& status) { return this->getDeviceStatus(status); };

    auto sink = [this](OperationResult result, const K10CR2DeviceStatus& status)
    {
        NewStatusCb cb;
        {
            const std::lock_guard<std::mutex> lock(this->cb_mtx_);
            cb = this->cb_;
        }
        if (cb)
            cb(result, status);
    };

    return this->poller_.start(producer, sink, types::PollInterval(std::chrono::milliseconds(this->poll_rate_ms_)));
}

OperationResult K10CR2::stopStatusPolling()
{
    return this->poller_.stop();
}

bool K10CR2::isStatusPollingRunning() const
{
    return this->poller_.isRunning();
}

// -- Discovery --------------------------------------------------------------------------------------------------------

OperationResult K10CR2::getDeviceList(ThorlabsSNList& list)
{
    return kinesis::enumerateByTypeId(kK10CR2ThorlabsID, list);
}

bool K10CR2::isCompatibleSerial(const ThorlabsSN& serial)
{
    return kinesis::serialMatchesTypeId(serial, kK10CR2ThorlabsID);
}

// ---------------------------------------------------------------------------------------------------------------------

} // END NAMESPACES

// ---------------------------------------------------------------------------------------------------------------------
