/*
 *      Copyright(C) Milethos Technologies SLU. 2025
 *      All rights reserved. Reproduction in whole or in
 *      part is prohibited without the written consent of
 *      the copyright holder.
 */

#pragma once

// C++ INCLUDES
#include <string>

// LEOBASE INCLUDES
#include "LeoBase/Aliases/Global"
#include "LeoBase/System/types/filesystem_types.h"

// LEOBASE MACROS
LEOBASE_MODULE_BEGIN(system)
BEGIN_NAMESPACE(files)

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Retrieves the current working directory.
 *
 * This function gets the absolute path of the current working
 * directory. It uses platform-specific implementations.
 *
 * @return std::string The absolute path of the current working directory.
 */
LEOBASE_EXPORT std::string getCurrentDir();

/**
 * @brief Retrieves the system's temporary directory path.
 *
 * This function retrieves the temporary directory:
 * - Windows: Uses `GetTempPathA()`, with a fallback to `"C:/TEMP"`.
 * - Unix: Uses `$TMPDIR`, `$TMP`, or `$TEMP` variables. Defaults to `"/tmp/"`.
 *
 * @return A string containing the system's temporary directory path.
 */
LEOBASE_EXPORT std::string getTempDir();

/**
 * @brief Retrieves the operating system's root directory.
 *
 * - Windows: It returns the OS installation drive (e.g., "C:/").
 * - Unix: It always returns "/".
 *
 * @return The root directory as a string.
 */
LEOBASE_EXPORT std::string getOsRootDir();

/**
 * @brief Sanitizes a filename by replacing invalid characters.
 *
 * This function ensures that the filename is safe for use on different operating systems
 * by replacing characters that are not allowed in filenames.
 *
 * @param filename The input filename.
 * @param rep The string to replace invalid characters (default: empty string).
 * @return A sanitized version of the filename.
 */
LEOBASE_EXPORT std::string sanitizeFilename(const std::string& filename, const std::string& rep = "");

/**
 * @brief Sanitizes a filename in-place by replacing invalid characters.
 *
 * This function modifies the given string directly to ensure that it is a valid filename
 * by replacing invalid characters.
 *
 * @param filename The input filename (modified in-place).
 * @param rep The string to replace invalid characters (default: empty string).
 * @return A reference to the modified filename.
 */
LEOBASE_EXPORT std::string& sanitizeFilenameInPlace(std::string& filename, const std::string& rep = "");

/**
 * @brief Extracts the folder name from a given folder path.
 *
 * This function removes any trailing slashes and extracts the last directory name.
 * It works for both Windows and Unix-style paths.
 *
 * @param folderPath The full folder path.
 * @return The extracted folder name, or an empty string if the input is empty.
 */
LEOBASE_EXPORT std::string getFoldernameFromFolderpath(const std::string& folderPath);

/**
 * @brief Extracts the parent path from a given folder path.
 *
 * This function removes any trailing slashes and returns the parent directory.
 * It works for both Windows and Unix-style paths.
 *
 * @param folderPath The full folder path.
 * @return The extracted parent path, or an empty string if no parent exists.
 */
LEOBASE_EXPORT std::string getPathFromFolderPath(const std::string& folderPath);

/**
 * @brief Extracts the filename from a given file path.
 *
 * This function works for both Windows and Unix-style paths.
 * If the path ends with a directory separator, it returns an empty string.
 *
 * @param filePath The full file path.
 * @param keepExt If true, retains the file extension; if false, removes it.
 * @return The extracted filename, or an empty string if no filename exists.
 */
LEOBASE_EXPORT std::string getFilenameFromFilepath(const std::string& filePath, bool keepExt = true);

/**
 * @brief Extracts the directory path from a given file path.
 *
 * This function works for both Windows and Unix-style paths.
 * If the path does not contain a filename, it returns the original path.
 *
 * @param filePath The full file path.
 * @return The extracted directory path.
 */
LEOBASE_EXPORT std::string getPathFromFilepath(const std::string& filePath);

/**
 * @brief Extracts the file extension from a given file path.
 *
 * This function returns the extension with the dot (e.g., ".txt").
 * If no extension exists, it returns an empty string.
 *
 * @param filePath The full file path.
 * @return std::string The file extension, or an empty string if none exists.
 */
