/*
 *      Copyright(C) Milethos Technologies SLU. 2025
 *      All rights reserved. Reproduction in whole or in
 *      part is prohibited without the written consent of
 *      the copyright holder.
 */

#pragma once

// C++ INCLUDES
#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include <unordered_map>

// LEOBASE INCLUDES
#include "LeoBase/Aliases/Global"
#include "LeoBase/Helpers/strings_utils.h"
#include "LeoBase/System/types/filesystem_types.h"
#include "LeoBase/System/filesystem/filesystem_utils.h"

// LEOBASE MACROS
LEOBASE_MODULE_BEGIN(system)
BEGIN_NAMESPACE(files)

// ---------------------------------------------------------------------------------------------------------------------

using namespace helpers;

class FolderTree;
class RootFolderTree;

using FolderTreePtr = std::shared_ptr<FolderTree>;
using FolderTreeWPtr = std::weak_ptr<FolderTree>;

using RootFolderTreePtr = std::shared_ptr<RootFolderTree>;
using RootFolderTreeWPtr = std::weak_ptr<RootFolderTree>;


using FolderTreePtrV = std::vector<FolderTreePtr>;
using SubfolderMap = std::unordered_map<std::string, FolderTreePtr>;


class LEOBASE_EXPORT FolderTree :  public std::enable_shared_from_this<FolderTree>
{

public:

    // TODO ADD ROOT WEAKPTR.

    // Data members.
    types::BasicFolderInfo folderInfo_;   ///< Folder metadata
    types::BasicFileInfoV filesInfoV_;    ///< List of files in the folder
    SubfolderMap subfoldersMap_;          ///< Subfolders within this folder
    FolderTreeWPtr parentFolder_;         ///< Weak ptr to previous folder (prevents circular reference).
    RootFolderTreeWPtr rootFolder_;    ///< Weak ptr to previous folder (prevents circular reference).

    FolderTree(types::BasicFolderInfo folder_info, RootFolderTreePtr root_dolder, FolderTreePtr parent_folder):
        folderInfo_(folder_info),
        parentFolder_(parent_folder),
        rootFolder_(root_dolder)
    {}

    FolderTree() :
        folderInfo_(types::BasicFolderInfo()),
        parentFolder_(std::weak_ptr<FolderTree>())
    {}

    // TODO THE DYNAMIC SIZE CALCULATION NOT WORKS IN RELEASE?
    // I THINK IT WAS FIXED.

    void merge(FolderTreePtr otherFolderTree)
    {
        for (auto& otherSubfolder : otherFolderTree->subfoldersMap_)
        {
            // std::cout << "Im: " << this->folderInfo_.folderName << " and im merge with: " << otherSubfolder.second->folderInfo_.folderName << std::endl;

            auto it = std::find_if(this->subfoldersMap_.begin(), this->subfoldersMap_.end(),
                                   [&otherSubfolder](const auto& existingSubfolder)
                                   {
                                       return existingSubfolder.second->folderInfo_.folderName == otherSubfolder.second->folderInfo_.folderName;
                                   });

            if (it != this->subfoldersMap_.end())
            {
                // [FIX]: Store the child size before merging to know how much it grows
                size_t oldSize = it->second->folderInfo_.sizeB;

                // Merge the existing folder with the one from otherFolderTree.
                it->second->merge(otherSubfolder.second);

                // [FIX]: Add to THIS folder the difference with the child's growth
                this->folderInfo_.sizeB += (it->second->folderInfo_.sizeB - oldSize);
            }
            else
            {
                // Insert new folder from otherFolderTree.
                this->subfoldersMap_.emplace(otherSubfolder.first, otherSubfolder.second);

                this->folderInfo_.sizeB += otherSubfolder.second->folderInfo_.sizeB;
            }
        }

        // Copy files from the other folder to this folder.
        for (const auto& file : otherFolderTree->filesInfoV_)
        {
            auto it_file = std::find_if(this->filesInfoV_.begin(), this->filesInfoV_.end(),
                                        [&file](const auto& exitingBasicFileInfo)
                                        {
                                            return exitingBasicFileInfo.fileName == file.fileName;
                                        });

            if (it_file == this->filesInfoV_.end())
            {
                // Update size of folder.
                // Update the folder size on THIS folder, not the father's
                // The folder' size will spread upwards thanks to the previous code block
                this->folderInfo_.sizeB += file.sizeB;

                // Add file to internal vector.
                this->filesInfoV_.push_back(file);
            }
        }
    }


