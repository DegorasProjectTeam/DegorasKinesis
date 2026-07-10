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
#include <bitset>
#include <string>

// PROJECT INCLUDES
#include "LibDegorasKinesis/Global/libdegoraskinesis_export.h"
#include "LibDegorasKinesis/Common/common_types.h"


// NAMESPACES
namespace dpkin
{
namespace dcservo
{

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Adapter binding one Benchtop DC Servo axis (serial + channel) to its BDC_* operations.
 *
 * @details This is the ONLY place that knows the Kinesis Benchtop.DCServo C API. A device personality composes one
 *          of these per axis (M30X holds one, M30XY holds two). Every method returns a self-contained DeviceError by
 *          value; there is no stored "last error".
 *
 * Threading:
 *  - Controller-scoped calls (open/close/checkConnection) operate on the serial and are channel-independent; they
 *    take the global discoveryMtx() because they mutate the Kinesis device-list/connection state. For a multi-axis
 *    device, the personality calls these on ONE channel only.
 *  - All per-axis steady-state calls take serialMtx(serial), so two different devices run concurrently.
 *  - No method holds any lock across a sleep.
 *
 * Reads: position and status are read from the SDK's internal poll cache (never issuing a Request*, which the PoC
 *        warns can reset the buffer). Freshness is judged by the last-message timer; a stale read returns READ_FAILED
 *        instead of a misleading zero/garbage value reported as success.
 */
class LIBDEGORASKINESIS_EXPORT DCServoChannel
{
public:

    DCServoChannel(types::ThorlabsSN serial, types::Channel channel);

    types::ThorlabsSN serialNo() const { return this->serial_; }
    types::Channel channel() const { return this->channel_; }

    // -- Controller-scoped lifecycle (channel-independent; call on one channel only for a multi-axis device) --
    types::DeviceError open();                       ///< Build device list + BDC_Open (FT_DeviceNotFound -> DEVICE_NOT_FOUND).
    types::DeviceError close();                      ///< BDC_Close.
    bool isConnected() const;                        ///< BDC_CheckConnection.

    // -- Per-axis connection setup --
    types::DeviceError loadSettings(const std::string& named);   ///< Named settings, or default if @p named is empty.
    types::DeviceError startPolling(int rate_ms);                ///< BDC_StartPolling (SDK-internal cache refresh).
    void stopPolling() noexcept;                                 ///< BDC_StopPolling (teardown-safe).
    void enableFreshnessTimer(int timeout_ms) noexcept;          ///< BDC_EnableLastMsgTimer (read-validity signal).
    void clearMessageQueue() noexcept;                           ///< BDC_ClearMessageQueue (poll status does not auto-clear).

    // -- Per-axis motion --
    types::DeviceError enable(bool on);                          ///< BDC_EnableChannel / BDC_DisableChannel.
    types::DeviceError home();                                   ///< BDC_Home (fire-and-forget).
    types::DeviceError stop(types::StopMode mode);               ///< BDC_StopProfiled / BDC_StopImmediate.
    types::DeviceError moveAbsolute(int device_units);           ///< BDC_MoveToPosition.
    types::DeviceError moveRelative(int device_units);           ///< BDC_MoveRelative.
    types::DeviceError jog(types::TravelDirection direction);    ///< BDC_MoveJog.
    types::DeviceError setVelocity(const types::VelocityProfile& profile);   ///< Convert mm->device + BDC_SetVelParamsBlock.
    types::DeviceError setJog(const types::JogParameters& params);           ///< Convert mm->device + BDC_SetJogParamsBlock.

    // -- Per-axis reads (cache-only + freshness; READ_FAILED on stale/comms-lost) --
    types::DeviceError readStatusBits(std::bitset<32>& out_bits);
    types::DeviceError readPosition(int& out_raw);
    types::DeviceError deviceToReal(types::PhysicalUnit unit, int device_units, double& out_real);
    types::DeviceError realToDevice(types::PhysicalUnit unit, double real, int& out_device_units);

private:

    types::ThorlabsSN serial_;
    types::Channel channel_;
};

// ---------------------------------------------------------------------------------------------------------------------

}} // END NAMESPACES

// ---------------------------------------------------------------------------------------------------------------------
