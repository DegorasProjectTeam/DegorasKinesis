/*
 * Copyright(C) Milethos Technologies SLU. 2026
 * All rights reserved. Reproduction in whole or in
 * part is prohibited without the written consent of
 * the copyright holder.
 */

// UnitaryTestCallbackHandler.cpp

// C++ INCLUDES
#include <functional>
#include <string>
#include <thread>
#include <vector>
#include <stdexcept>
#include <future>
#include <unordered_map>
#include <any>
#include <typeindex>

// LEOBASE INCLUDES
#include <LeoBase/Aliases/Testing>
#include "LeoBase/Callbacks/callback_handler.h"
#include "LeoBase/Testing/unitary/unitary_test_macros.h"

using namespace leobase;
using namespace leobase::testing;

// ---------------------------------------------------------------------------------------------------------------------
// 1. Test Declarations
// ---------------------------------------------------------------------------------------------------------------------

// --- Registration Tests ---
UT_DECLARE_TEST(CallbackHandler_Register_Basic);
UT_DECLARE_TEST(CallbackHandler_Register_DuplicateId_EmplaceBehavior);
UT_DECLARE_TEST(CallbackHandler_Register_Batch_Success);
UT_DECLARE_TEST(CallbackHandler_Register_Batch_EmptyMap);

// --- State & Lookup Tests ---
UT_DECLARE_TEST(CallbackHandler_HasCallback_Logic);
UT_DECLARE_TEST(CallbackHandler_Remove_ExistingAndNonExisting);
UT_DECLARE_TEST(CallbackHandler_Clear_StateVerification);

// --- InvokeSync Tests ---
UT_DECLARE_TEST(CallbackHandler_InvokeSync_VoidNoArgs);
UT_DECLARE_TEST(CallbackHandler_InvokeSync_WithArgsAndReturn);
UT_DECLARE_TEST(CallbackHandler_InvokeSync_MissingId_Throws);
UT_DECLARE_TEST(CallbackHandler_InvokeSync_TypeMismatch_Throws);
UT_DECLARE_TEST(CallbackHandler_InvokeSync_CallbackThrows_Propagates);

// --- InvokeAsync Tests ---
UT_DECLARE_TEST(CallbackHandler_InvokeAsync_ExecutionAndReturn);
UT_DECLARE_TEST(CallbackHandler_InvokeAsync_MissingId_Throws);
UT_DECLARE_TEST(CallbackHandler_InvokeAsync_TypeMismatch_Throws);
UT_DECLARE_TEST(CallbackHandler_InvokeAsync_CallbackThrows_FuturePropagates);

// --- Concurrency Tests ---
UT_DECLARE_TEST(CallbackHandler_Concurrency_StressTest);

// --- Edge Cases & Robustness Tests ---
UT_DECLARE_TEST(CallbackHandler_Invoke_ReferenceArguments);
UT_DECLARE_TEST(CallbackHandler_DifferentIdTypes_NoCollision);
UT_DECLARE_TEST(CallbackHandler_Register_EmptyFunction_Throws);
UT_DECLARE_TEST(CallbackHandler_Clear_InvokingAfterClearThrows);
UT_DECLARE_TEST(CallbackHandler_Mixed_SyncAndAsync_SameCallback);

// ---------------------------------------------------------------------------------------------------------------------
// 2. Test Definitions
// ---------------------------------------------------------------------------------------------------------------------

// =====================================================================================================================
// REGISTRATION TESTS
// =====================================================================================================================

/**
 * @brief Tests basic single registration with a lambda.
 */
UT_DEFINE_TEST(CallbackHandler_Register_Basic)
{
    leobase::callbacks::CallbackHandler handler;
    handler.registerCallback<int, void()>(1, []() {});
    UT_EXPECTED_TRUE(handler.hasCallback(1));
}

/**
 * @brief Evaluates map::emplace behavior.
 * Since emplace DOES NOT overwrite existing keys, registering a second callback
 * with the same ID should preserve the first one.
 */
