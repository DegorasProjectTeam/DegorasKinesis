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

#pragma once

// C++ INCLUDES
#include <bitset>
#include <string>

// PROJECT INCLUDES
#include "DegorasKinesis/Global/degoraskinesis_export.h"
#include "DegorasKinesis/Common/common_types.h"


// NAMESPACES
namespace dpkin
{
namespace intstepper
{

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Adapter binding one Integrated Stepper Motor controller (by serial) to its ISC_* operations.
 *
 * @details This is the ONLY place that knows the Kinesis Integrated Stepper Motors (ISC_*) C API. Unlike the Benchtop
 *          DC Servo family, an integrated-stepper device (e.g. the K10CR2/M rotation stage) exposes a single built-in
 *          axis addressed by serial number alone, so this adapter takes no channel argument. A device personality
 *          composes exactly one of these. Every method returns a self-contained DeviceError by value; there is no
 *          stored "last error".
 *
 * Threading:
 *  - Controller-scoped calls (open/close/checkConnection) take the global discoveryMtx() because they mutate the
 *    Kinesis device-list/connection state.
 *  - All steady-state calls take serialMtx(serial), so two different devices run concurrently.
 *  - No method holds any lock across a sleep.
 *
 * Reads: position and status are read from the SDK's internal poll cache (never issuing a Request*, which can reset
 *        the buffer). Freshness is judged by the last-message timer; a stale read returns READ_FAILED instead of a
 *        misleading zero/garbage value reported as success.
 */
class DEGORASKINESIS_EXPORT IntStepperController
{
public:

    explicit IntStepperController(types::ThorlabsSN serial);

    types::ThorlabsSN serialNo() const;

    // -- Controller-scoped lifecycle --
    types::DeviceError open();                       ///< Build device list + ISC_Open (FT_DeviceNotFound -> DEVICE_NOT_FOUND).
    types::DeviceError close();                      ///< ISC_Close.
    bool isConnected() const;                        ///< ISC_CheckConnection.

    // -- Connection setup --
    types::DeviceError loadSettings(const std::string& named);   ///< Named settings, or default if @p named is empty.
    types::DeviceError startPolling(int rate_ms);                ///< ISC_StartPolling (SDK-internal cache refresh).
    void stopPolling() noexcept;                                 ///< ISC_StopPolling (teardown-safe).
    void enableFreshnessTimer(int timeout_ms) noexcept;          ///< ISC_EnableLastMsgTimer (read-validity signal).
    void clearMessageQueue() noexcept;                           ///< ISC_ClearMessageQueue (poll status does not auto-clear).

    // -- Motion --
    types::DeviceError enable(bool on);                          ///< ISC_EnableChannel / ISC_DisableChannel.
    types::DeviceError home();                                   ///< ISC_Home (fire-and-forget).
    types::DeviceError stop(types::StopMode mode);               ///< ISC_StopProfiled / ISC_StopImmediate.
    types::DeviceError moveAbsolute(int device_units);           ///< ISC_MoveToPosition.
    types::DeviceError moveRelative(int device_units);           ///< ISC_MoveRelative.
    types::DeviceError jog(types::TravelDirection direction);    ///< ISC_MoveJog.
    types::DeviceError setVelocity(const types::VelocityProfile& profile);   ///< Convert real->device + ISC_SetVelParamsBlock.
    types::DeviceError setJog(const types::JogParameters& params);           ///< Convert real->device + ISC_SetJogParamsBlock.

    // -- Reads (cache-only + freshness; READ_FAILED on stale/comms-lost) --
    types::DeviceError readStatusBits(std::bitset<32>& out_bits);
    types::DeviceError readPosition(int& out_raw);
    types::DeviceError deviceToReal(types::PhysicalUnit unit, int device_units, double& out_real);
    types::DeviceError realToDevice(types::PhysicalUnit unit, double real, int& out_device_units);

private:

    types::ThorlabsSN serial_;
};

// ---------------------------------------------------------------------------------------------------------------------

}} // END NAMESPACES

// ---------------------------------------------------------------------------------------------------------------------
