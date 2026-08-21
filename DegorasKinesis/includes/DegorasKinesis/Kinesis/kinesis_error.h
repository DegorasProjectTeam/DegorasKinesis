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

// PROJECT INCLUDES
#include "DegorasKinesis/Global/degoraskinesis_export.h"
#include "DegorasKinesis/Common/common_types.h"


// NAMESPACES
namespace dpkin
{
namespace kinesis
{

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Map a raw Kinesis FT_ status/return code to a high-level OperationResult category.
 * @param code The raw short returned by any Kinesis call.
 * @return The category. The raw code is preserved separately in DeviceError::kinesis_code.
 * @note The FT_ code family is shared across every Kinesis module, so this mapping is used by all Layer-2 adapters
 *       (Benchtop DC Servo, Integrated Stepper, ...). Pure function (no vendor-SDK dependency in its interface), so
 *       it is unit-testable without hardware.
 */
DEGORASKINESIS_EXPORT types::OperationResult categoryFromKinesis(short code);

// ---------------------------------------------------------------------------------------------------------------------

}} // END NAMESPACES

// ---------------------------------------------------------------------------------------------------------------------
