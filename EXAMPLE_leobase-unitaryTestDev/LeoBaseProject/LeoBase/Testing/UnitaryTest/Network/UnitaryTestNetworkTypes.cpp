/*
 * Copyright(C) Milethos Technologies SLU. 2026
 * All rights reserved. Reproduction in whole or in
 * part is prohibited without the written consent of
 * the copyright holder.
*/

//C++ INCLUDES

#include <string>
#include <optional>
#include <vector>

// LeoBase INCLUDES
#include <LeoBase/Aliases/Testing>
#include "LeoBase/Network/types/network_types.h"
#include "LeoBase/Testing/unitary/unitary_test_macros.h"

using namespace leobase;
using namespace leobase::testing;
using namespace leobase::network::types;

// =======================================================================
// DECLARATIONS
// =======================================================================

UT_DECLARE_TEST(NetAdptInfo_ToJsonStr_FullyPopulated_SerializesAllFields);
UT_DECLARE_TEST(NetAdptInfo_ToJsonStr_EmptyFields_HandlesOptionalsCorrectly);
UT_DECLARE_TEST(NetAdptInfo_ToJsonStr_EnumCasting_SerializesTypeAsInteger);
UT_DECLARE_TEST(NetAdptInfo_ToJsonStr_Formatting_RespectsBracesParameter);

// =======================================================================
// DEFINITIONS
// =======================================================================

/**
 * @brief Tests that a fully populated NetAdptInfo struct serializes all its fields correctly.
 */
UT_DEFINE_TEST(NetAdptInfo_ToJsonStr_FullyPopulated_SerializesAllFields)
{
    NetAdptInfo info;
    info.id = "eth0";
    info.name = "Ethernet Adapter";
    info.descr = "Realtek PCIe GbE Family Controller";
    info.ip = "192.168.1.100";
    info.mac = "00:1A:2B:3C:4D:5E";
    info.type = NetIntfType::ETHERNET;
    info.mtu = 1500;

    std::string json = info.toJsonStr();

    // Verify that the string is not empty
    UT_EXPECTED_FALSE(json.empty());

    // Verify that the basic keys are present
    UT_EXPECTED_TRUE(json.find("id") != std::string::npos);
    UT_EXPECTED_TRUE(json.find("name") != std::string::npos);
    UT_EXPECTED_TRUE(json.find("descr") != std::string::npos);
    UT_EXPECTED_TRUE(json.find("ip") != std::string::npos);
    UT_EXPECTED_TRUE(json.find("mac") != std::string::npos);
    UT_EXPECTED_TRUE(json.find("mtu") != std::string::npos);

    // Verify that the values are also injected into the string
    UT_EXPECTED_TRUE(json.find("eth0") != std::string::npos);
    UT_EXPECTED_TRUE(json.find("Ethernet Adapter") != std::string::npos);
    UT_EXPECTED_TRUE(json.find("192.168.1.100") != std::string::npos);
    UT_EXPECTED_TRUE(json.find("00:1A:2B:3C:4D:5E") != std::string::npos);
    UT_EXPECTED_TRUE(json.find("1500") != std::string::npos);
}

/**
 * @brief Tests serialization behavior when strings are empty and optionals are nullopt.
 */
UT_DEFINE_TEST(NetAdptInfo_ToJsonStr_EmptyFields_HandlesOptionalsCorrectly)
{
    NetAdptInfo info; // Default to empty strings
    info.type = NetIntfType::UNKNOWN;
    info.mtu = std::nullopt;

    // Should not crash when invoking OptionalStr() or processing nullopt
    std::string json = info.toJsonStr();

    UT_EXPECTED_FALSE(json.empty());

    // Keys must still exist in the JSON (even if their values are null/empty in the helper)
    UT_EXPECTED_TRUE(json.find("id") != std::string::npos);
    UT_EXPECTED_TRUE(json.find("mtu") != std::string::npos);
}

/**
 * @brief Ensures the NetIntfType enum is correctly cast to a numeric value in the output.
 */
UT_DEFINE_TEST(NetAdptInfo_ToJsonStr_EnumCasting_SerializesTypeAsInteger)
{
    NetAdptInfo infoWifi;
    infoWifi.type = NetIntfType::WIFI; // WIFI is 2 based on the enum

    std::string jsonWifi = infoWifi.toJsonStr();

    // Look for the type key and the value 2
    UT_EXPECTED_TRUE(jsonWifi.find("type") != std::string::npos);
    UT_EXPECTED_TRUE(jsonWifi.find("2") != std::string::npos);

    NetAdptInfo infoLoopback;
    infoLoopback.type = NetIntfType::LOOPBACK; // LOOPBACK is 3

    std::string jsonLoopback = infoLoopback.toJsonStr();
    UT_EXPECTED_TRUE(jsonLoopback.find("3") != std::string::npos);
}

/**
 * @brief Verifies that the 'braces' argument alters the output string.
 */
UT_DEFINE_TEST(NetAdptInfo_ToJsonStr_Formatting_RespectsBracesParameter)
{
    NetAdptInfo info;
    info.id = "test_interface";

    std::string jsonWithBraces = info.toJsonStr(0, 0, true);
    std::string jsonWithoutBraces = info.toJsonStr(0, 0, false);

    // The strings must be different if the flag is respected
    UT_EXPECTED_NE(jsonWithBraces, jsonWithoutBraces);

    // Assuming jsonWithBraces includes '{' and jsonWithoutBraces does not
    if (jsonWithBraces.find("{") != std::string::npos) {
        UT_EXPECTED_TRUE(jsonWithoutBraces.find("{") == std::string::npos);
    }
}

// =======================================================================
// SESSION
// =======================================================================

UT_START_SESSION("LeoBase NetworkTypes Unitary Tests")

UT_REGISTER_TEST(Network, NetAdptInfo, NetAdptInfo_ToJsonStr_FullyPopulated_SerializesAllFields);
UT_REGISTER_TEST(Network, NetAdptInfo, NetAdptInfo_ToJsonStr_EmptyFields_HandlesOptionalsCorrectly);
UT_REGISTER_TEST(Network, NetAdptInfo, NetAdptInfo_ToJsonStr_EnumCasting_SerializesTypeAsInteger);
UT_REGISTER_TEST(Network, NetAdptInfo, NetAdptInfo_ToJsonStr_Formatting_RespectsBracesParameter);

UT_RUN_TESTS();

UT_FINISH_SESSION()
