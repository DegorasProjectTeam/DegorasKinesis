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

// C++ INCLUDES
#include <cstdint>
#include <string>

// PROJECT INCLUDES
#include "LibDegorasKinesis/Global/libdegoraskinesis_export.h"


// NAMESPACES
namespace dpkin
{
namespace kinesis
{

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Result code for Kinesis simulator helper operations.
 * @note The simulator executable and the per-process Kinesis simulation API initialisation are different things:
 *       the executable may be running even if this process has not called TLI_InitializeSimulations().
 */
enum class KinesisSimulatorResult : std::uint8_t
{
    OK = 0,                     ///< Operation completed successfully.
    ALREADY_CONNECTED,          ///< Simulator API connection was already initialised by this process.
    ALREADY_DISCONNECTED,       ///< Simulator API connection was not initialised by this process.
    SIMULATOR_NOT_RUNNING,      ///< External Kinesis Simulator executable is not running.
    BUSY_BY_OTHER_PROCESS,      ///< Another process owns the simulator interprocess lock.
    MUTEX_ERROR,                ///< Failed to create, acquire, release, or close the interprocess mutex.
    KINESIS_INITIALIZE_ERROR,   ///< TLI_InitializeSimulations() failed or could not be completed safely.
    KINESIS_UNINITIALIZE_ERROR  ///< TLI_UninitializeSimulations() failed or could not be completed safely.
};

/// @brief Convert a Kinesis simulator result code to a human-readable string.
LIBDEGORASKINESIS_EXPORT std::string toString(KinesisSimulatorResult result);

/**
 * @brief Whether the Thorlabs Kinesis Simulator executable is running.
 * @return true if "Thorlabs.MotionControl.KinesisSimulator.exe" is present in the local process list.
 * @note Only inspects the process list; it does not call any Kinesis API nor start the simulator.
 */
LIBDEGORASKINESIS_EXPORT bool isKinesisSimulatorRunning();

/**
 * @brief Initialise this process' connection to the Kinesis simulator (TLI_InitializeSimulations).
 * @return OK if this process initialised it, ALREADY_CONNECTED if it had already, else an error result.
 * @note Serialised across cooperating processes via a named Windows mutex. Does not start the simulator executable.
 */
LIBDEGORASKINESIS_EXPORT KinesisSimulatorResult connectKinesisSimulator();

/**
 * @brief Uninitialise this process' connection to the Kinesis simulator (TLI_UninitializeSimulations).
 * @return OK on success, ALREADY_DISCONNECTED if this process had not initialised it, else an error result.
 */
LIBDEGORASKINESIS_EXPORT KinesisSimulatorResult disconnectKinesisSimulator();

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief RAII guard around the process' Kinesis simulator connection.
 * @details Calls connectKinesisSimulator() on construction and, only if THIS session established the connection
 *          (result() == OK), disconnectKinesisSimulator() on destruction. This prevents leaking the interprocess
 *          mutex on early returns. If the simulator was already connected by someone else (ALREADY_CONNECTED), the
 *          session does not tear it down.
 */
class LIBDEGORASKINESIS_EXPORT KinesisSimulatorSession
{
public:

    KinesisSimulatorSession();
    ~KinesisSimulatorSession();

    KinesisSimulatorSession(const KinesisSimulatorSession&) = delete;
    KinesisSimulatorSession& operator=(const KinesisSimulatorSession&) = delete;
    KinesisSimulatorSession(KinesisSimulatorSession&&) = delete;
    KinesisSimulatorSession& operator=(KinesisSimulatorSession&&) = delete;

    /// @brief The result of the connect attempt made at construction.
    KinesisSimulatorResult result() const { return this->result_; }

    /// @brief True if the simulator API is usable for this process (freshly connected or already connected).
    bool usable() const
    {
        return this->result_ == KinesisSimulatorResult::OK ||
               this->result_ == KinesisSimulatorResult::ALREADY_CONNECTED;
    }

private:

    KinesisSimulatorResult result_;
};

// ---------------------------------------------------------------------------------------------------------------------

}} // END NAMESPACES

// ---------------------------------------------------------------------------------------------------------------------
