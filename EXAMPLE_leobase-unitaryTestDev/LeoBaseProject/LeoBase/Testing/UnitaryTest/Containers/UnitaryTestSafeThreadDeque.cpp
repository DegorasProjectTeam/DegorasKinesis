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

// LeoBase INCLUDES
#include <LeoBase/Aliases/Containers>
#include <LeoBase/Aliases/Testing>

// Include your actual headers here
#include "LeoBase/Containers/thread_safe_deque.h"
#include "LeoBase/Testing/unitary/unitary_test_macros.h"

using namespace leobase;
using namespace leobase::testing;
using namespace leobase::containers;

// =======================================================================
// DECLARATIONS
// =======================================================================

UT_DECLARE_TEST(InitialState_IsEmpty);
UT_DECLARE_TEST(PushBackAndTryPopFront_MaintainsFIFO);
UT_DECLARE_TEST(PushFrontAndTryPopBack_MaintainsLIFOBehavior);
UT_DECLARE_TEST(MoveSemantics_WorksWithUniquePtr);
UT_DECLARE_TEST(RemoveAll_RemovesMatchingElements);
UT_DECLARE_TEST(WaitAndPop_BlocksUntilElementAdded);
UT_DECLARE_TEST(Stop_WakesUpWaitingThreadsAndClears);
UT_DECLARE_TEST(Restart_AllowsNewElementsAfterStop);
UT_DECLARE_TEST(ConcurrentProducersAndConsumers_MaintainsIntegrity);

// =======================================================================
// DEFINITIONS
// =======================================================================

/**
 * @brief Checks that the deque initializes correctly with size 0.
 */
UT_DEFINE_TEST(InitialState_IsEmpty)
{
    ThreadSafeDeque<int> deque;
    
    UT_EXPECTED_TRUE(deque.empty());
    //UT_EXPECTED_EQ(deque.size(), 0u);
    UT_EXPECTED_EQ(deque.size(), size_t(0));
    UT_EXPECTED_FALSE(deque.tryPopFront().has_value());
}

/**
 * @brief Checks basic FIFO (First-In-First-Out) behavior using pushBack and tryPopFront.
 */
UT_DEFINE_TEST(PushBackAndTryPopFront_MaintainsFIFO)
{
    ThreadSafeDeque<int> deque;
    
    deque.pushBack(10);
    deque.pushBack(20);
    deque.pushBack(30);
    
    //UT_EXPECTED_EQ(deque.size(), 3u);
    UT_EXPECTED_EQ(deque.size(), size_t(3));
    UT_EXPECTED_FALSE(deque.empty());
    
    UT_EXPECTED_EQ(deque.tryPopFront().value(), 10);
    UT_EXPECTED_EQ(deque.tryPopFront().value(), 20);
    UT_EXPECTED_EQ(deque.tryPopFront().value(), 30);
    
    UT_EXPECTED_TRUE(deque.empty());
}

/**
 * @brief Checks bidirectional behavior using pushFront and tryPopBack.
 */
UT_DEFINE_TEST(PushFrontAndTryPopBack_MaintainsLIFOBehavior)
{
    ThreadSafeDeque<int> deque;
    
    deque.pushFront(10);
    deque.pushFront(20); // 20 is now at the front
    
    int value = 0;
    UT_EXPECTED_TRUE(deque.tryPopBack(value)); // Should pop 10 (the back)
    UT_EXPECTED_EQ(value, 10);
    
    UT_EXPECTED_TRUE(deque.tryPopBack(value)); // Should pop 20
    UT_EXPECTED_EQ(value, 20);
}

/**
 * @brief Verifies that move-only types (like std::unique_ptr) can be pushed and popped.
 */
UT_DEFINE_TEST(MoveSemantics_WorksWithUniquePtr)
{
    ThreadSafeDeque<std::unique_ptr<int>> deque;
    
    deque.pushBack(std::make_unique<int>(42));
    
    auto opt = deque.tryPopFront();
    UT_EXPECTED_TRUE(opt.has_value());
    UT_EXPECTED_EQ(*(opt.value()), 42);
}

/**
 * @brief Verifies that removeAll correctly erases all instances of a specific value.
 */
UT_DEFINE_TEST(RemoveAll_RemovesMatchingElements)
{
    ThreadSafeDeque<int> deque;
    
    deque.pushBack(1);
    deque.pushBack(2);
    deque.pushBack(2);
    deque.pushBack(3);
    
    deque.removeAll(2);
    //Must be the same type
    //UT_EXPECTED_EQ(deque.size(), 2u);
    UT_EXPECTED_EQ(deque.size(), size_t(2));
    UT_EXPECTED_EQ(deque.tryPopFront().value(), 1);
    UT_EXPECTED_EQ(deque.tryPopFront().value(), 3);
}

