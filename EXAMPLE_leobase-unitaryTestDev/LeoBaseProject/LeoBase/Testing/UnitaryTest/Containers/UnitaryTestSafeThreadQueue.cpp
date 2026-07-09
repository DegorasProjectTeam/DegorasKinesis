/*
 * Copyright(C) Milethos Technologies SLU. 2026
 * All rights reserved. Reproduction in whole or in
 * part is prohibited without the written consent of
 * the copyright holder.
*/

#include <thread>
#include <chrono>
#include <vector>
#include <atomic>
#include <memory>
#include <string>

// LeoBase INCLUDES
#include <LeoBase/Aliases/Containers>
#include <LeoBase/Aliases/Testing>

// Include your actual headers here
#include "LeoBase/Containers/thread_safe_queue.h"
#include "LeoBase/Testing/unitary/unitary_test_macros.h"

using namespace leobase;
using namespace leobase::testing;
using namespace leobase::containers;

// =======================================================================
// DECLARATIONS
// =======================================================================

UT_DECLARE_TEST(InitialState_IsEmpty);
UT_DECLARE_TEST(PushAndTryPop_MaintainsFIFO);
UT_DECLARE_TEST(TryPopReference_WorksCorrectly);
UT_DECLARE_TEST(MoveSemantics_WorksWithUniquePtr);
UT_DECLARE_TEST(RemoveAll_RemovesMatchingElements);
UT_DECLARE_TEST(WaitAndPop_BlocksUntilElementAdded);
UT_DECLARE_TEST(Stop_WakesUpWaitingThreads);
UT_DECLARE_TEST(Clear_EmptiesQueueAndResetsState);
UT_DECLARE_TEST(ConcurrentProducersAndConsumers_MaintainsIntegrity);

// =======================================================================
// DEFINITIONS
// =======================================================================

/**
 * @brief Checks that the queue initializes correctly with size 0.
 */
UT_DEFINE_TEST(InitialState_IsEmpty)
{
    ThreadSafeQueue<int> queue;
    
    UT_EXPECTED_TRUE(queue.empty());
    UT_EXPECTED_EQ(queue.size(), size_t(0));
    UT_EXPECTED_FALSE(queue.tryPop().has_value());
}

/**
 * @brief Checks basic FIFO (First-In-First-Out) behavior using push and tryPop.
 */
UT_DEFINE_TEST(PushAndTryPop_MaintainsFIFO)
{
    ThreadSafeQueue<std::string> queue;
    
    queue.push("first");
    queue.push("second");
    queue.push("third");
    
    UT_EXPECTED_EQ(queue.size(), size_t(3));
    UT_EXPECTED_FALSE(queue.empty());
    
    UT_EXPECTED_EQ(queue.tryPop().value(), std::string("first"));
    UT_EXPECTED_EQ(queue.tryPop().value(), std::string("second"));
    UT_EXPECTED_EQ(queue.tryPop().value(), std::string("third"));
    
    UT_EXPECTED_TRUE(queue.empty());
}

/**
 * @brief Verifies the boolean returning tryPop overload works.
 */
UT_DEFINE_TEST(TryPopReference_WorksCorrectly)
{
    ThreadSafeQueue<int> queue;
    int value = 0;
    
    UT_EXPECTED_FALSE(queue.tryPop(value)); // Should fail if empty
    
    queue.push(42);
    UT_EXPECTED_TRUE(queue.tryPop(value));
    UT_EXPECTED_EQ(value, 42);
}

/**
 * @brief Verifies that move-only types (like std::unique_ptr) can be pushed and popped.
 */
UT_DEFINE_TEST(MoveSemantics_WorksWithUniquePtr)
{
    ThreadSafeQueue<std::unique_ptr<int>> queue;
    
    queue.push(std::make_unique<int>(100));
    
    auto opt = queue.tryPop();
    UT_EXPECTED_TRUE(opt.has_value());
    UT_EXPECTED_TRUE(opt.value().get() != nullptr);
    UT_EXPECTED_EQ(*(opt.value()), 100);
}

/**
 * @brief Verifies that removeAll correctly erases all instances of a specific value.
 */
UT_DEFINE_TEST(RemoveAll_RemovesMatchingElements)
{
    ThreadSafeQueue<int> queue;
    
    queue.push(1);
    queue.push(5);
    queue.push(2);
    queue.push(5);
    queue.push(3);
    
    queue.removeAll(5);
    
    UT_EXPECTED_EQ(queue.size(), size_t(3));
    UT_EXPECTED_EQ(queue.tryPop().value(), 1);
    UT_EXPECTED_EQ(queue.tryPop().value(), 2);
    UT_EXPECTED_EQ(queue.tryPop().value(), 3);
}

