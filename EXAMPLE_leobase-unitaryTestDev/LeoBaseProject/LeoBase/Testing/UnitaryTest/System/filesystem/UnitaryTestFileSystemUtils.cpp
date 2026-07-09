/*
 * Copyright(C) Milethos Technologies SLU. 2026
 * All rights reserved. Reproduction in whole or in
 * part is prohibited without the written consent of
 * the copyright holder.
 */

// UnitaryTest FilesystemUtils Test

// C++ INCLUDES
#include <iostream>
#include <string>
#include <vector>
#include <fstream>

// LEOBASE INCLUDES
#include <LeoBase/Aliases/Testing>
#include "LeoBase/System/filesystem/filesystem_utils.h"
#include "LeoBase/System/types/filesystem_types.h"

// ---------------------------------------------------------------------------------------------------------------------

using namespace leobase;
using namespace leobase::testing;
using namespace leobase::system::files;
using namespace leobase::system::types;
// 1. Test Declarations

// String / Path Manipulation Tests
UT_DECLARE_TEST(Filesystem_CurrentDirCheck);
UT_DECLARE_TEST(Filesystem_SanitizeFilename);
UT_DECLARE_TEST(Filesystem_GetFoldername);
UT_DECLARE_TEST(Filesystem_GetPathFromFolder);
UT_DECLARE_TEST(Filesystem_GetFilenameFromFile);
UT_DECLARE_TEST(Filesystem_GetPathFromFile);
UT_DECLARE_TEST(Filesystem_GetExtension);
UT_DECLARE_TEST(Filesystem_WindowsDriveExtraction);

// Validation Tests
UT_DECLARE_TEST(Filesystem_IsValidFilepath);
UT_DECLARE_TEST(Filesystem_IsValidDir);
UT_DECLARE_TEST(Filesystem_PathFormatDetection); // isWindowsPath, isUnixPath

// Conversion Tests
UT_DECLARE_TEST(Filesystem_PathConversions);

// I/O Operations (Real filesystem interaction)
UT_DECLARE_TEST(Filesystem_IO_DirectoryLifecycle); // Create, Exist, Delete Dir
UT_DECLARE_TEST(Filesystem_IO_FileLifecycle);      // Create, Exist, Delete File
UT_DECLARE_TEST(Filesystem_IO_CopyOperations);     // Copy File/Dir
UT_DECLARE_TEST(Filesystem_IO_ListFiles);          // listFilesInDirectory
UT_DECLARE_TEST(Filesystem_IO_FileInfo);           // getFileInfo

// 2. Test Definitions

// --- PATH MANIPULATION TESTS ---

/**
 * @brief Checks if getCurrentDir returns a non-empty string.
 */
UT_DEFINE_TEST(Filesystem_CurrentDirCheck)
{
    std::string cwd;
    UT_EXPECT_NO_THROW(cwd = getCurrentDir());
    UT_EXPECTED_FALSE(cwd.empty());

    // According to implementation, it appends a slash and converts to Unix style
    if (!cwd.empty())
    {
        UT_EXPECTED_TRUE(cwd.back() == '/');
        UT_EXPECTED_TRUE(cwd.find('\\') == std::string::npos); // Should be normalized
    }
}

/**
 * @brief Tests filename sanitization (removing illegal chars).
 */
UT_DEFINE_TEST(Filesystem_SanitizeFilename)
{
    // 1. Standard Sanitize
    std::string dirty = "file:name?.txt";
    std::string clean = sanitizeFilename(dirty, "_");
    UT_EXPECTED_TRUE(clean == "file_name_.txt");

    // 2. Empty returns default
    UT_EXPECTED_TRUE(sanitizeFilename("") == "default_filename");

    // 3. All invalid chars
    std::string allBad = "<>:\"/\\|?*";
    std::string allRep = "_________";
    UT_EXPECTED_TRUE(sanitizeFilename(allBad, "_") == allRep);

    // 4. In-Place
    std::string inPlace = "bad|name";
    sanitizeFilenameInPlace(inPlace, "-");
    UT_EXPECTED_TRUE(inPlace == "bad-name");
}

/**
 * @brief Tests extracting folder name from a full path.
 */
