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
//#include <LeoBase/Crypto/HashCombine.hpp>

// LEOBASE MACROS
LEOBASE_MODULE_BEGIN(network)

// ---------------------------------------------------------------------------------------------------------------------

// TODO HASHER
// TODO LEOEXCEPTIONS
// TODO FROM STRING

class LeoURI
{

public:

    LeoURI() = default;

    explicit LeoURI(const std::string& uri);

    ~LeoURI() = default;

    std::string toString() const;

    std::string toJsonStr(size_t indent_size = 0, size_t level = 0, bool braces = true) const;

    // Getters
    std::string getScheme() const { return scheme_; }
    std::string getAuthority() const { return authority_; }
    std::string getPath() const { return path_; }
    std::string getQuery() const { return query_; }
    std::string getFragment() const { return fragment_; }

    // Setters
    void setScheme(const std::string& scheme) { scheme_ = scheme; }
    void setAuthority(const std::string& authority) { authority_ = authority; }
    void setPath(const std::string& path) { path_ = path; }
    void setQuery(const std::string& query) { query_ = query; }
    void setFragment(const std::string& fragment) { fragment_ = fragment; }

protected:

    std::string scheme_;
    std::string authority_;
    std::string path_;
    std::string query_;
    std::string fragment_;
};

// ---------------------------------------------------------------------------------------------------------------------

LEOBASE_MODULE_END
