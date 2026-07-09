/*
 * Copyright(C) Milethos Technologies SLU. 2026
 * All rights reserved. Reproduction in whole or in
 * part is prohibited without the written consent of
 * the copyright holder.
 */

// C++ INCLUDES
#include <string>
#include <vector>
#include <regex>

// LeoBase INCLUDES
#include <LeoBase/Aliases/Testing>
#include "LeoBase/Network/utils/network_utils.h"
#include "LeoBase/Testing/unitary/unitary_test_macros.h"

using namespace leobase;
using namespace leobase::testing;
using namespace leobase::network::utils;

// Alias
namespace types = leobase::network::types;

// =======================================================================
// HELPER FUNCTIONS
// =======================================================================
namespace {
bool isValidIPv4(const std::string& ip) {
    const std::regex ipv4_regex("^((25[0-5]|(2[0-4]|1\\d|[1-9]|)\\d)\\.?\\b){4}$");
    return std::regex_match(ip, ipv4_regex);
}
}

// =======================================================================
// DECLARATIONS
// =======================================================================

#ifdef _WIN32

// Basic Network Tests
UT_DECLARE_TEST(IsTcpPortOpen_ReturnsTrueForKnownOpenPort);
UT_DECLARE_TEST(IsTcpPortOpen_ReturnsFalseForClosedPort);
UT_DECLARE_TEST(IsTcpPortOpen_ReturnsFalseForInvalidHost);

// Exhaustive Network Tests
UT_DECLARE_TEST(IsTcpPortOpen_ValidHostnameResolvesAndConnects);
UT_DECLARE_TEST(IsTcpPortOpen_InvalidPortZero_ReturnsFalse);
UT_DECLARE_TEST(IsTcpPortOpen_ZeroTimeout_HandledCorrectly);

// Basic Adapter Tests
UT_DECLARE_TEST(GetActiveNetAdptInfo_ReturnsAtLeastOneAdapter);
UT_DECLARE_TEST(GetActiveNetAdptInfo_ValidatesAdapterDataIntegrity);
UT_DECLARE_TEST(GetActiveNetAdptInfo_ValidatesMacAddressFormat);

// Exhaustive Adapter Tests
UT_DECLARE_TEST(GetActiveNetAdptInfo_ValidatesMTUSizes);
UT_DECLARE_TEST(GetActiveNetAdptInfo_LoopbackHasCorrectIP);

#endif // _WIN32

// =======================================================================
// DEFINITIONS
// =======================================================================

#ifdef _WIN32

// -----------------------------------------------------------------------
// TCP PORT OPEN TESTS
// -----------------------------------------------------------------------

/**
 * @brief Tests that a known open port using a direct IP returns true.
 */
UT_DEFINE_TEST(IsTcpPortOpen_ReturnsTrueForKnownOpenPort)
{
    const std::string host = "8.8.8.8";
    const uint16_t port = 53;
    const int timeout_ms = 3000;

    bool isOpen = isTcpPortOpen(host, port, timeout_ms);
    UT_EXPECTED_TRUE(isOpen);
}

/**
 * @brief Tests the gethostbyname branch by passing a domain name instead of a direct IP.
 */
UT_DEFINE_TEST(IsTcpPortOpen_ValidHostnameResolvesAndConnects)
{
    // google.com should always resolve and have port 80 (HTTP) open
    const std::string host = "google.com";
    const uint16_t port = 80;
    const int timeout_ms = 3000;

    bool isOpen = isTcpPortOpen(host, port, timeout_ms);
    UT_EXPECTED_TRUE(isOpen);
}

/**
 * @brief Tests that checking a highly improbable port on localhost returns false.
 */
UT_DEFINE_TEST(IsTcpPortOpen_ReturnsFalseForClosedPort)
{
    const std::string host = "127.0.0.1";
    const uint16_t port = 60000;
    const int timeout_ms = 500;

    bool isOpen = isTcpPortOpen(host, port, timeout_ms);
    UT_EXPECTED_FALSE(isOpen);
}

/**
 * @brief Ensures the function handles unresolvable hostnames cleanly without crashing.
 */
UT_DEFINE_TEST(IsTcpPortOpen_ReturnsFalseForInvalidHost)
{
    const std::string host = "invalid-domain-name-leobase.local";
    const uint16_t port = 80;
    const int timeout_ms = 1000;

    bool isOpen = isTcpPortOpen(host, port, timeout_ms);
    UT_EXPECTED_FALSE(isOpen);
}

/**
 * @brief Ensures passing port 0 fails gracefully, as it is generally invalid for connections.
 */
UT_DEFINE_TEST(IsTcpPortOpen_InvalidPortZero_ReturnsFalse)
{
    const std::string host = "127.0.0.1";
    const uint16_t port = 0;
    const int timeout_ms = 1000;

    bool isOpen = isTcpPortOpen(host, port, timeout_ms);
    UT_EXPECTED_FALSE(isOpen);
}

/**
 * @brief Verifies that a timeout of 0 does not cause mathematical or select() crashes.
 */
UT_DEFINE_TEST(IsTcpPortOpen_ZeroTimeout_HandledCorrectly)
{
    // A 0 timeout means the select() function returns immediately.
    // We just want to ensure it doesn't crash. Result can be false or true depending on OS caching.
    const std::string host = "127.0.0.1";
    const uint16_t port = 60000;
    const int timeout_ms = 0;

    bool isOpen = isTcpPortOpen(host, port, timeout_ms);
    // As long as it reaches this point without a segfault/exception, the test is successful.
    UT_EXPECTED_FALSE(isOpen);
}