UT_DEFINE_TEST(CallbackHandler_Register_DuplicateId_EmplaceBehavior)
{
    leobase::callbacks::CallbackHandler handler;

    // Register first callback returning 10
    handler.registerCallback<int, int()>(100, []() { return 10; });

    // Attempt to register second callback on the same ID returning 99
    handler.registerCallback<int, int()>(100, []() { return 99; });

    // The result should still be 10, because emplace ignores the second insertion
    int result = handler.invokeSync<std::function<int()>>(100);
    UT_EXPECTED_TRUE(result == 10);
}

/**
 * @brief Tests batch registration using unordered_map.
 */
UT_DEFINE_TEST(CallbackHandler_Register_Batch_Success)
{
    leobase::callbacks::CallbackHandler handler;
    std::unordered_map<std::string, leobase::callbacks::CallbackPair> batch;

    auto func1 = std::function<void()>([](){});
    batch.emplace("event1", std::make_pair(std::any(func1), std::type_index(typeid(std::function<void()>))));
    batch.emplace("event2", std::make_pair(std::any(func1), std::type_index(typeid(std::function<void()>))));

    handler.registerCallbacks(batch);

    UT_EXPECTED_TRUE(handler.hasCallback(std::string("event1")));
    UT_EXPECTED_TRUE(handler.hasCallback(std::string("event2")));
}

/**
 * @brief Ensures registering an empty batch does not cause errors.
 */
UT_DEFINE_TEST(CallbackHandler_Register_Batch_EmptyMap)
{
    leobase::callbacks::CallbackHandler handler;
    std::unordered_map<int, leobase::callbacks::CallbackPair> emptyBatch;

    UT_EXPECT_NO_THROW(handler.registerCallbacks(emptyBatch));
}

// =====================================================================================================================
// STATE & LOOKUP TESTS
// =====================================================================================================================

/**
 * @brief Validates hasCallback for both existing and missing IDs.
 */
UT_DEFINE_TEST(CallbackHandler_HasCallback_Logic)
{
    leobase::callbacks::CallbackHandler handler;
    handler.registerCallback<int, void()>(1, []() {});

    UT_EXPECTED_TRUE(handler.hasCallback(1));
    UT_EXPECTED_FALSE(handler.hasCallback(2));
}

/**
 * @brief Tests removing an existing element and ensures removing a non-existent element is safe.
 */
UT_DEFINE_TEST(CallbackHandler_Remove_ExistingAndNonExisting)
{
    leobase::callbacks::CallbackHandler handler;
    handler.registerCallback<int, void()>(1, []() {});

    UT_EXPECTED_TRUE(handler.hasCallback(1));

    handler.removeCallback(1); // Should succeed
    UT_EXPECTED_FALSE(handler.hasCallback(1));

    UT_EXPECT_NO_THROW(handler.removeCallback(999)); // Should not crash
}

/**
 * @brief Tests that clearCallbacks empties the entire map safely.
 * (Assuming clearCallbacks() is implemented as: callbackMap_.clear(); inside a lock).
 */
UT_DEFINE_TEST(CallbackHandler_Clear_StateVerification)
{
    leobase::callbacks::CallbackHandler handler;
    handler.registerCallback<int, void()>(1, []() {});
    handler.registerCallback<int, void()>(2, []() {});

    handler.clearCallbacks();

    UT_EXPECTED_FALSE(handler.hasCallback(1));
    UT_EXPECTED_FALSE(handler.hasCallback(2));
}

// =====================================================================================================================
// INVOKESYNC TESTS
// =====================================================================================================================

/**
 * @brief Tests invoking a standard void function synchronously.
 */
UT_DEFINE_TEST(CallbackHandler_InvokeSync_VoidNoArgs)
{
    leobase::callbacks::CallbackHandler handler;
    bool executed = false;
    handler.registerCallback<int, void()>(1, [&executed]() { executed = true; });

    UT_EXPECT_NO_THROW(handler.invokeSync<std::function<void()>>(1));
    UT_EXPECTED_TRUE(executed);
}

