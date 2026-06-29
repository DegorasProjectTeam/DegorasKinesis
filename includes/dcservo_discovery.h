/*
 *  Copyright (C) 2018-2026 Degoras Project Team
 *
 *  This file is part of a small-scale research or utility tool built atop
 *  the Degoras Project infrastructure, released under the MIT License.
 *
 *  SPDX-License-Identifier: MIT
 *
 *  See the LICENSE file in the root directory for full license details.
 */

#pragma once

// PROJECT INCLUDES
#include "libthorlabskinesis_global.h"
#include "common_types.h"


// NAMESPACES
namespace thorlabs
{
namespace dcservo
{

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Enumerate the serial numbers of connected devices of a given Kinesis type.
 * @param type_id The Kinesis device type id (e.g. 101 for M30XY, 105 for M30X).
 * @param[out] list Filled with the discovered serial numbers (cleared first).
 * @return OPERATION_OK on success, THORLABS_INTERNAL_ERROR if the device-list build/query failed.
 * @note Rebuilds the global Kinesis device list under discoveryMtx(). An already-open device does not appear here.
 */
LIBTHORLABSKINESIS_EXPORT types::OperationResult enumerateByTypeId(int type_id, types::ThorlabsSNList& list);

// ---------------------------------------------------------------------------------------------------------------------

}} // END NAMESPACES

// ---------------------------------------------------------------------------------------------------------------------
