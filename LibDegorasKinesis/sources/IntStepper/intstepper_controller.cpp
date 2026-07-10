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
#include <Thorlabs/Thorlabs.MotionControl.IntegratedStepperMotors.h>

// PROJECT INCLUDES
#include "LibDegorasKinesis/IntStepper/intstepper_controller.h"
#include "LibDegorasKinesis/Kinesis/kinesis_api_lock.h"
#include "LibDegorasKinesis/Kinesis/kinesis_error.h"


// NAMESPACES
namespace dpkin
{
namespace intstepper
{

using namespace dpkin::types;
using namespace dpkin::kinesis;

namespace
{

// ponytail: post-command settle, mirroring the DCServo adapter's command pacing for real hardware. Calibration
// knob; held OUTSIDE the lock so it never blocks other devices' I/O.
constexpr int kSettleMs = 10;

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

IntStepperController::IntStepperController(ThorlabsSN serial) :
    serial_(std::move(serial))
{}

// -- Controller-scoped lifecycle --------------------------------------------------------------------------------------

DeviceError IntStepperController::open()
{
    const std::lock_guard<std::mutex> lock(discoveryMtx());

    const short build = TLI_BuildDeviceList();
    if (build != 0)
        return makeError(build, "TLI_BuildDeviceList");

    return makeError(ISC_Open(this->serial_.c_str()), "ISC_Open");
}

DeviceError IntStepperController::close()
{
    const std::lock_guard<std::mutex> lock(discoveryMtx());
    ISC_Close(this->serial_.c_str());
    return DeviceError{};
}

bool IntStepperController::isConnected() const
{
    const std::lock_guard<std::mutex> lock(serialMtx(this->serial_));
    return ISC_CheckConnection(this->serial_.c_str());
}

// -- Connection setup -------------------------------------------------------------------------------------------------

DeviceError IntStepperController::loadSettings(const std::string& named)
{
    const std::lock_guard<std::mutex> lock(serialMtx(this->serial_));
    const bool ok = named.empty()
        ? ISC_LoadSettings(this->serial_.c_str())
        : ISC_LoadNamedSettings(this->serial_.c_str(), named.c_str());
    if (!ok)
        return categoryError(OperationResult::LOAD_SETTINGS_ERROR, "ISC_LoadSettings");
    return DeviceError{};
}

DeviceError IntStepperController::startPolling(int rate_ms)
{
    const std::lock_guard<std::mutex> lock(serialMtx(this->serial_));
    if (!ISC_StartPolling(this->serial_.c_str(), rate_ms))
        return categoryError(OperationResult::START_POLLING_ERROR, "ISC_StartPolling");
    return DeviceError{};
}

void IntStepperController::stopPolling() noexcept
{
    // Teardown-only and best-effort. If a detached, wedged worker still holds serialMtx (blocked in an ISC call on
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
    ISC_StopPolling(this->serial_.c_str());
}

void IntStepperController::enableFreshnessTimer(int timeout_ms) noexcept
{
    const std::lock_guard<std::mutex> lock(serialMtx(this->serial_));
    ISC_EnableLastMsgTimer(this->serial_.c_str(), true, timeout_ms);
}

void IntStepperController::clearMessageQueue() noexcept
{
    const std::lock_guard<std::mutex> lock(serialMtx(this->serial_));
    ISC_ClearMessageQueue(this->serial_.c_str());
}

// -- Motion -----------------------------------------------------------------------------------------------------------

DeviceError IntStepperController::enable(bool on)
{
    short code;
    {
        const std::lock_guard<std::mutex> lock(serialMtx(this->serial_));
        code = on ? ISC_EnableChannel(this->serial_.c_str())
                  : ISC_DisableChannel(this->serial_.c_str());
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(kSettleMs));
    return makeError(code, on ? "ISC_EnableChannel" : "ISC_DisableChannel");
}

DeviceError IntStepperController::home()
{
    short code;
    {
        const std::lock_guard<std::mutex> lock(serialMtx(this->serial_));
        code = ISC_Home(this->serial_.c_str());
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(kSettleMs));
    return makeError(code, "ISC_Home");
}

DeviceError IntStepperController::stop(StopMode mode)
{
    short code;
    {
        const std::lock_guard<std::mutex> lock(serialMtx(this->serial_));
        code = (mode == StopMode::IMMEDIATE)
            ? ISC_StopImmediate(this->serial_.c_str())
            : ISC_StopProfiled(this->serial_.c_str());
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(kSettleMs));
    return makeError(code, "ISC_Stop");
}

DeviceError IntStepperController::moveAbsolute(int device_units)
{
    const std::lock_guard<std::mutex> lock(serialMtx(this->serial_));
    return makeError(ISC_MoveToPosition(this->serial_.c_str(), device_units), "ISC_MoveToPosition");
}

DeviceError IntStepperController::moveRelative(int device_units)
{
    const std::lock_guard<std::mutex> lock(serialMtx(this->serial_));
    return makeError(ISC_MoveRelative(this->serial_.c_str(), device_units), "ISC_MoveRelative");
}

DeviceError IntStepperController::jog(TravelDirection direction)
{
    const MOT_TravelDirection dir = static_cast<MOT_TravelDirection>(toType(direction));
    const std::lock_guard<std::mutex> lock(serialMtx(this->serial_));
    return makeError(ISC_MoveJog(this->serial_.c_str(), dir), "ISC_MoveJog");
}

DeviceError IntStepperController::setVelocity(const VelocityProfile& profile)
{
    short code;
    {
        const std::lock_guard<std::mutex> lock(serialMtx(this->serial_));

        int min_dev = 0, max_dev = 0, acc_dev = 0;
        code = ISC_GetDeviceUnitFromRealValue(this->serial_.c_str(),
                                              profile.min, &min_dev, toType(PhysicalUnit::VELOCITY));
        if (code != 0)
            return makeError(code, "ISC_GetDeviceUnitFromRealValue(min velocity)");

        code = ISC_GetDeviceUnitFromRealValue(this->serial_.c_str(),
                                              profile.max, &max_dev, toType(PhysicalUnit::VELOCITY));
        if (code != 0)
            return makeError(code, "ISC_GetDeviceUnitFromRealValue(max velocity)");

        code = ISC_GetDeviceUnitFromRealValue(this->serial_.c_str(),
                                              profile.acc, &acc_dev, toType(PhysicalUnit::ACCELERATION));
        if (code != 0)
            return makeError(code, "ISC_GetDeviceUnitFromRealValue(acceleration)");

        MOT_VelocityParameters raw{};
        raw.minVelocity = min_dev;
        raw.maxVelocity = max_dev;
        raw.acceleration = acc_dev;
        code = ISC_SetVelParamsBlock(this->serial_.c_str(), &raw);
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(kSettleMs));
    return makeError(code, "ISC_SetVelParamsBlock");
}

DeviceError IntStepperController::setJog(const JogParameters& params)
{
    short code;
    {
        const std::lock_guard<std::mutex> lock(serialMtx(this->serial_));

        int step_dev = 0, min_dev = 0, max_dev = 0, acc_dev = 0;
        code = ISC_GetDeviceUnitFromRealValue(this->serial_.c_str(),
                                              params.step_size, &step_dev, toType(PhysicalUnit::DISTANCE));
        if (code != 0)
            return makeError(code, "ISC_GetDeviceUnitFromRealValue(jog step)");

        code = ISC_GetDeviceUnitFromRealValue(this->serial_.c_str(),
                                              params.vel_profile.min, &min_dev, toType(PhysicalUnit::VELOCITY));
        if (code != 0)
            return makeError(code, "ISC_GetDeviceUnitFromRealValue(jog min velocity)");

        code = ISC_GetDeviceUnitFromRealValue(this->serial_.c_str(),
                                              params.vel_profile.max, &max_dev, toType(PhysicalUnit::VELOCITY));
        if (code != 0)
            return makeError(code, "ISC_GetDeviceUnitFromRealValue(jog max velocity)");

        code = ISC_GetDeviceUnitFromRealValue(this->serial_.c_str(),
                                              params.vel_profile.acc, &acc_dev, toType(PhysicalUnit::ACCELERATION));
        if (code != 0)
            return makeError(code, "ISC_GetDeviceUnitFromRealValue(jog acceleration)");

        MOT_JogParameters raw{};
        raw.mode = static_cast<MOT_JogModes>(toType(params.mode));
        raw.stepSize = static_cast<unsigned int>(step_dev);
        raw.velParams.minVelocity = min_dev;
        raw.velParams.maxVelocity = max_dev;
        raw.velParams.acceleration = acc_dev;
        raw.stopMode = static_cast<MOT_StopModes>(toType(params.stop_mode));
        code = ISC_SetJogParamsBlock(this->serial_.c_str(), &raw);
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(kSettleMs));
    return makeError(code, "ISC_SetJogParamsBlock");
}

// -- Reads (cache-only + freshness) -----------------------------------------------------------------------------------

DeviceError IntStepperController::readStatusBits(std::bitset<32>& out_bits)
{
    out_bits.reset();
    const std::lock_guard<std::mutex> lock(serialMtx(this->serial_));

    if (ISC_HasLastMsgTimerOverrun(this->serial_.c_str()))
        return categoryError(OperationResult::READ_FAILED, "ISC_GetStatusBits (stale: last-message timer overrun)");

    out_bits = std::bitset<32>(ISC_GetStatusBits(this->serial_.c_str()));
    return DeviceError{};
}

DeviceError IntStepperController::readPosition(int& out_raw)
{
    out_raw = 0;
    const std::lock_guard<std::mutex> lock(serialMtx(this->serial_));

    if (ISC_HasLastMsgTimerOverrun(this->serial_.c_str()))
        return categoryError(OperationResult::READ_FAILED, "ISC_GetPosition (stale: last-message timer overrun)");

    out_raw = ISC_GetPosition(this->serial_.c_str());
    return DeviceError{};
}

DeviceError IntStepperController::deviceToReal(PhysicalUnit unit, int device_units, double& out_real)
{
    out_real = 0.0;
    const std::lock_guard<std::mutex> lock(serialMtx(this->serial_));
    const short code = ISC_GetRealValueFromDeviceUnit(this->serial_.c_str(),
                                                      device_units, &out_real, toType(unit));
    return makeError(code, "ISC_GetRealValueFromDeviceUnit");
}

DeviceError IntStepperController::realToDevice(PhysicalUnit unit, double real, int& out_device_units)
{
    out_device_units = 0;
    const std::lock_guard<std::mutex> lock(serialMtx(this->serial_));
    const short code = ISC_GetDeviceUnitFromRealValue(this->serial_.c_str(),
                                                      real, &out_device_units, toType(unit));
    return makeError(code, "ISC_GetDeviceUnitFromRealValue");
}

// ---------------------------------------------------------------------------------------------------------------------

}} // END NAMESPACES

// ---------------------------------------------------------------------------------------------------------------------