/**
 * @brief Tests passing arguments and retrieving return values synchronously.
 */
UT_DEFINE_TEST(CallbackHandler_InvokeSync_WithArgsAndReturn)
{
    leobase::callbacks::CallbackHandler handler;
    handler.registerCallback<int, std::string(const std::string&, int)>(1, [](const std::string& s, int n) {
        return s + std::to_string(n);
    });

    std::string result = handler.invokeSync<std::function<std::string(const std::string&, int)>>(1, "Age: ", 30);
    UT_EXPECTED_TRUE(result == "Age: 30");
}

/**
 * @brief Tests missing ID exception logic.
 */
UT_DEFINE_TEST(CallbackHandler_InvokeSync_MissingId_Throws)
{
    leobase::callbacks::CallbackHandler handler;
    UT_EXPECT_THROW(handler.invokeSync<std::function<void()>>(404), std::invalid_argument);//TODO: Leobase Exception needed
}

/**
 * @brief Tests type safety enforcement.
 */
UT_DEFINE_TEST(CallbackHandler_InvokeSync_TypeMismatch_Throws)
{
    leobase::callbacks::CallbackHandler handler;
    handler.registerCallback<int, void(int)>(1, [](int) {});

    using WrongType = std::function<void(std::string)>;
    UT_EXPECT_THROW(handler.invokeSync<WrongType>(1, "Test"), std::invalid_argument);//TODO: Leobase Exception needed
}

/**
 * @brief Crucial Test: If the callback itself throws an exception, invokeSync should propagate it.
 */
UT_DEFINE_TEST(CallbackHandler_InvokeSync_CallbackThrows_Propagates)
{
    leobase::callbacks::CallbackHandler handler;
    handler.registerCallback<int, void()>(1, []() { throw std::runtime_error("Internal Error"); });

    UT_EXPECT_THROW(handler.invokeSync<std::function<void()>>(1), std::runtime_error);
}

// =====================================================================================================================
// INVOKE ASYNC TESTS
// =====================================================================================================================

/**
 * @brief Tests running asynchronously and retrieving a valid future.
 */
UT_DEFINE_TEST(CallbackHandler_InvokeAsync_ExecutionAndReturn)
{
    leobase::callbacks::CallbackHandler handler;
    handler.registerCallback<int, int(int)>(1, [](int x) { return x * x; });

    std::future<int> fut = handler.invokeAsync<std::function<int(int)>>(1, 5);
    UT_EXPECTED_TRUE(fut.get() == 25); //TODO Get, alternativo, GET espera --> Bloqueante
}

/**
 * @brief Tests missing ID for async invocation. Note: It should throw immediately, not inside the future.
 */
UT_DEFINE_TEST(CallbackHandler_InvokeAsync_MissingId_Throws)
{
    leobase::callbacks::CallbackHandler handler;
    UT_EXPECT_THROW(handler.invokeAsync<std::function<void()>>(404), std::invalid_argument); //TODO::LEOBASE EXCEPTION**?
}

/**
 * @brief Tests type mismatch for async invocation. Should throw immediately.
 */
UT_DEFINE_TEST(CallbackHandler_InvokeAsync_TypeMismatch_Throws)
{
    leobase::callbacks::CallbackHandler handler;
    handler.registerCallback<int, void(int)>(1, [](int) {});

    using WrongType = std::function<void()>;
    UT_EXPECT_THROW(handler.invokeAsync<WrongType>(1), std::invalid_argument);//TODO::LEOBASE EXCEPTIONS
}

/**
 * @brief If an async callback throws, the exception is stored in the std::future
 * and thrown when .get() is called.
 */
UT_DEFINE_TEST(CallbackHandler_InvokeAsync_CallbackThrows_FuturePropagates)
{
    leobase::callbacks::CallbackHandler handler;
    handler.registerCallback<int, void()>(1, []() { throw std::out_of_range("Out of bounds"); });//TODO::LEOBASE EXCEPTIONS

    std::future<void> fut = handler.invokeAsync<std::function<void()>>(1);

    // The exception is caught when we wait for the result
    UT_EXPECT_THROW(fut.get(), std::out_of_range);
}

