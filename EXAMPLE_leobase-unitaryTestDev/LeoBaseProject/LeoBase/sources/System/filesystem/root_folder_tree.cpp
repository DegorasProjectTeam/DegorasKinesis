/*
 *      Copyright(C) Milethos Technologies SLU. 2025
 *      All rights reserved. Reproduction in whole or in
 *      part is prohibited without the written consent of
 *      the copyright holder.
 */

// C++ INCLUDES
#include <vector>
#include <memory>
#include <string>
#include <unordered_map>

#if defined(WINDOWS) || defined(_WIN32)
#include <windows.h>
#include <Shlwapi.h>
#endif

// LEOBASE INCLUDES
#include "LeoBase/Helpers/strings_utils.h"
#include "LeoBase/System/filesystem/root_folder_tree.h"

// LEOBASE MACROS
LEOBASE_MODULE_BEGIN(system)
BEGIN_NAMESPACE(files)

// ---------------------------------------------------------------------------------------------------------------------

using namespace system::types;

RootFolderTreePtr RootFolderTree::makeFromParentPath(const std::string &parent_path)
{
    // Containers.
    std::string cleaned_path = parent_path;
    FolderTreePtr current = nullptr;

    // Normalize path.
    if (strings::startsWith(cleaned_path, "/"))
        strings::ltrimInPlace(cleaned_path, "/");
    if (strings::endsWith(cleaned_path, "/"))
        strings::rtrimInPlace(cleaned_path, "/");

    // Prepare the parent folder tree.
    std::vector<std::string> path_tokenized;
    strings::tokenize(path_tokenized, cleaned_path, "/", false);
    RootFolderTreePtr root_tree = std::make_shared<RootFolderTree>(RootFolderTree(path_tokenized[0]));

    // Add the subfolders.
    if(path_tokenized.size() > 1)
    {
        path_tokenized.erase(path_tokenized.begin());
        std::string subfolder_path = strings::join(path_tokenized, "/");
        root_tree->addSubfolder(subfolder_path, BasicFolderInfo{path_tokenized[path_tokenized.size()-1]});
    }

    // Return the parent tree.
    return root_tree;
}

RootFolderTreePtr RootFolderTree::makeRecursivelyFromParentPath(const std::string& parent_path)
{
    auto root_folder_tree = RootFolderTree::makeFromParentPath(parent_path);
    RootFolderTree::fillRecursively(parent_path, root_folder_tree);

    return root_folder_tree;
}

RootFolderTreePtr RootFolderTree::makeEmpty()
{
    // Return the parent tree.
    return std::make_shared<RootFolderTree>(RootFolderTree());
}

FolderTreePtrV RootFolderTree::getAllSubfolders() const
{
    FolderTreePtrV vec;
    for (const auto& pair : this->subfoldersMap_)
        vec.push_back(pair.second);
    return vec;
}

FolderTreePtrV RootFolderTree::getAllSubfolders(const std::string &path) const
{
    // Containers.
    std::string cleaned_path = path;
    FolderTreePtr current = nullptr;

    // Normalize path.
    if (strings::startsWith(cleaned_path, "/"))
        strings::ltrimInPlace(cleaned_path, "/");
    if (strings::endsWith(cleaned_path, "/"))
        strings::rtrimInPlace(cleaned_path, "/");

    auto it = this->subfoldersMap_.find(cleaned_path);
    if (it == this->subfoldersMap_.end())
        return {};

    FolderTreePtrV vec;
    for (const auto& pair : it->second->subfoldersMap_)
        vec.push_back(pair.second);
    return vec;
}

FolderTreePtr RootFolderTree::getSubfolder(const std::string &path) const
{
    // Containers.
    std::string cleaned_path = path;
    FolderTreePtr current = nullptr;

    // Normalize path.
    if (strings::startsWith(cleaned_path, "/"))
        strings::ltrimInPlace(cleaned_path, "/");
    if (strings::endsWith(cleaned_path, "/"))
        strings::rtrimInPlace(cleaned_path, "/");

    // Search the folder.
    auto it = subfoldersMap_.find(cleaned_path);
    return (it != subfoldersMap_.end()) ? it->second : nullptr;
}

void RootFolderTree::addFile(const BasicFileInfo &file)
{
    this->subfoldersMap_[this->parentFolderName_]->addFile(file);
}