UT_DEFINE_TEST(Filesystem_GetFoldername)
{
    // Standard case
    UT_EXPECTED_TRUE(getFoldernameFromFolderpath("C:/Users/Admin/Docs") == "Docs");
    // Trailing slash case (should be ignored)
    UT_EXPECTED_TRUE(getFoldernameFromFolderpath("C:/Users/Admin/Docs/") == "Docs");
    // Root case
    UT_EXPECTED_TRUE(getFoldernameFromFolderpath("Docs") == "Docs");
    // Empty
    UT_EXPECTED_TRUE(getFoldernameFromFolderpath("").empty());
}

/**
 * @brief Tests extracting the parent path from a folder path.
 */
UT_DEFINE_TEST(Filesystem_GetPathFromFolder)
{
    UT_EXPECTED_TRUE(getPathFromFolderPath("C:/Users/Admin/Docs") == "C:/Users/Admin/");
    UT_EXPECTED_TRUE(getPathFromFolderPath("C:/Users/Admin/Docs/") == "C:/Users/Admin/");
    UT_EXPECTED_TRUE(getPathFromFolderPath("Docs").empty()); // No parent
}

/**
 * @brief Tests extracting filename from path (with/without extension).
 */
UT_DEFINE_TEST(Filesystem_GetFilenameFromFile)
{
    std::string path = "C:/Data/image.png";

    UT_EXPECTED_TRUE(getFilenameFromFilepath(path, true) == "image.png");
    UT_EXPECTED_TRUE(getFilenameFromFilepath(path, false) == "image");

    // Edge case: No path, just file
    UT_EXPECTED_TRUE(getFilenameFromFilepath("image.png", true) == "image.png");

    // Edge case: Double extension
    UT_EXPECTED_TRUE(getFilenameFromFilepath("archive.tar.gz", false) == "archive.tar");
}

/**
 * @brief Tests extracting the directory part of a file path.
 */
UT_DEFINE_TEST(Filesystem_GetPathFromFile)
{
    UT_EXPECTED_TRUE(getPathFromFilepath("C:/Data/image.png") == "C:/Data/");
    UT_EXPECTED_TRUE(getPathFromFilepath("/usr/bin/exec") == "/usr/bin/");
    UT_EXPECTED_TRUE(getPathFromFilepath("image.png").empty());
}

/**
 * @brief Tests extension extraction.
 */
UT_DEFINE_TEST(Filesystem_GetExtension)
{
    UT_EXPECTED_TRUE(getFileExtension("test.txt") == "txt");
    UT_EXPECTED_TRUE(getFileExtension("/path/to/file.tar.gz") == "gz");
    UT_EXPECTED_TRUE(getFileExtension("Makefile").empty());
    UT_EXPECTED_TRUE(getFileExtension("").empty());
}

/**
 * @brief Tests Windows drive letter extraction.
 */
UT_DEFINE_TEST(Filesystem_WindowsDriveExtraction)
{
    UT_EXPECTED_TRUE(getWindowsDriveFromPath("C:\\Windows") == "C:\\");
    UT_EXPECTED_TRUE(getWindowsDriveFromPath("D:/Games") == "D:/");
    UT_EXPECTED_TRUE(getWindowsDriveFromPath("/usr/bin").empty());
    UT_EXPECTED_TRUE(getWindowsDriveFromPath("Relative/Path").empty());
}

// --- VALIDATION TESTS ---

/**
 * @brief Tests isValidFilepath logic.
 */
UT_DEFINE_TEST(Filesystem_IsValidFilepath)
{
    // Positive cases
    UT_EXPECTED_TRUE(isValidFilepath("C:/test.txt"));
    UT_EXPECTED_TRUE(isValidFilepath("file.log"));

    // Negative cases
    UT_EXPECTED_FALSE(isValidFilepath(""));
    UT_EXPECTED_FALSE(isValidFilepath("C:/folder/")); // Ends in slash
    UT_EXPECTED_FALSE(isValidFilepath("C:/bad?file.txt")); // Invalid char

    // Mixed slashes are explicitly forbidden in implementation
    UT_EXPECTED_FALSE(isValidFilepath("C:\\User/Mixed.txt"));
}

/**
 * @brief Tests isValidDir logic.
 */
UT_DEFINE_TEST(Filesystem_IsValidDir)
{
    UT_EXPECTED_TRUE(isValidDir("C:/Windows/"));
    UT_EXPECTED_TRUE(isValidDir("C:\\Windows\\"));

    // Mixed slashes
    UT_EXPECTED_FALSE(isValidDir("C:/Windows\\System32"));

    // Invalid chars
    UT_EXPECTED_FALSE(isValidDir("C:/Fold?er"));

    // Must end with slash according to implementation logic (lastChar check)
    UT_EXPECTED_FALSE(isValidDir("C:/Windows"));
}

