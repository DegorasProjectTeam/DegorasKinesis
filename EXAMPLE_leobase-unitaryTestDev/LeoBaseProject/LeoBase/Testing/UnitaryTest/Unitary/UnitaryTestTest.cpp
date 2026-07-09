/*
 * Copyright(C) Milethos Technologies SLU. 2026
 * All rights reserved. Reproduction in whole or in
 * part is prohibited without the written consent of
 * the copyright holder.
 */

// UnitaryTest Orchestrator UnitaryTest

// C++ INCLUDES
#include <string>
#include <stdexcept>
#include <vector>
#include <functional>

// LEOBASE INCLUDES
#include <LeoBase/Aliases/Testing>

// --- NEW INCLUDES FOR CUSTOM EXCEPTIONS ---
#include <LeoBase/Exceptions/base_exception.h>
#include <LeoBase/Exceptions/leobase_exception.h>
#include <LeoBase/Exceptions/canceled_exception.h>

// ---------------------------------------------------------------------------------------------------------------------

using namespace leobase;
using namespace leobase::testing;
using namespace leobase::exceptions;

// --- EXCEPTION HELPERS ---
namespace {
// Standard Helpers
void throwRuntime() { throw std::runtime_error("Test intentional error"); }
void throwLogic() { throw std::logic_error("Test logic error"); }
void throwCustomInt() { throw 404; }
void noThrowFunc() { int a = 1; (void)a; }

// --- NEW HELPERS FOR CUSTOM EXCEPTIONS ---
void throwLeoBaseEx() {
    throw LeoBaseException("TestModule", "HelperScope", "Simulated LeoBase Error");
}

void throwCanceledEx() {
    // Generates message: "[LeoBase::TestModule::HelperScope] Action was canceled."
    throw CanceledException("LeoBase", "TestModule", "HelperScope");
}

// --- ADVANCED VALIDATION HELPER ---
// Executes 'action', captures 'ExType', and verifies that the message contains 'expectedText'
template <typename ExType>
bool checkExceptionAndValue(std::function<void()> action, const std::string& expectedText)
{
    try {
        action();
        return false; // Failure: Did not throw anything
    }
    catch (const ExType& e) {
        // Partial success: You threw the correct type. Now we validate the value.
        std::string msg = e.what();
        return (msg.find(expectedText) != std::string::npos);
    }
    catch (...) {
        return false; // Failed: Another type of exception thrown
    }
}
}
// -------------------------------------------------------------------

// 1. Test Declarations

// Orchestrator Tests
UT_DECLARE_TEST(OrchestratorRegistration)
UT_DECLARE_TEST(OrchestratorExecutionFlow)
UT_DECLARE_TEST(OrchestratorSingletonIntegrity)

// Standard Exception Tests
UT_DECLARE_TEST(ExceptionSpecificThrow)
UT_DECLARE_TEST(ExceptionAnyThrow)
UT_DECLARE_TEST(ExceptionNoThrow)
UT_DECLARE_TEST(ExceptionCodeBlocks)

// Custom Exception Tests
UT_DECLARE_TEST(ExceptionLeoBaseCustom)
UT_DECLARE_TEST(ExceptionAdvancedValueCheck)

// 2. Define Tests

/**
 * @brief Validates that tests can be registered into the internal dictionary correctly.
 */
UT_DEFINE_TEST(OrchestratorRegistration)
{
    UnitaryTest& ut = UnitaryTest::instance();

    static class ConnectivityProbe : public UnitaryTestBase {
    public:
        ConnectivityProbe() : UnitaryTestBase("ConnectivityProbe") {}
        void runTest() override {}
    } probe;

    ut.registerTest("Testing", "Internals", &probe);

    UT_EXPECTED_TRUE(true);
}

UT_DEFINE_TEST(OrchestratorSingletonIntegrity)
{
    UnitaryTest& ut = UnitaryTest::instance();
    std::string sessionName = "ValidationSession";
    ut.setSessionName(std::move(sessionName));
    ut.setForceShowResults(true);

    UT_FORCE_PASS();
}