/**
 * @brief Tests that waitAndPop blocks the thread until an element is pushed.
 */
UT_DEFINE_TEST(WaitAndPop_BlocksUntilElementAdded)
{
    ThreadSafeQueue<int> queue;
    std::atomic<bool> threadStarted{false};
    
    auto future = std::async(std::launch::async, [&]() {
        threadStarted = true;
        return queue.waitAndPop();
    });
    
    while (!threadStarted) { std::this_thread::yield(); }
    std::this_thread::sleep_for(std::chrono::milliseconds(20)); // Ensure it's blocked
    
    queue.push(99);
    
    auto opt = future.get();
    UT_EXPECTED_TRUE(opt.has_value());
    UT_EXPECTED_EQ(opt.value(), 99);
}

/**
 * @brief Tests that calling stop() wakes up any waiting threads and returns nullopt.
 */
UT_DEFINE_TEST(Stop_WakesUpWaitingThreads)
{
    ThreadSafeQueue<int> queue;
    
    auto future = std::async(std::launch::async, [&]() {
        return queue.waitAndPop();
    });
    
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    
    queue.stop();
    
    auto opt = future.get();
    UT_EXPECTED_FALSE(opt.has_value()); // Should be nullopt since it was stopped
}

/**
 * @brief Tests that clear() removes all elements and resets the stop state.
 */
UT_DEFINE_TEST(Clear_EmptiesQueueAndResetsState)
{
    ThreadSafeQueue<int> queue;
    
    queue.push(1);
    queue.push(2);
    queue.stop();
    
    queue.clear();
    
    UT_EXPECTED_TRUE(queue.empty());
    UT_EXPECTED_EQ(queue.size(), size_t(0));
    
    // Because clear() resets stopReq_, push should work again
    queue.push(3);
    UT_EXPECTED_EQ(queue.tryPop().value(), 3);
}

/**
 * @brief Stress test with multiple producers and consumers to guarantee thread safety.
 */
UT_DEFINE_TEST(ConcurrentProducersAndConsumers_MaintainsIntegrity)
{
    ThreadSafeQueue<int> queue;
    const int numItemsPerThread = 1000;
    const int numThreads = 4;
    
    std::atomic<int> totalConsumed{0};
    
    // Launch consumers
    std::vector<std::thread> consumers;
    for (int i = 0; i < numThreads; ++i)
    {
        consumers.emplace_back([&]() {
            while (true)
            {
                auto opt = queue.waitAndPop();
                if (!opt.has_value()) break; // Exit loop if stopped
                totalConsumed++;
            }
        });
    }
    
    // Launch producers
    std::vector<std::thread> producers;
    for (int i = 0; i < numThreads; ++i)
    {
        producers.emplace_back([&]() {
            for (int j = 0; j < numItemsPerThread; ++j)
            {
                queue.push(1);
            }
        });
    }
    
    // Wait for producers
    for (auto& p : producers) p.join();
    
    // Wait for consumers to drain the queue
    while (!queue.empty()) { std::this_thread::yield(); }
    
    // Stop consumers and wait for them to finish
    queue.stop();
    for (auto& c : consumers) c.join();
    
    UT_EXPECTED_EQ(totalConsumed.load(), numItemsPerThread * numThreads);
}

// =======================================================================
// SESSION
// =======================================================================

UT_START_SESSION("LeoBase ThreadSafeQueue Unitary Tests")

UT_REGISTER_TEST(Containers, ThreadSafeQueue, InitialState_IsEmpty);
UT_REGISTER_TEST(Containers, ThreadSafeQueue, PushAndTryPop_MaintainsFIFO);
UT_REGISTER_TEST(Containers, ThreadSafeQueue, TryPopReference_WorksCorrectly);
UT_REGISTER_TEST(Containers, ThreadSafeQueue, MoveSemantics_WorksWithUniquePtr);
UT_REGISTER_TEST(Containers, ThreadSafeQueue, RemoveAll_RemovesMatchingElements);
UT_REGISTER_TEST(Containers, ThreadSafeQueue, WaitAndPop_BlocksUntilElementAdded);
UT_REGISTER_TEST(Containers, ThreadSafeQueue, Stop_WakesUpWaitingThreads);
UT_REGISTER_TEST(Containers, ThreadSafeQueue, Clear_EmptiesQueueAndResetsState);
UT_REGISTER_TEST(Containers, ThreadSafeQueue, ConcurrentProducersAndConsumers_MaintainsIntegrity);

UT_RUN_TESTS();

UT_FINISH_SESSION()
