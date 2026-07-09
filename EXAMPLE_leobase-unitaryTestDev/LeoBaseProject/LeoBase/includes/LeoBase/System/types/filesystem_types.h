/*
 *      Copyright(C) Milethos Technologies SLU. 2025
 *      All rights reserved. Reproduction in whole or in
 *      part is prohibited without the written consent of
 *      the copyright holder.
 */

#pragma once

// C++ INCLUDES
#include <vector>
#include <string>

// LEOBASE INCLUDES
#include "LeoBase/Aliases/Global"

// LEOBASE MACROS
LEOBASE_MODULE_BEGIN(system)
BEGIN_NAMESPACE(types)

// ---------------------------------------------------------------------------------------------------------------------
// Aliases.

// TODO ADD PATHS

struct LEOBASE_EXPORT BasicFileInfo
{
    /**
     * @brief Default constructor.
     */
    BasicFileInfo() = default;

    /**
     * @brief Constructs a file info structure with given metadata.
     * @param file_path File path or file name.
     * @param perm File permission string (e.g., `"rw-r--r--"`).
     * @param owner File owner name.
     * @param group File group name.
     * @param mod_time Last modified time (ISO8601).
     * @param size_b File size in bytes.
     */
    BasicFileInfo(const std::string& file_path, const std::string& perm = "", const std::string& owner = "",
                  const std::string& group = "", const std::string& mod_time = "", size_t size_b = 0);

    void setFilePath(const std::string &file_path);

    // Data members.
    std::string fileName;     ///< File name (without path).
    std::string filePath;     ///< Complete file path (includes file name).
    std::string path;         ///< File path (not includes file).
    std::string fileExt;      ///< File extension.
    std::string permissions;  ///< Permissions string.
    std::string owner;        ///< File owner.
    std::string group;        ///< File group.
    std::string modTime;      ///< File last modified time (ISO8601).
    std::size_t sizeB;        ///< File size in bytes.
};

// TODO ADD PATH

struct LEOBASE_EXPORT BasicFolderInfo
{
    /**
     * @brief Default constructor.
     */
    BasicFolderInfo() :
        sizeB(0)
    {}

    /**
     * @brief Constructs a folder info structure with given metadata.
     * @param folder_path Folder path or folder name.
     * @param perm Folder permission string (e.g., `"rwxr-xr-x"`).
     * @param owner Folder owner name.
     * @param group Folder group name.
     * @param mod_time Last modified time (ISO8601).
     * @param size_b Folder size in bytes.
     */
    BasicFolderInfo(const std::string& folder_path, const std::string& perm = "", const std::string& owner = "",
                    const std::string& group = "", const std::string& mod_time = "", size_t size_b = 0);

    // Data members.
    std::string folderName;   ///< Folder name (without path).
    std::string folderPath;   ///< Complete folder path (includes folder name).
    std::string permissions;  ///< Permissions string.
    std::string owner;        ///< Folder owner.
    std::string group;        ///< Folder group.
    std::string modTime;      ///< File last modified time (ISO8601).
    std::size_t sizeB;        ///< Folder size in bytes.
};

/// Alias for a vector of BasicFileInfo struct.
using BasicFileInfoV = std::vector<BasicFileInfo>;

/// Alias for a vector of BasicFolderInfo struct.
using BasicFolderInfoV = std::vector<BasicFolderInfo>;

// ---------------------------------------------------------------------------------------------------------------------

END_NAMESPACE(types)
LEOBASE_MODULE_END