UT_DEFINE_TEST(OrchestratorExecutionFlow)
{
    UnitaryTest& ut = UnitaryTest::instance();
    bool instanceIsValid = (&ut != nullptr);
    UT_EXPECTED_TRUE(instanceIsValid);
}

// --- STANDARD EXCEPTION TESTS ---

UT_DEFINE_TEST(ExceptionSpecificThrow)
{
    UT_EXPECT_THROW(throwRuntime(), std::runtime_error)
    UT_EXPECT_THROW(throwLogic(), std::logic_error)
}

UT_DEFINE_TEST(ExceptionAnyThrow)
{
    UT_EXPECT_ANY_THROW(throwRuntime())
    UT_EXPECT_ANY_THROW(throwCustomInt())
}

UT_DEFINE_TEST(ExceptionNoThrow)
{
    UT_EXPECT_NO_THROW(noThrowFunc())
}

UT_DEFINE_TEST(ExceptionCodeBlocks)
{
    UT_EXPECT_THROW({
        std::vector<int> v;
        (void)v.at(100);
    }, std::out_of_range)

    UT_EXPECT_NO_THROW({
        std::string s = "LeoBase";
        s += "_Testing";
        if(s.empty())
            throw std::runtime_error("Empty");
    })
}

UT_DEFINE_TEST(ExceptionLeoBaseCustom)
    {
        // 1. Exact Type Matching
        UT_EXPECT_THROW(throwLeoBaseEx(), LeoBaseException)
        UT_EXPECT_THROW(throwCanceledEx(), CanceledException)

        // 2. Inheritance & Polymorphism (check catch by parent)
        UT_EXPECT_THROW(throwCanceledEx(), BaseException)

        // 3. Any throw check
        UT_EXPECT_ANY_THROW(throwLeoBaseEx())
    }


UT_DEFINE_TEST(ExceptionAdvancedValueCheck)
{
    // Case A: Verify that LeoBaseException contains "Simulated"
    UT_EXPECTED_TRUE(
        checkExceptionAndValue<LeoBaseException>(
            throwLeoBaseEx,
            "Simulated"
            )
        );

    // Case B: Verify that CanceledException contains "Action was canceled"
    UT_EXPECTED_TRUE(
        checkExceptionAndValue<CanceledException>(
            [&]{ throwCanceledEx(); },
            "Action was canceled"
            )
        );

    // Case C: Verify that it contains the specific Scope "HelperScope"
    UT_EXPECTED_TRUE(
        checkExceptionAndValue<CanceledException>(
            throwCanceledEx,
            "HelperScope"
            )
        );
}

// 3. Main session execution
UT_START_SESSION("UnitaryTest Master Orchestrator Session")

UT_FORCE_STOP_IF_SUBTEST_FAIL(false);

// Register Orchestrator Tests
UT_REGISTER_TEST(Testing, Orchestrator, OrchestratorRegistration);
UT_REGISTER_TEST(Testing, Orchestrator, OrchestratorSingletonIntegrity);
UT_REGISTER_TEST(Testing, Orchestrator, OrchestratorExecutionFlow);

// Register Standard Exception Tests
UT_REGISTER_TEST(Testing, Exceptions, ExceptionSpecificThrow);
UT_REGISTER_TEST(Testing, Exceptions, ExceptionAnyThrow);
UT_REGISTER_TEST(Testing, Exceptions, ExceptionNoThrow);
UT_REGISTER_TEST(Testing, Exceptions, ExceptionCodeBlocks);

// Register Custom Exception Tests
UT_REGISTER_TEST(Testing, Exceptions, ExceptionLeoBaseCustom);
UT_REGISTER_TEST(Testing, Exceptions, ExceptionAdvancedValueCheck);

UT_RUN_TESTS();

UT_FINISH_SESSION();
