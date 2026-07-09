/*
 * Copyright(C) Milethos Technologies SLU. 2026
 * All rights reserved. Reproduction in whole or in
 * part is prohibited without the written consent of
 * the copyright holder.
 */

// C++ INCLUDES
#include <memory>
#include <type_traits>
#include <any>

// LeoBase INCLUDES
#include <LeoBase/Aliases/Testing>
#include "LeoBase/Communication/communication_director.h"
#include "LeoBase/Communication/i_communication_builder.h"
#include "LeoBase/Exceptions/leobase_exception.h"
#include "LeoBase/Testing/unitary/unitary_test_macros.h"

using namespace leobase;
using namespace leobase::testing;
using namespace leobase::communication;
using namespace leobase::exceptions;

// =======================================================================
// HELPER CLASSES
// =======================================================================
namespace {

    /**
     * @brief A simple mock builder to inject into the Director.
     */
    class MockCommunicationBuilder : public ICommunicationBuilder
    {
    public:
        bool produceAllReturnValue{true};
        bool produceAllWasCalled{false};

        bool start() override { return true; }
        bool stop() override { return true; }
        void waitServer() override { }
        void configureCallbacks(const std::any&, const containers::AnyUnorderedMap&) override {}

        bool produceAll() override 
        {
            produceAllWasCalled = true;
            return produceAllReturnValue;
        }
    };

    /**
     * @brief A testable derived class to expose the protected buildAll() method.
     */
    class TestableCommunicationDirector : public CommunicationDirector
    {
    public:
        // Expose the protected method for testing purposes
        bool publicBuildAll() 
        {
            return buildAll();
        }
    };

}

// =======================================================================
// DECLARATIONS
// =======================================================================

UT_DECLARE_TEST(IsNonCopyable);
UT_DECLARE_TEST(SetAndGetBuilder_WorkCorrectly);
UT_DECLARE_TEST(BuildAll_ThrowsException_WhenBuilderIsNull);
UT_DECLARE_TEST(BuildAll_CallsProduceAll_WhenBuilderIsSet);

// =======================================================================
// DEFINITIONS
// =======================================================================

/**
 * @brief Confirms that the director inherits the NonCopyable restriction.
 */
UT_DEFINE_TEST(IsNonCopyable)
{
    bool isCopyConstructible = std::is_copy_constructible<CommunicationDirector>::value;
    bool isCopyAssignable = std::is_copy_assignable<CommunicationDirector>::value;
    
    UT_EXPECTED_FALSE(isCopyConstructible);
    UT_EXPECTED_FALSE(isCopyAssignable);
}

/**
 * @brief Verifies that setting and getting the builder pointer works symmetrically.
 */
UT_DEFINE_TEST(SetAndGetBuilder_WorkCorrectly)
{
    CommunicationDirector director;
    auto mockBuilder = ICommunicationBuilder::makeShared<MockCommunicationBuilder>();
    
    // Initially, it should be null
    UT_EXPECTED_TRUE(director.getCommunicationBuilder() == nullptr);

    // Set the builder
    director.setCommunicationBuilder(mockBuilder);
    
    // Ensure we retrieve the exact same instance
    UT_EXPECTED_TRUE(director.getCommunicationBuilder() == mockBuilder);
}

/**
 * @brief Ensures that calling buildAll without a valid builder throws LeoBaseException.
 */
UT_DEFINE_TEST(BuildAll_ThrowsException_WhenBuilderIsNull)
{
    TestableCommunicationDirector director;
    bool exceptionCaught = false;

    // We explicitly test for your custom exception type
    try 
    {
        director.publicBuildAll();
    }
    catch (const LeoBaseException& e) 
    {
        exceptionCaught = true;
    }
    catch (...)
    {
        // Caught something else, test should fail (handled by expected_true below)
    }

    UT_EXPECTED_TRUE(exceptionCaught);
}

/**
 * @brief Verifies that buildAll delegates the call to the builder's produceAll method.
 */
UT_DEFINE_TEST(BuildAll_CallsProduceAll_WhenBuilderIsSet)
{
    TestableCommunicationDirector director;
    auto mockBuilder = ICommunicationBuilder::makeShared<MockCommunicationBuilder>();
    
    // Configure mock to return a specific value
    mockBuilder->produceAllReturnValue = true;
    
    director.setCommunicationBuilder(mockBuilder);
    
    UT_EXPECTED_FALSE(mockBuilder->produceAllWasCalled);
    
    bool result = director.publicBuildAll();
    
    UT_EXPECTED_TRUE(mockBuilder->produceAllWasCalled);
    UT_EXPECTED_TRUE(result == true);
}

// =======================================================================
// SESSION
// =======================================================================

UT_START_SESSION("LeoBase Communication CommunicationDirector Unitary Tests")

UT_REGISTER_TEST(Communication, Director, IsNonCopyable);
UT_REGISTER_TEST(Communication, Director, SetAndGetBuilder_WorkCorrectly);
UT_REGISTER_TEST(Communication, Director, BuildAll_ThrowsException_WhenBuilderIsNull);
UT_REGISTER_TEST(Communication, Director, BuildAll_CallsProduceAll_WhenBuilderIsSet);

UT_RUN_TESTS();

UT_FINISH_SESSION()