LEOBASE_EXPORT std::string getFileExtension(const std::string& filePath);

/**
 * @brief Extracts the Windows drive from a given path (e.g., "C:/").
 *
 * If the path starts with a valid Windows drive letter, this function extracts it.
 * Otherwise, it returns an empty string.
 *
 * @param path The input path.
 * @return std::string The extracted drive letter (e.g., "C:/") or an empty string if none exists.
 */
LEOBASE_EXPORT std::string getWindowsDriveFromPath(const std::string& path);

/**
 * @brief Validates if a given path is a valid file path.
 *
 * This function ensures that the provided path:
 * - Is not empty.
 * - Contains a valid filename (extracted using `getFilenameFromFilepath()`).
 * - Does not contain invalid characters (`<>:"|?*`).
 * - Does not mix forward (`/`) and backward (`\`) slashes.
 * - Does not end with a slash (`/` or `\`), as files should not.
 *
 * @param path The file path to validate.
 * @return bool True if the path is a valid file path, false otherwise.
 */
LEOBASE_EXPORT bool isValidFilepath(const std::string& path);

/**
 * @brief Checks if a given path is a valid directory path.
 *
 * Ensures the path:
 * - Does not contain invalid characters (`<>:"|?*`).
 * - Does not mix forward (`/`) and backward (`\`) slashes.
 * - Allows Windows paths with drive letters (e.g., "C:\Users\").
 * - Ensures directories end with a slash (`/` or `\`)
 * - Allows empty strings.
 *
 * @param path The directory path to validate.
 * @return bool True if the path is a valid directory, false otherwise.
 */
LEOBASE_EXPORT bool isValidDir(const std::string& dir);

/**
 * @brief Checks if a given path follows the Windows format.
 *
 * This function determines if a path follows Windows-style notation.
 * A Windows path typically:
 * - Contains backslashes (`\`) as directory separators.
 * - Starts with a drive letter followed by `:\` (e.g., "C:\").
 *
 * @param path The file path to check.
 * @return bool True if the path follows Windows format, otherwise false.
 */
LEOBASE_EXPORT bool isWindowsPath(const std::string& path);

/**
 * @brief Checks if a given path follows the Unix/Linux format.
 *
 * This function determines if a path is in Unix-style notation.
 * A Unix path typically:
 * - Uses forward slashes (`/`) as directory separators.
 * - Does not contain a drive letter with `:` notation.
 *
 * @param path The file path to check.
 * @return bool True if the path follows Unix format, otherwise false.
 */
LEOBASE_EXPORT bool isUnixPath(const std::string& path);


/**
 * @brief Checks if a given path starts with a valid Windows drive letter (e.g., "C:/").
 *
 * A valid Windows drive must:
 * - Start with a letter (A-Z or a-z).
 * - Followed by a colon (`:`).
 * - Optionally followed by a backslash (`\`) or forward slash (`/`).
 *
 * @param path The input path.
 * @return bool True if the path starts with a Windows drive letter, false otherwise.
 */
LEOBASE_EXPORT bool startsWithWindowsDrive(const std::string& path);

/**
 * @brief Converts a Windows-style path to a Unix-style path.
 *
 * This function replaces Windows-style backslashes (`\`) with Unix-style
 * forward slashes (`/`). If a Windows drive letter is detected (e.g., "C:\\path"),
 *  it is preserved as "/c/path" for compatibility.
 *
 * @param path The Windows-style file path.
 * @return The converted Unix-style file path.
 */
LEOBASE_EXPORT std::string convertWindowsPathToUnix(const std::string& path);

/**
 * @brief Converts a Windows-style path to a Unix-style path in place.
 *
 * This function modifies the given string directly by:
 * - Replacing backslashes (`\`) with forward slashes (`/`).
 * - Preserving relative paths.
 * - Converting a Windows drive letter (e.g., "C:\\path") to "/c/path".
 *
 * @param path The Windows-style file path (modified in place).
 * @return A reference to the modified Unix-style file path.
 */
LEOBASE_EXPORT std::string& convertWindowsPathToUnixInPlace(std::string& dir);

/**
 * @brief Converts a Unix-style path to a Windows-style path.
 *
 * This function replaces Unix-style forward slashes (`/`) with Windows-style
 * backslashes (`\`). If the path follows Unix mount-point notation (e.g., "/c/path"),
 * the drive letter is restored (e.g., "C:\\path").
 *
 * @param path The Unix-style file path.
 * @return The converted Windows-style file path.
 */
