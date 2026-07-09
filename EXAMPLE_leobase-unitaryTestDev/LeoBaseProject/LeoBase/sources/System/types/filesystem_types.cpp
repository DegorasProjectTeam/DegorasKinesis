/*
 *      Copyright(C) Milethos Technologies SLU. 2026
 *      All rights reserved. Reproduction in whole or in
 *      part is prohibited without the written consent of
 *      the copyright holder.
 */

// C++ INCLUDES
#include <string>

// LEOBASE INCLUDES
#include "LeoBase/Helpers/strings_utils.h"
#include "LeoBase/System/types/filesystem_types.h"
#include "LeoBase/System/filesystem/filesystem_utils.h"

// LEOBASE MACROS
LEOBASE_MODULE_BEGIN(system)
BEGIN_NAMESPACE(types)

// ---------------------------------------------------------------------------------------------------------------------

BasicFileInfo::BasicFileInfo(const std::string &file_path, const std::string &perm, const std::string &owner,
                             const std::string &group, const std::string &mod_time, size_t size_b) :
    permissions(perm),
    owner(owner),
    group(group),
    modTime(mod_time),
    sizeB(size_b)
{
    this->filePath = file_path;
    this->fileName = files::getFilenameFromFilepath(file_path);
    this->fileExt = files::getFileExtension(file_path);
    this->path = files::getPathFromFilepath(file_path);
}

void BasicFileInfo::setFilePath(const std::string &file_path)
{
    this->filePath = file_path;
    this->fileName = files::getFilenameFromFilepath(file_path);
    this->fileExt = files::getFileExtension(file_path);
    this->path = files::getPathFromFilepath(file_path);
}

BasicFolderInfo::BasicFolderInfo(const std::string &folder_path, const std::string &perm, const std::string &owner,
                                 const std::string &group, const std::string &mod_time, size_t size_b) :
    permissions(perm),
    owner(owner),
    group(group),
    modTime(mod_time),
    sizeB(size_b)
{
    // TODO STANDARIZE THE PATH.

    std::vector<std::string> path_tokenized;
    helpers::strings::tokenize(path_tokenized, folder_path, "/");
    this->folderPath = folder_path;
    this->folderName = path_tokenized.back();
}

// ---------------------------------------------------------------------------------------------------------------------




END_NAMESPACE(types)
LEOBASE_MODULE_END
