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
LEOBASE_MODULE_BEGIN(system)
BEGIN_NAMESPACE(types)

// ---------------------------------------------------------------------------------------------------------------------

using PID = unsigned;

/**
 * @brief Common enumeration for disk types, reusable across Windows and Linux.
 */
enum class DiskType : std::uint16_t
{
    UNKNOWN,    ///< Unknown or undetermined disk type.
    HDD,        ///< Spinning hard disk drive.
    SSD,        ///< Solid-state drive.
    REMOVABLE,  ///< USB stick or removable storage.
    VIRTUAL     ///< Virtual or mapped network drive.
};

struct DiskInfo
{
    std::string path;           ///< Mount point or drive letter.
    DiskType type;              ///< Disk type.
    float usagePercent;         ///< Percent of space used.
    uint64_t totalBytes;        ///< Total size of the disk in bytes.
    uint64_t usedBytes;         ///< Used bytes.
    uint64_t freeBytes;         ///< Available bytes to user.

    std::string toJsonStr(size_t indentSize = 0, size_t level = 0, bool braces = true) const
    {
        StreamableFieldValuePairV jsonData =
            {
                {"path", path.empty() ? OptionalStr() : path},
                {"type", static_cast<std::uint16_t>(type)},
                {"usagePercent", usagePercent},
                {"totalBytes", totalBytes},
                {"usedBytes", usedBytes},
                {"freeBytes", freeBytes}
            };

        return helpers::strings::generateJsonStr(jsonData, indentSize, level, braces);
    }
};

using DiskInfoV = std::vector<DiskInfo>;

// ---------------------------------------------------------------------------------------------------------------------

END_NAMESPACE(types)
LEOBASE_MODULE_END
