/*
 * Copyright(C) Milethos Technologies SLU. 2026
 * All rights reserved. Reproduction in whole or in
 * part is prohibited without the written consent of
 * the copyright holder.
*/

#include <memory>
#include <string>

// LeoBase INCLUDES
#include <LeoBase/Aliases/Testing>

// Include your actual headers here
#include "LeoBase/Module/i_implementation.h"
#include "LeoBase/Testing/unitary/unitary_test_macros.h"

using namespace leobase;
using namespace leobase::testing;
using namespace leobase::module;

// =======================================================================
// DUMMY CLASSES FOR TESTING
// =======================================================================

// Dummy class to test basic instantiation and argument forwarding
class DummyImplementation : public IImplementation
{
public:
    int value;
    std::string text;

    DummyImplementation(int v, std::string t) : value(v), text(std::move(t)) {}
    ~DummyImplementation() override = default;
};

// Dummy class to test polymorphic destruction
class DestructorTrackerImplementation : public IImplementation
{
public:
    bool& destroyedFlag_;

    DestructorTrackerImplementation(bool& flag) : destroyedFlag_(flag) 
    {
        destroyedFlag_ = false;
    }

    ~DestructorTrackerImplementation() override
    {
        destroyedFlag_ = true;
    }
};

// =======================================================================
// DECLARATIONS
// =======================================================================

UT_DECLARE_TEST(MakeShared_CreatesValidInstanceAndForwardsArgs);
UT_DECLARE_TEST(MakeUnique_CreatesValidInstanceAndForwardsArgs);
UT_DECLARE_TEST(VirtualDestructor_ProperlyDestroysDerivedClass);

// =======================================================================
// DEFINITIONS
// =======================================================================

/**
 * @brief Tests that makeShared correctly constructs a derived type and forwards arguments.
 */
UT_DEFINE_TEST(MakeShared_CreatesValidInstanceAndForwardsArgs)
{
    // Act
    auto sharedImpl = IImplementation::makeShared<DummyImplementation>(42, "LeoBase");

    // Assert
    UT_EXPECTED_TRUE(sharedImpl.get() != nullptr);
    UT_EXPECTED_EQ(sharedImpl->value, 42);
    UT_EXPECTED_EQ(sharedImpl->text, std::string("LeoBase"));
}

/**
 * @brief Tests that makeUnique correctly constructs a derived type and forwards arguments.
 */
UT_DEFINE_TEST(MakeUnique_CreatesValidInstanceAndForwardsArgs)
{
    // Act
    auto uniqueImpl = IImplementation::makeUnique<DummyImplementation>(99, "Module");

    // Assert
    UT_EXPECTED_TRUE(uniqueImpl.get() != nullptr);
    UT_EXPECTED_EQ(uniqueImpl->value, 99);
    UT_EXPECTED_EQ(uniqueImpl->text, std::string("Module"));
}

/**
 * @brief Ensures that deleting the base interface pointer properly calls the derived destructor.
 */
UT_DEFINE_TEST(VirtualDestructor_ProperlyDestroysDerivedClass)
{
    bool isDestroyed = false;

    // Create scope to force destruction
    {
        // Store the derived object in a base class pointer
        IImplementationUniquePtr basePtr = IImplementation::makeUnique<DestructorTrackerImplementation>(isDestroyed);
        
        UT_EXPECTED_FALSE(isDestroyed);
    } // basePtr goes out of scope here, triggering destruction

    // Assert that the derived class destructor was actually executed
    UT_EXPECTED_TRUE(isDestroyed);
}

// =======================================================================
// SESSION
// =======================================================================

UT_START_SESSION("LeoBase IImplementation Unitary Tests")

UT_REGISTER_TEST(Module, IImplementation, MakeShared_CreatesValidInstanceAndForwardsArgs);
UT_REGISTER_TEST(Module, IImplementation, MakeUnique_CreatesValidInstanceAndForwardsArgs);
UT_REGISTER_TEST(Module, IImplementation, VirtualDestructor_ProperlyDestroysDerivedClass);

UT_RUN_TESTS();

UT_FINISH_SESSION()