    /**
     * @brief Adds a file to the folder.
     */
    void addFile(const types::BasicFileInfo& file)
    {
        types::BasicFileInfo file_aux = file;

        std::string full_path = folderInfo_.folderName;
        size_t file_size = file.sizeB; // File size to propagate

        // Traverse up to construct the full path
        FolderTreeWPtr parent = parentFolder_;
        while (auto parentPtr = parent.lock())
        {
            full_path = parentPtr->folderInfo_.folderName + "/" + full_path;
            parent = parentPtr->parentFolder_;
        }

        // Append the full path to the file name
        file_aux.setFilePath(full_path + "/" + file.fileName);

        // Store the file
        this->filesInfoV_.push_back(file_aux);

        // **Update folder sizes recursively**
        FolderTreePtr current = shared_from_this();
        while (current)
        {
            current->folderInfo_.sizeB += file_size;
            if (auto parentPtr = current->parentFolder_.lock())
                current = parentPtr;
            else
                break;
        }
    }

    FolderTreePtrV getAllSubfolders() const
    {
        FolderTreePtrV vec;

        // Recursively collect all subfolders
        for (const auto& pair : this->subfoldersMap_)
        {
            // Add the current subfolder
            vec.push_back(pair.second);

            // Recursively get all sub-subfolders
            FolderTreePtrV deeperSubfolders = pair.second->getAllSubfolders();
            vec.insert(vec.end(), deeperSubfolders.begin(), deeperSubfolders.end());
        }

        return vec;
    }

    types::BasicFileInfoV getSubfolderFiles() const
    {
        return this->filesInfoV_;
    }

    types::BasicFileInfoV getAllSubfoldersFiles() const
    {
        types::BasicFileInfoV vec;

        // Get the files of the current subfolder.
        vec.insert(vec.end(), this->filesInfoV_.begin(), this->filesInfoV_.end());

        // Recursively collect files from all subfolders
        for (const auto& pair : this->subfoldersMap_)
        {
            // Recursively get files from all sub-subfolders
            const auto& deeperFiles = pair.second->getAllSubfoldersFiles();
            vec.insert(vec.end(), deeperFiles.begin(), deeperFiles.end());
        }

        return vec;
    }

    /**
    //  * @brief Retrieves a subfolder by name.
    //  */
    FolderTreePtr getSubfolder(const std::string& path)
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

    // /**
    //  * @brief Retrieves the parent folder.
    //  */
    // FolderTreePtr getParent()
    // {
    //     return prevFolder_.lock();
    // }

    void clear()
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
        filesInfoV_.clear();
    }

    // TODO EXPERIMENTAL
    std::string listContents(size_t level = 0) const
    {
        std::ostringstream stream;
        std::string indent = std::string(level * 2, ' '); // Indentation for hierarchy

        // Print current folder
        stream << indent << "📂 " << folderInfo_.folderName << "\n";

        // Print files
        for (const auto& file : filesInfoV_)
        {
            stream << indent << "  📄 " << file.fileName << " (" << file.sizeB << " bytes, "
                   << file.permissions << ")\n";
        }

        // Recursively list subfolders
        for (const auto& [name, folderPtr] : subfoldersMap_)
        {
            if (folderPtr)
                stream << folderPtr->listContents(level + 1);
        }

        return stream.str();
    }

    std::optional<std::string> findSubfolderName(const std::string& folderName)
    {
        auto it_subfolder = std::find_if(this->subfoldersMap_.begin(), this->subfoldersMap_.end(),
                                         [folderName](const auto& existingSubfolder)
                                         {
                                             return existingSubfolder.second->folderInfo_.folderName.find(folderName) != std::string::npos;
                                         });


        if (it_subfolder == this->subfoldersMap_.end())
        {
            return {};
        }

        return it_subfolder->second->folderInfo_.folderName;
    }
};

