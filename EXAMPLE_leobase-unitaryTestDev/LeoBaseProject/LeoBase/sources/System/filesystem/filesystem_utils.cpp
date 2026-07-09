/*
 *      Copyright(C) Milethos Technologies SLU. 2025
 *      All rights reserved. Reproduction in whole or in
 *      part is prohibited without the written consent of
 *      the copyright holder.
 */

// C++ INCLUDES
#include <algorithm>
#include <iostream>
#include <string>
#include <unordered_set>
#include <fstream>
#if defined(WINDOWS) || defined(_WIN32)
#include <direct.h>
#include <windows.h>
#include <tlhelp32.h>
#include <fileapi.h>
#include <Shlwapi.h>
#else
#include <unistd.h>
#endif

// LEOBASE INCLUDES
#include "LeoBase/Global/global_definitions.h"
#include "LeoBase/System/filesystem/filesystem_utils.h"

// LEOBASE MACROS
LEOBASE_MODULE_BEGIN(system)
BEGIN_NAMESPACE(files)

// TODO ADD EXCEPTIONS CHECKING IS VALIDPATH??

// ---------------------------------------------------------------------------------------------------------------------
// Generic implementations.

using namespace leobase::system::types;

std::string getCurrentDir()
{
    char buff[FILENAME_MAX];
    GetCurrentDir( buff, FILENAME_MAX );
    std::string current_working_dir(buff);
    current_working_dir.append("\\");
    convertWindowsPathToUnixInPlace(current_working_dir);
    return current_working_dir;
}

std::string sanitizeFilename(const std::string &filename, const std::string &replacement)
{
    if (filename.empty())
        return kDefaultFilename;

    static const std::unordered_set<char> invalidChars = {'<', '>', ':', '"', '/', '\\', '|', '?', '*'};

    std::string sanitized = filename;

    for (char& c : sanitized)
    {
        if (invalidChars.find(c) != invalidChars.end())
            c = replacement[0];
    }

    if (sanitized.empty() || sanitized == replacement)
        sanitized = kDefaultFilename;

    return sanitized;
}

std::string &sanitizeFilenameInPlace(std::string &filename, const std::string &replacement)
{
    if (filename.empty())
    {
        filename = kDefaultFilename;
        return filename;
    }

    static const std::unordered_set<char> invalidChars = {'<', '>', ':', '"', '/', '\\', '|', '?', '*'};

    for (char& c : filename)
    {
        if (invalidChars.find(c) != invalidChars.end())
            c = replacement[0];
    }

    if (filename.empty() || filename == replacement)
        filename = kDefaultFilename;

    return filename;
}

std::string getFoldernameFromFolderpath(const std::string& folder_path)
{
    if (folder_path.empty())
        return "";

    // Normalize by removing any trailing slashes
    std::string cleaned_path = folder_path;
    while (!cleaned_path.empty() && (cleaned_path.back() == '/' || cleaned_path.back() == '\\'))
        cleaned_path.pop_back();

    // Find the last separator
    size_t lastSlash = cleaned_path.find_last_of("/\\");
    if (lastSlash == std::string::npos)
        return cleaned_path;

    return cleaned_path.substr(lastSlash + 1);
}

std::string getPathFromFolderPath(const std::string& folder_path)
{
    if (folder_path.empty())
        return "";

    // Normalize by removing any trailing slashes
    std::string cleaned_path = folder_path;
    while (!cleaned_path.empty() && (cleaned_path.back() == '/' || cleaned_path.back() == '\\'))
        cleaned_path.pop_back();

    // Find the last separator
    size_t lastSlash = cleaned_path.find_last_of("/\\");
    if (lastSlash == std::string::npos)
        return "";

    return cleaned_path.substr(0, lastSlash + 1);
}

std::string getFilenameFromFilepath(const std::string& path, bool keep_extension)
{
    if (path.empty())
        return "";

    size_t lastSlash = path.find_last_of("/\\");
    if (lastSlash == std::string::npos)
        lastSlash = -1;

    std::string filename = path.substr(lastSlash + 1);
    if (!keep_extension)
    {
        size_t lastDot = filename.find_last_of('.');
        if (lastDot != std::string::npos)
            filename = filename.substr(0, lastDot);
    }

    return filename;
}

std::string getPathFromFilepath(const std::string& file_path)
{
    if (file_path.empty())
        return "";

    size_t lastSlash = file_path.find_last_of("/\\");
    if (lastSlash == std::string::npos)
        return "";

    return file_path.substr(0, lastSlash + 1);
}

