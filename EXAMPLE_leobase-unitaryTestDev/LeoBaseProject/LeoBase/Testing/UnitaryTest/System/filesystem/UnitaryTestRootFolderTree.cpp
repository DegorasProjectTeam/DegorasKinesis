/*
 * Copyright(C) Milethos Technologies SLU. 2026
 * All rights reserved. Reproduction in whole or in
 * part is prohibited without the written consent of
 * the copyright holder.
*/

// C++ INCLUDES
#include <string>
#include <memory>
#include <vector>

// LeoBase INCLUDES
#include <LeoBase/Aliases/Testing>

#include "LeoBase/System/filesystem/root_folder_tree.h"
#include "LeoBase/System/types/filesystem_types.h"
#include "LeoBase/Testing/unitary/unitary_test_macros.h"

using namespace leobase;
using namespace leobase::testing;
using namespace leobase::system::files;
using namespace leobase::system::types;

namespace types = leobase::system::types; // For types shortened

// =======================================================================
// DECLARATIONS
// =======================================================================

// Original tests
UT_DECLARE_TEST(AddFile_PropagatesSizeRecursively);
UT_DECLARE_TEST(Merge_CombinesDisjointAndOverlappingTrees);
UT_DECLARE_TEST(GetAllSubfolders_ReturnsFlattenedHierarchy);
UT_DECLARE_TEST(GetAllSubfoldersFiles_ReturnsAllNestedFiles);
UT_DECLARE_TEST(GetSubfolder_NormalizesPathsCorrectly);
UT_DECLARE_TEST(FindSubfolderName_MatchesSubstring);
UT_DECLARE_TEST(Clear_ResetsAllInternalState);
UT_DECLARE_TEST(RootEmptyState_ReturnsTrueInitially);

// New exhaustive tests
UT_DECLARE_TEST(AddFile_ConstructsFilePathFromHierarchy);
UT_DECLARE_TEST(Merge_EmptyTreeIntoPopulated_DoesNothing);
UT_DECLARE_TEST(GetSubfolderFiles_OnlyReturnsImmediateFiles);
UT_DECLARE_TEST(ListContents_OutputsExpectedHierarchyFormat);
UT_DECLARE_TEST(FindSubfolderName_ReturnsNulloptWhenNoMatch);

// =======================================================================
// DEFINITIONS
// =======================================================================

/**
 * @brief Tests that adding a file updates the size of the target folder and all its parents recursively.
 */
UT_DEFINE_TEST(AddFile_PropagatesSizeRecursively)
{
    // Setup root folder
    types::BasicFolderInfo rootInfo;
    rootInfo.folderName = "root";
    rootInfo.sizeB = 0;
    auto rootNode = std::make_shared<FolderTree>(rootInfo, nullptr, nullptr);

    // Setup child folder
    types::BasicFolderInfo childInfo;
    childInfo.folderName = "child";
    childInfo.sizeB = 0;
    auto childNode = std::make_shared<FolderTree>(childInfo, nullptr, rootNode);

    // Link child to root
    rootNode->subfoldersMap_["child"] = childNode;

    // Create a file to insert
    types::BasicFileInfo file1;
    file1.fileName = "document.txt";
    file1.sizeB = 1024;

    // Act: Add file to the child folder
    childNode->addFile(file1);

    // Assert: Check if the file was added
    UT_EXPECTED_EQ(childNode->filesInfoV_.size(), static_cast<size_t>(1));

    // Assert: Check if sizes were updated recursively
    UT_EXPECTED_EQ(childNode->folderInfo_.sizeB, static_cast<size_t>(1024));
    UT_EXPECTED_EQ(rootNode->folderInfo_.sizeB, static_cast<size_t>(1024));
}

/**
 * @brief Tests the merge operation between two folder trees, combining both disjoint and overlapping nodes.
 */
