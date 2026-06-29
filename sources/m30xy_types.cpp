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
#include "m30xy_types.h"


// NAMESPACES
namespace thorlabs
{
namespace types
{

// ---------------------------------------------------------------------------------------------------------------------

M30XYChannelStatus::M30XYChannelStatus() :
    channel(Channel::X_CHANNEL),
    valid(false),
    flags(),
    pos_raw(0),
    pos_mm(0.0)
{}

M30XYChannelStatus::M30XYChannelStatus(Channel ch) :
    M30XYChannelStatus()
{
    this->channel = ch;
}

std::string M30XYChannelStatus::toJsonStr() const
{
    std::ostringstream ss;
    ss << "{"
       << "\"channel\": " << static_cast<int>(this->channel) << ","
       << "\"valid\": " << (this->valid ? "true" : "false") << ","
       << "\"pos_raw\": " << this->pos_raw << ","
       << "\"pos_mm\": " << this->pos_mm << ","
       << "\"flags\": " << this->flags.toJsonStr()
       << "}";
    return ss.str();
}

// ---------------------------------------------------------------------------------------------------------------------

M30XYDeviceStatus::M30XYDeviceStatus() :
    serial_no(),
    connected(false),
    chann_x(Channel::X_CHANNEL),
    chann_y(Channel::Y_CHANNEL)
{}

std::string M30XYDeviceStatus::toJsonStr() const
{
    std::ostringstream ss;
    ss << "{"
       << "\"serial_no\": \"" << this->serial_no << "\","
       << "\"connected\": " << (this->connected ? "true" : "false") << ","
       << "\"chann_x\": " << this->chann_x.toJsonStr() << ","
       << "\"chann_y\": " << this->chann_y.toJsonStr()
       << "}";
    return ss.str();
}

// ---------------------------------------------------------------------------------------------------------------------

}} // END NAMESPACES

// ---------------------------------------------------------------------------------------------------------------------