std::string getFileExtension(const std::string& path)
{
    if (path.empty())
        return "";

    size_t lastSlash = path.find_last_of("/\\");
    if (lastSlash == std::string::npos)
        lastSlash = -1;

    size_t lastDot = path.find_last_of('.');
    if (lastDot == std::string::npos || lastDot < lastSlash + 1)
        return "";

    return path.substr(lastDot + 1);
}

std::string getWindowsDriveFromPath(const std::string& path)
{
    if (startsWithWindowsDrive(path))
        return path.substr(0, 3); // e.g., "C:/" or "D:\"

    return "";
}

bool isValidFilepath(const std::string& path)
{
    if (path.empty())
        return false;

    // Get the filename using the helper function
    std::string filename = getFilenameFromFilepath(path);

    if (filename.empty())
        return false;

    static const std::string invalidChars = "<>:\"|?*";

    if (filename.find_first_of(invalidChars) != std::string::npos)
        return false;

    // Check for mixed path separators
    bool hasBackslash = (path.find('\\') != std::string::npos);
    bool hasForwardSlash = (path.find('/') != std::string::npos);

    if (hasBackslash && hasForwardSlash)
        return false;

    // Ensure the path does not end with a slash (it must be a file)
    char lastChar = path.back();
    if (lastChar == '\\' || lastChar == '/')
        return false;

    return true;
}

bool isValidDir(const std::string& path)
{
    if (path.empty())
        return true;

    static const std::string invalidChars = "<>:\"|?*";

    // Handle Windows drive letters
    if (startsWithWindowsDrive(path))
    {
        // If it's just a drive letter like "C:", it's invalid
        if (path.length() == 2)
            return false;

        // Ensure the drive letter is followed by a valid separator (e.g., "C:/Users/")
        if (path[2] != '\\' && path[2] != '/')
            return false;

    }

    // Check for invalid characters (excluding the drive letter part)
    std::string pathWithoutDrive = path;
    if (startsWithWindowsDrive(path))
        pathWithoutDrive = path.substr(2); // Ignore "C:"

    if (pathWithoutDrive.find_first_of(invalidChars) != std::string::npos)
        return false;

    // Check for mixed path separators
    bool hasBackslash = (path.find('\\') != std::string::npos);
    bool hasForwardSlash = (path.find('/') != std::string::npos);
    if (hasBackslash && hasForwardSlash)
        return false;

    // Ensure directory paths end with a valid slash
    char lastChar = path.back();
    if (lastChar != '/' && lastChar != '\\')
        return false;

    return true;
}

bool isWindowsPath(const std::string& path)
{
    if (path.find('\\') != std::string::npos)
        return true;

    // Check for a Windows-style drive letter (e.g., "C:\")
    if (path.size() > 2 && std::isalpha(path[0]) && path[1] == ':' && path[2] == '\\')
        return true;

    if (path.size() > 2 && std::isalpha(path[0]) && path[1] == ':' && path[2] == '/')
        return false;

    return false;
}

bool isUnixPath(const std::string& path)
{
    return !isWindowsPath(path);
}

bool startsWithWindowsDrive(const std::string& path)
{
    if (path.size() < 2)
        return false;

    if (!std::isalpha(path[0]))
        return false;

    if (path[1] != ':')
        return false;

    if (path.size() > 2 && path[2] != '\\' && path[2] != '/')
        return false;

    return true;
}

std::string convertWindowsPathToUnix(const std::string& path)
{
    if (!isWindowsPath(path))
        return path;

    std::string unixPath = path;

    std::replace(unixPath.begin(), unixPath.end(), '\\', '/');

    return unixPath;
}

std::string& convertWindowsPathToUnixInPlace(std::string& path)
{
    if (!isWindowsPath(path))
        return path;

    std::replace(path.begin(), path.end(), '\\', '/');

    return path;
}

std::string convertUnixPathToWindows(const std::string& path)
{
    if (!isUnixPath(path))
        return path;

    std::string win_path = path;

    std::replace(win_path.begin(), win_path.end(), '/', '\\');

    return win_path;
}

std::string& convertUnixPathToWindowsInPlace(std::string& path)
{
    if (!isUnixPath(path))
        return path;

    std::replace(path.begin(), path.end(), '/', '\\');

    return path;
}

// ---------------------------------------------------------------------------------------------------------------------
// Windows Implementations.

#ifdef _WIN32

