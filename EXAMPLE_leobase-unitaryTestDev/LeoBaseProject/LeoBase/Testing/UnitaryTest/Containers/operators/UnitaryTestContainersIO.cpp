/*
 * Copyright(C) Milethos Technologies SLU. 2026
 * All rights reserved. Reproduction in whole or in
 * part is prohibited without the written consent of
 * the copyright holder.
 */

// C++ INCLUDES
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <utility>

// LeoBase INCLUDES
#include <LeoBase/Aliases/Testing>

#include "LeoBase/Containers/operators/containers_io.h"
#include "LeoBase/Testing/unitary/unitary_test_macros.h"

using namespace leobase;
using namespace leobase::testing;
using namespace leobase::containers::operators;

// =======================================================================
// ADL WORKAROUND
// Forcing the compiler to bring the overloaded << operators for STL types
// into the current scope so they can be found when printing std:: types.
// =======================================================================
using leobase::containers::operators::operator<<;

// =======================================================================
// DECLARATIONS
// =======================================================================

UT_DECLARE_TEST(Vector_PrintsCorrectly);
UT_DECLARE_TEST(Pair_PrintsCorrectly);
UT_DECLARE_TEST(Map_PrintsCorrectly);
UT_DECLARE_TEST(Multimap_PrintsCorrectly);
UT_DECLARE_TEST(NestedContainers_PrintCorrectly);

// =======================================================================
// DEFINITIONS
// =======================================================================

/**
 * @brief Verifies that std::vector prints correctly (empty, single, and multiple elements).
 */
UT_DEFINE_TEST(Vector_PrintsCorrectly)
{
    std::ostringstream oss;
    
    // Empty vector
    std::vector<int> emptyVec;
    oss << emptyVec;
    UT_EXPECTED_TRUE(oss.str() == "[]");

    // Single element
    oss.str(""); oss.clear();
    std::vector<int> singleVec{42};
    oss << singleVec;
    UT_EXPECTED_TRUE(oss.str() == "[42]");

    // Multiple elements
    oss.str(""); oss.clear();
    std::vector<std::string> multiVec{"a", "b", "c"};
    oss << multiVec;
    UT_EXPECTED_TRUE(oss.str() == "[a, b, c]");
}

/**
 * @brief Verifies that std::pair formats as (first, second).
 */
UT_DEFINE_TEST(Pair_PrintsCorrectly)
{
    std::ostringstream oss;
    std::pair<int, std::string> p{1, "LeoBase"};
    
    oss << p;
    UT_EXPECTED_TRUE(oss.str() == "(1, LeoBase)");
}

/**
 * @brief Verifies that std::map formats cleanly with comma-separated key:value pairs.
 */
UT_DEFINE_TEST(Map_PrintsCorrectly)
{
    std::ostringstream oss;
    
    // Empty map
    std::map<int, std::string> emptyMap;
    oss << emptyMap;
    UT_EXPECTED_TRUE(oss.str() == "{}");

    // Non empty map, populated map
    oss.str(""); oss.clear();
    std::map<int, std::string> populatedMap{{1, "One"}, {2, "Two"}};
    oss << populatedMap;
    UT_EXPECTED_TRUE(oss.str() == "{1: One, 2: Two}");
}

/**
 * @brief Verifies that std::multimap formats correctly, including duplicate keys.
 */
UT_DEFINE_TEST(Multimap_PrintsCorrectly)
{
    std::ostringstream oss;
    std::multimap<std::string, int> mmap;
    mmap.insert({"key1", 10});
    mmap.insert({"key1", 20});
    
    oss << mmap;
    // Because multimap iteration order for identical keys is deterministic in modern C++
    UT_EXPECTED_TRUE(oss.str() == "{key1: 10, key1: 20}");
}

/**
 * @brief Ensures the operators compose well with nested containers.
 */
UT_DEFINE_TEST(NestedContainers_PrintCorrectly)
{
    std::ostringstream oss;
    
    // Vector of Pairs
    std::vector<std::pair<int, int>> vecOfPairs{{1, 2}, {3, 4}};
    oss << vecOfPairs;
    UT_EXPECTED_TRUE(oss.str() == "[(1, 2), (3, 4)]");

    // Map of Vectors
    oss.str(""); oss.clear();
    std::map<std::string, std::vector<int>> mapOfVecs;
    mapOfVecs["nums"] = {1, 2, 3};
    oss << mapOfVecs;
    UT_EXPECTED_TRUE(oss.str() == "{nums: [1, 2, 3]}");
}

// =======================================================================
// SESSION
// =======================================================================

UT_START_SESSION("LeoBase Containers I/O Operators Unitary Tests")

UT_REGISTER_TEST(Containers, Operators, Vector_PrintsCorrectly);
UT_REGISTER_TEST(Containers, Operators, Pair_PrintsCorrectly);
UT_REGISTER_TEST(Containers, Operators, Map_PrintsCorrectly);
UT_REGISTER_TEST(Containers, Operators, Multimap_PrintsCorrectly);
UT_REGISTER_TEST(Containers, Operators, NestedContainers_PrintCorrectly);

UT_RUN_TESTS();

UT_FINISH_SESSION()