/**
 * @brief Tests detection of Windows vs Unix paths.
 */
UT_DEFINE_TEST(Filesystem_PathFormatDetection)
{
    UT_EXPECTED_TRUE(isWindowsPath("C:\\Windows"));
    UT_EXPECTED_TRUE(isWindowsPath("\\Network\\Share"));
    UT_EXPECTED_FALSE(isWindowsPath("/usr/bin"));

    UT_EXPECTED_TRUE(isUnixPath("/var/log"));
    UT_EXPECTED_FALSE(isUnixPath("C:\\"));

    UT_EXPECTED_TRUE(startsWithWindowsDrive("C:/"));
    UT_EXPECTED_FALSE(startsWithWindowsDrive("/mnt"));
}

// --- CONVERSION TESTS ---

/**
 * @brief Tests conversion between separators.
 */
UT_DEFINE_TEST(Filesystem_PathConversions)
{
    std::string win = "C:\\User\\Docs";
    std::string unixLike = "C:/User/Docs";

    UT_EXPECTED_TRUE(convertWindowsPathToUnix(win) == unixLike);
    UT_EXPECTED_TRUE(convertUnixPathToWindows(unixLike) == win);

    // In-place
    std::string path = win;
    convertWindowsPathToUnixInPlace(path);
    UT_EXPECTED_TRUE(path == unixLike);
}

// --- I/O OPERATIONS (SANDBOX) ---

/**
 * @brief Helper to create a safe temporary path for testing.
 */
std::string getTestSandboxPath()
{
    std::string temp = getTempDir(); // Returns unix style with slash
    return temp + "LeoBase_UnitTests_FS/";
}

/**
 * @brief Tests creating and deleting directories.
 */
UT_DEFINE_TEST(Filesystem_IO_DirectoryLifecycle)
{
    std::string sandbox = getTestSandboxPath() + "DirTest/";

    // 1. Cleanup before start (just in case)
    deleteDirectory(sandbox);

    // 2. Create Directory
    UT_EXPECTED_FALSE(directoryExists(sandbox));
    UT_EXPECTED_TRUE(createDirectory(sandbox));
    UT_EXPECTED_TRUE(directoryExists(sandbox));

    // 3. Create Nested Directory
    std::string nested = sandbox + "Deep/Level/";
    UT_EXPECTED_TRUE(createDirectory(nested));
    UT_EXPECTED_TRUE(directoryExists(nested));

    // 4. Delete Directory
    UT_EXPECTED_TRUE(deleteDirectory(sandbox));
    UT_EXPECTED_FALSE(directoryExists(sandbox));
}

/**
 * @brief Tests creating and deleting empty files.
 */
UT_DEFINE_TEST(Filesystem_IO_FileLifecycle)
{
    std::string sandbox = getTestSandboxPath() + "FileTest/";
    std::string filePath = sandbox + "testfile.txt";

    createDirectory(sandbox);

    // 1. Create File
    UT_EXPECTED_FALSE(fileExists(filePath));
    UT_EXPECTED_TRUE(createEmptyFile(filePath));
    UT_EXPECTED_TRUE(fileExists(filePath));

    // 2. Delete File
    UT_EXPECTED_TRUE(deleteFile(filePath));
    UT_EXPECTED_FALSE(fileExists(filePath));

    // Cleanup
    deleteDirectory(sandbox);
}

/**
 * @brief Tests copying files and directories.
 */
UT_DEFINE_TEST(Filesystem_IO_CopyOperations)
{
    std::string sandbox = getTestSandboxPath() + "CopyTest/";
    std::string srcDir = sandbox + "Src/";
    std::string dstDir = sandbox + "Dst/";
    std::string srcFile = srcDir + "data.txt";
    std::string copiedFile = dstDir + "data.txt"; // copyDirectory keeps structure

    createDirectory(srcDir);

    // Write dummy content
    {
        std::ofstream ofs(srcFile);
        ofs << "Hello World";
        ofs.close();
    }

    // 1. Copy File
    std::string individualCopy = sandbox + "copied_data.txt";
    UT_EXPECTED_TRUE(copyFile(srcFile, individualCopy));
    UT_EXPECTED_TRUE(fileExists(individualCopy));

    // 2. Copy Directory
    UT_EXPECTED_TRUE(copyDirectory(srcDir, dstDir));
    UT_EXPECTED_TRUE(directoryExists(dstDir));
    UT_EXPECTED_TRUE(fileExists(copiedFile)); // File inside dir should exist

    // Cleanup
    deleteDirectory(sandbox);
}

