/*
 *      Copyright(C) Milethos Technologies SLU. 2025
 *      All rights reserved. Reproduction in whole or in
 *      part is prohibited without the written consent of
 *      the copyright holder.
 */

// C++ INCLUDES
#include <cstddef>
#include <mutex>
#include <random>
#include <array>
#include <set>
#include <chrono>

// LEOBASE INCLUDES
#include "LeoBase/Crypto/leo_uuid_generator.h"

// LEOBASE MACROS
LEOBASE_MODULE_BEGIN(crypto)

// ---------------------------------------------------------------------------------------------------------------------

LeoUUIDGenerator::LeoUUIDGenerator() :
    rd_(std::random_device()),
    gen_(std::mt19937_64{std::random_device{}()})
{
    // Check the entropy.
    if(this->rd_.entropy() == 0.0)
    {
        auto now = std::chrono::high_resolution_clock::now();
        auto now_int = std::chrono::time_point_cast<std::chrono::nanoseconds>(now).time_since_epoch().count();
        std::uint_fast64_t seed = std::hash<decltype(now_int)>{}(now_int);
        this->gen_ = std::mt19937_64(seed);
    }
    else
        this->gen_ = std::mt19937_64(this->rd_());
}

LeoUUIDGenerator &LeoUUIDGenerator::getInstance()
{
    // Guaranteed to be destroyed, instantiated on first use.
    static LeoUUIDGenerator instance;
    return instance;
}

LeoUUID LeoUUIDGenerator::generateUUIDv4()
{
    return LeoUUIDGenerator::getInstance().generateUUIDv4Private();
}

LeoUUID LeoUUIDGenerator::generateUUIDv4Private()
{
    // Auxiliar containers.
    std::array<std::byte, 16> bytes;
    std::uniform_int_distribution<> distrib(0, 255);
    LeoUUID uuid;

    // Random generation.
    for(auto& byte : bytes)
        byte = static_cast<std::byte>(distrib(this->gen_));

    // Set the version to 4 (random)
    bytes[6] = static_cast<std::byte>((static_cast<std::uint8_t>(bytes[6]) & 0x0F) | 0x40);

    // Set the variant to 1 (RFC4122)
    bytes[8] = static_cast<std::byte>((static_cast<std::uint8_t>(bytes[8]) & 0x3F) | 0x80);

    // Generate the UUID.
    uuid = LeoUUID(bytes);

    // Return the generated uuid.
    return uuid;
}

// ---------------------------------------------------------------------------------------------------------------------

LEOBASE_MODULE_END