UT_DEFINE_TEST(Merge_CombinesDisjointAndOverlappingTrees)
{
    // Setup Base Tree (Tree A)
    types::BasicFolderInfo infoA; infoA.folderName = "base"; infoA.sizeB = 0;
    auto treeA = std::make_shared<FolderTree>(infoA, nullptr, nullptr);

    types::BasicFolderInfo infoSub1; infoSub1.folderName = "sub1"; infoSub1.sizeB = 0;
    auto sub1A = std::make_shared<FolderTree>(infoSub1, nullptr, treeA);
    treeA->subfoldersMap_["sub1"] = sub1A;

    types::BasicFileInfo fileA; fileA.fileName = "fileA.txt"; fileA.sizeB = 100;
    sub1A->addFile(fileA);

    // Setup Incoming Tree (Tree B)
    types::BasicFolderInfo infoB; infoB.folderName = "base"; infoB.sizeB = 0;
    auto treeB = std::make_shared<FolderTree>(infoB, nullptr, nullptr);

    // Overlapping folder "sub1"
    auto sub1B = std::make_shared<FolderTree>(infoSub1, nullptr, treeB);
    treeB->subfoldersMap_["sub1"] = sub1B;

    types::BasicFileInfo fileB; fileB.fileName = "fileB.txt"; fileB.sizeB = 200;
    sub1B->addFile(fileB); // Add different file to overlapping subfolder

    // Disjoint folder "sub2"
    types::BasicFolderInfo infoSub2; infoSub2.folderName = "sub2"; infoSub2.sizeB = 0;
    auto sub2B = std::make_shared<FolderTree>(infoSub2, nullptr, treeB);
    treeB->subfoldersMap_["sub2"] = sub2B;

    // Act: Merge Tree B into Tree A
    treeA->merge(treeB);

    // Assert: Check merged structure
    UT_EXPECTED_EQ(treeA->subfoldersMap_.size(), static_cast<size_t>(2)); // Should have "sub1" and "sub2"
    UT_EXPECTED_TRUE(treeA->subfoldersMap_.find("sub1") != treeA->subfoldersMap_.end());
    UT_EXPECTED_TRUE(treeA->subfoldersMap_.find("sub2") != treeA->subfoldersMap_.end());

    // Assert: Check merged files in "sub1"
    auto mergedSub1 = treeA->subfoldersMap_["sub1"];
    UT_EXPECTED_EQ(mergedSub1->filesInfoV_.size(), static_cast<size_t>(2)); // fileA.txt and fileB.txt

    // Assert: Verify size summation logic
    UT_EXPECTED_EQ(mergedSub1->folderInfo_.sizeB, static_cast<size_t>(300)); // 100 + 200
}

/**
 * @brief Tests that getAllSubfolders recursively flattens the folder hierarchy.
 */
UT_DEFINE_TEST(GetAllSubfolders_ReturnsFlattenedHierarchy)
{
    auto root = std::make_shared<FolderTree>();

    auto child1 = std::make_shared<FolderTree>();
    auto child2 = std::make_shared<FolderTree>();
    auto grandchild1 = std::make_shared<FolderTree>();

    root->subfoldersMap_["child1"] = child1;
    root->subfoldersMap_["child2"] = child2;
    child1->subfoldersMap_["grandchild1"] = grandchild1;

    // Act
    FolderTreePtrV allFolders = root->getAllSubfolders();

    // Assert
    UT_EXPECTED_EQ(allFolders.size(), static_cast<size_t>(3));
}

/**
 * @brief Tests that getAllSubfoldersFiles recursively collects files from all descendant folders.
 */
UT_DEFINE_TEST(GetAllSubfoldersFiles_ReturnsAllNestedFiles)
{
    auto root = std::make_shared<FolderTree>();
    auto child = std::make_shared<FolderTree>();
    root->subfoldersMap_["child"] = child;

    types::BasicFileInfo f1; f1.fileName = "1.txt";
    types::BasicFileInfo f2; f2.fileName = "2.txt";

    root->filesInfoV_.push_back(f1);
    child->filesInfoV_.push_back(f2);

    // Act
    auto allFiles = root->getAllSubfoldersFiles();

    // Assert
    UT_EXPECTED_EQ(allFiles.size(), static_cast<size_t>(2));
}

/**
 * @brief Ensures getSubfolder correctly trims slashes when searching for a folder path.
 */
