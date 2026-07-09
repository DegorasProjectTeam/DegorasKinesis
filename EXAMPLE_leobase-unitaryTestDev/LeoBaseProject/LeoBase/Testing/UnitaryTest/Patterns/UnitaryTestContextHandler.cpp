/*
 * Copyright(C) Milethos Technologies SLU. 2026
 * All rights reserved. Reproduction in whole or in
 * part is prohibited without the written consent of
 * the copyright holder.
*/

#include <memory>
#include <stdexcept>

// LeoBase INCLUDES
#include <LeoBase/Aliases/Patterns>
#include <LeoBase/Aliases/Testing>

// Include your actual headers here
#include "LeoBase/Patterns/context_handler.h"
#include "LeoBase/Testing/unitary/unitary_test_macros.h"

using namespace leobase;
using namespace leobase::testing;
using namespace leobase::patterns;

// =======================================================================
// MOCKS & FIXTURES
// =======================================================================

// Global counters
static int g_initCalls = 0;
static int g_stopCalls = 0;

// Custom exception for tests
class MockInitException : public std::runtime_error
{
public:
    MockInitException() : std::runtime_error("Mock init failed!") {}
};

struct DummyContext
{
    int someValue = 0;
};

// 1. Mock with normal behavior
class MockContextHandlerImp : public ContextHandlerImp<DummyContext>
{
public:
    void initContext() override
    {
        g_initCalls++;
        this->context_ = std::make_unique<DummyContext>();
        this->context_->someValue = 42;
    }

    void stopContext() override
    {
        g_stopCalls++;
        this->context_.reset();
    }
};

// 2. Mock that throws an exception upon initialization
class ThrowingMockContextImp : public ContextHandlerImp<DummyContext>
{
public:
    void initContext() override
    {
        throw MockInitException();
    }

    void stopContext() override
    {
        // Should not be called if init fails, but implemented to satisfy the interface
        g_stopCalls++;
    }
};

// Wrapper for testing
class TestContextHandler : public ContextHandler<DummyContext>
{
public:
    TestContextHandler(ContextHandlerImpUniquePtr<DummyContext> imp)
        : ContextHandler<DummyContext>(std::move(imp))
    {}
};

// Helper to clean up state
void resetMockState()
{
    g_initCalls = 0;
    g_stopCalls = 0;
}


// =======================================================================
// DECLARATIONS
// =======================================================================

UT_DECLARE_TEST(InitialState_IsEmpty);
UT_DECLARE_TEST(ContextHandler_NormalUsage_DoesNotThrow);
UT_DECLARE_TEST(NormalUsage_DoesNotThrow);
UT_DECLARE_TEST(InitThrows_StateRemainsClean);
UT_DECLARE_TEST(SingleInstance_CallsInitAndStop);
UT_DECLARE_TEST(MultipleInstances_ShareContext)
UT_DECLARE_TEST(GetContext_ReturnsValidPointer);


// =======================================================================
// DEFINITIONS
// =======================================================================

/**
 * @brief Initial state before creating anything.
 */
UT_DEFINE_TEST(InitialState_IsEmpty)
{
    resetMockState();

    UT_EXPECTED_FALSE(TestContextHandler::isInitialized());
    UT_EXPECTED_EQ(TestContextHandler::nInstances(), 0u); // Using the EQ macro
}

/**
 * @brief Checks that the normal instantiation flow does not throw unexpected exceptions.
 */
UT_DEFINE_TEST(NormalUsage_DoesNotThrow)
{
    resetMockState();

    UT_EXPECT_NO_THROW(
        TestContextHandler handler(std::make_unique<MockContextHandlerImp>())
        );
}

/**
 * @brief EXCEPTIONS: If the user's initContext fails and throws an exception,
 * the handler should not be registered as instantiated (avoids corrupted states).
 */
