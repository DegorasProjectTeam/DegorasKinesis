/*
 *      Copyright(C) Milethos Technologies SLU. 2025
 *      All rights reserved. Reproduction in whole or in
 *      part is prohibited without the written consent of
 *      the copyright holder.
 */

// C++ INCLUDES
#include <sstream>

// LEOBASE INCLUDES
#include "LeoBase/Helpers/strings_utils.h"
#include "LeoBase/Network/leo_url.h"

// LEOBASE MACROS
LEOBASE_MODULE_BEGIN(network)

// ---------------------------------------------------------------------------------------------------------------------

LeoURL::LeoURL(const std::string& url)
    : LeoURI(url)
{
    std::string auth_aux = this->authority_;

    // Find the user/password part (if present)
    auto userpwEnd = auth_aux.find('@');
    if (userpwEnd != std::string::npos)
    {
        // Find the separator between username and password
        auto sep = auth_aux.find(':');

        if (sep != std::string::npos && sep < userpwEnd) {
            username_ = auth_aux.substr(0, sep);
            password_ = auth_aux.substr(sep + 1, userpwEnd - sep - 1);
        } else {
            username_ = auth_aux.substr(0, userpwEnd);
            password_ = "";
        }

        auth_aux = auth_aux.substr(userpwEnd + 1);
    }

    // Extract hostname and port
    auto hostEnd = auth_aux.find(':');
    if (hostEnd != std::string::npos)
    {
        int port_aux;
        hostname_ = auth_aux.substr(0, hostEnd);
        std::string port_str = auth_aux.substr(hostEnd + 1);

        try
        {
            port_aux = std::stoi(port_str);
            if (port_aux < 0 || port_aux > 65535)
                throw std::out_of_range("Port number out of range: " + port_str);
        }
        catch (const std::exception& e)
        {
            throw std::invalid_argument("Invalid port number: " + port_str);
        }

        this->port_ = static_cast<unsigned>(port_aux);
    }
    else
    {
        hostname_ = auth_aux;  // No port, entire authority is hostname
        port_.reset();
    }
}

std::string LeoURL::toString(bool encode) const
{
    std::stringstream ss;

    // Add protocol.
    if(!this->scheme_.empty())
        ss << this->scheme_ << "://";

    // Add username and password if present
    if (!username_.empty() || !password_.empty())
    {
        ss << (encode ? urlEncode(username_) : username_);
        if (!password_.empty())
            ss << ":" << (encode ? urlEncode(password_) : password_);
        ss << "@";
    }

    // Add hostname.
    ss << hostname_;

    // Add port.
    if (port_)
        ss << ":" << port_.value();

    // Add path.
    ss << (encode ? urlEncode(path_, false) : path_);

    // Add query.
    if (!this->query_.empty())
    {
        ss << "?";
        ss << (encode ? urlEncode(this->query_) : this->query_);
    }

    // Add fragment.
    if (!fragment_.empty())
    {
        ss << "#";
        ss << (encode ? urlEncode(fragment_) : fragment_);
    }

    return ss.str();
}

std::string LeoURL::toJsonStr(size_t indent_size, size_t level, bool braces) const
{
    StreamableFieldValuePairV json_data =
    {
        {"", LeoURI::toJsonStr(indent_size, level, false)},
        {"username", this->username_.empty() ? OptionalStr() : this->username_},
        {"password", this->password_.empty() ? OptionalStr() : this->password_},
        {"hostname", this->hostname_.empty() ? OptionalStr() : this->hostname_},
        {"port", this->port_},
        {"url", this->toString()},
        {"url_encoded", this->toString(true)}
    };

    return helpers::strings::generateJsonStr(json_data, indent_size, level, braces);
}

std::string LeoURL::getUsername() const
{
    return this->username_;
}

std::string LeoURL::getPassword() const
{
    return this->password_;
}

std::string LeoURL::getHostname() const
{
    return this->hostname_;
}

std::optional<unsigned> LeoURL::getPort() const
{
    return port_;
}

void LeoURL::setUsername(const std::string& username)
{
    this->username_ = username;
}

void LeoURL::setPassword(const std::string& password)
{
    this->password_ = password;
}

void LeoURL::setHostname(const std::string& hostname)
{
    this->hostname_ = hostname;
}

void LeoURL::setPort(unsigned port)
{
    this->port_ = port;
}

bool LeoURL::operator==(const LeoURL& other) const
{
    if (port_ != other.port_)
        return false;

    if (port_ && (port_.value() != other.port_.value()))
        return false;

    return this->username_ == other.username_ &&
           this->password_ == other.password_ &&
           this->scheme_ == other.scheme_ &&
           this->hostname_ == other.hostname_ &&
           this->path_ == other.path_ &&
           this->query_ == other.query_ &&
           this->fragment_ == other.fragment_;
}

std::string LeoURL::urlEncode(const std::string& value, bool enc_slash)
{
    std::ostringstream encoded;
    encoded.fill('0');
    encoded << std::hex;

    for (unsigned char c : value)
    {
        // RFC 3986
        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~' ||
            (!enc_slash && c == '/'))
        {
            encoded << c;
        }
        else
            encoded << '%' << std::setw(2) << std::uppercase << int(c);
    }
    return encoded.str();
}

// std::size_t LeoURL::Hasher::operator()(const URL& url) const
// {
// TODO: add parameters to hash? The path/username/etc must be encoded before hash?

//     std::size_t seed = 0;

//     hashCombine(seed,
//                 url.username_,
//                 url.password_,
//                 url.protocol_,
//                 url.hostname_,
//                 url.path_,
//                 url.port().valid() ? url.port().get() : 0);

//     return seed;
// }

// ---------------------------------------------------------------------------------------------------------------------

LEOBASE_MODULE_END