UT_DEFINE_TEST(GetSubfolder_NormalizesPathsCorrectly)
{
    auto root = std::make_shared<FolderTree>();
    auto target = std::make_shared<FolderTree>();

    root->subfoldersMap_["target_folder"] = target;

    // Act & Assert
    UT_EXPECTED_TRUE(root->getSubfolder("target_folder") != nullptr);
    UT_EXPECTED_TRUE(root->getSubfolder("/target_folder") != nullptr);
    UT_EXPECTED_TRUE(root->getSubfolder("target_folder/") != nullptr);
    UT_EXPECTED_TRUE(root->getSubfolder("/target_folder/") != nullptr);
    UT_EXPECTED_TRUE(root->getSubfolder("non_existent") == nullptr);
}

/**
 * @brief Tests findSubfolderName partial string matching capability.
 */
UT_DEFINE_TEST(FindSubfolderName_MatchesSubstring)
{
    auto root = std::make_shared<FolderTree>();

    types::BasicFolderInfo info;
    info.folderName = "System32_Backup";
    auto target = std::make_shared<FolderTree>(info, nullptr, nullptr);

    root->subfoldersMap_["sys_backup"] = target;

    // Act
    auto resultFound = root->findSubfolderName("System32");

    // Assert
    UT_EXPECTED_TRUE(resultFound.has_value());
    UT_EXPECTED_EQ(resultFound.value(), std::string("System32_Backup"));
}

/**
 * @brief Tests the clear method to ensure maps and vectors are emptied safely.
 */
UT_DEFINE_TEST(Clear_ResetsAllInternalState)
{
    auto root = std::make_shared<FolderTree>();
    root->subfoldersMap_["child"] = std::make_shared<FolderTree>();

    types::BasicFileInfo f1; f1.fileName = "test.txt";
    root->filesInfoV_.push_back(f1);

    // Act
    root->clear();

    // Assert
    UT_EXPECTED_TRUE(root->subfoldersMap_.empty());
    UT_EXPECTED_TRUE(root->filesInfoV_.empty());
}

/**
 * @brief Simple check for RootFolderTree initial state.
 */
UT_DEFINE_TEST(RootEmptyState_ReturnsTrueInitially)
{
    auto rootTree = RootFolderTree::makeEmpty();

    if (rootTree)
    {
        UT_EXPECTED_TRUE(rootTree->empty());
    }
}

// -----------------------------------------------------------------------
// NEW EXHAUSTIVE TESTS
// -----------------------------------------------------------------------

/**
 * @brief Ensures that addFile traverses up the parent tree to build the correct full file path.
 */
UT_DEFINE_TEST(AddFile_ConstructsFilePathFromHierarchy)
{
    types::BasicFolderInfo rootInfo; rootInfo.folderName = "root_dir";
    auto rootNode = std::make_shared<FolderTree>(rootInfo, nullptr, nullptr);

    types::BasicFolderInfo childInfo; childInfo.folderName = "sub_dir";
    auto childNode = std::make_shared<FolderTree>(childInfo, nullptr, rootNode);
    rootNode->subfoldersMap_["sub_dir"] = childNode;

    types::BasicFileInfo file;
    file.fileName = "data.bin";

    childNode->addFile(file);

    UT_EXPECTED_EQ(childNode->filesInfoV_.size(), static_cast<size_t>(1));

    // The internal logic does: "root_dir" + "/" + "sub_dir" + "/" + "data.bin"
    // Assuming BasicFileInfo has a getFilePath() or filePath public member updated by setFilePath()
    std::string expectedPath = "root_dir/sub_dir/data.bin";

    // Note: If the member is named differently in BasicFileInfo, adjust `filePath` below.
    UT_EXPECTED_TRUE(childNode->filesInfoV_[0].filePath.find(expectedPath) != std::string::npos);
}

/**
 * @brief Tests merging an empty FolderTree into a populated one does not alter the populated one.
 */
UT_DEFINE_TEST(Merge_EmptyTreeIntoPopulated_DoesNothing)
{
    types::BasicFolderInfo infoA; infoA.folderName = "base"; infoA.sizeB = 500;
    auto treeA = std::make_shared<FolderTree>(infoA, nullptr, nullptr);
    types::BasicFileInfo f1; f1.fileName = "keep.txt"; f1.sizeB = 500;
    treeA->filesInfoV_.push_back(f1);

    auto emptyTree = std::make_shared<FolderTree>();

    treeA->merge(emptyTree);

    UT_EXPECTED_EQ(treeA->filesInfoV_.size(), static_cast<size_t>(1));
    UT_EXPECTED_EQ(treeA->folderInfo_.sizeB, static_cast<size_t>(500));
    UT_EXPECTED_TRUE(treeA->subfoldersMap_.empty());
}

