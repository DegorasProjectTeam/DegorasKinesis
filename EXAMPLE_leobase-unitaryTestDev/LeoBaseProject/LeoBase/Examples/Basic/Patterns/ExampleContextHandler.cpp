/*
 *      Copyright(C) Milethos Technologies SLU. 2025
 *      All rights reserved. Reproduction in whole or in
 *      part is prohibited without the written consent of
 *      the copyright holder.
 */

// C++ INCLUDES
#include <iostream>

// LEOBASE INCLUDES
#include <LeoBase/Aliases/Patterns>
#include <LeoBase/Aliases/System>

// ---------------------------------------------------------------------------------------------------------------------

// LeoBase namespaces.
using namespace leobase::patterns;

// ---------------------------------------------------------------------------------------------------------------------

class DummyZMQContext
{
public:
    DummyZMQContext()  {std::cout << "[DummyZMQContext] ZMQ context created." << std::endl;}
    ~DummyZMQContext() {std::cout << "[DummyZMQContext] ZMQ context destroyed." << std::endl;}
    void sayHelloZmqContext() {std::cout << "[DummyZMQContext] ZMQ Say Hello." << std::endl;}
};

class ZMQContextHandlerImp : public ContextHandlerImp<DummyZMQContext>
{
    inline void initContext() override
    {
        ContextHandlerImp::context_ = std::make_unique<DummyZMQContext>();
    }

    inline void stopContext() override
    {
        ContextHandlerImp::context_.reset();
    }
};

class ZMQContextHandler : public ContextHandler<DummyZMQContext>
{
public:
    ZMQContextHandler() :
        ContextHandler(std::make_unique<ZMQContextHandlerImp>())
    {}
};

// ---------------------------------------------------------------------------------------------------------------------

// Example dummy CURL init function.
void DUMMY_CURL_INIT() {std::cout << "[DUMMY_CURL] Curl context created." << std::endl;}

// Example dummy CURL clean function.
void DUMMY_CURL_CLEAN() {std::cout << "[DUMMY_CURL] Curl context destroyed." << std::endl;}

// Implementation for initialize and stop the CURL context.
class CurlContextHandlerImp : public ContextHandlerImp<>
{
public:

    inline void initContext() override
    {
        DUMMY_CURL_INIT();
    }

    inline void stopContext() override
    {
        DUMMY_CURL_CLEAN();
    }
};

// ContextHandler specialization for CURL using the Imp.
class CurlContextHandler : public ContextHandler<void>
{
public:
    CurlContextHandler() :
        ContextHandler(std::make_unique<CurlContextHandlerImp>())
    {}
};

// Example of a manager that must use the CURL context.
class CurlSFTPManager : public CurlContextHandler
{
public:

    CurlSFTPManager() = default;

    void listFiles()
    {
        if(!CurlContextHandler::isInitialized())
            std::cout << "[CurlSFTPManager] FAIL - THIS NEVER MUST HAPPEND." << std::endl;
        std::cout << "[CurlSFTPManager] Curl listing files." << std::endl;
    }
};

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Main entry point of the program `ExampleContextHandler`.
 */
int main()
{
    // Initial log.
    std::cout << "===============================================" << std::endl;
    std::cout << "== Example ContextHandler" << std::endl;
    std::cout << "===============================================" << std::endl;
    std::cout << std::endl;
    std::cout << "Starting example..." << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;

    // ----------------------------------------------------------------------------------------------------
    // Instantiate the ZMQContextHandler.

    std::cout << "ZMQContextHandler Instances: " << ZMQContextHandler::nInstances() << std::endl;
    std::cout << "ZMQContextHandler Init: " << ZMQContextHandler::isInitialized() << std::endl;
    {
        ZMQContextHandler zmq_handler_1;
        std::cout << "ZMQContextHandler Instances: " << ZMQContextHandler::nInstances() << std::endl;
        std::cout << "ZMQContextHandler Init: " << ZMQContextHandler::isInitialized() << std::endl;
        ZMQContextHandler zmq_handler_2;
        std::cout << "ZMQContextHandler Instances: " << ZMQContextHandler::nInstances() << std::endl;
        ZMQContextHandler zmq_handler_3;
        std::cout << "ZMQContextHandler Instances: " << ZMQContextHandler::nInstances() << std::endl;
        zmq_handler_3.getContext()->sayHelloZmqContext();
    }
    std::cout << "ZMQContextHandler Init: " << ZMQContextHandler::isInitialized() << std::endl;
    std::cout << "ZMQContextHandler Instances: " << ZMQContextHandler::nInstances() << std::endl;
    std::cout << std::endl;

    // ----------------------------------------------------------------------------------------------------
    // Instantiate the CurlContextHandler.

    std::cout << "CurlContextHandler Instances: " << CurlContextHandler::nInstances() << std::endl;
    std::cout << "CurlContextHandler Init: " << CurlContextHandler::isInitialized() << std::endl;
    {
        CurlContextHandler curl_handler_1;
        std::cout << "CurlContextHandler Instances: " << CurlContextHandler::nInstances() << std::endl;
        std::cout << "CurlContextHandler Init: " << CurlContextHandler::isInitialized() << std::endl;
        CurlContextHandler curl_handler_2;
        std::cout << "CurlContextHandler Instances: " << CurlContextHandler::nInstances() << std::endl;
        CurlContextHandler curl_handler_3;
        std::cout << "CurlContextHandler Instances: " << CurlContextHandler::nInstances() << std::endl;
    }
    std::cout << "CurlContextHandler Init: " << CurlContextHandler::isInitialized() << std::endl;
    std::cout << "CurlContextHandler Instances: " << CurlContextHandler::nInstances() << std::endl;
    std::cout << std::endl;

    // ----------------------------------------------------------------------------------------------------
    // Instantiate the CurlSFTPManager.

    std::cout << "CurlContextHandler Instances: " << CurlContextHandler::nInstances() << std::endl;
    std::cout << "CurlContextHandler Init: " << CurlContextHandler::isInitialized() << std::endl;
    {
        CurlSFTPManager curl_handler_1;
        std::cout << "CurlContextHandler Instances: " << CurlContextHandler::nInstances() << std::endl;
        std::cout << "CurlContextHandler Init: " << CurlContextHandler::isInitialized() << std::endl;
        curl_handler_1.listFiles();
        CurlSFTPManager curl_handler_2;
        std::cout << "CurlContextHandler Instances: " << CurlContextHandler::nInstances() << std::endl;
    }
    std::cout << "CurlContextHandler Init: " << CurlContextHandler::isInitialized() << std::endl;
    std::cout << "CurlContextHandler Instances: " << CurlContextHandler::nInstances() << std::endl;
    std::cout << std::endl;

    //----------------------------------------------------------------------------------------------------
    // Final log.
    std::cout << std::endl << "Example finished!" << std::endl;
    std::cout << std::endl;
    leobase::system::utils::userInputPause("Press <ENTER> to exit...");

    // Final return.
    return 0;
}

// ---------------------------------------------------------------------------------------------------------------------
