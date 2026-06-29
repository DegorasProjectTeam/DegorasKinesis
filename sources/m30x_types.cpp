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

// C++ INCLUDES
#include <sstream>

// PROJECT INCLUDES
#include "m30x_types.h"


// NAMESPACES
namespace thorlabs
{
namespace types
{

// ---------------------------------------------------------------------------------------------------------------------

M30XChannelStatus::M30XChannelStatus() :
    valid(false),
    flags(),
    pos_raw(0),
    pos_mm(0.0)
{}

std::string M30XChannelStatus::toJsonStr() const
{
    std::ostringstream ss;
    ss << "{"
       << "\"valid\": " << (this->valid ? "true" : "false") << ","
       << "\"pos_raw\": " << this->pos_raw << ","
       << "\"pos_mm\": " << this->pos_mm << ","
       << "\"flags\": " << this->flags.toJsonStr()
       << "}";
    return ss.str();
}

// ---------------------------------------------------------------------------------------------------------------------

M30XDeviceStatus::M30XDeviceStatus() :
    serial_no(),
    connected(false),
    chann()
{}

std::string M30XDeviceStatus::toJsonStr() const
{
    std::ostringstream ss;
    ss << "{"
       << "\"serial_no\": \"" << this->serial_no << "\","
       << "\"connected\": " << (this->connected ? "true" : "false") << ","
       << "\"chann\": " << this->chann.toJsonStr()
       << "}";
    return ss.str();
}

// ---------------------------------------------------------------------------------------------------------------------

}} // END NAMESPACES

// ---------------------------------------------------------------------------------------------------------------------
