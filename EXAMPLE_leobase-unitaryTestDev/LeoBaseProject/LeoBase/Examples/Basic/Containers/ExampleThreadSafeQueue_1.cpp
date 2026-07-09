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
using namespace leobase::containers;
using namespace leobase::helpers;

// ---------------------------------------------------------------------------------------------------------------------

// Global mutex for synchronizing console output.
std::mutex kGMutex;

// Simulated producer function for pushing elements into the queue.
void producer(ThreadSafeQueue<int>& queue, int id, int start, int count)
{
    for (int i = 0; i < count; ++i)
    {
        int value = start + i;
        queue.push(value);

        {
            std::lock_guard<std::mutex> lock(kGMutex);
            std::cout << "[Producer " << id << "] Pushed: " << value << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

// Simulated consumer function for popping elements from the queue.
void consumer(ThreadSafeQueue<int>& queue, int id)
{
    while (true)
    {
        std::optional<int> value = queue.waitAndPop();

        if (!value)
        {
            std::lock_guard<std::mutex> lock(kGMutex);
            std::cout << "[Consumer " << id << "] Queue stopped, exiting." << std::endl;
            break;
        }

        std::lock_guard<std::mutex> lock(kGMutex);
        std::cout << "[Consumer " << id << "] Popped: " << *value << std::endl;
    }
}

// Producer function with a delay before adding more data.
void delayedProducer(ThreadSafeQueue<int>& queue, int id, int start, int count)
{
    for (int i = 0; i < count / 2; ++i)
    {
        int value = start + i;
        queue.push(value);

        {
            std::lock_guard<std::mutex> lock(kGMutex);
            std::cout << "[Producer " << id << "] Pushed: " << value << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // First wait before resuming production
    std::this_thread::sleep_for(std::chrono::seconds(2));

    std::cout << "[Producer " << id << "] Resuming production..." << std::endl;

    for (int i = count / 2; i < count; ++i)
    {
        int value = start + i;
        queue.push(value);

        {
            std::lock_guard<std::mutex> lock(kGMutex);
            std::cout << "[Producer " << id << "] Pushed: " << value << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // Second wait before stopping the queue
    std::this_thread::sleep_for(std::chrono::seconds(2));

    std::cout << "[Producer " << id << "] No more data. Requesting queue stop." << std::endl;
    queue.stop();
}

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Main entry point of the program `ExampleThreadSafeQueue_1`.
 */
int main()
{
    // Initial log.
    std::cout << strings::generateExampleTitle("Example ThreadSafeQueue 1");

    //----------------------------------------------------------------------------------------------------
    // Testing Basic Push/Pop Operations

    ThreadSafeQueue<int> queue;

    std::cout << "[INFO] Testing basic push/pop operations..." << std::endl;

    int in = 40;
    int out = 0;

    queue.push(10);
    queue.push(20);
    queue.push(30);
    queue.push(std::move(in));

    std::cout << "  - TryPop: " << queue.tryPop().value() << std::endl;
    queue.tryPop(out);
    std::cout << "  - TryPop: " << out << std::endl;
    std::cout << "  - Queue Size: " << queue.size() << std::endl;

    std::cout << "[SUCCESS] Basic operations tested." << std::endl << std::endl;

    //----------------------------------------------------------------------------------------------------
    // Testing removeAll()

    std::cout << "[INFO] Testing element removal..." << std::endl;

    queue.clear();
    queue.push(42);
    queue.push(10);
    queue.push(42);
    queue.push(50);

    std::cout << "  - Before removeAll(42), Queue Size: " << queue.size() << std::endl;
    queue.removeAll(42);
    std::cout << "  - After removeAll(42), Queue Size: " << queue.size() << std::endl;

    std::cout << "[SUCCESS] Element removal tested." << std::endl << std::endl;

    //----------------------------------------------------------------------------------------------------
    // Running Multi-threaded Producers and Consumers

    std::cout << "[INFO] Starting producers and consumers..." << std::endl;

    queue.clear();

    std::thread producer_thread1(producer, std::ref(queue), 1, 100, 10);
    std::thread producer_thread2(producer, std::ref(queue), 2, 200, 10);

    std::thread consumer_thread1(consumer, std::ref(queue), 1);
    std::thread consumer_thread2(consumer, std::ref(queue), 2);

    producer_thread1.join();
    producer_thread2.join();

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    queue.stop();

    consumer_thread1.join();
    consumer_thread2.join();

    std::cout << "[SUCCESS] Multi-threaded operations completed." << std::endl << std::endl;

    //----------------------------------------------------------------------------------------------------
    // Handling a stopped queue

    std::cout << "[INFO] Testing stopped queue behavior..." << std::endl;

    queue.push(99);
    queue.stop();

    std::optional<int> popped = queue.tryPop();
    if (!popped)
        std::cout << "[INFO] Queue correctly prevents popping after stop." << std::endl;
    else
        std::cout << "[FAIL] Queue should not allow popping after stop!" << std::endl;

    std::cout << "[SUCCESS] Stopped queue behavior verified." << std::endl << std::endl;

    std::cout << "[INFO] Starting delayed producer and consumers..." << std::endl;

    //----------------------------------------------------------------------------------------------------
    // Delayed Producer-Consumer Test

    ThreadSafeQueue<int> delayed_queue;

    // Create a producer that waits before adding more data
    std::thread producer_delayed_thread(delayedProducer, std::ref(delayed_queue), 1, 100, 10);

    // Create consumers
    std::thread consumer_delayed_thread1(consumer, std::ref(delayed_queue), 1);
    std::thread consumer_delayed_thread2(consumer, std::ref(delayed_queue), 2);

    producer_delayed_thread.join();
    consumer_delayed_thread1.join();
    consumer_delayed_thread2.join();

    std::cout << "[SUCCESS] Delayed producer-consumer test completed." << std::endl << std::endl;

    //----------------------------------------------------------------------------------------------------
    // Final log.
    std::cout << strings::generateExampleEnd();
    leobase::system::utils::userInputPause("Press <ENTER> to exit...");

    // Final return.
    return 0;
}

// ---------------------------------------------------------------------------------------------------------------------