// =====================================================================================================================
// CONCURRENCY TESTS
// =====================================================================================================================

/**
 * @brief Heavy stress test hitting register, has, sync, and async simultaneously.
 */
UT_DEFINE_TEST(CallbackHandler_Concurrency_StressTest)
{
    leobase::callbacks::CallbackHandler handler;
    const int numThreads = 10;
    std::vector<std::thread> workers;
    std::atomic<int> successfulInvocations{0};

    // All threads will try to register, check, and invoke simultaneously
    for (int i = 0; i < numThreads; ++i) {
        workers.emplace_back([&handler, i, &successfulInvocations]() {

            // 1. Concurrent Registration
            int id = i % 3; // Intentional collisions to test emplace safety
            handler.registerCallback<int, int()>(id, [id]() { return id * 10; });

            // 2. Concurrent Lookup
            if (handler.hasCallback(id)) {

                // 3. Concurrent Invocation
                try {
                    int result = handler.invokeSync<std::function<int()>>(id);
                    if (result >= 0) {
                        successfulInvocations++;
                    }
                } catch (...) {
                    // Ignore valid exceptions caused by thread race conditions
                }
            }
        });
    }

    for (auto& t : workers)
        t.join();

    // Just verify the system didn't deadlock or crash
    UT_EXPECTED_TRUE(successfulInvocations.load() > 0);
}

// =====================================================================================================================
// ROBUSTNESS TESTS
// =====================================================================================================================

/**
 * @brief Ensures perfectly forwarded arguments can be mutated if passed by reference.
 * If std::forward is failing, this will not modify the original string.
 */
UT_DEFINE_TEST(CallbackHandler_Invoke_ReferenceArguments)
{
    leobase::callbacks::CallbackHandler handler;

    handler.registerCallback<int, void(std::string&)>((int)200, [](std::string& str) {
        str += " modified";
    });

    std::string testString = "original";

    handler.invokeSync<std::function<void(std::string&)>>((int)200, testString);

    UT_EXPECTED_TRUE(testString == "original modified");
}

/**
 * @brief Verifies that hashing logic distinguishes between different ID types
 * that might look similar (int 1 vs string "1").
 */
UT_DEFINE_TEST(CallbackHandler_DifferentIdTypes_NoCollision)
{
    leobase::callbacks::CallbackHandler handler;
    int callCount = 0;

    // Register with integer 1
    handler.registerCallback<int, void()>(1, [&]() { callCount += 10; });

    // Register with string "1"
    handler.registerCallback<std::string, void()>("1", [&]() { callCount += 5; });

    // Invoke both
    handler.invokeSync<std::function<void()>>(1);
    handler.invokeSync<std::function<void()>>(std::string("1"));

    // If they collided and overwrote each other, the result wouldn't be 15
    UT_EXPECTED_TRUE(callCount == 15);
}

/**
 * @brief What happens if a user registers a nullptr / empty std::function?
 * The system should accept it, but throwing std::bad_function_call on execution.
 */
UT_DEFINE_TEST(CallbackHandler_Register_EmptyFunction_Throws)
{
    leobase::callbacks::CallbackHandler handler;
    std::function<void()> emptyFunc = nullptr;

    handler.registerCallback<int, void()>(99, emptyFunc);

    // Registration succeeds, but invoking a null std::function must throw
    UT_EXPECT_THROW(handler.invokeSync<std::function<void()>>(99), std::bad_function_call);
}

/**
 * @brief Ensures that clearCallbacks effectively destroys all internal state,
 * resulting in invalid_argument if invoked afterward.
 */