/**
 * @brief Tests listing files in a directory.
 */
UT_DEFINE_TEST(Filesystem_IO_ListFiles)
{
    std::string sandbox = getTestSandboxPath() + "ListTest/";
    createDirectory(sandbox);
    createEmptyFile(sandbox + "A.txt");
    createEmptyFile(sandbox + "B.log");

    // Fix: Removed 'types::' prefix as we are using the namespace now
    BasicFileInfoV files;
    bool result = listFilesInDirectory(sandbox, files);

    UT_EXPECTED_TRUE(result);
    // Note: "." and ".." are filtered out in implementation
    UT_EXPECTED_TRUE(files.size() >= 2);

    bool foundA = false;
    for(const auto& f : files) {
        if(f.fileName == "A.txt") foundA = true;
    }
    UT_EXPECTED_TRUE(foundA);

    // Cleanup
    deleteDirectory(sandbox);
}

/**
 * @brief Tests retrieving file information (size, etc).
 */
UT_DEFINE_TEST(Filesystem_IO_FileInfo)
{
    std::string sandbox = getTestSandboxPath() + "InfoTest/";
    std::string filePath = sandbox + "info.txt";
    createDirectory(sandbox);

    std::string content = "12345"; // 5 bytes
    {
        std::ofstream ofs(filePath);
        ofs << content;
        ofs.close();
    }

    // Fix: Removed 'types::' prefix
    BasicFileInfo info;
    UT_EXPECTED_TRUE(getFileInfo(filePath, info));

    UT_EXPECTED_TRUE(info.fileName == "info.txt");
    UT_EXPECTED_TRUE(info.fileExt == "txt");
    UT_EXPECTED_TRUE(info.sizeB == 5);

    // Permissions string check "r----" - minimal check
    UT_EXPECTED_FALSE(info.permissions.empty());

    // Cleanup
    deleteDirectory(sandbox);
}

// 3. Main Session Execution

UT_START_SESSION("FilesystemUtils Unitary Tests")

// Stop on failure is optional, but useful for I/O tests to prevent leftover garbage if logic breaks early.
UT_FORCE_STOP_IF_SUBTEST_FAIL(false);

// Register Path Manipulation Tests
UT_REGISTER_TEST(LeoBase, System, Filesystem_CurrentDirCheck);
UT_REGISTER_TEST(LeoBase, System, Filesystem_SanitizeFilename);
UT_REGISTER_TEST(LeoBase, System, Filesystem_GetFoldername);
UT_REGISTER_TEST(LeoBase, System, Filesystem_GetPathFromFolder);
UT_REGISTER_TEST(LeoBase, System, Filesystem_GetFilenameFromFile);
UT_REGISTER_TEST(LeoBase, System, Filesystem_GetPathFromFile);
UT_REGISTER_TEST(LeoBase, System, Filesystem_GetExtension);
UT_REGISTER_TEST(LeoBase, System, Filesystem_WindowsDriveExtraction);

// Register Validation Tests
UT_REGISTER_TEST(LeoBase, System, Filesystem_IsValidFilepath);
UT_REGISTER_TEST(LeoBase, System, Filesystem_IsValidDir);
UT_REGISTER_TEST(LeoBase, System, Filesystem_PathFormatDetection);

// Register Conversion Tests
UT_REGISTER_TEST(LeoBase, System, Filesystem_PathConversions);

// Register I/O Tests
UT_REGISTER_TEST(LeoBase, System, Filesystem_IO_DirectoryLifecycle);
UT_REGISTER_TEST(LeoBase, System, Filesystem_IO_FileLifecycle);
UT_REGISTER_TEST(LeoBase, System, Filesystem_IO_CopyOperations);
UT_REGISTER_TEST(LeoBase, System, Filesystem_IO_ListFiles);
UT_REGISTER_TEST(LeoBase, System, Filesystem_IO_FileInfo);

UT_RUN_TESTS();

UT_FINISH_SESSION()