/**
 * @brief Tests that waitAndPop blocks the thread until an element is pushed.
 */
UT_DEFINE_TEST(WaitAndPop_BlocksUntilElementAdded)
{
    ThreadSafeDeque<int> deque;
    std::atomic<bool> threadStarted{false};
    
    auto future = std::async(std::launch::async, [&]() {
        threadStarted = true;
        return deque.waitAndPopFront();
    });
    
    // Give the thread a moment to start and block
    while (!threadStarted) { std::this_thread::yield(); }
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    
    deque.pushBack(99);
    
    auto opt = future.get();
    UT_EXPECTED_TRUE(opt.has_value());
    UT_EXPECTED_EQ(opt.value(), 99);
}

/**
 * @brief Tests that calling stop() wakes up any waiting threads and returns nullopt.
 */
UT_DEFINE_TEST(Stop_WakesUpWaitingThreadsAndClears)
{
    ThreadSafeDeque<int> deque;
    
    deque.pushBack(1);
    
    auto future = std::async(std::launch::async, [&]() {
        // First pop will get '1'
        auto first = deque.waitAndPopFront();
        // Second pop will block until stop() is called
        auto second = deque.waitAndPopFront();
        return second;
    });
    
    // Allow the async thread to process the first item and block on the second
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    
    deque.stop();
    
    auto opt = future.get();
    UT_EXPECTED_FALSE(opt.has_value()); // Must be nullopt because it was stopped
    UT_EXPECTED_TRUE(deque.empty());
}

/**
 * @brief Tests that restart() allows normal operations to resume after a stop.
 */
UT_DEFINE_TEST(Restart_AllowsNewElementsAfterStop)
{
    ThreadSafeDeque<int> deque;
    
    deque.stop();
    deque.pushBack(10); // Should be ignored because it's stopped
    UT_EXPECTED_TRUE(deque.empty());
    
    deque.restart();
    deque.pushBack(20); // Should be accepted now
    //UT_EXPECTED_EQ(deque.size(), 1u);
    UT_EXPECTED_EQ(deque.size(), size_t(1));
    UT_EXPECTED_EQ(deque.tryPopFront().value(), 20);
}

/**
 * @brief Stress test with multiple producers and consumers to guarantee thread safety.
 */
UT_DEFINE_TEST(ConcurrentProducersAndConsumers_MaintainsIntegrity)
{
    ThreadSafeDeque<int> deque;
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
                auto opt = deque.waitAndPopFront();
                if (!opt.has_value()) break; // Stopped
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
                deque.pushBack(1);
            }
        });
    }
    
    // Wait for producers to finish
    for (auto& p : producers) p.join();
    
    // Wait until deque is completely empty
    while (!deque.empty()) { std::this_thread::yield(); }
    
    // Stop consumers (will return nullopt and break their loops)
    deque.stop();
    for (auto& c : consumers) c.join();
    
    // Verify that exactly (numItemsPerThread * numThreads) items were pushed and popped
    UT_EXPECTED_EQ(totalConsumed.load(), numItemsPerThread * numThreads);
}

// =======================================================================
// SESSION
// =======================================================================

UT_START_SESSION("LeoBase ThreadSafeDeque Unitary Tests")

UT_REGISTER_TEST(Containers, ThreadSafeDeque, InitialState_IsEmpty);
UT_REGISTER_TEST(Containers, ThreadSafeDeque, PushBackAndTryPopFront_MaintainsFIFO);
UT_REGISTER_TEST(Containers, ThreadSafeDeque, PushFrontAndTryPopBack_MaintainsLIFOBehavior);
UT_REGISTER_TEST(Containers, ThreadSafeDeque, MoveSemantics_WorksWithUniquePtr);
UT_REGISTER_TEST(Containers, ThreadSafeDeque, RemoveAll_RemovesMatchingElements);
UT_REGISTER_TEST(Containers, ThreadSafeDeque, WaitAndPop_BlocksUntilElementAdded);
UT_REGISTER_TEST(Containers, ThreadSafeDeque, Stop_WakesUpWaitingThreadsAndClears);
UT_REGISTER_TEST(Containers, ThreadSafeDeque, Restart_AllowsNewElementsAfterStop);
UT_REGISTER_TEST(Containers, ThreadSafeDeque, ConcurrentProducersAndConsumers_MaintainsIntegrity);

UT_RUN_TESTS();

UT_FINISH_SESSION()
