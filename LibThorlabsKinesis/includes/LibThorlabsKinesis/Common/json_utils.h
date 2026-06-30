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
#include <string>
#include <vector>


// NAMESPACES
namespace thorlabs
{
namespace json
{

// ---------------------------------------------------------------------------------------------------------------------
// Minimal JSON helpers, internal to the library, for round-tripping the compact JSON produced by the types'
// toJsonStr(). They are NOT a general-purpose JSON parser: the extractors are tolerant of surrounding whitespace
// (so both the compact and the prettified forms parse), are key-based and best-effort (a missing key yields the
// supplied default), and assume the object shapes this library emits. They are not exported across the DLL boundary.
// ---------------------------------------------------------------------------------------------------------------------

/// @brief Re-indent a compact JSON string into a human-readable (pretty) multi-line form.
std::string prettify(const std::string& compact);

/// @brief Read a boolean value by key (true if the token is "true").
bool getBool(const std::string& json, const std::string& key, bool def = false);

/// @brief Read an integer value by key.
int getInt(const std::string& json, const std::string& key, int def = 0);

/// @brief Read a floating-point value by key.
double getDouble(const std::string& json, const std::string& key, double def = 0.0);

/// @brief Read a (quoted) string value by key.
std::string getString(const std::string& json, const std::string& key, const std::string& def = std::string());

/// @brief Read a nested object value by key, returned as its raw "{ ... }" substring (empty if absent).
std::string getObject(const std::string& json, const std::string& key);

/// @brief Read an array of booleans by key.
std::vector<bool> getBoolArray(const std::string& json, const std::string& key);

// ---------------------------------------------------------------------------------------------------------------------

}} // END NAMESPACES

// ---------------------------------------------------------------------------------------------------------------------
