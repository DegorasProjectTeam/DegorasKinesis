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

// C++ INCLUDES
#include <chrono>
#include <mutex>
#include <thread>
#include <utility>

// THORLABS INCLUDES
#include <Thorlabs/Thorlabs.MotionControl.Benchtop.DCServo.h>

// PROJECT INCLUDES
#include "LibDegorasKinesis/DCServo/dcservo_channel.h"
#include "LibDegorasKinesis/Kinesis/kinesis_api_lock.h"
#include "LibDegorasKinesis/Kinesis/kinesis_error.h"


// NAMESPACES
namespace dpkin
{
namespace dcservo
{

using namespace dpkin::types;
using namespace dpkin::kinesis;

namespace
{

// ponytail: post-command settle, mirroring the PoC's "safe sleep" command pacing for real hardware. Calibration
// knob; held OUTSIDE the lock so it never blocks other devices' I/O.
constexpr int kChannelSettleMs = 10;

DeviceError makeError(short code, std::string context)
{
    DeviceError e;
    e.category = categoryFromKinesis(code);
    e.kinesis_code = code;
    e.context = std::move(context);
    return e;
}

DeviceError categoryError(OperationResult category, std::string context)
{
    DeviceError e;
    e.category = category;
    e.kinesis_code = 0;
    e.context = std::move(context);
    return e;
}

} // namespace

// ---------------------------------------------------------------------------------------------------------------------

DCServoChannel::DCServoChannel(ThorlabsSN serial, Channel channel) :
    serial_(std::move(serial)),
    channel_(channel)
{}

// -- Controller-scoped lifecycle --------------------------------------------------------------------------------------

DeviceError DCServoChannel::open()
{
    const std::lock_guard<std::mutex> lock(discoveryMtx());

    const short build = TLI_BuildDeviceList();
    if (build != 0)
        return makeError(build, "TLI_BuildDeviceList");

    return makeError(BDC_Open(this->serial_.c_str()), "BDC_Open");
}

DeviceError DCServoChannel::close()
{
    const std::lock_guard<std::mutex> lock(discoveryMtx());
    return makeError(BDC_Close(this->serial_.c_str()), "BDC_Close");
}

bool DCServoChannel::isConnected() const
{
    const std::lock_guard<std::mutex> lock(serialMtx(this->serial_));
    return BDC_CheckConnection(this->serial_.c_str());
}

// -- Per-axis connection setup ----------------------------------------------------------------------------------------

DeviceError DCServoChannel::loadSettings(const std::string& named)
{
    const std::lock_guard<std::mutex> lock(serialMtx(this->serial_));
    const bool ok = named.empty()
        ? BDC_LoadSettings(this->serial_.c_str(), toType(this->channel_))
        : BDC_LoadNamedSettings(this->serial_.c_str(), toType(this->channel_), named.c_str());
    if (!ok)
        return categoryError(OperationResult::LOAD_SETTINGS_ERROR, "BDC_LoadSettings");
    return DeviceError{};
}

DeviceError DCServoChannel::startPolling(int rate_ms)
{
    const std::lock_guard<std::mutex> lock(serialMtx(this->serial_));
    if (!BDC_StartPolling(this->serial_.c_str(), toType(this->channel_), rate_ms))
        return categoryError(OperationResult::START_POLLING_ERROR, "BDC_StartPolling");
    return DeviceError{};
}

void DCServoChannel::stopPolling() noexcept
{
    // Teardown-only and best-effort. If a detached, wedged worker still holds serialMtx (blocked in a BDC call on
    // dead hardware), do not block shutdown forever: bound the wait and skip if it cannot be acquired.
    std::unique_lock<std::mutex> lock(serialMtx(this->serial_), std::defer_lock);
    const std::chrono::steady_clock::time_point deadline =
        std::chrono::steady_clock::now() + std::chrono::milliseconds(100);
    while (!lock.try_lock())
    {
        if (std::chrono::steady_clock::now() >= deadline)
            return;
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
    }
    BDC_StopPolling(this->serial_.c_str(), toType(this->channel_));
}

void DCServoChannel::enableFreshnessTimer(int timeout_ms) noexcept
{
    const std::lock_guard<std::mutex> lock(serialMtx(this->serial_));
    BDC_EnableLastMsgTimer(this->serial_.c_str(), toType(this->channel_), true, timeout_ms);
}

void DCServoChannel::clearMessageQueue() noexcept
{
    const std::lock_guard<std::mutex> lock(serialMtx(this->serial_));
    BDC_ClearMessageQueue(this->serial_.c_str(), toType(this->channel_));
}

// -- Per-axis motion --------------------------------------------------------------------------------------------------

DeviceError DCServoChannel::enable(bool on)
{
    short code;
    {
        const std::lock_guard<std::mutex> lock(serialMtx(this->serial_));
        code = on ? BDC_EnableChannel(this->serial_.c_str(), toType(this->channel_))
                  : BDC_DisableChannel(this->serial_.c_str(), toType(this->channel_));
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(kChannelSettleMs));
    return makeError(code, on ? "BDC_EnableChannel" : "BDC_DisableChannel");
}

DeviceError DCServoChannel::home()
{
    short code;
    {
        const std::lock_guard<std::mutex> lock(serialMtx(this->serial_));
        code = BDC_Home(this->serial_.c_str(), toType(this->channel_));
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(kChannelSettleMs));
    return makeError(code, "BDC_Home");
}

DeviceError DCServoChannel::stop(StopMode mode)
{
    short code;
    {
        const std::lock_guard<std::mutex> lock(serialMtx(this->serial_));
        code = (mode == StopMode::IMMEDIATE)
            ? BDC_StopImmediate(this->serial_.c_str(), toType(this->channel_))
            : BDC_StopProfiled(this->serial_.c_str(), toType(this->channel_));
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(kChannelSettleMs));
    return makeError(code, "BDC_Stop");
}

DeviceError DCServoChannel::moveAbsolute(int device_units)
{
    const std::lock_guard<std::mutex> lock(serialMtx(this->serial_));
    return makeError(BDC_MoveToPosition(this->serial_.c_str(), toType(this->channel_), device_units),
                     "BDC_MoveToPosition");
}

DeviceError DCServoChannel::moveRelative(int device_units)
{
    const std::lock_guard<std::mutex> lock(serialMtx(this->serial_));
    return makeError(BDC_MoveRelative(this->serial_.c_str(), toType(this->channel_), device_units),
                     "BDC_MoveRelative");
}

DeviceError DCServoChannel::jog(TravelDirection direction)
{
    const MOT_TravelDirection dir = static_cast<MOT_TravelDirection>(toType(direction));
    const std::lock_guard<std::mutex> lock(serialMtx(this->serial_));
    return makeError(BDC_MoveJog(this->serial_.c_str(), toType(this->channel_), dir), "BDC_MoveJog");
}

DeviceError DCServoChannel::setVelocity(const VelocityProfile& profile)
{
    short code;
    {
        const std::lock_guard<std::mutex> lock(serialMtx(this->serial_));

        int min_dev = 0, max_dev = 0, acc_dev = 0;
        code = BDC_GetDeviceUnitFromRealValue(this->serial_.c_str(), toType(this->channel_),
                                              profile.min, &min_dev, toType(PhysicalUnit::VELOCITY));
        if (code != 0)
            return makeError(code, "BDC_GetDeviceUnitFromRealValue(min velocity)");

        code = BDC_GetDeviceUnitFromRealValue(this->serial_.c_str(), toType(this->channel_),
                                              profile.max, &max_dev, toType(PhysicalUnit::VELOCITY));
        if (code != 0)
            return makeError(code, "BDC_GetDeviceUnitFromRealValue(max velocity)");

        code = BDC_GetDeviceUnitFromRealValue(this->serial_.c_str(), toType(this->channel_),
                                              profile.acc, &acc_dev, toType(PhysicalUnit::ACCELERATION));
        if (code != 0)
            return makeError(code, "BDC_GetDeviceUnitFromRealValue(acceleration)");

        MOT_VelocityParameters raw{};
        raw.minVelocity = min_dev;
        raw.maxVelocity = max_dev;
        raw.acceleration = acc_dev;
        code = BDC_SetVelParamsBlock(this->serial_.c_str(), toType(this->channel_), &raw);
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(kChannelSettleMs));
    return makeError(code, "BDC_SetVelParamsBlock");
}

DeviceError DCServoChannel::setJog(const JogParameters& params)
{
    short code;
    {
        const std::lock_guard<std::mutex> lock(serialMtx(this->serial_));

        int step_dev = 0, min_dev = 0, max_dev = 0, acc_dev = 0;
        code = BDC_GetDeviceUnitFromRealValue(this->serial_.c_str(), toType(this->channel_),
                                              params.step_size, &step_dev, toType(PhysicalUnit::DISTANCE));
        if (code != 0)
            return makeError(code, "BDC_GetDeviceUnitFromRealValue(jog step)");

        code = BDC_GetDeviceUnitFromRealValue(this->serial_.c_str(), toType(this->channel_),
                                              params.vel_profile.min, &min_dev, toType(PhysicalUnit::VELOCITY));
        if (code != 0)
            return makeError(code, "BDC_GetDeviceUnitFromRealValue(jog min velocity)");

        code = BDC_GetDeviceUnitFromRealValue(this->serial_.c_str(), toType(this->channel_),
                                              params.vel_profile.max, &max_dev, toType(PhysicalUnit::VELOCITY));
        if (code != 0)
            return makeError(code, "BDC_GetDeviceUnitFromRealValue(jog max velocity)");

        code = BDC_GetDeviceUnitFromRealValue(this->serial_.c_str(), toType(this->channel_),
                                              params.vel_profile.acc, &acc_dev, toType(PhysicalUnit::ACCELERATION));
        if (code != 0)
            return makeError(code, "BDC_GetDeviceUnitFromRealValue(jog acceleration)");

        MOT_JogParameters raw{};
        raw.mode = static_cast<MOT_JogModes>(toType(params.mode));
        raw.stepSize = static_cast<unsigned int>(step_dev);
        raw.velParams.minVelocity = min_dev;
        raw.velParams.maxVelocity = max_dev;
        raw.velParams.acceleration = acc_dev;
        raw.stopMode = static_cast<MOT_StopModes>(toType(params.stop_mode));
        code = BDC_SetJogParamsBlock(this->serial_.c_str(), toType(this->channel_), &raw);
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(kChannelSettleMs));
    return makeError(code, "BDC_SetJogParamsBlock");
}

// -- Per-axis reads (cache-only + freshness) --------------------------------------------------------------------------

DeviceError DCServoChannel::readStatusBits(std::bitset<32>& out_bits)
{
    out_bits.reset();
    const std::lock_guard<std::mutex> lock(serialMtx(this->serial_));

    if (BDC_HasLastMsgTimerOverrun(this->serial_.c_str(), toType(this->channel_)))
        return categoryError(OperationResult::READ_FAILED, "BDC_GetStatusBits (stale: last-message timer overrun)");

    out_bits = std::bitset<32>(BDC_GetStatusBits(this->serial_.c_str(), toType(this->channel_)));
    return DeviceError{};
}

DeviceError DCServoChannel::readPosition(int& out_raw)
{
    out_raw = 0;
    const std::lock_guard<std::mutex> lock(serialMtx(this->serial_));

    if (BDC_HasLastMsgTimerOverrun(this->serial_.c_str(), toType(this->channel_)))
        return categoryError(OperationResult::READ_FAILED, "BDC_GetPosition (stale: last-message timer overrun)");

    out_raw = BDC_GetPosition(this->serial_.c_str(), toType(this->channel_));
    return DeviceError{};
}

DeviceError DCServoChannel::deviceToReal(PhysicalUnit unit, int device_units, double& out_real)
{
    out_real = 0.0;
    const std::lock_guard<std::mutex> lock(serialMtx(this->serial_));
    const short code = BDC_GetRealValueFromDeviceUnit(this->serial_.c_str(), toType(this->channel_),
                                                      device_units, &out_real, toType(unit));
    return makeError(code, "BDC_GetRealValueFromDeviceUnit");
}

DeviceError DCServoChannel::realToDevice(PhysicalUnit unit, double real, int& out_device_units)
{
    out_device_units = 0;
    const std::lock_guard<std::mutex> lock(serialMtx(this->serial_));
    const short code = BDC_GetDeviceUnitFromRealValue(this->serial_.c_str(), toType(this->channel_),
                                                      real, &out_device_units, toType(unit));
    return makeError(code, "BDC_GetDeviceUnitFromRealValue");
}

// ---------------------------------------------------------------------------------------------------------------------

}} // END NAMESPACES

// ---------------------------------------------------------------------------------------------------------------------
