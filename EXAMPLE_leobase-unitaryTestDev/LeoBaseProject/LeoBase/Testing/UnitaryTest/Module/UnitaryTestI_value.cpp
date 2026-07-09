/*
 * Copyright(C) Milethos Technologies SLU. 2026
 * All rights reserved. Reproduction in whole or in
 * part is prohibited without the written consent of
 * the copyright holder.
 */

// C++ INCLUDES
#include <memory>
#include <string>

// LeoBase INCLUDES
#include <LeoBase/Aliases/Testing>

#include "LeoBase/Module/i_values.h"
#include "LeoBase/Testing/unitary/unitary_test_macros.h"

using namespace leobase;
using namespace leobase::testing;
using namespace leobase::module;

// =======================================================================
// HELPER CLASSES
// =======================================================================
namespace {
    /**
     * @brief Dummy class derived from IValues to test the factory templates.
     */
    class DummyValues : public IValues 
    {
    public:
        int value;
        std::string text;

        // Constructor to test parameter forwarding
        DummyValues(int v, const std::string& t) : value(v), text(t) {}
        
        ~DummyValues() override = default;
    };
}

// =======================================================================
// DECLARATIONS
// =======================================================================

UT_DECLARE_TEST(MakeShared_ForwardsArgsAndCreatesInstance);
UT_DECLARE_TEST(MakeUnique_ForwardsArgsAndCreatesInstance);
UT_DECLARE_TEST(TypeAliases_ResolveAndManageMemory);

// =======================================================================
// DEFINITIONS
// =======================================================================

/**
 * @brief Tests that makeShared correctly forwards arguments to the derived class constructor.
 */
UT_DEFINE_TEST(MakeShared_ForwardsArgsAndCreatesInstance)
{
    // Act
    std::shared_ptr<DummyValues> sharedDummy = IValues::makeShared<DummyValues>(42, "SharedTest");

    // Assert
    UT_EXPECTED_TRUE(sharedDummy != nullptr);
    UT_EXPECTED_EQ(sharedDummy->value, 42);
    UT_EXPECTED_EQ(sharedDummy->text, std::string("SharedTest"));
}

/**
 * @brief Tests that makeUnique correctly forwards arguments to the derived class constructor.
 */
UT_DEFINE_TEST(MakeUnique_ForwardsArgsAndCreatesInstance)
{
    // Act
    std::unique_ptr<DummyValues> uniqueDummy = IValues::makeUnique<DummyValues>(84, "UniqueTest");

    // Assert
    UT_EXPECTED_TRUE(uniqueDummy != nullptr);
    UT_EXPECTED_EQ(uniqueDummy->value, 84);
    UT_EXPECTED_EQ(uniqueDummy->text, std::string("UniqueTest"));
}

/**
 * @brief Validates that the custom type aliases resolve correctly and can hold derived objects.
 */
UT_DEFINE_TEST(TypeAliases_ResolveAndManageMemory)
{
    // Test IValuesUniquePtr
    IValuesUniquePtr uniqueBase = std::make_unique<DummyValues>(1, "BaseUnique");
    UT_EXPECTED_TRUE(uniqueBase != nullptr);

    // Test IValuesPtr
    IValuesPtr sharedBase = std::make_shared<DummyValues>(2, "BaseShared");
    UT_EXPECTED_TRUE(sharedBase != nullptr);

    // Test IValuesEWeakPtr
    IValuesEWeakPtr weakBase = sharedBase;
    UT_EXPECTED_FALSE(weakBase.expired());
    
    // Verify weak pointer loses reference when shared is reset
    sharedBase.reset();
    UT_EXPECTED_TRUE(weakBase.expired());
}

// =======================================================================
// SESSION
// =======================================================================

UT_START_SESSION("LeoBase Module IValues Unitary Tests")

UT_REGISTER_TEST(Module, IValues, MakeShared_ForwardsArgsAndCreatesInstance);
UT_REGISTER_TEST(Module, IValues, MakeUnique_ForwardsArgsAndCreatesInstance);
UT_REGISTER_TEST(Module, IValues, TypeAliases_ResolveAndManageMemory);

UT_RUN_TESTS();

UT_FINISH_SESSION()
