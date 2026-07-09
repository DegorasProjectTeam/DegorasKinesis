/*
 *      Copyright(C) Milethos Technologies SLU. 2025
 *      All rights reserved. Reproduction in whole or in
 *      part is prohibited without the written consent of
 *      the copyright holder.
 */

// C++ INCLUDES
#include <iostream>

// LEOBASE INCLUDES
#include <LeoBase/Aliases/System>
#include <LeoBase/Aliases/Helpers>

// ---------------------------------------------------------------------------------------------------------------------

// LeoBase namespaces.
using namespace leobase::helpers;
using namespace leobase::system;
using namespace leobase::system::files;
using namespace leobase::system::types;

// ---------------------------------------------------------------------------------------------------------------------


// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Main entry point of the program `ExampleFilesystemUtils`.
 */
int main()
{    
    // Initial log.
    std::cout << strings::generateExampleTitle("Example FilesystemFolderTree");

    // ----------------------------------------------------------------------------------------------------
    // Basic Folder Structure Creation.

    // RootFolderTreePtr tree_1 = RootFolderTree::makeFromParentPath("home");

    // std::cout << "Before adding folders:\n" << tree_1->listContents() << std::endl;

    // tree_1->addSubfolder("home/visualizer", BasicFolderInfo{"visualizer"});
    // tree_1->addSubfolder("home/visualizer/angel", BasicFolderInfo{"angel"});
    // tree_1->addSubfolder("home/visualizer/angel/Mision", BasicFolderInfo{"Mision"});
    // tree_1->addSubfolder("home/visualizer/angel/Mission_123008_260620230735", BasicFolderInfo{"Mission_123008_260620230735"});
    // tree_1->addSubfolder("home/visualizer/angel/Angelito's Random Mix v8 - MP3", BasicFolderInfo{"Angelito's Random Mix v8 - MP3"});

    // std::cout << "After adding folders:\n" << tree_1->listContents() << std::endl;

    // tree_1->getSubfolder("home/visualizer/angel")->addFile({"resume.pdf", "rw-r--r--", "user", "group", "", 1024});
    // tree_1->getSubfolder("home/visualizer/angel/Angelito's Random Mix v8 - MP3")->addFile({"fiesta pagana.mp3", "rw-r--r--", "angel", "root", "", 2048});

    // std::cout << "After adding files:\n" << tree_1->listContents() << std::endl;

    // ----------------------------------------------------------------------------------------------------
    // Basic Folder Structure Creation 2.

    // RootFolderTreePtr tree_2 = RootFolderTree::makeFromParentPath("home/milethos/manu");

    // std::cout << "Before adding folders:\n" << tree_2->listContents() << std::endl;

    // tree_2->addSubfolder("home/milethos/manu/Mision", BasicFolderInfo{"Mision"});
    // tree_2->addSubfolder("home/milethos/manu/Mission_123008_260620230735", BasicFolderInfo{"Mission_123008_260620230735"});
    // tree_2->addSubfolder("home/milethos/manu/Angelito's Random Mix v8 - MP3", BasicFolderInfo{"Angelito's Random Mix v8 - MP3"});

    // std::cout << "After adding folders:\n" << tree_2->listContents() << std::endl;

    // tree_2->getSubfolder("home/milethos/manu")->addFile({"resume.pdf", "rw-r--r--", "user", "group", "", 1024});
    // tree_2->getSubfolder("home/milethos/manu/Angelito's Random Mix v8 - MP3")->addFile({"fiesta pagana.mp3", "rw-r--r--", "angel", "root", "", 2048});

    // std::cout << "After adding files:\n" << tree_2->listContents() << std::endl;


    // ----------------------------------------------------------------------------------------------------
    // Example: Basic Folder Structure Creation.

    // ParentFolderTree root_folder("RootFolder");

    // // Adding basic folders and files
    // root_folder.addSubfolder("Documents", {"Documents"});
    // root_folder.addSubfolder("Music", {"Music"});
    // root_folder.addFile({"readme.txt", "rw-r--r--", "user", "group", "", 1024});
    // std::cout << "Basic Folder Structure:\n" << root_folder.listContents() << std::endl;


    // ----------------------------------------------------------------------------------------------------
    // Example: Nested Folder Creation.

    // Adding nested subfolders
    // FolderTreePtr documents = root_folder.getSubfolder("Documents");
    // if (documents)
    // {
    //     documents->addSubfolder("Projects", {"Projects"});
    //     documents->addSubfolder("Personal", {"Personal"});
    //     documents->addSubfolder("Milethos/Data/Randon Project", {"Randon Project"});
    //     documents->addFile({"resume.pdf", "rw-r--r--", "user", "group", "", 2048});
    //     root_folder.getSubfolder("Documents/Milethos/Data/Randon Project")->addFile({"test_project.cpp"});
    // }
    // else
    // {
    //     std::cout << "Null folder!" << std::endl;
    //     return 1;
    // }

    // // Output tree structure
    // std::cout << "Nested Folder Structure:\n" << root_folder.listContents() << std::endl;

    // ----------------------------------------------------------------------------------------------------
    // Example: Navigating Up the Tree (Finding Parent).

    // // FolderTreePtr cppFolder = root_folder.getSubfolder("Documents/Projects/C++");
    // // if (cppFolder)
    // // {
    // //     std::cout << "C++ Folder Parent: " << cppFolder->getParent()->folderInfo_.folderName << std::endl;
    // // }

    // ----------------------------------------------------------------------------------------------------
    // Example: Handling Files at Multiple Levels.

    // root_folder.addFile({"logfile.log", "rw-r--r--", "user", "group", "", 5120});
    // FolderTreePtr musicFolder = root_folder.getSubfolder("Music");
    // if (musicFolder)
    // {
    //     musicFolder->addFile({"song.mp3", "rw-r--r--", "user", "group", "", 8192});
    // }

    // ----------------------------------------------------------------------------------------------------
    // Example: Merge two folder tree.

    // RootFolderTreePtr tree_1 = RootFolderTree::makeFromParentPath("home");

    // tree_1->addSubfolder("home/visualizer", BasicFolderInfo{"visualizer"});
    // tree_1->addSubfolder("home/visualizer/angel", BasicFolderInfo{"angel"});
    // tree_1->addSubfolder("home/visualizer/angel/Mision", BasicFolderInfo{"Mision"});
    // tree_1->addSubfolder("home/visualizer/angel/Mision/Mission_123008", BasicFolderInfo{"Mission_123008"});
    // tree_1->addSubfolder("home/visualizer/angel/Mision/Mission_123008/Leg_1", BasicFolderInfo{"Leg_1"});
    // tree_1->addSubfolder("home/visualizer/angel/Mision/Mission_123008/Leg_2", BasicFolderInfo{"Leg_2"});
    // tree_1->addSubfolder("home/visualizer/angel/Mision/Mission_123008/Leg_3", BasicFolderInfo{"Leg_3"});

    // tree_1->getSubfolder("home/visualizer/angel/Mision/Mission_123008/Leg_1")->addFile({"file_1.xml", "rw-r--r--", "user", "group", "", 1024});
    // tree_1->getSubfolder("home/visualizer/angel/Mision/Mission_123008/Leg_1")->addFile({"file_2.xml", "rw-r--r--", "user", "group", "", 1024});

    // tree_1->getSubfolder("home/visualizer/angel/Mision/Mission_123008/Leg_2")->addFile({"file_3.xml", "rw-r--r--", "user", "group", "", 1024});

    // tree_1->getSubfolder("home/visualizer/angel/Mision/Mission_123008/Leg_3")->addFile({"file_4.xml", "rw-r--r--", "user", "group", "", 1024});

    // std::cout << "Tree 1:\n" << tree_1->listContents() << std::endl;

    // RootFolderTreePtr tree_2 = RootFolderTree::makeFromParentPath("home");

    // tree_2->addSubfolder("home/pepito", BasicFolderInfo{"pepito"});
    // tree_2->addSubfolder("home/pepito/angel", BasicFolderInfo{"angel"});
    // tree_2->addSubfolder("home/pepito/angel/Mision", BasicFolderInfo{"Mision"});
    // tree_2->addSubfolder("home/pepito/angel/Mision/Mission_123008", BasicFolderInfo{"Mission_123008"});
    // tree_2->addSubfolder("home/pepito/angel/Mision/Mission_123008/Leg_1", BasicFolderInfo{"Leg_1"});
    // tree_2->addSubfolder("home/pepito/angel/Mision/Mission_123008/Leg_3", BasicFolderInfo{"Leg_3"});

    // tree_2->getSubfolder("home/pepito/angel/Mision/Mission_123008/Leg_1")->addFile({"file_1.tif", "rw-r--r--", "user", "group", "", 1024});
    // tree_2->getSubfolder("home/pepito/angel/Mision/Mission_123008/Leg_1")->addFile({"file_2.tif", "rw-r--r--", "user", "group", "", 1024});

    // tree_2->getSubfolder("home/pepito/angel/Mision/Mission_123008/Leg_3")->addFile({"file_4.tif", "rw-r--r--", "user", "group", "", 1024});

    // std::cout << "Tree 2:\n" << tree_2->listContents() << std::endl;

    // auto subfolder_1 = tree_1->getSubfolder("home/visualizer/angel/Mision/Mission_123008");
    // auto subfolder_2 = tree_2->getSubfolder("home/pepito/angel/Mision/Mission_123008");

    // std::cout << "Get subfolder 1:\n" << subfolder_1->listContents() << std::endl;
    // std::cout << "Get subfolder 2:\n" << subfolder_2->listContents() << std::endl;

    // std::cout << "Subfolder 1 size: " << subfolder_1->folderInfo_.sizeB << std::endl;
    // std::cout << "Subfolder 2 size: " << subfolder_2->folderInfo_.sizeB << std::endl;

    // std::cout << "Merge subfolder tree." << std::endl;
    // subfolder_1->merge(subfolder_2);

    // std::cout << "Subfolder 1 after merge:\n" << subfolder_1->listContents() << std::endl;
    // std::cout << "Subfolder size after merger: " << subfolder_1->folderInfo_.sizeB << std::endl;

    // std::cout << "Files size: " << subfolder_1->filesInfoV_.size() << std::endl;

    // for (auto& it : subfolder_1->getAllSubfoldersFiles())
    // {
    //     std::cout << "File info: " << it.filePath << std::endl;
    // }

    // ----------------------------------------------------------------------------------------------------
    // List directory recursively.
    std::string local_dir = "F:/workspace/test_copy_remote/interpretation/Mission_125001_300120241020/";

    std::cout << "Creating root folder tree recursively to: " << local_dir << std::endl;

    auto root_folder_tree = RootFolderTree::makeRecursivelyFromParentPath(local_dir);

    std::cout << "Root folder tree contents: \n" << root_folder_tree->listContents() << std::endl;

    //----------------------------------------------------------------------------------------------------
    // Final log.
    std::cout << strings::generateExampleEnd();
    leobase::system::utils::userInputPause("Press <ENTER> to exit...");

    // Final return.
    return 0;
}

// ---------------------------------------------------------------------------------------------------------------------
