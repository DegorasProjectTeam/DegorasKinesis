/*
 *      Copyright(C) Milethos Technologies SLU. 2025
 *      All rights reserved. Reproduction in whole or in
 *      part is prohibited without the written consent of
 *      the copyright holder.
 */

// C++ INCLUDES
#include <cstdint>

// LEOBASE INCLUDES
#include "LeoBase/Global/global_definitions.h"
#include "LeoBase/Containers/any_equal.h"
#include "LeoBase/Exceptions/leobase_exception.h"

// LEOBASE MACROS
LEOBASE_MODULE_BEGIN(containers)

// ---------------------------------------------------------------------------------------------------------------------

bool AnyEqual::operator()(const std::any &a, const std::any &b) const
{
    // Result container.
    bool res;

    // Different types are not equal.
    if (a.type() != b.type())
    {
        res = false;
    }
    else
    {
        // Compare known types.
        if (a.type() == typeid(int))               res = AnyTypeEqual<int>{}(a, b);
        else if (a.type() == typeid(short))        res = AnyTypeEqual<short>{}(a, b);
        else if (a.type() == typeid(long))         res = AnyTypeEqual<long>{}(a, b);
        else if (a.type() == typeid(long long))    res = AnyTypeEqual<long long>{}(a, b);
        else if (a.type() == typeid(unsigned int))       res = AnyTypeEqual<unsigned int>{}(a, b);
        else if (a.type() == typeid(unsigned short))     res = AnyTypeEqual<unsigned short>{}(a, b);
        else if (a.type() == typeid(unsigned long))      res = AnyTypeEqual<unsigned long>{}(a, b);
        else if (a.type() == typeid(unsigned long long)) res = AnyTypeEqual<unsigned long long>{}(a, b);
        else if (a.type() == typeid(std::size_t))   res = AnyTypeEqual<std::size_t>{}(a, b);
        else if (a.type() == typeid(std::int8_t))   res = AnyTypeEqual<std::int8_t>{}(a, b);
        else if (a.type() == typeid(std::int16_t))  res = AnyTypeEqual<std::int16_t>{}(a, b);
        else if (a.type() == typeid(std::int32_t))  res = AnyTypeEqual<std::int32_t>{}(a, b);
        else if (a.type() == typeid(std::int64_t))  res = AnyTypeEqual<std::int64_t>{}(a, b);
        else if (a.type() == typeid(std::uint8_t))  res = AnyTypeEqual<std::uint8_t>{}(a, b);
        else if (a.type() == typeid(std::uint16_t)) res = AnyTypeEqual<std::uint16_t>{}(a, b);
        else if (a.type() == typeid(std::uint32_t)) res = AnyTypeEqual<std::uint32_t>{}(a, b);
        else if (a.type() == typeid(std::uint64_t)) res = AnyTypeEqual<std::uint64_t>{}(a, b);
        else if (a.type() == typeid(std::string))   res = AnyTypeEqual<std::string>{}(a, b);
        else if (a.type() == typeid(const char*))   res = AnyTypeEqual<const char*>{}(a, b);
        // Not supported types.
        else
        {
            std::string error = "Unsupported type in comparison: " + std::string(a.type().name());
            throw exceptions::LeoBaseException(kModN,"CustomAnyEqual::()", error);
        }
    }

    // Return the result.
    return res;
}

// ---------------------------------------------------------------------------------------------------------------------

LEOBASE_MODULE_END