std::string getTempDir()
{
    char tempPath[MAX_PATH];

    // Retrieve the temporary path
    DWORD length = GetTempPathA(MAX_PATH, tempPath);
    if (length > 0 && length < MAX_PATH)
    {
        std::string tmp_path(tempPath);
        convertWindowsPathToUnixInPlace(tmp_path);
        return tmp_path;
    }

    // Fallback.
    return getOsRootDir() + "TEMP";
}

std::string getOsRootDir()
{
    char systemDrive[MAX_PATH];
    if (GetEnvironmentVariableA("SystemDrive", systemDrive, MAX_PATH))
    {
        return std::string(systemDrive) + "/";
    }
    return "C:/";
}

//TODO: Devolver error en vez de bool
bool createDirectory(const std::string& path)
{
    if (path.empty())
        return false;

    std::string currentPath;
    size_t pos = 0;
    bool success = true;

    // Iterate through path components
    while ((pos = path.find_first_of("\\/", pos)) != std::string::npos)
    {
        currentPath = path.substr(0, pos);
        pos++; // Move past the separator

        if (currentPath.empty())
            continue;

        if (!directoryExists(currentPath))
        {
            if (!CreateDirectoryA(currentPath.c_str(), NULL))
            {
                DWORD error = GetLastError();
                if (error != ERROR_ALREADY_EXISTS)
                {
                    success = false;
                    break;
                }
            }
        }

    }

    // Create the final directory
    if (!CreateDirectoryA(path.c_str(), NULL))
    {
        DWORD error = GetLastError();
        if (error != ERROR_ALREADY_EXISTS)
            success = false;
    }

    return success;
}

bool createEmptyFile(const std::string& filePath)
{
    if(filePath.empty() || fileExists(filePath))
        return false;

    // Extract directory part of the file path
    size_t lastSlash = filePath.find_last_of("\\/");
    if (lastSlash != std::string::npos)
    {
        std::string dirPath = filePath.substr(0, lastSlash);
        if (!dirPath.empty() && !createDirectory(dirPath))
            return false;
    }

    std::ofstream file(filePath, std::ios::out | std::ios::app);
    if (!file)
        return false;

    file.close();
    return true;
}

bool deleteDirectory(const std::string& dir)
{
    if (dir.empty() || !PathIsDirectoryA(dir.c_str()))
        return false;

    std::string path = convertUnixPathToWindows(dir);

    std::string searchPath = path + "\\*"; // Append wildcard to list files
    WIN32_FIND_DATAA findFileData;
    HANDLE hFind = FindFirstFileA(searchPath.c_str(), &findFileData);

    if (hFind == INVALID_HANDLE_VALUE)
        return false;

    do
    {
        const std::string fileName = findFileData.cFileName;
        if (fileName == "." || fileName == "..")
            continue;

        std::string fullPath = path + "\\" + fileName;

        if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            // Recursively delete subdirectory
            if (!deleteDirectory(fullPath))
            {
                FindClose(hFind);
                return false;
            }
        }
        else
        {
            // Delete file
            if (!DeleteFileA(fullPath.c_str()))
            {
                FindClose(hFind);
                return false;
            }
        }

    } while (FindNextFileA(hFind, &findFileData) != 0);

    FindClose(hFind);

    // Remove the now-empty directory
    return RemoveDirectoryA(path.c_str()) != 0;
}

bool deleteFile(const std::string& file_path)
{
    if (file_path.empty())
        return false;

    std::string path = convertUnixPathToWindows(file_path);

    return DeleteFileA(file_path.c_str()) != 0;
}

bool copyFile(const std::string& source_file, const std::string& dest_file)
{
    std::string path_src = convertUnixPathToWindows(source_file);
    std::string path_dest = convertUnixPathToWindows(dest_file);

    return CopyFileA(path_src.c_str(), path_dest.c_str(), FALSE);
}

bool copyDirectory(const std::string& source_dir, const std::string& dest_dir)
{
    std::string src = convertUnixPathToWindows(source_dir);
    std::string dest = convertUnixPathToWindows(dest_dir);

    DWORD srcAttrib = GetFileAttributesA(src.c_str());
    if (srcAttrib == INVALID_FILE_ATTRIBUTES || !(srcAttrib & FILE_ATTRIBUTE_DIRECTORY))
    {
        return false;
    }

    if (!CreateDirectoryA(dest.c_str(), NULL) && GetLastError() != ERROR_ALREADY_EXISTS)
    {
        return false;
    }

    std::string searchPath = src + "\\*";
    WIN32_FIND_DATAA findFileData;
    HANDLE hFind = FindFirstFileA(searchPath.c_str(), &findFileData);

    if (hFind == INVALID_HANDLE_VALUE)
    {
        return false;
    }

    bool success = true;

    do
    {
        std::string itemName = findFileData.cFileName;
        if (itemName == "." || itemName == "..") continue;

        std::string srcPath = src + "\\" + itemName;
        std::string destPath = dest + "\\" + itemName;

        if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            if (!copyDirectory(srcPath, destPath))
            {
                success = false;
            }
        }
        else
        {
            if (!copyFile(srcPath, destPath))
            {
                success = false;
            }
        }

    } while (FindNextFileA(hFind, &findFileData));

    FindClose(hFind);
    return success;
}

