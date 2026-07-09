/*
 *      Copyright(C) Milethos Technologies SLU. 2025
 *      All rights reserved. Reproduction in whole or in
 *      part is prohibited without the written consent of
 *      the copyright holder.
 */

#pragma once

// C++ INCLUDES
#include <cstdint>

// LEOBASE INCLUDES
#include "LeoBase/Aliases/Global"
#include "LeoBase/Aliases/Helpers"

// LEOBASE MACROS
LEOBASE_MODULE_BEGIN(network)
BEGIN_NAMESPACE(types)

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Common enumeration for network interface types, reusable across Windows and Linux.
 */
enum class NetIntfType : std::uint16_t
{
    UNKNOWN,    ///< Unknown or undetermined interface type.
    ETHERNET,   ///< Wired Ethernet interface.
    WIFI,       ///< Wireless Wi-Fi interface.
    LOOPBACK,   ///< Local loopback interface (127.0.0.1).
    TUNNEL,     ///< Tunnel interface (e.g., VPN, IPv6-over-IPv4).
    VIRTUAL,    ///< Virtual interface (e.g., Firewire, virtualization adapters).
    OTHER       ///< Other or unclassified interface type.
};

struct NetAdptInfo
{
    std::string id;
    std::string name;
    std::string descr;
    std::string ip;
    std::string mac;
    NetIntfType type;
    std::optional<std::uint32_t> mtu;

    std::string toJsonStr(size_t indent_size = 0, size_t level = 0, bool braces = true) const
    {
        StreamableFieldValuePairV json_data =
            {
                {"id", this->id.empty() ? OptionalStr() : this->id},
                {"name", this->name.empty() ? OptionalStr() : this->name},
                {"descr", this->descr.empty() ? OptionalStr() : this->descr},
                {"ip", this->ip.empty() ? OptionalStr() : this->ip},
                {"mac", this->mac.empty() ? OptionalStr() : this->mac},
                {"type", static_cast<std::uint16_t>(type)},
                {"mtu", this->mtu}
            };

        return helpers::strings::generateJsonStr(json_data, indent_size, level, braces);
    }
};

using NetAdptInfoV = std::vector<NetAdptInfo>;

// ---------------------------------------------------------------------------------------------------------------------

END_NAMESPACE(types)
LEOBASE_MODULE_END