LEOBASE_EXPORT std::string convertUnixPathToWindows(const std::string& path);

/**
 * @brief Converts a Unix-style path to a Windows-style path in place.
 *
 * This function modifies the given string directly by:
 * - Replacing forward slashes (`/`) with backslashes (`\`).
 * - Preserving relative paths.
 * - Converting a Unix drive notation (e.g., "/c/path") to "C:\\path".
 *
 * @param path The Unix-style file path (modified in place).
 * @return A reference to the modified Windows-style file path.
 */
LEOBASE_EXPORT std::string& convertUnixPathToWindowsInPlace(std::string& dir);

/**
 * @brief Creates a directory and all its parent directories if they do not exist.
 * @param dir The full path to the directory.
 * @return true if the directory exists or was successfully created, false otherwise.
 */
LEOBASE_EXPORT bool createDirectory(const std::string& dir);

/**
 * @brief Creates an empty file at the specified path if it doesn't already exist.
 *
 * This function checks whether the file exists and, if not, creates all necessary
 * parent directories and then the file itself. The file is created empty and
 * left untouched if it already exists.
 *
 * @param filePath The full (absolute or relative) path to the file to be created.
 * @return true if the file was successfully created; false if the file exists,
 *         the path is empty, or an error occurred during directory or file creation.
 */
LEOBASE_EXPORT bool createEmptyFile(const std::string& filePath);

/**
 * @brief Recursively deletes a directory and all its contents.
 *
 * This function removes all files and subdirectories within the specified directory.
 * The function is implemented to work seamlessly on both Windows and Unix systems.
 *
 * @param dir Path to the directory to be deleted.
 * @return True if the directory and its contents were successfully deleted, false otherwise.
 */
LEOBASE_EXPORT bool deleteDirectory(const std::string& dir);

/**
 * @brief Deletes a single file from the filesystem.
 *
 * This function deletes the file specified by `file_path`. The function is
 * implemented to work seamlessly on both Windows and Unix systems.
 *
 * @param filePath Path to the file to be deleted.
 * @return True if the file was successfully deleted, false otherwise.
 */
LEOBASE_EXPORT bool deleteFile(const std::string& filePath);

/**
 * @brief Copies a file from a source location to a destination.
 *
 * This function works across both Windows and Unix platforms. It ensures that
 * file paths are properly handled for each operating system.
 *
 * @param sourceFile Path to the source file.
 * @param destFile Path to the destination file.
 * @return True if the file was successfully copied, false otherwise.
 */
LEOBASE_EXPORT bool copyFile(const std::string& sourceFile, const std::string& destFile);

/**
 * @brief Recursively copies a directory and all its contents to a new location.
 *
 * This function copies all files and subdirectories from the source directory to the destination.
 * It handles platform-specific path conversions internally.
 *
 * @param sourceDir Path to the source directory.
 * @param destDir Path to the destination directory.
 * @return True if the directory was successfully copied, false otherwise.
 */
LEOBASE_EXPORT bool copyDirectory(const std::string& sourceDir, const std::string& destDir);

/**
 * @brief Checks if a file exists.
 *
 * This function verifies whether a file exists at the given path.
 * It supports both Windows and Unix file systems.
 *
 * @param filePath Path to the file.
 * @return True if the file exists, false otherwise.
 */
LEOBASE_EXPORT bool fileExists(const std::string& filePath);

/**
 * @brief Checks if a directory exists.
 *
 * This function determines whether a given directory exists.
 * It ensures compatibility with both Windows and Unix-like systems.
 *
 * @param dir_path Path to the directory.
 * @return True if the directory exists, false otherwise.
 */
LEOBASE_EXPORT bool directoryExists(const std::string& dir_path);

LEOBASE_EXPORT bool getFileInfo(const std::string& filePath, types::BasicFileInfo& fileInfo);

// TODO ADD TIME
LEOBASE_EXPORT bool listFilesInDirectory(const std::string& dir, types::BasicFileInfoV& files);

// ---------------------------------------------------------------------------------------------------------------------

END_NAMESPACE(files)
LEOBASE_MODULE_END
