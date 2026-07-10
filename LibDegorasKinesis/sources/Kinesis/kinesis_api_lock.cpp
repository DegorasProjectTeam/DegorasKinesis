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
#include <memory>
#include <unordered_map>

// PROJECT INCLUDES
#include "LibDegorasKinesis/Kinesis/kinesis_api_lock.h"


// NAMESPACES
namespace dpkin
{
namespace kinesis
{

// ---------------------------------------------------------------------------------------------------------------------

std::mutex& discoveryMtx()
{
    static std::mutex mutex;
    return mutex;
}

std::mutex& serialMtx(const std::string& serial)
{
    // The map stores unique_ptr<mutex> so a rehash never relocates a mutex a caller is holding, and entries are
    // never erased, so a returned reference stays valid for the process lifetime.
    static std::mutex registry_mtx;
    static std::unordered_map<std::string, std::unique_ptr<std::mutex>> registry;

    const std::lock_guard<std::mutex> lock(registry_mtx);
    std::unique_ptr<std::mutex>& slot = registry[serial];
    if (!slot)
        slot = std::make_unique<std::mutex>();
    return *slot;
}

// ---------------------------------------------------------------------------------------------------------------------

}} // END NAMESPACES

// ---------------------------------------------------------------------------------------------------------------------
