/*
 * Copyright(C) Milethos Technologies SLU. 2026
 * All rights reserved. Reproduction in whole or in
 * part is prohibited without the written consent of
 * the copyright holder.
*/
// C++ INCLUDES
#include <string>

// LeoBase INCLUDES
#include <LeoBase/Aliases/Containers>
#include <LeoBase/Aliases/Testing>

#include "LeoBase/Containers/any_unordered_map.h"
#include "LeoBase/Testing/unitary/unitary_test_macros.h"

using namespace leobase;
using namespace leobase::testing;
using namespace leobase::containers;

// =======================================================================
// DECLARATIONS
// =======================================================================

// Tests for getValue<T>
UT_DECLARE_TEST(GetValue_ValidKeyAndType_ReturnsValue);
UT_DECLARE_TEST(GetValue_MissingKey_ThrowsException);
UT_DECLARE_TEST(GetValue_TypeMismatch_ThrowsException);

// Tests for tryGetValue<T>
UT_DECLARE_TEST(TryGetValue_ValidKeyAndType_ReturnsOptionalWithValue);
UT_DECLARE_TEST(TryGetValue_MissingKey_ReturnsNullopt);
UT_DECLARE_TEST(TryGetValue_TypeMismatch_ReturnsNullopt);

// =======================================================================
// DEFINITIONS
// =======================================================================

/**
 * @brief Tests that getValue returns the correct casted value when key and type are correct.
 */
UT_DEFINE_TEST(GetValue_ValidKeyAndType_ReturnsValue)
{
    AnyUnorderedMap map;
    map[std::any(std::string("my_integer_key"))] = std::any(42);
    map[std::any(100)] = std::any(std::string("hello world"));

    // Check retrieval of integer value using a string key
    int intVal = getValue<int>(map, std::string("my_integer_key"));
    UT_EXPECTED_EQ(intVal, 42);

    // Check retrieval of string value using an integer key
    std::string strVal = getValue<std::string>(map, 100);
    UT_EXPECTED_EQ(strVal, std::string("hello world"));
}

/**
 * @brief Tests that getValue throws a LeoBaseException when the key does not exist.
 */
UT_DEFINE_TEST(GetValue_MissingKey_ThrowsException)
{
    AnyUnorderedMap map;
    map[std::any(std::string("existing_key"))] = std::any(42);

    UT_EXPECT_THROW(
        getValue<int>(map, std::string("missing_key")), 
        exceptions::LeoBaseException
    );
}

/**
 * @brief Tests that getValue throws a LeoBaseException when trying to cast to the wrong type.
 */
UT_DEFINE_TEST(GetValue_TypeMismatch_ThrowsException)
{
    AnyUnorderedMap map;
    map[std::any(std::string("key1"))] = std::any(42); // Stored as int

    // Try to retrieve the integer as a float, which should trigger a bad_any_cast
    UT_EXPECT_THROW(
        getValue<float>(map, std::string("key1")), 
        exceptions::LeoBaseException
    );
}

/**
 * @brief Tests that tryGetValue returns a valid std::optional when key and type are correct.
 */
UT_DEFINE_TEST(TryGetValue_ValidKeyAndType_ReturnsOptionalWithValue)
{
    AnyUnorderedMap map;
    map[std::any(std::string("my_float_key"))] = std::any(3.14f);

    std::optional<float> optVal = tryGetValue<float>(map, std::string("my_float_key"));
    
    UT_EXPECTED_TRUE(optVal.has_value());
    UT_EXPECTED_EQ_F(optVal.value(), 3.14f, 0.001f); // Floating point comparison
}

/**
 * @brief Tests that tryGetValue returns std::nullopt when the key does not exist.
 */
UT_DEFINE_TEST(TryGetValue_MissingKey_ReturnsNullopt)
{
    AnyUnorderedMap map;
    
    std::optional<int> optVal = tryGetValue<int>(map, std::string("ghost_key"));
    
    UT_EXPECTED_FALSE(optVal.has_value());
}

/**
 * @brief Tests that tryGetValue returns std::nullopt when trying to cast to the wrong type.
 */
UT_DEFINE_TEST(TryGetValue_TypeMismatch_ReturnsNullopt)
{
    AnyUnorderedMap map;
    map[std::any(std::string("key1"))] = std::any(std::string("A string value")); 

    // Try to retrieve the string as a double
    std::optional<double> optVal = tryGetValue<double>(map, std::string("key1"));
    
    UT_EXPECTED_FALSE(optVal.has_value());
}

// =======================================================================
// SESSION
// =======================================================================

UT_START_SESSION("LeoBase AnyUnorderedMap Unitary Tests")

// Register getValue tests
UT_REGISTER_TEST(Containers, AnyUnorderedMap, GetValue_ValidKeyAndType_ReturnsValue);
UT_REGISTER_TEST(Containers, AnyUnorderedMap, GetValue_MissingKey_ThrowsException);
UT_REGISTER_TEST(Containers, AnyUnorderedMap, GetValue_TypeMismatch_ThrowsException);

// Register tryGetValue tests
UT_REGISTER_TEST(Containers, AnyUnorderedMap, TryGetValue_ValidKeyAndType_ReturnsOptionalWithValue);
UT_REGISTER_TEST(Containers, AnyUnorderedMap, TryGetValue_MissingKey_ReturnsNullopt);
UT_REGISTER_TEST(Containers, AnyUnorderedMap, TryGetValue_TypeMismatch_ReturnsNullopt);

UT_RUN_TESTS();

UT_FINISH_SESSION()
