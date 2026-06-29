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

// C++ INCLUDES
#include <cstdint>
#include <string>

// PROJECT INCLUDES
#include "libthorlabskinesis_global.h"


// NAMESPACES
namespace thorlabs
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
LIBTHORLABSKINESIS_EXPORT std::string toString(KinesisSimulatorResult result);

/**
 * @brief Whether the Thorlabs Kinesis Simulator executable is running.
 * @return true if "Thorlabs.MotionControl.KinesisSimulator.exe" is present in the local process list.
 * @note Only inspects the process list; it does not call any Kinesis API nor start the simulator.
 */
LIBTHORLABSKINESIS_EXPORT bool isKinesisSimulatorRunning();

/**
 * @brief Initialise this process' connection to the Kinesis simulator (TLI_InitializeSimulations).
 * @return OK if this process initialised it, ALREADY_CONNECTED if it had already, else an error result.
 * @note Serialised across cooperating processes via a named Windows mutex. Does not start the simulator executable.
 */
LIBTHORLABSKINESIS_EXPORT KinesisSimulatorResult connectKinesisSimulator();

/**
 * @brief Uninitialise this process' connection to the Kinesis simulator (TLI_UninitializeSimulations).
 * @return OK on success, ALREADY_DISCONNECTED if this process had not initialised it, else an error result.
 */
LIBTHORLABSKINESIS_EXPORT KinesisSimulatorResult disconnectKinesisSimulator();

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief RAII guard around the process' Kinesis simulator connection.
 * @details Calls connectKinesisSimulator() on construction and, only if THIS session established the connection
 *          (result() == OK), disconnectKinesisSimulator() on destruction. This prevents leaking the interprocess
 *          mutex on early returns. If the simulator was already connected by someone else (ALREADY_CONNECTED), the
 *          session does not tear it down.
 */
class LIBTHORLABSKINESIS_EXPORT KinesisSimulatorSession
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

} // END NAMESPACES

// ---------------------------------------------------------------------------------------------------------------------
