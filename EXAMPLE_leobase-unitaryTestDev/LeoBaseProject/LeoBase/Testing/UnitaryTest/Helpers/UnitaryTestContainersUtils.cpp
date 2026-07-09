/*
 * Copyright(C) Milethos Technologies SLU. 2026
 * All rights reserved. Reproduction in whole or in
 * part is prohibited without the written consent of
 * the copyright holder.
 */

// C++ INCLUDES
#include <map>
#include <unordered_map>
#include <string>
#include <vector>
#include <algorithm>

// LEOBASE INCLUDES
#include <LeoBase/Aliases/Testing>
#include "LeoBase/Helpers/containers_utils.h"
#include "LeoBase/Testing/unitary/unitary_test_macros.h"

// =======================================================================
// NAMESPACES
// =======================================================================
using namespace leobase;
using namespace leobase::testing;
using namespace leobase::helpers::containers;

// =======================================================================
// DECLARATIONS
// =======================================================================

UT_DECLARE_TEST(EmptyContainer_ReturnsEmptyVector);
UT_DECLARE_TEST(StandardMap_ReturnsAllKeys);
UT_DECLARE_TEST(Multimap_ReturnsUniqueKeysOnly);
UT_DECLARE_TEST(UnorderedMap_ReturnsAllKeys);

// =======================================================================
// DEFINITIONS
// =======================================================================

/**
 * @brief Verifies that passing an empty map returns an empty vector.
 */
UT_DEFINE_TEST(EmptyContainer_ReturnsEmptyVector)
{
    std::map<int, std::string> empty_map;
    
    auto keys = getMapKeys(empty_map);
    
    UT_EXPECTED_TRUE(keys.empty() == true);
    UT_EXPECTED_TRUE(keys.size() == 0);
}

/**
 * @brief Verifies that extracting keys from a standard std::map works correctly
 * and maintains the sorted order.
 */
UT_DEFINE_TEST(StandardMap_ReturnsAllKeys)
{
    std::map<int, std::string> my_map = {
        {1, "One"}, 
        {2, "Two"}, 
        {3, "Three"}
    };
    
    auto keys = getMapKeys(my_map);
    
    UT_EXPECTED_TRUE(keys.size() == 3);
    // In std::map, keys are inherently sorted
    UT_EXPECTED_TRUE(keys[0] == 1);
    UT_EXPECTED_TRUE(keys[1] == 2);
    UT_EXPECTED_TRUE(keys[2] == 3);
}

/**
 * @brief Verifies that the equal_range logic correctly skips duplicate keys 
 * in a std::multimap, returning only a vector of unique keys.
 */
UT_DEFINE_TEST(Multimap_ReturnsUniqueKeysOnly)
{
    std::multimap<std::string, int> my_multimap = {
        {"Alpha", 10},
        {"Alpha", 20}, // Duplicate key
        {"Beta", 30},
        {"Gamma", 40},
        {"Gamma", 50}  // Duplicate key
    };
    
    auto keys = getMapKeys(my_multimap);
    
    // We expect exactly 3 unique keys
    UT_EXPECTED_TRUE(keys.size() == 3);
    UT_EXPECTED_TRUE(keys[0] == "Alpha");
    UT_EXPECTED_TRUE(keys[1] == "Beta");
    UT_EXPECTED_TRUE(keys[2] == "Gamma");
}

/**
 * @brief Verifies that the helper also works with unordered_map.
 * Note: Since order is not guaranteed, we sort the resulting vector before asserting.
 */
UT_DEFINE_TEST(UnorderedMap_ReturnsAllKeys)
{
    std::unordered_map<int, std::string> my_umap = {
        {30, "Thirty"}, 
        {10, "Ten"}, 
        {20, "Twenty"}
    };
    
    auto keys = getMapKeys(my_umap);
    
    UT_EXPECTED_TRUE(keys.size() == 3);
    std::sort(keys.begin(), keys.end());
    
    UT_EXPECTED_TRUE(keys[0] == 10);
    UT_EXPECTED_TRUE(keys[1] == 20);
    UT_EXPECTED_TRUE(keys[2] == 30);
}

// =======================================================================
// SESSION
// =======================================================================

UT_START_SESSION("LeoBase Containers Utils Unitary Tests")

UT_REGISTER_TEST(Helpers, Containers, EmptyContainer_ReturnsEmptyVector);
UT_REGISTER_TEST(Helpers, Containers, StandardMap_ReturnsAllKeys);
UT_REGISTER_TEST(Helpers, Containers, Multimap_ReturnsUniqueKeysOnly);
UT_REGISTER_TEST(Helpers, Containers, UnorderedMap_ReturnsAllKeys);

UT_RUN_TESTS();

UT_FINISH_SESSION()