class LEOBASE_EXPORT RootFolderTree : public std::enable_shared_from_this<RootFolderTree>
{
public:
    // TODO NORMALIZAR PATHS PARA TRABAJAR CON WINDOWS O UNIX.
    // TODO LANZAR EXCEPCIONES SI LOS PATH NO SON VALIDOS???

    static RootFolderTreePtr makeFromParentPath(const std::string& parent_path);

    static RootFolderTreePtr makeRecursivelyFromParentPath(const std::string& parent_path);

    static RootFolderTreePtr makeEmpty();

    types::BasicFolderInfo getFolderInfo() const;

    FolderTreePtrV getAllSubfolders() const;

    FolderTreePtrV getAllSubfolders(const std::string& path) const;

    FolderTreePtr getSubfolder(const std::string& path) const;

    void addFile(const types::BasicFileInfo& file);

    //FolderTreePtr addSubfolder(const std::string& folder_path);

    FolderTreePtr addSubfolder(const std::string& folder_path, const types::BasicFolderInfo& folder_info, bool includes_parent = true);

    void clear();

    bool empty() const
    {
        return this->subfoldersMap_.empty();
    }

    bool generateLocalFolderTree(const std::string& subfolder_path = "", const std::string& local_path = "") const
    {
        if(this->empty())
            return false;

        // Containers.
        std::string cleaned_local_path = local_path;
        std::string cleaned_subfolder_path = subfolder_path;
        bool success = true;
        FolderTreePtrV subfolders;

        // Normalize paths.
        if (strings::startsWith(cleaned_local_path, "/"))
            strings::ltrimInPlace(cleaned_local_path, "/");
        if (strings::endsWith(cleaned_local_path, "/"))
            strings::rtrimInPlace(cleaned_local_path, "/");
        if (strings::startsWith(cleaned_subfolder_path, "/"))
            strings::ltrimInPlace(cleaned_subfolder_path, "/");
        if (strings::endsWith(cleaned_subfolder_path, "/"))
            strings::rtrimInPlace(cleaned_subfolder_path, "/");

        if(cleaned_subfolder_path.empty())
            subfolders = this->getAllSubfolders();
        else
            subfolders = this->getSubfolder(cleaned_subfolder_path)->getAllSubfolders();

        // Get root folder path (base folder itself).
        std::string base_folder = cleaned_local_path + "/" + this->getSubfolder(cleaned_subfolder_path)->folderInfo_.folderName;

        // Create the **base** folder itself.
        if (!files::createDirectory(base_folder))
        {
            return false; // Fail early if the base folder cannot be created.
        }

        // Keep only the path.
        cleaned_subfolder_path = files::getPathFromFilepath(cleaned_subfolder_path);

        for (const auto& folder : subfolders)
        {
            std::string path = folder->folderInfo_.folderPath;
            helpers::strings::ltrimInPlace(path, cleaned_subfolder_path);
            path = cleaned_local_path + "/" + path;
            if(!files::createDirectory(path))
            {
                success = false;
                break;
            }
        }

        return success;
    }

    // TODO EXPERIMENTAL
    std::string listContents() const
    {
        std::ostringstream stream;

        auto it = this->subfoldersMap_.find(this->parentFolderName_);

        return it->second->listContents(0).c_str();
    }

protected:
    RootFolderTree() = default;

    // TODO
    //RootFolderTree(const types::BasicFolderInfo& parent_folder_info);

    explicit RootFolderTree(const std::string& parent_folder_name);

#if defined(WINDOWS) || defined(_WIN32)
    static void fillRecursively(const std::string& dir, RootFolderTreePtr rootFolderTree);
#else
    static void fillRecursively(const std::string& dir, RootFolderTreePtr rootFolderTree)
    {}
#endif

private:
    std::string parentFolderName_;
    SubfolderMap subfoldersMap_;
};

// ---------------------------------------------------------------------------------------------------------------------

END_NAMESPACE(files)
LEOBASE_MODULE_END
