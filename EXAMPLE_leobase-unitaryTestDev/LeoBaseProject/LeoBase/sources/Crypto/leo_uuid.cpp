/*
 *      Copyright(C) Milethos Technologies SLU. 2025
 *      All rights reserved. Reproduction in whole or in
 *      part is prohibited without the written consent of
 *      the copyright holder.
 */

// C++ INCLUDES
#include <cstddef>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <array>

// LEOBASE INCLUDES
#include "LeoBase/Crypto/leo_uuid.h"

// LEOBASE MACROS
LEOBASE_MODULE_BEGIN(crypto)

// ---------------------------------------------------------------------------------------------------------------------

LeoUUID::LeoUUID()
{
    this->clear();
}

LeoUUID::LeoUUID(const std::array<std::byte, 16> &bytes):
    bytes_(bytes)
{}

std::string LeoUUID::toRFC4122String() const
{
    std::stringstream ss;
    ss << std::hex << std::setfill('0');

    // time-low
    for (size_t i = 0; i < 4; i++)
        ss << std::setw(2) << static_cast<int>(this->bytes_[i]);
    ss << '-';

    // time-mid
    for (size_t i = 4; i < 6; i++)
        ss << std::setw(2) << static_cast<int>(this->bytes_[i]);
    ss << '-';

    // time-high-and-version
    for (size_t i = 6; i < 8; i++)
        ss << std::setw(2) << static_cast<int>(this->bytes_[i]);
    ss << '-';

    // clock-seq-and-reserved and clock-seq-low
    for (size_t i = 8; i < 10; i++)
        ss << std::setw(2) << static_cast<int>(this->bytes_[i]);
    ss << '-';

    // node
    for (size_t i = 10; i < 16; i++)
        ss << std::setw(2) << static_cast<int>(this->bytes_[i]);

    return ss.str();
}

const std::array<std::byte, 16> &LeoUUID::getBytes() const
{
    return this->bytes_;
}

void LeoUUID::clear()
{
    std::fill(this->bytes_.begin(), this->bytes_.end(), std::byte{0});
}

bool operator<(const LeoUUID &a, const LeoUUID &b)
{
    return a.getBytes() < b.getBytes();
}

bool operator>(const LeoUUID &a, const LeoUUID &b)
{
    return a.getBytes() > b.getBytes();
}

bool operator<=(const LeoUUID &a, const LeoUUID &b)
{
    return a.getBytes() <= b.getBytes();
}

bool operator>=(const LeoUUID &a, const LeoUUID &b)
{
    return a.getBytes() >= b.getBytes();
}

bool operator==(const LeoUUID &a, const LeoUUID &b)
{
    return a.getBytes() == b.getBytes();
}

bool operator!=(const LeoUUID &a, const LeoUUID &b)
{
    return a.getBytes() != b.getBytes();
}

// ---------------------------------------------------------------------------------------------------------------------

LEOBASE_MODULE_END
