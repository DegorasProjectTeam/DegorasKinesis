/*
 *  LibThorlabsKinesis - An extensible C++ library for controlling Thorlabs Kinesis motion devices.
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
#include <mutex>
#include <string>

// PROJECT INCLUDES
#include "LibThorlabsKinesis/libthorlabskinesis_global.h"


// NAMESPACES
namespace thorlabs
{

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Process-wide mutex serialising the global Kinesis discovery / simulator calls (TLI_*).
 * @return Reference to the single discovery mutex.
 * @note The Kinesis device-list build/query and the simulator init/uninit are process-global and must never run
 *       concurrently with each other or with a connect. Per-device operations use serialMtx() instead, so device
 *       I/O on distinct serials is NOT serialised against each other.
 */
LIBTHORLABSKINESIS_EXPORT std::mutex& discoveryMtx();

/**
 * @brief Per-serial mutex serialising calls that target a single device.
 * @param serial The device serial number the caller is about to act on.
 * @return Reference to the mutex dedicated to @p serial (stable for the process lifetime).
 * @note Two different serials get two different mutexes, so independent devices can be driven concurrently.
 *       The returned reference is stable: registry entries are never moved or erased while the process runs.
 * @warning This assumes the Kinesis BDC_* API is thread-safe across distinct serials. If that assumption is
 *          disproved (milestone M-VAL), make this return a single shared mutex instead; call sites do not change
 *          because the lock is reached through this function rather than held as a member.
 */
LIBTHORLABSKINESIS_EXPORT std::mutex& serialMtx(const std::string& serial);

// ---------------------------------------------------------------------------------------------------------------------

} // END NAMESPACES

// ---------------------------------------------------------------------------------------------------------------------