FolderTreePtr RootFolderTree::addSubfolder(const std::string &folder_path,
                                           const types::BasicFolderInfo& folder_info,
                                           bool includes_parent)
{
    // Containers.
    std::vector<std::string> paths_tokenized;
    std::string cleaned_path = folder_path;

    // Normalize path.
    if(strings::startsWith(cleaned_path, "/"))
        strings::ltrimInPlace(cleaned_path, "/");
    if(strings::endsWith(cleaned_path, "/"))
        strings::rtrimInPlace(cleaned_path, "/");

    // Check if folder already exists.
    auto it = this->subfoldersMap_.find(cleaned_path);
    if (it != subfoldersMap_.end())
        return it->second;

    // Adds the parent name if necessary.
    if(includes_parent)
        strings::ltrimInPlace(cleaned_path, this->parentFolderName_);

    // Split path into directories
    strings::tokenize(paths_tokenized, cleaned_path, "/", false);

    // Start from the root
    FolderTreePtr current = subfoldersMap_[this->parentFolderName_];

    // Navigate recursively
    std::string path_aux = this->parentFolderName_;
    for (size_t idx = 0; idx < paths_tokenized.size(); ++idx)
    {
        std::string full_path = path_aux + "/" + paths_tokenized[idx];

        if (current->subfoldersMap_.find(paths_tokenized[idx]) == current->subfoldersMap_.end())
        {
            BasicFolderInfo new_folder_info = (idx == paths_tokenized.size() - 1) ? folder_info : BasicFolderInfo();
            new_folder_info.folderName = paths_tokenized[idx];
            new_folder_info.folderPath = full_path + "/";

            FolderTreePtr new_folder = std::make_shared<FolderTree>(new_folder_info, shared_from_this(), current);
            current->subfoldersMap_[paths_tokenized[idx]] = new_folder;
            subfoldersMap_[full_path] = new_folder;
        }

        current = this->subfoldersMap_[full_path];
        path_aux = full_path;
    }

    return current;
}

void RootFolderTree::clear()
{
    for (auto& [name, folderPtr] : subfoldersMap_)
    {
        if (folderPtr)
        {
            folderPtr->clear();
            folderPtr.reset();
        }
    }
    subfoldersMap_.clear();
}

#if defined(WINDOWS) || defined(_WIN32)
void RootFolderTree::fillRecursively(const std::string& dirPath, RootFolderTreePtr rootFolderTree)
{
    if (dirPath.empty())
        return;

    // "dirPath" must be received as unix path. So convert it to use with windows.h library
    auto w_path = convertUnixPathToWindows(dirPath);

    // Check path is directory.
    if (!PathIsDirectoryA(w_path.c_str()))
        return;

    if (w_path.back() != '\\')
        w_path += "\\";

    // Use ANSI version for FindFirstFileA.
    WIN32_FIND_DATAA find_file_data;
    HANDLE h_find;

    std::string search_path = w_path + "*";

    h_find = FindFirstFileA(search_path.c_str(), &find_file_data);
    if (h_find == INVALID_HANDLE_VALUE)
        return;

    do
    {
        std::string file_or_dir = find_file_data.cFileName;

        // Skip "." and ".." to avoid infinite loops.
        if (file_or_dir == "." || file_or_dir == "..")
            continue;

        if (find_file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            // Get folder path.
            std::string folder_path = dirPath + file_or_dir + "/";

            // TODO: fill with more info?
            // Create folder info struct, in constructor is filled folder name too.
            types::BasicFolderInfo folder_info(folder_path);

            // Add current directory to root tree.
            rootFolderTree->addSubfolder(folder_path, folder_info);

            // Recursive call for directories.
            RootFolderTree::fillRecursively(folder_path, rootFolderTree);
        }
        else
        {
            // Get file path.
            std::string file_path = dirPath + file_or_dir;

            // TODO: fill with more info?
            // Create file info struct, in constructor is filled filename and file extension too.
            types::BasicFileInfo file_info(file_path);

            // Get file size.
            file_info.sizeB = (static_cast<std::size_t>(find_file_data.nFileSizeHigh) << 32) |
                              find_file_data.nFileSizeLow;

            // Add this file to root tree to current subfolder.
            auto subfolder_path = getPathFromFilepath(file_path);
            rootFolderTree->getSubfolder(subfolder_path)->addFile(file_info);
        }

    } while (FindNextFileA(h_find, &find_file_data) != 0);

    FindClose(h_find);
}
#endif

RootFolderTree::RootFolderTree(const std::string& root_folder_name) :
    parentFolderName_({root_folder_name})
{
    // Initialize the root folder inside subfoldersMap_
    BasicFolderInfo root_info;
    root_info.folderName = this->parentFolderName_;
    root_info.folderPath = this->parentFolderName_ + "/";
    subfoldersMap_[this->parentFolderName_] = std::make_shared<FolderTree>(root_info, nullptr, nullptr);
}

// ---------------------------------------------------------------------------------------------------------------------

END_NAMESPACE(files)
LEOBASE_MODULE_END
