/*
 *      Copyright(C) Milethos Technologies SLU. 2025
 *      All rights reserved. Reproduction in whole or in
 *      part is prohibited without the written consent of
 *      the copyright holder.
 */

// C++ INCLUDES
#include <iostream>
#include <fstream>

// LEOBASE INCLUDES
#include <LeoBase/Aliases/System>
#include <LeoBase/Aliases/Helpers>

// ---------------------------------------------------------------------------------------------------------------------

// LeoBase namespaces.
using namespace leobase::system;
using namespace leobase::helpers;

// ---------------------------------------------------------------------------------------------------------------------

std::vector<std::string> test_dirs_valid =
{
    "/home/user/",
    "C:\\Users\\",
    "/var/log/",
    "/usr/local/bin/",
    "C:\\aux\\files\\"
};

std::vector<std::string> test_dirs_invalid =
{
    "/invalid|path/",
    "C:\\invalid:name\\",
    "relative_path|with?invalid",
    "/usr/local/bin/file.txt",
    "C:/Users/no_folder_file.doc"
};

std::vector<std::string> test_filepath_valid =
{
    "/home/user/document.txt",
    "C:\\Users\\file.doc",
    "C:\\Users\\files\\satan\\file.doc",
    "/usr/local/bin/unix_file",
    "relative_file.txt",
    "/only_filename"
};

std::vector<std::string> test_filepath_invalid =
{
    "C:\\Windows\\",
    "/var/log/",
    "/invalid|file.txt",
    "C:\\invalid:name.doc",
    "C:\\aux\\files/satan file"
};

std::vector<std::string> invalid_filenames =
{
    "inv|alid.txt",
    "inva:lid.doc",
    "inv<>alid.exe",
    "inva*lid?.pdf"
};

std::vector<std::string> test_windows_paths =
{
    "C:\\Users\\files\\document.txt",
    "\\Users\\files\\document.txt",
    "Users\\files\\document.txt",
    "\\Users\\files",
    "C:/Users/files/document.txt",
    "/home/user/file.txt",
    "relative/path/to/file",
    "D:\\Folder\\Subfolder\\file.exe"
};

std::vector<std::string> test_paths_drive_letters =
{
    "C:\\Users\\Documents\\file.txt",
    "D:/Projects/",
    "X:/Games/",
    "/home/user/file.txt",
    "relative/path/to/file",
    "E:",
    "Z:\\"
};

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Main entry point of the program `ExampleFilesystemUtils`.
 */