UT_DEFINE_TEST(InitThrows_StateRemainsClean)
{
    resetMockState();

    // Here the constructor will call initContext(), which will throw MockInitException.
    // Since it throws an exception, the object never finishes construction,
    // and the instance should NOT be added to the instances_ vector.
    UT_EXPECT_THROW(
        TestContextHandler handler(std::make_unique<ThrowingMockContextImp>()),
        MockInitException
        );

    // Verify that the system remains clean
    UT_EXPECTED_FALSE(TestContextHandler::isInitialized());
    UT_EXPECTED_EQ(TestContextHandler::nInstances(), 0u);
    UT_EXPECTED_EQ(g_stopCalls, 0); // Should not call stop because it was never initialized
}

/**
 * @brief Basic lifecycle (Init and Stop).
 */
UT_DEFINE_TEST(SingleInstance_CallsInitAndStop)
{
    resetMockState();

    {
        TestContextHandler handler(std::make_unique<MockContextHandlerImp>());

        UT_EXPECTED_TRUE(TestContextHandler::isInitialized());
        UT_EXPECTED_EQ(TestContextHandler::nInstances(), 1u);
        UT_EXPECTED_EQ(g_initCalls, 1);
        UT_EXPECTED_EQ(g_stopCalls, 0);
    }

    // Destroyed upon exiting scope
    UT_EXPECTED_FALSE(TestContextHandler::isInitialized());
    UT_EXPECTED_EQ(TestContextHandler::nInstances(), 0u);
    UT_EXPECTED_EQ(g_stopCalls, 1);
}

/**
 * @brief Multiple instances share initialization.
 */
UT_DEFINE_TEST(MultipleInstances_ShareContext)
{
    resetMockState();

    auto handler1 = std::make_unique<TestContextHandler>(std::make_unique<MockContextHandlerImp>());
    UT_EXPECTED_EQ(TestContextHandler::nInstances(), 1u);
    UT_EXPECTED_EQ(g_initCalls, 1);

    auto handler2 = std::make_unique<TestContextHandler>(std::make_unique<MockContextHandlerImp>());
    UT_EXPECTED_EQ(TestContextHandler::nInstances(), 2u);
    UT_EXPECTED_EQ(g_initCalls, 1); // Should not call init() again

    handler1.reset(); // Destroy one
    UT_EXPECTED_EQ(TestContextHandler::nInstances(), 1u);
    UT_EXPECTED_EQ(g_stopCalls, 0); // Stop() must wait for the last one

    handler2.reset(); // Destroy the last one
    UT_EXPECTED_EQ(TestContextHandler::nInstances(), 0u);
    UT_EXPECTED_EQ(g_stopCalls, 1);
}

/**
 * @brief getContext() returns the correct payload.
 */
UT_DEFINE_TEST(GetContext_ReturnsValidPointer)
{
    resetMockState();

    TestContextHandler handler(std::make_unique<MockContextHandlerImp>());

    const auto& contextPtr = handler.getContext();

    UT_EXPECTED_FALSE(contextPtr == nullptr);
    UT_EXPECTED_EQ(contextPtr->someValue, 42);
}


// =======================================================================
// SESSION
// =======================================================================

UT_START_SESSION("LeoBase ContextHandler Unitary Tests")

UT_REGISTER_TEST(Patterns, ContextHandler, InitialState_IsEmpty);
UT_REGISTER_TEST(Patterns, ContextHandler, NormalUsage_DoesNotThrow);
UT_REGISTER_TEST(Patterns, ContextHandler, InitThrows_StateRemainsClean);
UT_REGISTER_TEST(Patterns, ContextHandler, SingleInstance_CallsInitAndStop);
UT_REGISTER_TEST(Patterns, ContextHandler, MultipleInstances_ShareContext);
UT_REGISTER_TEST(Patterns, ContextHandler, GetContext_ReturnsValidPointer);

UT_RUN_TESTS();

UT_FINISH_SESSION()
