/*
 *      Copyright(C) Milethos Technologies SLU. 2025
 *      All rights reserved. Reproduction in whole or in
 *      part is prohibited without the written consent of
 *      the copyright holder.
 */

#pragma once

// C++ INCLUDES
#include <string>

// LEOBASE INCLUDES
#include "LeoBase/Aliases/Global"
#include "LeoBase/Network/leo_uri.h"
//#include <LeoBase/Crypto/HashCombine.hpp>

// LEOBASE MACROS
LEOBASE_MODULE_BEGIN(network)

// ---------------------------------------------------------------------------------------------------------------------

// TODO HASHER
// TODO LEOEXCEPTIONS
// TODO FROM STRING

class LEOBASE_EXPORT LeoURL : public LeoURI
{
public:

    LeoURL() = default;

    LeoURL(const LeoURL&) = default;

    LeoURL(LeoURL&&) = default;

    LeoURL& operator=(const LeoURL& other) = default;

    LeoURL& operator=(LeoURL&& other) = default;

    LeoURL(const std::string &url);

    ~LeoURL() = default;

    std::string toString(bool encoded = false) const;

    std::string toJsonStr(size_t indent_size = 0, size_t level = 0,
                          bool braces = true) const;

    std::string getUsername() const;

    std::string getPassword() const;

    std::string getHostname() const;

    OptionalU getPort() const;

    void setUsername(const std::string& username);

    void setPassword(const std::string& password);

    void setHostname(const std::string& hostname);

    void setPort(unsigned port);

    bool operator==(const LeoURL& other) const;

    // TODO PREGUNTAR A MANU
    // struct Hasher
    // {
    //     std::size_t operator()(const LeoURL& url) const;
    // };

protected:

    static std::string urlEncode(const std::string& value, bool enc_slash = true);

    std::string username_;
    std::string password_;
    std::string hostname_;
    OptionalU port_;
};

// ---------------------------------------------------------------------------------------------------------------------

LEOBASE_MODULE_END
