/*
 *      Copyright(C) Milethos Technologies SLU. 2025
 *      All rights reserved. Reproduction in whole or in
 *      part is prohibited without the written consent of
 *      the copyright holder.
 */

// C++ INCLUDES
#include <algorithm>
#include <sstream>

// LEOBASE INCLUDES
#include "LeoBase/Helpers/strings_utils.h"
#include "LeoBase/Network/leo_uri.h"

// LEOBASE MACROS
LEOBASE_MODULE_BEGIN(network)

// ---------------------------------------------------------------------------------------------------------------------

LeoURI::LeoURI(const std::string& uri)
{
    auto index = uri.find("://");
    if (index != std::string::npos)
    {
        scheme_ = uri.substr(0, index);
        index += 3;
    }
    else
    {
        index = 0;
    }

    auto authorityEnd = std::find(uri.begin() + index, uri.end(), '/');
    authority_.assign(uri.begin() + index, authorityEnd);

    auto pathStart = authorityEnd;
    auto queryStart = std::find(pathStart, uri.end(), '?');
    auto fragmentStart = std::find(queryStart, uri.end(), '#');

    path_.assign(pathStart, queryStart);
    if (queryStart != uri.end())
        query_.assign(queryStart + 1, fragmentStart);
    if (fragmentStart != uri.end())
        fragment_.assign(fragmentStart + 1, uri.end());
}

std::string LeoURI::toString() const
{
    std::stringstream ss;
    if (!scheme_.empty())
        ss << scheme_ << "://";
    ss << authority_;
    ss << path_;
    if (!query_.empty())
        ss << "?" << query_;
    if (!fragment_.empty())
        ss << "#" << fragment_;
    return ss.str();
}

std::string LeoURI::toJsonStr(size_t indent_size, size_t level, bool braces) const
{
    StreamableFieldValuePairV json_data =
        {
            {"scheme", this->scheme_.empty() ? OptionalStr() : this->scheme_},
            {"authority", this->authority_.empty() ? OptionalStr() : this->authority_},
            {"path", this->path_.empty() ? OptionalStr() : this->path_},
            {"query", this->query_.empty() ? OptionalStr() : this->query_},
            {"fragment", this->fragment_.empty() ? OptionalStr() : this->fragment_}
        };

    return helpers::strings::generateJsonStr(json_data, indent_size, level, braces);
}

// ---------------------------------------------------------------------------------------------------------------------

LEOBASE_MODULE_END
