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

// PROJECT INCLUDES
#include "LibDegorasKinesis/Global/libdegoraskinesis_export.h"
#include "LibDegorasKinesis/Common/common_types.h"


// NAMESPACES
namespace dpkin
{
namespace kinesis
{

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Enumerate the serial numbers of connected devices of a given Kinesis type.
 * @param type_id The Kinesis device type id (e.g. 101 for M30XY, 105 for M30X).
 * @param[out] list Filled with the discovered serial numbers (cleared first).
 * @return OPERATION_OK on success, THORLABS_INTERNAL_ERROR if the device-list build/query failed.
 * @note Rebuilds the global Kinesis device list under discoveryMtx(). An already-open device does not appear here.
 */
LIBDEGORASKINESIS_EXPORT types::OperationResult enumerateByTypeId(int type_id, types::ThorlabsSNList& list);

/**
 * @brief Whether a Thorlabs serial number belongs to a given Kinesis device type id.
 * @param serial  The serial number to check.
 * @param type_id The Kinesis device type id (e.g. 55 for K10CR2, 101 for M30XY, 105 for M30X).
 * @return true if @p serial is all digits and begins with the decimal digits of @p type_id followed by at least one
 *         more digit (the unit number); false otherwise.
 * @details Thorlabs serial numbers begin with the device type id (e.g. "55000002" is a type-55 K10CR2). Because type
 *          ids have different digit lengths, the type cannot be parsed unambiguously from a serial in isolation, so
 *          this validates a serial against a KNOWN type id by prefix rather than parsing one out. Pure string check:
 *          it contacts no device and is safe to call before connecting.
 */
LIBDEGORASKINESIS_EXPORT bool serialMatchesTypeId(const types::ThorlabsSN& serial, int type_id);

// ---------------------------------------------------------------------------------------------------------------------

}} // END NAMESPACES

// ---------------------------------------------------------------------------------------------------------------------
