/*
 *      Copyright(C) Milethos Technologies SLU. 2025
 *      All rights reserved. Reproduction in whole or in
 *      part is prohibited without the written consent of
 *      the copyright holder.
 */

// C++ INCLUDES
#ifdef _WIN32
#include <winsock2.h>
#include <windows.h>
#include <tlhelp32.h>
#include <pdh.h>
#include <pdhmsg.h>
#include <iphlpapi.h>
#include <ws2tcpip.h>
#include <iptypes.h>
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0600
#elif _WIN32_WINNT < 0x0600
#undef _WIN32_WINNT
#define _WIN32_WINNT 0x0600
#endif
#else
#endif

// LEOBASE INCLUDES
#include "LeoBase/Network/utils/network_utils.h"

// LEOBASE MACROS
LEOBASE_MODULE_BEGIN(network)
BEGIN_NAMESPACE(utils)

// ---------------------------------------------------------------------------------------------------------------------
// Generic implementations.

// ---------------------------------------------------------------------------------------------------------------------
// Windows Implementations.

#ifdef _WIN32

bool isTcpPortOpen(const std::string &hostname, uint16_t port, int timeout)
{
    WSADATA wsaData;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        return false;

    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET)
    {
        WSACleanup();
        return false;
    }

    // Set socket to non-blocking
    u_long mode = 1;
    ioctlsocket(sock, FIONBIO, &mode);

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = inet_addr(hostname.c_str());

    // Resolve hostname if not a direct IP
    if (serverAddr.sin_addr.s_addr == INADDR_NONE)
    {
        hostent* he = gethostbyname(hostname.c_str());
        if (!he) {
            closesocket(sock);
            WSACleanup();
            return false;
        }
        memcpy(&serverAddr.sin_addr, he->h_addr_list[0], he->h_length);
    }

    connect(sock, (sockaddr*)&serverAddr, sizeof(serverAddr));

    fd_set writeSet;
    FD_ZERO(&writeSet);
    FD_SET(sock, &writeSet);

    timeval tv;
    tv.tv_sec = timeout / 1000;
    tv.tv_usec = (timeout % 1000) * 1000;

    int sel = select(0, nullptr, &writeSet, nullptr, &tv);
    bool isConnected = (sel > 0) && FD_ISSET(sock, &writeSet);

    // Close and cleanup.
    closesocket(sock);
    WSACleanup();

    // Return.
    return isConnected;
}

types::NetAdptInfoV getActiveNetAdptInfo()
{
    types::NetAdptInfoV adapters;

    ULONG buff_size = 0;

    if (GetAdaptersAddresses(AF_INET,
                             GAA_FLAG_SKIP_ANYCAST | GAA_FLAG_SKIP_MULTICAST | GAA_FLAG_SKIP_DNS_SERVER,
                             nullptr, nullptr, &buff_size) != ERROR_BUFFER_OVERFLOW)
    {
        return adapters;
    }

    std::vector<char> buffer(buff_size);
    PIP_ADAPTER_ADDRESSES adapter_addrs = reinterpret_cast<PIP_ADAPTER_ADDRESSES>(buffer.data());

    if (GetAdaptersAddresses(AF_INET,
                             GAA_FLAG_SKIP_ANYCAST | GAA_FLAG_SKIP_MULTICAST | GAA_FLAG_SKIP_DNS_SERVER,
                             nullptr, adapter_addrs, &buff_size) != NO_ERROR)
    {
        return adapters;
    }

    while (adapter_addrs != nullptr)
    {
        if (adapter_addrs->OperStatus == IfOperStatusUp)
        {
            PIP_ADAPTER_UNICAST_ADDRESS unicast_addrs = adapter_addrs->FirstUnicastAddress;
            while (unicast_addrs != nullptr)
            {
                sockaddr_in* sockaddr = reinterpret_cast<sockaddr_in*>(unicast_addrs->Address.lpSockaddr);
                char* ip = inet_ntoa(sockaddr->sin_addr);

                // Friendly name and description
                char f_name_ch[260];
                char desc_ch[260];
                char df_char = ' ';

                WideCharToMultiByte(CP_ACP, 0, adapter_addrs->FriendlyName, -1, f_name_ch, 260, &df_char, nullptr);
                WideCharToMultiByte(CP_ACP, 0, adapter_addrs->Description, -1, desc_ch, 260, &df_char, nullptr);

                // MAC address
                std::ostringstream mac_ss;
                for (ULONG i = 0; i < adapter_addrs->PhysicalAddressLength; ++i)
                {
                    mac_ss << std::hex << std::setw(2) << std::setfill('0')
                    << static_cast<int>(adapter_addrs->PhysicalAddress[i]);
                    if (i < adapter_addrs->PhysicalAddressLength - 1)
                        mac_ss << ":";
                }

                // Interface type (basic string mapping)
                types::NetIntfType iface_type;
                switch (adapter_addrs->IfType)
                {
                    case IF_TYPE_ETHERNET_CSMACD:
                        iface_type = types::NetIntfType::ETHERNET;
                        break;
                    case IF_TYPE_IEEE80211:
                        iface_type = types::NetIntfType::WIFI;
                        break;
                    case IF_TYPE_SOFTWARE_LOOPBACK:
                        iface_type = types::NetIntfType::LOOPBACK;
                        break;
                    case IF_TYPE_TUNNEL:
                        iface_type = types::NetIntfType::TUNNEL;
                        break;
                    case IF_TYPE_IEEE1394:
                        iface_type = types::NetIntfType::VIRTUAL;
                        break;
                    default:
                        iface_type = types::NetIntfType::OTHER;
                        break;
                }

                types::NetAdptInfo adaptr;
                adaptr.id = std::string(adapter_addrs->AdapterName);
                adaptr.name = std::string(f_name_ch);
                adaptr.descr = std::string(desc_ch);
                adaptr.ip = std::string(ip);
                adaptr.mac = mac_ss.str();
                adaptr.type = iface_type;
                if (adapter_addrs->Mtu != 0xFFFFFFFF)
                    adaptr.mtu = adapter_addrs->Mtu;
                else
                    adaptr.mtu = std::nullopt;
                adapters.push_back(adaptr);

                unicast_addrs = unicast_addrs->Next;
            }
        }

        adapter_addrs = adapter_addrs->Next;
    }

    return adapters;
}

// ---------------------------------------------------------------------------------------------------------------------
// Unix Implementations.

#else

#endif

// ---------------------------------------------------------------------------------------------------------------------


END_NAMESPACE(utils)
LEOBASE_MODULE_END
