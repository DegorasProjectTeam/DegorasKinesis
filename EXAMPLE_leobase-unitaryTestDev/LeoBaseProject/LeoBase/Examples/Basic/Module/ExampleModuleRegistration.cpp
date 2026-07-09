/*
 *      Copyright(C) Milethos Technologies SLU. 2025
 *      All rights reserved. Reproduction in whole or in
 *      part is prohibited without the written consent of
 *      the copyright holder.
 */

// C++ INCLUDES
#include <iostream>

// LEOBASE INCLUDES
#include <LeoBase/Aliases/System>
#include <LeoBase/Aliases/Module>

// ---------------------------------------------------------------------------------------------------------------------

// LeoBase namespaces.
using leobase::module::ModuleStatus;
using leobase::module::ModuleRegistry;
using leobase::module::IModule;
using leobase::module::IParamsPtr;
using leobase::module::IValues;
using leobase::module::IValuesPtr;

// ---------------------------------------------------------------------------------------------------------------------

enum class TestModuleId : std::uint8_t
{
    MODULE_TEST,
    MODULE_UNKNOWN
};

struct TestValues : public IValues
{
    int value;
};

class TestImpl
{
public:

    TestImpl() {}

    ~TestImpl() {}

    void start()
    {
        std::cout << "--- TestImpl started." << std::endl;
    }

    void stop()
    {
        std::cout << "--- TestImpl stopped." << std::endl;
    }

    void process()
    {
        std::cout << "--- TestImpl process." << std::endl;
    }

    void getValue(IValuesPtr& values)
    {
        std::cout << "--- TestImpl getValue." << std::endl;
        auto retValue = std::dynamic_pointer_cast<TestValues>(values);
        if (retValue)
        {
            retValue->value = 2;
        }
    }
};

using TestImplPtr = std::shared_ptr<TestImpl>;

// ---------------------------------------------------------------------------------------------------------------------

class TestModule : public IModule
{
public:

    TestModule() : IModule()
    {
        //Left in black intentionally
    }

    ~TestModule() = default;

    ModuleStatus setup(IParamsPtr)
    {
        if (impl_)
        {
            return ModuleStatus::MODULE_ALREADY_SETUP;
        }

        impl_ = TestImplPtr(new TestImpl());

        return ModuleStatus::MODULE_OK;
    }

    ModuleStatus start(IParamsPtr)
    {
        if (!impl_)
        {
            return ModuleStatus::MODULE_NOT_SETUP;
        }

        impl_->start();

        return ModuleStatus::MODULE_OK;
    }

    ModuleStatus stop(IParamsPtr)
    {
        if (!impl_)
        {
            return ModuleStatus::MODULE_NOT_SETUP;
        }

        impl_->stop();

        return ModuleStatus::MODULE_OK;
    }

    ModuleStatus process(IParamsPtr)
    {
        if (!impl_)
        {
            return ModuleStatus::MODULE_NOT_SETUP;
        }

        impl_->process();

        return ModuleStatus::MODULE_OK;
    }

    ModuleStatus getValue(IParamsPtr, IValuesPtr& values)
    {
        if (!impl_)
        {
            return ModuleStatus::MODULE_NOT_SETUP;
        }

        impl_->getValue(values);

        return ModuleStatus::MODULE_OK;
    }

private:

    TestImplPtr impl_;
};

using TestModulePtr = std::shared_ptr<TestModule>;

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Main entry point of the program `ExampleModuleRegistration`.
 */
int main()
{

    // Initial log.
    std::cout << "===============================================" << std::endl;
    std::cout << "== ExampleModuleRegistration " << std::endl;
    std::cout << "===============================================" << std::endl;
    std::cout << std::endl;
    std::cout << "Starting example..." << std::endl;
    std::cout << std::endl;

    //----------------------------------------------------------------------------------------------------

    std::cout << "Register TestModule..." << std::endl;

    ModuleRegistry::instance().registerModule(TestModuleId::MODULE_TEST, TestModulePtr(new TestModule()));

    std::cout << "Register TestModule... Ok" << std::endl;

    //----------------------------------------------------------------------------------------------------

    auto module = ModuleRegistry::instance().getModule(TestModuleId::MODULE_TEST);
    if (module == nullptr)
    {
        std::cout << "TestModule not exist in Module Registry. Exit." << std::endl;
        return -1;
    }

    //----------------------------------------------------------------------------------------------------

    std::cout << "Setup TestModule..." << std::endl;

    IParamsPtr params1;
    module->setup(params1);

    std::cout << "Setup TestModule... Ok" << std::endl;

    //----------------------------------------------------------------------------------------------------

    std::cout << "Start TestModule..." << std::endl;

    IParamsPtr params2;
    module->start(params2);

    std::cout << "Start TestModule... Ok" << std::endl;

    //----------------------------------------------------------------------------------------------------

    std::cout << "Process TestModule..." << std::endl;

    IParamsPtr params3;
    module->process(params3);

    std::cout << "Process TestModule... Ok" << std::endl;

    //----------------------------------------------------------------------------------------------------

    std::cout << "Get value TestModule..." << std::endl;

    IParamsPtr params4;
    std::shared_ptr<IValues> retValue = std::make_shared<TestValues>();
    module->getValue(params4, retValue);

    auto vals = std::dynamic_pointer_cast<TestValues>(retValue);
    std::cout << "Get value result: " << vals->value << std::endl;

    std::cout << "Get value TestModule... Ok" << std::endl;

    //----------------------------------------------------------------------------------------------------

    std::cout << "Stop TestModule..." << std::endl;

    IParamsPtr params5;
    module->stop(params5);

    std::cout << "Stop TestModule... Ok" << std::endl;

    //----------------------------------------------------------------------------------------------------

    // Final log.
    std::cout << "Example finished!" << std::endl;
    std::cout << std::endl;
    leobase::system::utils::userInputPause("Press <ENTER> to exit...");

    // Final return.
    return 0;
}

// ---------------------------------------------------------------------------------------------------------------------