int main()
{
    // Initial log.
    std::cout << strings::generateExampleTitle("Example FilesystemUtils");

    // ----------------------------------------------------------------------------------------------------
    // Retrieving System Directories

    std::cout << "[INFO] Retrieving system directories..." << std::endl;
    std::cout << "[SUCCESS] Current Working Directory: " << files::getCurrentDir() << std::endl;
    std::cout << "[SUCCESS] Temporary Directory: " << files::getTempDir() << std::endl;
    std::cout << "[SUCCESS] OS Root Directory: " << files::getOsRootDir() << std::endl << std::endl;

    // ----------------------------------------------------------------------------------------------------
    // Testing getFilenameFromPath().

    std::cout << "[INFO] Extracting filenames from valid file paths..." << std::endl;
    for (const auto& path : test_filepath_valid)
    {
        std::cout << "  - Path: " << path
                  << " > Filename: \"" << files::getFilenameFromFilepath(path) << "\"" << std::endl;
    }
    std::cout << std::endl;

    std::cout << "[INFO] Extracting filenames (without extension)..." << std::endl;
    for (const auto& path : test_filepath_valid)
    {
        std::cout << "  - Path: " << path
                  << " > Filename (no extension): \"" << files::getFilenameFromFilepath(path, false) << "\"" << std::endl;
    }
    std::cout << std::endl;

    // ----------------------------------------------------------------------------------------------------
    // Testing getPathFromFilepath()

    std::cout << "[INFO] Extracting directory paths from valid file paths..." << std::endl;
    for (const auto& path : test_filepath_valid)
    {
        std::cout << "  - Path: " << path
                  << " > Directory: \"" << files::getPathFromFilepath(path) << "\"" << std::endl;
    }
    std::cout << std::endl;

    // ----------------------------------------------------------------------------------------------------
    // Testing getPathFromFilepath()

    std::cout << "[INFO] Extracting extensionss from valid file paths..." << std::endl;
    for (const auto& path : test_filepath_valid)
    {
        std::cout << "  - Path: " << path
                  << " > Directory: \"" << files::getFileExtension(path) << "\"" << std::endl;
    }
    std::cout << std::endl;

    // ----------------------------------------------------------------------------------------------------
    // Testing isValidFilepath()

    std::cout << "[INFO] Validating valid file paths..." << std::endl;
    for (const auto& path : test_filepath_valid)
    {
        std::cout << "  - Path: " << path
                  << " > Valid: " << (files::isValidFilepath(path) ? "Yes" : "No") << std::endl;
    }
    std::cout << std::endl;

    std::cout << "[INFO] Validating invalid file paths..." << std::endl;
    for (const auto& path : test_filepath_invalid)
    {
        std::cout << "  - Path: " << path
                  << " > Valid: " << (files::isValidFilepath(path) ? "Yes" : "No") << std::endl;
    }
    std::cout << std::endl;

    // ----------------------------------------------------------------------------------------------------
    // Testing isValidPath()

    std::cout << "[INFO] Validating valid directory paths..." << std::endl;
    for (const auto& path : test_dirs_valid)
    {
        std::cout << "  - Path: " << path
                  << " > Valid: " << (files::isValidDir(path) ? "Yes" : "No") << std::endl;
    }
    std::cout << std::endl;

    std::cout << "[INFO] Validating invalid directory paths..." << std::endl;
    for (const auto& path : test_dirs_invalid)
    {
        std::cout << "  - Path: " << path
                  << " > Valid: " << (files::isValidDir(path) ? "Yes" : "No") << std::endl;
    }
    std::cout << std::endl;


    // ----------------------------------------------------------------------------------------------------
    // Testing sanitizeFilename().

    std::cout << "[INFO] Sanitizing filenames..." << std::endl;

    for (const auto& filename : invalid_filenames)
    {
        std::string sanitized = files::sanitizeFilename(filename, "_");
        std::cout << "  - Original: " << filename << " > Sanitized: " << sanitized << std::endl;
    }
    std::cout << std::endl;

    // ----------------------------------------------------------------------------------------------------
    // Testing Windows Path Detection

    std::cout << "[INFO] Testing Windows path detection..." << std::endl;

    for (const auto& path : test_windows_paths)
    {
        std::cout << "  - Path: " << path << " > Windows Format: "
                  << (files::isWindowsPath(path) ? "Yes" : "No") << std::endl;
    }
    std::cout << std::endl;

    // ----------------------------------------------------------------------------------------------------
    // Testing Windows Path Conversion

    std::cout << "[INFO] Converting Windows paths to Unix format..." << std::endl;
    for (const auto& path : test_windows_paths)
    {
        std::string converted = files::convertWindowsPathToUnix(path);
        std::cout << "  - Original: " << path << " > Unix Path: " << converted << std::endl;
    }
    std::cout << std::endl;

    // ----------------------------------------------------------------------------------------------------
    // Testing Windows Drive detection.

    std::cout << "[INFO] Get the drive letters..." << std::endl;

    for (const auto& path : test_paths_drive_letters)
    {
        std::cout << "  - Path: " << path
                  << " | Starts with drive: " << (files::startsWithWindowsDrive(path) ? "Yes" : "No")
                  << " | Drive Extracted: " << files::getWindowsDriveFromPath(path) << std::endl;
    }
    std::cout << std::endl;

    // ----------------------------------------------------------------------------------------------------
    // Testing create folders.

    std::cout << "[INFO] Checking if directories exist before creation..." << std::endl;
    std::cout << "  - milethos/testing/data/: " <<
        (files::directoryExists("milethos/testing/data/") ? "Exists" : "Missing") << std::endl;
    std::cout << "  - angel/music/albums/:   "
              << (files::directoryExists("angel/music/albums/") ? "Exists" : "Missing") << std::endl;

    std::cout << "[ACTION] Creating directories..." << std::endl;
    files::createDirectory("milethos/testing/data/");
    files::createDirectory("angel/music/albums/");

    std::cout << "[RESULT] Directory existence after creation:" << std::endl;
    std::cout << "  - milethos/testing/data/: "
              << (files::directoryExists("milethos/testing/data/") ? "Exists" : "Missing") << std::endl;
    std::cout << "  - angel/music/albums/:   "
              << (files::directoryExists("angel/music/albums/") ? "Exists" : "Missing") << std::endl;
    std::cout << std::endl;

    // ----------------------------------------------------------------------------------------------------
    // Create a dummy file to test copy

    std::string dummyPath = "milethos/testing/data/testfile.txt";
    std::ofstream dummyFile(dummyPath);
    dummyFile << "Milethos test file\n";
    dummyFile.close();

    std::cout << "[INFO] Created dummy file: " << dummyPath << std::endl;
    std::cout << "  - File exists? " << (files::fileExists(dummyPath) ? "Yes" : "No") << std::endl;
    std::cout << std::endl;


    // ----------------------------------------------------------------------------------------------------
    // Create a dummy file to test delete

    std::string dummyPath2 = "milethos/testing/data/testfile_delete.txt";
    std::ofstream dummyFile2(dummyPath2);
    dummyFile2 << "Milethos test file 2\n";
    dummyFile2.close();

    std::cout << "[INFO] Created dummy file: " << dummyPath2 << std::endl;
    std::cout << "  - File exists? " << (files::fileExists(dummyPath2) ? "Yes" : "No") << std::endl;
    std::cout << std::endl;

    files::deleteFile(dummyPath2);

    std::cout << "[INFO] Deleted dummy file: " << dummyPath2 << std::endl;
    std::cout << "  - File exists? " << (files::fileExists(dummyPath2) ? "Yes" : "No") << std::endl;
    std::cout << std::endl;

    // ----------------------------------------------------------------------------------------------------
    // Testing copy folders.

    std::cout << "[ACTION] Copying 'milethos' to 'milethos_backup'..." << std::endl;
    files::copyDirectory("milethos", "milethos_backup");

    std::cout << "[RESULT] Checking copied file: milethos_backup/testing/data/testfile.txt..." << std::endl;
    std::cout << "  - File exists? "
              << (files::fileExists("milethos_backup/testing/data/testfile.txt") ? "Yes" : "No") << std::endl;
    std::cout << std::endl;

    // ----------------------------------------------------------------------------------------------------
    // Testing delete folders.

    std::cout << "[ACTION] Deleting all test folders..." << std::endl;
    files::deleteDirectory("milethos");
    files::deleteDirectory("angel/music/");
    files::deleteDirectory("angel/");
    files::deleteDirectory("milethos_backup");

    std::cout << "[RESULT] Post-deletion check:" << std::endl;
    std::cout << "  - milethos/ exists? " << (files::directoryExists("milethos") ? "Yes" : "No") << std::endl;
    std::cout << "  - angel/ exists?    " << (files::directoryExists("angel") ? "Yes" : "No") << std::endl;
    std::cout << "  - milethos_backup/ exists? " << (files::directoryExists("milethos_backup") ? "Yes" : "No") << std::endl;
    std::cout << std::endl;

    // ----------------------------------------------------------------------------------------------------
    // Testing createEmptyFile function.

    std::cout << "[ACTION] Creating test files..." << std::endl;

    // These should trigger folder creation too
    files::createEmptyFile("milethos/test1.txt");
    files::createEmptyFile("angel/music/song.txt");
    files::createEmptyFile("milethos_backup/notes/readme.md");

    // This should not create anything because the file already exists
    bool result = files::createEmptyFile("milethos/test1.txt");
    std::cout << "[INFO] Attempt to re-create 'milethos/test1.txt' returned: " << (result ? "Created" : "Skipped") << std::endl;

    std::cout << "[RESULT] Post-creation check:" << std::endl;
    std::cout << "  - milethos/ exists? " << (files::directoryExists("milethos") ? "Yes" : "No") << std::endl;
    std::cout << "  - angel/ exists?    " << (files::directoryExists("angel") ? "Yes" : "No") << std::endl;
    std::cout << "  - milethos_backup/ exists? " << (files::directoryExists("milethos_backup") ? "Yes" : "No") << std::endl;
    std::cout << "  - milethos/test1.txt exists? " << (files::fileExists("milethos/test1.txt") ? "Yes" : "No") << std::endl;
    std::cout << "  - angel/music/song.txt exists? " << (files::fileExists("angel/music/song.txt") ? "Yes" : "No") << std::endl;
    std::cout << "  - milethos_backup/notes/readme.md exists? " << (files::fileExists("milethos_backup/notes/readme.md") ? "Yes" : "No") << std::endl;
    std::cout << std::endl;

    // ----------------------------------------------------------------------------------------------------
    // Cleanup: Delete created files

    std::cout << "[ACTION] Cleaning up test files..." << std::endl;
    files::deleteFile("milethos/test1.txt");
    files::deleteFile("angel/music/song.txt");
    files::deleteFile("milethos_backup/notes/readme.md");

    // Optionally remove empty directories
    files::deleteDirectory("milethos");
    files::deleteDirectory("angel/music/");
    files::deleteDirectory("angel/");
    files::deleteDirectory("milethos_backup");

    std::cout << "[RESULT] Post-deletion check:" << std::endl;
    std::cout << "  - milethos/ exists? " << (files::directoryExists("milethos") ? "Yes" : "No") << std::endl;
    std::cout << "  - angel/ exists?    " << (files::directoryExists("angel") ? "Yes" : "No") << std::endl;
    std::cout << "  - milethos_backup/ exists? " << (files::directoryExists("milethos_backup") ? "Yes" : "No") << std::endl;
    std::cout << std::endl;

    //----------------------------------------------------------------------------------------------------
    // Final log.
    std::cout << strings::generateExampleEnd();
    leobase::system::utils::userInputPause("Press <ENTER> to exit...");

    // Final return.
    return 0;
}

// ---------------------------------------------------------------------------------------------------------------------
