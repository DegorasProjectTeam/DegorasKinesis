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

// PROJECT INCLUDES
#include "DegorasKinesis/Kinesis/kinesis_error.h"


// NAMESPACES
namespace dpkin
{
namespace kinesis
{

using namespace dpkin::types;

// ---------------------------------------------------------------------------------------------------------------------

OperationResult categoryFromKinesis(short code)
{
    switch (code)
    {
        case 0x00: return OperationResult::OPERATION_OK;       // FT_OK
        case 0x02: return OperationResult::DEVICE_NOT_FOUND;   // FT_DeviceNotFound
        case 0x03:                                             // FT_DeviceNotOpened
        case 0x04:                                             // FT_IOError
        case 0x07: return OperationResult::NOT_CONNECTED;      // FT_DeviceNotPresent
        default:   return OperationResult::THORLABS_INTERNAL_ERROR;  // FT_InvalidHandle/Resources/Parameter/IncorrectDevice/other
    }
}

// ---------------------------------------------------------------------------------------------------------------------

}} // END NAMESPACES

// ---------------------------------------------------------------------------------------------------------------------