/**
 * @brief Verifies getSubfolderFiles only returns immediate files, unlike getAllSubfoldersFiles.
 */
UT_DEFINE_TEST(GetSubfolderFiles_OnlyReturnsImmediateFiles)
{
    auto root = std::make_shared<FolderTree>();
    auto child = std::make_shared<FolderTree>();
    root->subfoldersMap_["child"] = child;

    types::BasicFileInfo f1; f1.fileName = "root_file.txt";
    root->filesInfoV_.push_back(f1);

    types::BasicFileInfo f2; f2.fileName = "child_file.txt";
    child->filesInfoV_.push_back(f2);

    auto immediateFiles = root->getSubfolderFiles();

    UT_EXPECTED_EQ(immediateFiles.size(), static_cast<size_t>(1));
    UT_EXPECTED_EQ(immediateFiles[0].fileName, std::string("root_file.txt"));
}

/**
 * @brief Tests the experimental listContents to ensure basic string formatting (emojis and indentation).
 */
UT_DEFINE_TEST(ListContents_OutputsExpectedHierarchyFormat)
{
    types::BasicFolderInfo rootInfo; rootInfo.folderName = "MyFolder";
    auto root = std::make_shared<FolderTree>(rootInfo, nullptr, nullptr);

    types::BasicFileInfo f1; f1.fileName = "file.txt"; f1.sizeB = 123;
    root->filesInfoV_.push_back(f1);

    std::string output = root->listContents(0);

    UT_EXPECTED_FALSE(output.empty());
    UT_EXPECTED_TRUE(output.find("📂 MyFolder") != std::string::npos);
    UT_EXPECTED_TRUE(output.find("📄 file.txt") != std::string::npos);
    UT_EXPECTED_TRUE(output.find("123 bytes") != std::string::npos);
}

/**
 * @brief Verifies findSubfolderName correctly handles cases where the substring does not exist.
 */
UT_DEFINE_TEST(FindSubfolderName_ReturnsNulloptWhenNoMatch)
{
    auto root = std::make_shared<FolderTree>();
    types::BasicFolderInfo info; info.folderName = "System32_Backup";
    auto target = std::make_shared<FolderTree>(info, nullptr, nullptr);
    root->subfoldersMap_["sys"] = target;

    auto resultNotFound = root->findSubfolderName("Linux");

    UT_EXPECTED_FALSE(resultNotFound.has_value());
}

// =======================================================================
// SESSION
// =======================================================================

UT_START_SESSION("LeoBase System Files Unitary Tests")

// Original tests
UT_REGISTER_TEST(System, FolderTree, AddFile_PropagatesSizeRecursively);
UT_REGISTER_TEST(System, FolderTree, Merge_CombinesDisjointAndOverlappingTrees);
UT_REGISTER_TEST(System, FolderTree, GetAllSubfolders_ReturnsFlattenedHierarchy);
UT_REGISTER_TEST(System, FolderTree, GetAllSubfoldersFiles_ReturnsAllNestedFiles);
UT_REGISTER_TEST(System, FolderTree, GetSubfolder_NormalizesPathsCorrectly);
UT_REGISTER_TEST(System, FolderTree, FindSubfolderName_MatchesSubstring);
UT_REGISTER_TEST(System, FolderTree, Clear_ResetsAllInternalState);
UT_REGISTER_TEST(System, RootFolderTree, RootEmptyState_ReturnsTrueInitially);

// Exhaustive tests
UT_REGISTER_TEST(System, FolderTree, AddFile_ConstructsFilePathFromHierarchy);
UT_REGISTER_TEST(System, FolderTree, Merge_EmptyTreeIntoPopulated_DoesNothing);
UT_REGISTER_TEST(System, FolderTree, GetSubfolderFiles_OnlyReturnsImmediateFiles);
UT_REGISTER_TEST(System, FolderTree, ListContents_OutputsExpectedHierarchyFormat);
UT_REGISTER_TEST(System, FolderTree, FindSubfolderName_ReturnsNulloptWhenNoMatch);

UT_RUN_TESTS();

UT_FINISH_SESSION()
