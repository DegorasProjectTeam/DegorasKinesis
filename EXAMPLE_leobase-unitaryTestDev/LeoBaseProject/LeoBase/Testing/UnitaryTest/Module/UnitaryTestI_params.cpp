/*
 * Copyright(C) Milethos Technologies SLU. 2026
 * All rights reserved. Reproduction in whole or in
 * part is prohibited without the written consent of
 * the copyright holder.
 */

// C++ INCLUDES
#include <memory>
#include <string>
#include <utility>

// LeoBase INCLUDES
#include <LeoBase/Aliases/Testing>
#include "LeoBase/Module/i_params.h"
#include "LeoBase/Testing/unitary/unitary_test_macros.h"

using namespace leobase;
using namespace leobase::testing;
using namespace leobase::module;

// =======================================================================
// HELPER CLASSES
// =======================================================================
namespace {
    /**
     * @brief Dummy class derived from IParams to test factories and move semantics.
     * Using the Passkey Idiom (Token).
     */
    class DummyParams : public IParams 
    {
    public:
        int configId;
        std::string configName;

        // Constructor MUST now accept the token as its first parameter
        DummyParams(IParams::ConstructorToken, int id, const std::string& name) 
            : configId(id), configName(name) {}
        
        // Custom move constructor for the derived class to test state transfer
        DummyParams(DummyParams&& other) noexcept 
            : IParams(std::move(other)), configId(other.configId), configName(std::move(other.configName)) 
        {
            other.configId = 0;
        }

        // Custom move assignment
        DummyParams& operator=(DummyParams&& other) noexcept 
        {
            if (this != &other) {
                IParams::operator=(std::move(other));
                configId = other.configId;
                configName = std::move(other.configName);
                other.configId = 0;
            }
            return *this;
        }

        ~DummyParams() override = default;
    };
}

// =======================================================================
// DECLARATIONS
// =======================================================================

UT_DECLARE_TEST(MakeShared_UsesTokenAndForwardsArgs);
UT_DECLARE_TEST(MakeUnique_UsesTokenAndForwardsArgs);
UT_DECLARE_TEST(MoveConstruct_TransfersStateSafely);
UT_DECLARE_TEST(MoveAssign_TransfersStateSafely);
UT_DECLARE_TEST(TypeAliases_ResolveAndManageMemory);

// =======================================================================
// DEFINITIONS
// =======================================================================

/**
 * @brief Tests that makeShared successfully passes the private token and forwards arguments.
 */
UT_DEFINE_TEST(MakeShared_UsesTokenAndForwardsArgs)
{
    // The factory calls DummyParams(ConstructorToken{}, 100, "NetworkConf") internally
    std::shared_ptr<DummyParams> sharedParams = IParams::makeShared<DummyParams>(100, "NetworkConf");

    UT_EXPECTED_TRUE(sharedParams != nullptr);
    UT_EXPECTED_EQ(sharedParams->configId, 100);
    UT_EXPECTED_EQ(sharedParams->configName, std::string("NetworkConf"));
}

/**
 * @brief Tests that makeUnique successfully passes the private token and forwards arguments.
 */
UT_DEFINE_TEST(MakeUnique_UsesTokenAndForwardsArgs)
{
    std::unique_ptr<DummyParams> uniqueParams = IParams::makeUnique<DummyParams>(200, "AudioConf");

    UT_EXPECTED_TRUE(uniqueParams != nullptr);
    UT_EXPECTED_EQ(uniqueParams->configId, 200);
    UT_EXPECTED_EQ(uniqueParams->configName, std::string("AudioConf"));
}

/**
 * @brief Ensures the explicitly defaulted move constructor works as intended.
 */
UT_DEFINE_TEST(MoveConstruct_TransfersStateSafely)
{
    //makeUnique here to instantiate the original object
    auto original = IParams::makeUnique<DummyParams>(300, "VideoConf");
    
    //Move construct (Dereference original to move the object itself, not pointer)
    DummyParams moved(std::move(*original));

    // We check if the moved Unique has all the values
    UT_EXPECTED_EQ(moved.configId, 300);
    UT_EXPECTED_EQ(moved.configName, std::string("VideoConf"));
    UT_EXPECTED_EQ(original->configId, 0); // Assuming our dummy resets it
    UT_EXPECTED_EQ(original->configName, "");
}

/**
 * @brief Ensures the explicitly defaulted move assignment operator works as intended.
 */
UT_DEFINE_TEST(MoveAssign_TransfersStateSafely)
{
    auto original = IParams::makeUnique<DummyParams>(400, "SystemConf");
    auto target = IParams::makeUnique<DummyParams>(0, "Empty");

    //Move object
    *target = std::move(*original);

    // Assert
    UT_EXPECTED_EQ(target->configId, 400);
    UT_EXPECTED_EQ(target->configName, std::string("SystemConf"));
    UT_EXPECTED_EQ(original->configId, 0);
    UT_EXPECTED_EQ(original->configName, "");
}

/**
 * @brief Validates that the custom type aliases resolve correctly.
 */
UT_DEFINE_TEST(TypeAliases_ResolveAndManageMemory)
{
    IParamsUniquePtr uniqueBase = IParams::makeUnique<DummyParams>(1, "BaseU");
    UT_EXPECTED_TRUE(uniqueBase != nullptr);

    IParamsPtr sharedBase = IParams::makeShared<DummyParams>(2, "BaseS");
    UT_EXPECTED_TRUE(sharedBase != nullptr);

    IParamsWeakPtr weakBase = sharedBase;
    UT_EXPECTED_FALSE(weakBase.expired());
    
    sharedBase.reset();
    UT_EXPECTED_TRUE(weakBase.expired());
}

// =======================================================================
// SESSION
// =======================================================================

UT_START_SESSION("LeoBase Module IParams Unitary Tests (Passkey Idiom)")

UT_REGISTER_TEST(Module, IParams, MakeShared_UsesTokenAndForwardsArgs);
UT_REGISTER_TEST(Module, IParams, MakeUnique_UsesTokenAndForwardsArgs);
UT_REGISTER_TEST(Module, IParams, MoveConstruct_TransfersStateSafely);
UT_REGISTER_TEST(Module, IParams, MoveAssign_TransfersStateSafely);
UT_REGISTER_TEST(Module, IParams, TypeAliases_ResolveAndManageMemory);

UT_RUN_TESTS();

UT_FINISH_SESSION()
