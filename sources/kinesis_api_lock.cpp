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
#include <memory>
#include <unordered_map>

// PROJECT INCLUDES
#include "kinesis_api_lock.h"


// NAMESPACES
namespace thorlabs
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

} // END NAMESPACES

// ---------------------------------------------------------------------------------------------------------------------