UT_DEFINE_TEST(CallbackHandler_Clear_InvokingAfterClearThrows)
{
    leobase::callbacks::CallbackHandler handler;
    //We create an empty callback
    handler.registerCallback<int, void()>(1, []() {});

    handler.clearCallbacks();

    // Must throw because the map is empty
    UT_EXPECT_THROW(handler.invokeSync<std::function<void()>>(1), std::invalid_argument);
}

/**
 * @brief Validates that the same registered callback can be invoked both
 * synchronously and asynchronously without errors
 */
UT_DEFINE_TEST(CallbackHandler_Mixed_SyncAndAsync_SameCallback) //TODO Revisar race condition con varios async
{
    leobase::callbacks::CallbackHandler handler;
    std::atomic<int> counter{0};

    handler.registerCallback<int, void()>(1, [&counter]() { counter++; });

    // Call sync
    handler.invokeSync<std::function<void()>>(1);

    // Call async and wait
    auto fut = handler.invokeAsync<std::function<void()>>(1);
    fut.get();

    UT_EXPECTED_TRUE(counter.load() == 2);
}

// ---------------------------------------------------------------------------------------------------------------------
// 3. Execution Session
// ---------------------------------------------------------------------------------------------------------------------

UT_START_SESSION("CallbackHandler Exhaustive Unitary Suite")

// Registration
UT_REGISTER_TEST(Callbacks, Handler, CallbackHandler_Register_Basic);
UT_REGISTER_TEST(Callbacks, Handler, CallbackHandler_Register_DuplicateId_EmplaceBehavior);
UT_REGISTER_TEST(Callbacks, Handler, CallbackHandler_Register_Batch_Success);
UT_REGISTER_TEST(Callbacks, Handler, CallbackHandler_Register_Batch_EmptyMap);

// State
UT_REGISTER_TEST(Callbacks, Handler, CallbackHandler_HasCallback_Logic);
UT_REGISTER_TEST(Callbacks, Handler, CallbackHandler_Remove_ExistingAndNonExisting);
UT_REGISTER_TEST(Callbacks, Handler, CallbackHandler_Clear_StateVerification);

// Sync
UT_REGISTER_TEST(Callbacks, Handler, CallbackHandler_InvokeSync_VoidNoArgs);
UT_REGISTER_TEST(Callbacks, Handler, CallbackHandler_InvokeSync_WithArgsAndReturn);
UT_REGISTER_TEST(Callbacks, Handler, CallbackHandler_InvokeSync_MissingId_Throws);
UT_REGISTER_TEST(Callbacks, Handler, CallbackHandler_InvokeSync_TypeMismatch_Throws);
UT_REGISTER_TEST(Callbacks, Handler, CallbackHandler_InvokeSync_CallbackThrows_Propagates);

// Async
UT_REGISTER_TEST(Callbacks, Handler, CallbackHandler_InvokeAsync_ExecutionAndReturn);
UT_REGISTER_TEST(Callbacks, Handler, CallbackHandler_InvokeAsync_MissingId_Throws);
UT_REGISTER_TEST(Callbacks, Handler, CallbackHandler_InvokeAsync_TypeMismatch_Throws);
UT_REGISTER_TEST(Callbacks, Handler, CallbackHandler_InvokeAsync_CallbackThrows_FuturePropagates);

// Concurrency
UT_REGISTER_TEST(Callbacks, Handler, CallbackHandler_Concurrency_StressTest);


//Robustness test
UT_REGISTER_TEST(Callbacks, Handler, CallbackHandler_Invoke_ReferenceArguments);
UT_REGISTER_TEST(Callbacks, Handler, CallbackHandler_DifferentIdTypes_NoCollision);
UT_REGISTER_TEST(Callbacks, Handler, CallbackHandler_Register_EmptyFunction_Throws);
UT_REGISTER_TEST(Callbacks, Handler, CallbackHandler_Clear_InvokingAfterClearThrows);
UT_REGISTER_TEST(Callbacks, Handler, CallbackHandler_Mixed_SyncAndAsync_SameCallback);


UT_RUN_TESTS();

UT_FINISH_SESSION()
