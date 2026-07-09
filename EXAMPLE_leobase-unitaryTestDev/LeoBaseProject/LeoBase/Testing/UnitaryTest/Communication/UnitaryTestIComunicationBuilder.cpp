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

#include "LeoBase/Communication/i_communication_builder.h"
#include "LeoBase/Testing/unitary/unitary_test_macros.h"

using namespace leobase;
using namespace leobase::testing;
using namespace leobase::communication;

// =======================================================================
// HELPER CLASSES
// =======================================================================
namespace {

    /**
     * @brief Mock implementation of ICommunicationBuilder for testing.
     */
    class MockCommunicationBuilder : public ICommunicationBuilder
    {
    public:
        bool started{false};
        bool stopped{false};
        bool produced{false};
        bool waited{false};

        bool start() override { started = true; return true; }
        bool stop() override { stopped = true; return true; }
        bool produceAll() override { produced = true; return true; }
        void waitServer() override { waited = true; }
        
        void configureCallbacks(const std::any& moduleId, 
                                const containers::AnyUnorderedMap& actionCallbacks) override 
        {
            // Mock implementation
            (void)moduleId;
            (void)actionCallbacks;
        }
    };

}

// =======================================================================
// DECLARATIONS
// =======================================================================

UT_DECLARE_TEST(MakeShared_CreatesValidInstance);
UT_DECLARE_TEST(MakeUnique_CreatesValidInstance);
UT_DECLARE_TEST(Interface_IsNonCopyable);
UT_DECLARE_TEST(Polymorphism_ExecutesDerivedLogic);

// =======================================================================
// DEFINITIONS
// =======================================================================

/**
 * @brief Verifies that the static factory makeShared produces a valid shared pointer.
 */
UT_DEFINE_TEST(MakeShared_CreatesValidInstance)
{
    auto builder = ICommunicationBuilder::makeShared<MockCommunicationBuilder>();
    
    UT_EXPECTED_TRUE(builder != nullptr);
    // Verify type through alias compatibility
    ICommunicationBuilderPtr basePtr = builder;
    UT_EXPECTED_TRUE(basePtr != nullptr);
}

/**
 * @brief Verifies that the static factory makeUnique produces a valid unique pointer.
 */
UT_DEFINE_TEST(MakeUnique_CreatesValidInstance)
{
    auto builder = ICommunicationBuilder::makeUnique<MockCommunicationBuilder>();
    
    UT_EXPECTED_TRUE(builder != nullptr);
    UT_EXPECTED_TRUE((std::is_same_v<decltype(builder), std::unique_ptr<MockCommunicationBuilder>>));
}

/**
 * @brief Confirms that any builder inheriting from ICommunicationBuilder 
 * cannot be copied due to NonCopyable inheritance.
 */
UT_DEFINE_TEST(Interface_IsNonCopyable)
{
    bool isCopyConstructible = std::is_copy_constructible<MockCommunicationBuilder>::value;
    bool isCopyAssignable = std::is_copy_assignable<MockCommunicationBuilder>::value;
    
    UT_EXPECTED_FALSE(isCopyConstructible);
    UT_EXPECTED_FALSE(isCopyAssignable);
}

/**
 * @brief Verifies that calling methods through the interface pointer 
 * triggers the mock implementation logic.
 */
UT_DEFINE_TEST(Polymorphism_ExecutesDerivedLogic)
{
    ICommunicationBuilderUniquePtr builder = ICommunicationBuilder::makeUnique<MockCommunicationBuilder>();
    auto* mock = static_cast<MockCommunicationBuilder*>(builder.get());

    UT_EXPECTED_TRUE(builder->start());
    UT_EXPECTED_TRUE(mock->started);

    UT_EXPECTED_TRUE(builder->produceAll());
    UT_EXPECTED_TRUE(mock->produced);

    builder->waitServer();
    UT_EXPECTED_TRUE(mock->waited);

    UT_EXPECTED_TRUE(builder->stop());
    UT_EXPECTED_TRUE(mock->stopped);
}

// =======================================================================
// SESSION
// =======================================================================

UT_START_SESSION("LeoBase Communication ICommunicationBuilder Unitary Tests")

UT_REGISTER_TEST(Communication, Builder, MakeShared_CreatesValidInstance);
UT_REGISTER_TEST(Communication, Builder, MakeUnique_CreatesValidInstance);
UT_REGISTER_TEST(Communication, Builder, Interface_IsNonCopyable);
UT_REGISTER_TEST(Communication, Builder, Polymorphism_ExecutesDerivedLogic);

UT_RUN_TESTS();

UT_FINISH_SESSION()