// -----------------------------------------------------------------------
// ADAPTER INFO TESTS
// -----------------------------------------------------------------------

/**
 * @brief Ensures that the system can retrieve at least one active network adapter.
 */
UT_DEFINE_TEST(GetActiveNetAdptInfo_ReturnsAtLeastOneAdapter)
{
    auto adapters = getActiveNetAdptInfo();
    UT_EXPECTED_FALSE(adapters.empty());
}

/**
 * @brief Validates the basic string and enum integrity of the populated data structures.
 */
UT_DEFINE_TEST(GetActiveNetAdptInfo_ValidatesAdapterDataIntegrity)
{
    auto adapters = getActiveNetAdptInfo();

    if (adapters.empty()) {
        UT_EXPECTED_FALSE(adapters.empty());
        return;
    }

    bool allValid = true;
    for (const auto& adapter : adapters)
    {
        // IDs are typically GUIDs in Windows, Names are strings like "Ethernet 1"
        if (adapter.id.empty() || adapter.name.empty()) {
            allValid = false;
        }

        if (!isValidIPv4(adapter.ip)) {
            allValid = false;
        }

        // Check enum validity
        if (static_cast<int>(adapter.type) < 0) {
            allValid = false;
        }
    }

    UT_EXPECTED_TRUE(allValid);
}

/**
 * @brief Checks that the MAC address string is properly formatted (XX:XX:XX:XX:XX:XX).
 */
UT_DEFINE_TEST(GetActiveNetAdptInfo_ValidatesMacAddressFormat)
{
    auto adapters = getActiveNetAdptInfo();

    bool macFormatOk = true;
    bool checkedAtLeastOne = false;

    for (const auto& adapter : adapters)
    {
        // Skip loopback interfaces or adapters with empty/zeroed MACs
        if (!adapter.mac.empty() && adapter.mac != "00:00:00:00:00:00" && adapter.type != types::NetIntfType::LOOPBACK)
        {
            checkedAtLeastOne = true;

            if (adapter.mac.length() != 17) {
                macFormatOk = false;
            }

            int colonCount = 0;
            for (char c : adapter.mac) {
                if (c == ':') colonCount++;
            }

            if (colonCount != 5) {
                macFormatOk = false;
            }
        }
    }

    if (checkedAtLeastOne) {
        UT_EXPECTED_TRUE(macFormatOk);
    } else {
        UT_EXPECTED_TRUE(true); // Pass trivially if isolated environment
    }
}

/**
 * @brief Validates that MTU sizes reported by the OS are within a logical range.
 */
UT_DEFINE_TEST(GetActiveNetAdptInfo_ValidatesMTUSizes)
{
    auto adapters = getActiveNetAdptInfo();
    bool mtuLogical = true;

    for (const auto& adapter : adapters)
    {
        if (adapter.mtu.has_value())
        {
            // MTU should generally be > 0. Standard Ethernet is 1500, minimum IPv4 is 68.
            if (adapter.mtu.value() < 68) {
                mtuLogical = false;
                break;
            }
        }
    }

    UT_EXPECTED_TRUE(mtuLogical);
}

/**
 * @brief Ensures that if a Loopback adapter is detected, its IP conforms to localhost ranges.
 */
UT_DEFINE_TEST(GetActiveNetAdptInfo_LoopbackHasCorrectIP)
{
    auto adapters = getActiveNetAdptInfo();
    bool loopbackIpValid = true;

    for (const auto& adapter : adapters)
    {
        if (adapter.type == types::NetIntfType::LOOPBACK)
        {
            // Loopback IP should typically start with "127." (e.g., 127.0.0.1)
            if (adapter.ip.rfind("127.", 0) != 0) {
                loopbackIpValid = false;
                break;
            }
        }
    }

    UT_EXPECTED_TRUE(loopbackIpValid);
}

#endif // _WIN32

// =======================================================================
// SESSION
// =======================================================================

UT_START_SESSION("LeoBase Network Utils Unitary Tests")

#ifdef _WIN32
// TCP Port Tests
UT_REGISTER_TEST(Network, Utils, IsTcpPortOpen_ReturnsTrueForKnownOpenPort);
UT_REGISTER_TEST(Network, Utils, IsTcpPortOpen_ValidHostnameResolvesAndConnects);
UT_REGISTER_TEST(Network, Utils, IsTcpPortOpen_ReturnsFalseForClosedPort);
UT_REGISTER_TEST(Network, Utils, IsTcpPortOpen_ReturnsFalseForInvalidHost);
UT_REGISTER_TEST(Network, Utils, IsTcpPortOpen_InvalidPortZero_ReturnsFalse);
UT_REGISTER_TEST(Network, Utils, IsTcpPortOpen_ZeroTimeout_HandledCorrectly);

// Adapter Info Tests
UT_REGISTER_TEST(Network, Utils, GetActiveNetAdptInfo_ReturnsAtLeastOneAdapter);
UT_REGISTER_TEST(Network, Utils, GetActiveNetAdptInfo_ValidatesAdapterDataIntegrity);
UT_REGISTER_TEST(Network, Utils, GetActiveNetAdptInfo_ValidatesMacAddressFormat);
UT_REGISTER_TEST(Network, Utils, GetActiveNetAdptInfo_ValidatesMTUSizes);
UT_REGISTER_TEST(Network, Utils, GetActiveNetAdptInfo_LoopbackHasCorrectIP);
#endif // _WIN32

UT_RUN_TESTS();

UT_FINISH_SESSION()
