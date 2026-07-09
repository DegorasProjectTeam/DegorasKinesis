/*
 *      Copyright(C) Milethos Technologies SLU. 2025
 *      All rights reserved. Reproduction in whole or in
 *      part is prohibited without the written consent of
 *      the copyright holder.
 */

// C++ INCLUDES
#include <iostream>
#include <thread>

// LEOBASE INCLUDES
#include <LeoBase/Aliases/System>
#include <LeoBase/Aliases/Containers>
#include <LeoBase/Aliases/Helpers>

// ---------------------------------------------------------------------------------------------------------------------

// LeoBase namespaces.
using namespace leobase::helpers;
using namespace leobase::containers;

// ---------------------------------------------------------------------------------------------------------------------

// Global mutex to synchronize console output.
std::mutex kGMutex;

// Simulated producer function for pushing elements into the deque.
void producer(ThreadSafeDeque<int>& deque, int id, int start, int count)
{
    for (int i = 0; i < count; ++i)
    {
        int value = start + i;
        deque.pushBack(value);

        {
            std::lock_guard<std::mutex> lock(kGMutex);
            std::cout << "[Producer " << id << "] Pushed: " << value << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

// Simulated consumer function for popping elements from the deque.
void consumer(ThreadSafeDeque<int>& deque, int id)
{
    while (true)
    {
        std::optional<int> value = deque.waitAndPopFront();

        if (!value)
        {
            std::lock_guard<std::mutex> lock(kGMutex);
            std::cout << "[Consumer " << id << "] deque stopped, exiting." << std::endl;
            break;
        }

        std::lock_guard<std::mutex> lock(kGMutex);
        std::cout << "[Consumer " << id << "] Popped: " << *value << std::endl;
    }
}

// Simulated producer function for pushing elements into the deque.
void producerMovement(ThreadSafeDeque<int>& deque, int id, int start, int count)
{
    for (int i = 0; i < count; ++i)
    {
        int value = start + i;
        deque.pushBack(std::move(value));

        {
            std::lock_guard<std::mutex> lock(kGMutex);
            std::cout << "[Producer " << id << "] Pushed: " << value << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

// Simulated consumer function for popping elements from the deque.
void consumerMovement(ThreadSafeDeque<int>& deque, int id)
{
    int value;
    bool res;
    while (true)
    {
        res = deque.waitAndPopFront(value);

        if (!res)
        {
            std::lock_guard<std::mutex> lock(kGMutex);
            std::cout << "[Consumer " << id << "] deque stopped, exiting." << std::endl;
            break;
        }

        std::lock_guard<std::mutex> lock(kGMutex);
        std::cout << "[Consumer " << id << "] Popped: " << value << std::endl;
    }
}

// Simulated producer function with an intentional wait before adding more data.
void delayedProducer(ThreadSafeDeque<int>& deque, int id, int start, int count)
{
    for (int i = 0; i < count / 2; ++i)
    {
        int value = start + i;
        deque.pushBack(value);

        {
            std::lock_guard<std::mutex> lock(kGMutex);
            std::cout << "[Producer " << id << "] Pushed: " << value << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // First wait: Let consumers process existing data
    std::this_thread::sleep_for(std::chrono::seconds(2));

    std::cout << "[Producer " << id << "] Resuming production..." << std::endl;

    for (int i = count / 2; i < count; ++i)
    {
        int value = start + i;
        deque.pushBack(value);

        {
            std::lock_guard<std::mutex> lock(kGMutex);
            std::cout << "[Producer " << id << "] Pushed: " << value << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // Second wait before stopping the deque
    std::this_thread::sleep_for(std::chrono::seconds(2));

    std::cout << "[Producer " << id << "] No more data. Requesting deque stop." << std::endl;
    deque.stop();
}

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Main entry point of the program `ExampleThreadSafeDeque_1`.
 */
int main()
{    
    // Initial log.
    std::cout << strings::generateExampleTitle("Example ThreadSafeDeque 1");

    //----------------------------------------------------------------------------------------------------
    // Testing Basic Push/Pop Operations.

    ThreadSafeDeque<int> deque;

    std::cout << "[INFO] Testing basic push/pop operations..." << std::endl;

    int in = 40;
    int out = 0;

    deque.pushBack(10);
    deque.pushBack(20);
    deque.pushBack(30);
    deque.pushBack(std::move(in));

    std::cout << "  - TryPop: " << deque.tryPopFront().value() << std::endl;
    deque.tryPopFront(out);
    std::cout << "  - TryPop: " << out << std::endl;
    std::cout << "  - deque Size: " << deque.size() << std::endl;

    std::cout << "[SUCCESS] Basic operations tested." << std::endl << std::endl;

    //----------------------------------------------------------------------------------------------------
    // Testing removeAll()

    std::cout << "[INFO] Testing element removal..." << std::endl;

    deque.clear();
    deque.pushBack(42);
    deque.pushBack(10);
    deque.pushBack(42);
    deque.pushBack(50);

    std::cout << "  - Before removeAll(42), deque Size: " << deque.size() << std::endl;
    deque.removeAll(42);
    std::cout << "  - After removeAll(42), deque Size: " << deque.size() << std::endl;

    std::cout << "[SUCCESS] Element removal tested." << std::endl << std::endl;

    //----------------------------------------------------------------------------------------------------
    // Running Multi-threaded Producers and Consumers.

    std::cout << "[INFO] Starting producers and consumers..." << std::endl;

    deque.clear();

    std::thread producer_thread1(producer, std::ref(deque), 1, 100, 10);
    std::thread producer_thread2(producer, std::ref(deque), 2, 200, 10);

    std::thread consumer_thread1(consumer, std::ref(deque), 1);
    std::thread consumer_thread2(consumer, std::ref(deque), 2);

    producer_thread1.join();
    producer_thread2.join();

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    deque.stop();

    consumer_thread1.join();
    consumer_thread2.join();

    std::cout << "[SUCCESS] Multi-threaded operations completed." << std::endl << std::endl;

    //----------------------------------------------------------------------------------------------------
    // Running Multi-threaded Producers and Consumers withj move semantics.

    std::cout << "[INFO] Starting producers and consumers with move semantics..." << std::endl;

    deque.clear();

    std::thread producer_mov_thread1(producerMovement, std::ref(deque), 1, 100, 10);
    std::thread producer_mov_thread2(producerMovement, std::ref(deque), 2, 200, 10);

    std::thread consumer_mov_thread1(consumerMovement, std::ref(deque), 1);
    std::thread consumer_mov_thread2(consumerMovement, std::ref(deque), 2);

    producer_mov_thread1.join();
    producer_mov_thread2.join();

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    deque.stop();

    consumer_mov_thread1.join();
    consumer_mov_thread2.join();

    std::cout << "[SUCCESS] Multi-threaded operations with move semantics completed." << std::endl << std::endl;

    //----------------------------------------------------------------------------------------------------
    // Handling a stopped deque

    std::cout << "[INFO] Testing stopped deque behavior..." << std::endl;

    deque.pushBack(99);
    deque.stop();

    std::optional<int> popped = deque.tryPopFront();
    if (!popped)
        std::cout << "[INFO] deque correctly prevents popping after stop." << std::endl;
    else
        std::cout << "[FAIL] deque should not allow popping after stop!" << std::endl;

    std::cout << "[SUCCESS] Stopped deque behavior verified." << std::endl << std::endl;

    std::cout << "[INFO] Starting delayed producer and consumers..." << std::endl;

    //----------------------------------------------------------------------------------------------------
    // Handling a stopped deque

    ThreadSafeDeque<int> delayed_deque;

    // Create a producer that waits before adding more data
    std::thread producer_delayed_thread(delayedProducer, std::ref(delayed_deque), 1, 100, 10);

    // Create consumers
    std::thread consumer_delayed_thread1(consumer, std::ref(delayed_deque), 1);
    std::thread consumer_delayed_thread2(consumer, std::ref(delayed_deque), 2);

    producer_delayed_thread.join();
    consumer_delayed_thread1.join();
    consumer_delayed_thread2.join();

    std::cout << "[SUCCESS] Delayed producer-consumer test completed." << std::endl << std::endl;

    //----------------------------------------------------------------------------------------------------
    // Test Front & Back Operations**

    std::cout << "[INFO] Testing Front & Back Push/Pop Operations..." << std::endl;

    // Push elements to the front and back.
    deque.clear();
    deque.pushFront(5);
    deque.pushFront(10);
    deque.pushBack(20);
    deque.pushBack(25);

    // Try popping elements from front and back.
    std::cout << "  - TryPopFront: " << deque.tryPopFront().value() << std::endl;
    std::cout << "  - TryPopBack: " << deque.tryPopBack().value() << std::endl;

    std::cout << "  - Deque Size after pops: " << deque.size() << std::endl;

    std::cout << "[SUCCESS] Front & Back Push/Pop tested." << std::endl << std::endl;

    //----------------------------------------------------------------------------------------------------
    // Final log.
    std::cout << strings::generateExampleEnd();
    leobase::system::utils::userInputPause("Press <ENTER> to exit...");

    // Final return.
    return 0;
}

// ---------------------------------------------------------------------------------------------------------------------
