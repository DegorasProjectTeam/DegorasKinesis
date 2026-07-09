/*
 *      Copyright(C) Milethos Technologies SLU. 2025
 *      All rights reserved. Reproduction in whole or in
 *      part is prohibited without the written consent of
 *      the copyright holder.
 */

#pragma once

// C++ INCLUDES
#include <string>
#include <cstdint>

// LEOBASE INCLUDES
#include "LeoBase/Aliases/Global"
#include "LeoBase/Network/types/network_types.h"

// LEOBASE MACROS
LEOBASE_MODULE_BEGIN(network)
BEGIN_NAMESPACE(utils)

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Checks if a TCP connection to the specified host and port can be established.
 *
 * This function attempts to establish a non-blocking TCP connection to the given hostname and port,
 * with a user-defined timeout. It is suitable for checking if a service (e.g., SSH, SFTP, HTTP) is
 * available without performing protocol-level handshakes.
 *
 * @param hostname The hostname or IP address of the remote server.
 * @param port The TCP port to connect to (default is 22, commonly used by SSH/SFTP).
 * @param timeout Timeout in milliseconds for the connection attempt (default is 2000 ms).
 * @return true if the connection could be established (port is open and reachable), false otherwise.
 *
 * @note This function only checks TCP-level connectivity. It does not perform any protocol-level specific
 * verification (e.g., SSH banner or SFTP handshake).
 *
 */
LEOBASE_EXPORT bool isTcpPortOpen(const std::string& hostname, uint16_t port = 22, int timeout = 2000);

/**
 * @brief Retrieves a list of active IPv4 network adapters on the current system.
 *
 * This function enumerates all network interfaces currently in the "up" state (active) and with at
 * least one IPv4 address assigned. For each interface, it collects relevant metadata such as:
 * - Unique identifier.
 * - Human-readable name and description.
 * - Primary IPv4 address.
 * - MAC address.
 * - Interface type (e.g., Ethernet, Wi-Fi, Loopback).
 * - Maximum transmission unit (MTU).
 *
 * @return A vector of NetAdptInfo structures representing each active IPv4 network adapter.
 */
LEOBASE_EXPORT types::NetAdptInfoV getActiveNetAdptInfo();

// ---------------------------------------------------------------------------------------------------------------------

END_NAMESPACE(utils)
LEOBASE_MODULE_END
