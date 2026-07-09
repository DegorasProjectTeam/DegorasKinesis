/*
 *      Copyright(C) Milethos Technologies SLU. 2025
 *      All rights reserved. Reproduction in whole or in
 *      part is prohibited without the written consent of
 *      the copyright holder.
 */

// C++ INCLUDES
#include <cstdint>

// LEOBASE INCLUDES
#include "LeoBase/Containers/any_hasher.h"
#include "LeoBase/Global/global_definitions.h"
#include "LeoBase/Exceptions/leobase_exception.h"

// LEOBASE MACROS
LEOBASE_MODULE_BEGIN(containers)

// ---------------------------------------------------------------------------------------------------------------------

std::size_t AnyHasher::operator()(const std::any &key) const
{
    // Hash the type.
    std::size_t hash_val = key.type().hash_code();

    // Hash the known types.
    if (key.type() == typeid(int))               hash_val ^= AnyTypeHasher<int>{}(key);
    else if (key.type() == typeid(short))        hash_val ^= AnyTypeHasher<short>{}(key);
    else if (key.type() == typeid(long))         hash_val ^= AnyTypeHasher<long>{}(key);
    else if (key.type() == typeid(long long))    hash_val ^= AnyTypeHasher<long long>{}(key);
    else if (key.type() == typeid(unsigned int))       hash_val ^= AnyTypeHasher<unsigned int>{}(key);
    else if (key.type() == typeid(unsigned short))     hash_val ^= AnyTypeHasher<unsigned short>{}(key);
    else if (key.type() == typeid(unsigned long))      hash_val ^= AnyTypeHasher<unsigned long>{}(key);
    else if (key.type() == typeid(unsigned long long)) hash_val ^= AnyTypeHasher<unsigned long long>{}(key);
    else if (key.type() == typeid(std::size_t))   hash_val ^= AnyTypeHasher<std::size_t>{}(key);
    else if (key.type() == typeid(std::int8_t))   hash_val ^= AnyTypeHasher<std::int8_t>{}(key);
    else if (key.type() == typeid(std::int16_t))  hash_val ^= AnyTypeHasher<std::int16_t>{}(key);
    else if (key.type() == typeid(std::int32_t))  hash_val ^= AnyTypeHasher<std::int32_t>{}(key);
    else if (key.type() == typeid(std::int64_t))  hash_val ^= AnyTypeHasher<std::int64_t>{}(key);
    else if (key.type() == typeid(std::uint8_t))  hash_val ^= AnyTypeHasher<std::uint8_t>{}(key);
    else if (key.type() == typeid(std::uint16_t)) hash_val ^= AnyTypeHasher<std::uint16_t>{}(key);
    else if (key.type() == typeid(std::uint32_t)) hash_val ^= AnyTypeHasher<std::uint32_t>{}(key);
    else if (key.type() == typeid(std::uint64_t)) hash_val ^= AnyTypeHasher<std::uint64_t>{}(key);
    else if (key.type() == typeid(std::string))   hash_val ^= AnyTypeHasher<std::string>{}(key);
    else if (key.type() == typeid(const char*))   hash_val ^= AnyTypeHasher<const char*>{}(key);
    // Not supported types.
    else
    {
        std::string error = "Unsupported type as key: " + std::string(key.type().name());
        throw exceptions::LeoBaseException(kModN,"CustomAnyHasher::()", error);
    }

    // Return the hash value.
    return hash_val;
}

// ---------------------------------------------------------------------------------------------------------------------

LEOBASE_MODULE_END
