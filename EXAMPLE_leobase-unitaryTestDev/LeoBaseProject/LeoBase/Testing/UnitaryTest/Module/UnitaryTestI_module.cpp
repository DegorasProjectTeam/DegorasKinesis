/*
 * Copyright(C) Milethos Technologies SLU. 2026
 * All rights reserved. Reproduction in whole or in
 * part is prohibited without the written consent of
 * the copyright holder.
 */

// C++ INCLUDES
#include <memory>

// LeoBase INCLUDES
#include <LeoBase/Aliases/Testing>
#include "LeoBase/Module/i_module.h"
#include "LeoBase/Testing/unitary/unitary_test_macros.h"

using namespace leobase;
using namespace leobase::testing;
using namespace leobase::module;

// =======================================================================
// HELPER CLASSES
// =======================================================================
namespace {
    /**
     * @brief A concrete instantiation of IModule to test its default behaviors.
     */
    class DefaultTestModule : public IModule 
    {
    public:
        DefaultTestModule() = default;
        ~DefaultTestModule() override = default;
    };
}

// =======================================================================
// DECLARATIONS
// =======================================================================

UT_DECLARE_TEST(DefaultLifecycleMethods_ReturnNotImplemented);
UT_DECLARE_TEST(DefaultDataMethods_ReturnNotImplemented);
UT_DECLARE_TEST(DefaultEventMethods_ReturnExpectedDefaults);
UT_DECLARE_TEST(DefaultStateMethods_ReturnExpectedDefaults);

// =======================================================================
// DEFINITIONS
// =======================================================================

/**
 * @brief Tests the default behavior of module lifecycle methods (setup, start, stop).
 */
UT_DEFINE_TEST(DefaultLifecycleMethods_ReturnNotImplemented)
{
    DefaultTestModule module;
    //IParamsPtr is shared
    IParamsPtr dummyParams = nullptr;

    UT_EXPECTED_TRUE(module.setup(dummyParams) == ModuleStatus::NOT_IMPLEMENTED);
    UT_EXPECTED_TRUE(module.start(dummyParams) == ModuleStatus::NOT_IMPLEMENTED);
    UT_EXPECTED_TRUE(module.stop(dummyParams) == ModuleStatus::NOT_IMPLEMENTED);
}

/**
 * @brief Tests the default behavior of data setting and getting methods.
 */
UT_DEFINE_TEST(DefaultDataMethods_ReturnNotImplemented)
{
    DefaultTestModule module;
    IParamsPtr dummyParams = nullptr;
    IValuesPtr dummyValues = nullptr;

    UT_EXPECTED_TRUE(module.setValue(dummyParams, dummyValues) == ModuleStatus::NOT_IMPLEMENTED);
    
    // For getValue, the output parameter is passed by reference
    IValuesPtr outputValues = nullptr;
    UT_EXPECTED_TRUE(module.getValue(dummyParams, outputValues) == ModuleStatus::NOT_IMPLEMENTED);
}

/**
 * @brief Tests the default behavior of the observer and event queuing methods.
 */
UT_DEFINE_TEST(DefaultEventMethods_ReturnExpectedDefaults)
{
    DefaultTestModule module;
    messaging::IEventPtr dummyEvent = nullptr;

    UT_EXPECTED_TRUE(module.pushEvent(dummyEvent) == ModuleStatus::NOT_IMPLEMENTED);
    
    // update() should return false by default
    UT_EXPECTED_FALSE(module.update(dummyEvent));
}

/**
 * @brief Tests the default behavior of the process loop, status check, and implementation getter.
 */
UT_DEFINE_TEST(DefaultStateMethods_ReturnExpectedDefaults)
{
    DefaultTestModule module;
    IParamsPtr dummyParams = nullptr;
    IImplementationPtr dummyImpl = nullptr;

    UT_EXPECTED_TRUE(module.process(dummyParams) == ModuleStatus::NOT_IMPLEMENTED);
    UT_EXPECTED_TRUE(module.getImpl(dummyImpl) == ModuleStatus::NOT_IMPLEMENTED);
    
    // isReady() should return false by default
    UT_EXPECTED_FALSE(module.isReady());
}

// =======================================================================
// SESSION
// =======================================================================

UT_START_SESSION("LeoBase Module IModule Unitary Tests")

UT_REGISTER_TEST(Module, IModule, DefaultLifecycleMethods_ReturnNotImplemented);
UT_REGISTER_TEST(Module, IModule, DefaultDataMethods_ReturnNotImplemented);
UT_REGISTER_TEST(Module, IModule, DefaultEventMethods_ReturnExpectedDefaults);
UT_REGISTER_TEST(Module, IModule, DefaultStateMethods_ReturnExpectedDefaults);

UT_RUN_TESTS();

UT_FINISH_SESSION()
