/*
 * Copyright(C) Milethos Technologies SLU. 2026
 * All rights reserved. Reproduction in whole or in
 * part is prohibited without the written consent of
 * the copyright holder.
 */

// C++ INCLUDES
#include <string>
#include <memory>

// LeoBase INCLUDES
#include <LeoBase/Aliases/Testing>

// Include your actual headers here. Adjust the path as necessary.
#include "LeoBase/Patterns/observer.h"
#include "LeoBase/Testing/unitary/unitary_test_macros.h"

using namespace leobase;
using namespace leobase::testing;
using namespace leobase::patterns;

// =======================================================================
// HELPER CLASSES
// =======================================================================
namespace {

    /**
     * @brief A concrete observer for testing integer updates.
     */
    class MockIntObserver : public Observer<int> 
    {
    public:
        int lastReceivedValue{0};
        bool wasCalled{false};
        bool updated{false};

        bool update(const int& to_update) noexcept override 
        {
            lastReceivedValue = to_update;
            wasCalled = true;
            updated = true;
            return updated;
        }
    };

    /**
     * @brief A concrete observer for testing string updates and return values.
     */
    class MockStringObserver : public Observer<std::string> 
    {
    public:
        std::string lastReceivedString;
        bool updated{false};

        bool update(const std::string& to_update) noexcept override 
        {
            // Simulate a failure if the string is empty
            if (to_update.empty()) 
            {
                this->updated = false;
                return updated;
            }
            lastReceivedString = to_update;
            this->updated = true;
            return updated;
        }
    };

}

// =======================================================================
// DECLARATIONS
// =======================================================================

UT_DECLARE_TEST(UpdateIsCalledSuccessfully_WithBasicType);
UT_DECLARE_TEST(UpdateIsCalledSuccessfully_WithComplexType);
UT_DECLARE_TEST(PolymorphicCall_ResolvesToDerivedImplementation);

// =======================================================================
// DEFINITIONS
// =======================================================================

/**
 * @brief Verifies that a concrete observer of a basic type receives updates.
 */
UT_DEFINE_TEST(UpdateIsCalledSuccessfully_WithBasicType)
{
    MockIntObserver observer;
    
    UT_EXPECTED_FALSE(observer.wasCalled);
    UT_EXPECTED_EQ(observer.lastReceivedValue, 0);

    bool result = observer.update(42);
    
    UT_EXPECTED_TRUE(result);
    UT_EXPECTED_TRUE(observer.wasCalled);
    UT_EXPECTED_EQ(observer.lastReceivedValue, 42);
}

/**
 * @brief Verifies that a concrete observer of a complex type receives updates 
 * and correctly returns the expected boolean status.
 */
UT_DEFINE_TEST(UpdateIsCalledSuccessfully_WithComplexType)
{
    MockStringObserver observer;
    
    // Test successful update
    bool resultOk = observer.update("LeoBase Event");
    UT_EXPECTED_TRUE(resultOk);
    UT_EXPECTED_TRUE(observer.lastReceivedString == "LeoBase Event");

    // Test failed update (simulated by empty string in our mock)
    bool resultFail = observer.update("");
    UT_EXPECTED_FALSE(resultFail);
    // Should retain the last valid string
    UT_EXPECTED_TRUE(observer.lastReceivedString == "LeoBase Event"); 
}

/**
 * @brief Verifies that calling update through a base class pointer correctly
 * invokes the derived class implementation (polymorphism).
 */
UT_DEFINE_TEST(PolymorphicCall_ResolvesToDerivedImplementation)
{
    // Create derived object but store it in a base class unique_ptr
    std::unique_ptr<Observer<int>> polymorphicObserver = std::make_unique<MockIntObserver>();
    
    // Call through the interface
    bool result = polymorphicObserver->update(99);
    UT_EXPECTED_TRUE(result);

    // Downcast to verify internal state changed correctly
    auto* mockObserver = dynamic_cast<MockIntObserver*>(polymorphicObserver.get());
    
    // The dynamic_cast should succeed, and the state should reflect the update
    UT_EXPECTED_TRUE(mockObserver != nullptr);
    if (mockObserver) 
    {
        UT_EXPECTED_TRUE(mockObserver->wasCalled);
        UT_EXPECTED_EQ(mockObserver->lastReceivedValue, 99);
    }
}

// =======================================================================
// SESSION
// =======================================================================

UT_START_SESSION("LeoBase Patterns Observer Unitary Tests")

UT_REGISTER_TEST(Patterns, Observer, UpdateIsCalledSuccessfully_WithBasicType);
UT_REGISTER_TEST(Patterns, Observer, UpdateIsCalledSuccessfully_WithComplexType);
UT_REGISTER_TEST(Patterns, Observer, PolymorphicCall_ResolvesToDerivedImplementation);

UT_RUN_TESTS();

UT_FINISH_SESSION()