bool fileExists(const std::string& file_path)
{
    std::string winPath = convertUnixPathToWindows(file_path);
    DWORD fileAttrib = GetFileAttributesA(winPath.c_str());
    return (fileAttrib != INVALID_FILE_ATTRIBUTES && !(fileAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

bool directoryExists(const std::string& dir_path)
{
    std::string winPath = convertUnixPathToWindows(dir_path);
    DWORD fileAttrib = GetFileAttributesA(winPath.c_str());
    return (fileAttrib != INVALID_FILE_ATTRIBUTES && (fileAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

bool getFileInfo(const std::string& filePath, types::BasicFileInfo& fileInfo)
{
    if (filePath.empty())
        return false;

    // Convert into windows path.
    std::string win_path = convertUnixPathToWindows(filePath);

    WIN32_FIND_DATAA find_file_data;
    HANDLE h_find = FindFirstFileA(win_path.c_str(), &find_file_data);
    if (h_find == INVALID_HANDLE_VALUE)
        return false;

    // Check if file path is a directory.
    if (find_file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        return false;

    // Create file info struct, in constructor is filled filename and file extension too.
    types::BasicFileInfo file_info(filePath);

    // Fill file size.
    file_info.sizeB = (static_cast<std::size_t>(find_file_data.nFileSizeHigh) << 32) |
                      find_file_data.nFileSizeLow;

    // Fill permisions.
    auto attributes = find_file_data.dwFileAttributes;
    std::string permissions = "---------";
    if (attributes & FILE_ATTRIBUTE_READONLY)   permissions[0] = 'r';
    if (attributes & FILE_ATTRIBUTE_HIDDEN)     permissions[1] = 'h';
    if (attributes & FILE_ATTRIBUTE_SYSTEM)     permissions[2] = 's';
    if (attributes & FILE_ATTRIBUTE_DIRECTORY)  permissions[3] = 'd';
    if (attributes & FILE_ATTRIBUTE_ARCHIVE)    permissions[4] = 'a';

    file_info.permissions = std::move(permissions);

    fileInfo = std::move(file_info);

    return true;
}

bool listFilesInDirectory(const std::string& dir, BasicFileInfoV& files)
{
    if (dir.empty() || !PathIsDirectoryA(dir.c_str()))
        return false;

    std::string path = convertUnixPathToWindows(dir);

    if (path.back() != '\\')
        path += "\\";

    WIN32_FIND_DATAA findFileData;
    HANDLE hFind;
    std::string searchPath = path + "*";

    hFind = FindFirstFileA(searchPath.c_str(), &findFileData);
    if (hFind == INVALID_HANDLE_VALUE)
        return false;

    do
    {
        std::string fileName = findFileData.cFileName;

        if (fileName == "." || fileName == "..")
            continue;

        BasicFileInfo fileInfo;
        fileInfo.filePath = path + fileName;
        fileInfo.filePath = convertWindowsPathToUnix(fileInfo.filePath);
        fileInfo.fileName = fileName;
        fileInfo.path = convertWindowsPathToUnix(path);

        // Extract file extension
        size_t ext_pos = fileName.find_last_of('.');
        fileInfo.fileExt = (ext_pos != std::string::npos) ? fileName.substr(ext_pos + 1) : "";

        // Get file size
        fileInfo.sizeB = (static_cast<std::size_t>(findFileData.nFileSizeHigh) << 32) |
                         findFileData.nFileSizeLow;

        fileInfo.modTime = "";

        fileInfo.permissions = "";

        files.push_back(fileInfo);

    } while (FindNextFileA(hFind, &findFileData) != 0);

    FindClose(hFind);
    return !files.empty();
}

// ---------------------------------------------------------------------------------------------------------------------
// Unix Implementations.

#else

#endif

// ---------------------------------------------------------------------------------------------------------------------

END_NAMESPACE(utils)
LEOBASE_MODULE_END
