/*
 *      Copyright(C) Milethos Technologies SLU. 2025
 *      All rights reserved. Reproduction in whole or in
 *      part is prohibited without the written consent of
 *      the copyright holder.
 */

#pragma once

// LEOBASE INCLUDES
#include <LeoBase/Testing/unitary/unitary_test_base.h>

// ---------------------------------------------------------------------------------------------------------------------

// UNIT TEST MACROS
#define UT_START_SESSION(SessionName) \
int main() { \
        leobase::testing::UnitaryTest::instance().clear(); \
        leobase::testing::UnitaryTest::instance().setSessionName(std::string(SessionName)); \
        bool gStopOnSubtestFail = false; \
        bool final_res = true;\



#define UT_FINISH_SESSION()\
    return (final_res ? 0 : 1); \
}

#define UT_FORCE_SHOW_RESULTS(enable) \
do { \
        leobase::testing::UnitaryTest::instance().clear(); \
        leobase::testing::UnitaryTest::instance().setForceShowResults(enable); \
} while(0);

#define UT_FORCE_STOP_IF_SUBTEST_FAIL(enable) \
    do { \
        gStopOnSubtestFail = (enable); \
} while(0);

#define UT_DECLARE_TEST(TestName) \
    class Test_##TestName : public leobase::testing::UnitaryTestBase { \
        public: \
        Test_##TestName() : leobase::testing::UnitaryTestBase(#TestName) {} \
            static Test_##TestName* instance() { \
                static Test_##TestName test; \
                return &test; \
        } \
            void runTest() override; \
    };

#define UT_DEFINE_TEST(TestName) \
void Test_##TestName::runTest()

#define UT_REGISTER_TEST(Module, Submodule, TestName) \
    do { \
        Test_##TestName::instance()->setStopOnSubtestFail(gStopOnSubtestFail); \
        leobase::testing::UnitaryTest::instance().registerTest(#Module, #Submodule, Test_##TestName::instance()); \
} while(0);

#define UT_RUN_TESTS() \
    final_res = leobase::testing::UnitaryTest::instance().runTests()

#define UT_FORCE_PASS() \
    do { \
            bool expected = this->result_.load(); \
            this->result_.compare_exchange_strong(expected, forcePass()); \
            return; \
} while(0);

#define UT_FORCE_FAIL() \
    do { \
            bool expected = this->result_.load(); \
            this->result_.compare_exchange_strong(expected, forceFail()); \
            return; \
} while(0);

#define UT_EXPECTED_TRUE(arg1) \
    do { \
            bool expected = this->result_.load(); \
            if (this->stopOnSubtestFail_) \
            this->result_.compare_exchange_strong(expected, expected && expectTrue(arg1)); \
            else \
            this->result_.compare_exchange_strong(expected, expectTrue(arg1)); \
} while(0);

#define UT_EXPECTED_FALSE(arg1) \
    do { \
            bool expected = this->result_.load(); \
            if (this->stopOnSubtestFail_) \
            this->result_.compare_exchange_strong(expected, expected && expectFalse(arg1)); \
            else \
            this->result_.compare_exchange_strong(expected, expectFalse(arg1)); \
} while(0);

#define UT_EXPECTED_EQ(arg1, arg2) \
    do { \
            bool expected = this->result_.load(); \
            if (this->stopOnSubtestFail_) \
            this->result_.compare_exchange_strong(expected, expected && expectEQ(arg1, arg2)); \
            else \
            this->result_.compare_exchange_strong(expected, expectEQ(arg1, arg2)); \
} while(0);

#define UT_EXPECTED_EQ_F(arg1, arg2, eps) \
    do { \
            bool expected = this->result_.load(); \
            if (this->stopOnSubtestFail_) \
            this->result_.compare_exchange_strong(expected, expected && expectEQ(arg1, arg2, eps)); \
            else \
            this->result_.compare_exchange_strong(expected, expectEQ(arg1, arg2, eps)); \
} while(0);

#define UT_EXPECTED_NE(arg1, arg2) \
    do { \
            bool expected = this->result_.load(); \
            if (this->stopOnSubtestFail_) \
            this->result_.compare_exchange_strong(expected, expected && expectNE(arg1, arg2)); \
            else \
            this->result_.compare_exchange_strong(expected, expectNE(arg1, arg2)); \
} while(0);

#define UT_EXPECTED_NE_F(arg1, arg2, eps) \
    do { \
            bool expected = this->result_.load(); \
            if (this->stopOnSubtestFail_) \
            this->result_.compare_exchange_strong(expected, expected && expectNE(arg1, arg2, eps)); \
            else \
            this->result_.compare_exchange_strong(expected, expectNE(arg1, arg2, eps)); \
} while(0);

#define UT_CUSTOM_CHECK(func, ...) \
    do { \
            bool expected = this->result_.load(); \
            if (this->stopOnSubtestFail_) \
            this->result_.compare_exchange_strong(expected, expected && customCheck(func, __VA_ARGS__)); \
            else \
            this->result_.compare_exchange_strong(expected, customCheck(func, __VA_ARGS__)); \
} while(0);

#define UT_EXPECTED_TRUE(arg1) \
    do { \
        bool expected = this->result_.load(); \
        if (this->stopOnSubtestFail_) \
        this->result_.compare_exchange_strong(expected, expected && expectTrue(arg1)); \
        else \
        this->result_.compare_exchange_strong(expected, expectTrue(arg1)); \
} while(0);


#define UT_EXPECTED_FALSE(arg1) \
    do { \
        bool expected = this->result_.load(); \
        if (this->stopOnSubtestFail_) \
        this->result_.compare_exchange_strong(expected, expected && expectFalse(arg1)); \
        else \
        this->result_.compare_exchange_strong(expected, expectFalse(arg1)); \
} while(0);


#define UT_EXPECTED_EQ(arg1, arg2) \
    do { \
        bool expected = this->result_.load(); \
        if (this->stopOnSubtestFail_) \
        this->result_.compare_exchange_strong(expected, expected && expectEQ(arg1, arg2)); \
        else \
        this->result_.compare_exchange_strong(expected, expectEQ(arg1, arg2)); \
} while(0);

#define UT_EXPECTED_EQ_F(arg1, arg2, eps) \
do { \
        bool expected = this->result_.load(); \
        if (this->stopOnSubtestFail_) \
        this->result_.compare_exchange_strong(expected, expected && expectEQ(arg1, arg2, eps)); \
        else \
        this->result_.compare_exchange_strong(expected, expectEQ(arg1, arg2, eps)); \
} while(0);

#define UT_EXPECTED_NE(arg1, arg2) \
do { \
        bool expected = this->result_.load(); \
        if (this->stopOnSubtestFail_) \
        this->result_.compare_exchange_strong(expected, expected && expectNE(arg1, arg2)); \
        else \
        this->result_.compare_exchange_strong(expected, expectNE(arg1, arg2)); \
} while(0);


#define UT_EXPECTED_NE_F(arg1, arg2, eps) \
do { \
        bool expected = this->result_.load(); \
        if (this->stopOnSubtestFail_) \
        this->result_.compare_exchange_strong(expected, expected && expectNE(arg1, arg2, eps)); \
        else \
        this->result_.compare_exchange_strong(expected, expectNE(arg1, arg2, eps)); \
} while(0);

#define UT_FORCE_FAIL() \
do { \
        bool expected = this->result_.load(); \
        this->result_.compare_exchange_strong(expected, forceFail()); \
        return; \
} while(0);

#define UT_FORCE_PASS() \
do { \
        bool expected = this->result_.load(); \
        this->result_.compare_exchange_strong(expected, forcePass()); \
        return; \
} while(0);


#define UT_EXPECT_THROW(statement, exception_type) \
do { \
        bool expected = this->result_.load(); \
        bool res = this->expectThrow<exception_type>([&](){statement;}, #statement, #exception_type); \
        this->result_.compare_exchange_strong(expected, res); \
} while(0); \


// #define UT_EXPECT_ANY_THROW(statement)\
// do{\
//     bool expected = this->result_.load();\
//     this->result_.compare_exchange_strong(expected, expectAnyThrow()([&]{statement;}, #statement));\
//     return;\
// }while(0);


// #define UT_EXPECT_NO_THROW(statement)\
// do{\
//     bool expected = this->result_.load();\
//     this->result_.compare_exchange_strong(expected,expectNoThrow()([&]{statement;}, #statement));\
//     return;\
// }while(0);


// #define UT_EXPECT_THROW(statement, exception_type) \
// do { \
//         bool expected = this->expectThrow<exception_type>([&](){ statement; }, #statement, #exception_type); \
//         if (!expected) { \
//             this->result_.store(false); \
//             if (this->stopOnSubtestFail_) return; \
//     } \
// } while(0);

#define UT_EXPECT_ANY_THROW(statement) \
    do { \
        bool expected = this->expectAnyThrow([&](){ statement; }); \
        if (!expected) { \
            this->result_.store(false); \
            if (this->stopOnSubtestFail_) return; \
    } \
} while(0);

#define UT_EXPECT_NO_THROW(statement) \
    do { \
            bool expected = this->expectNoThrow([&](){ statement; }); \
            if (!expected) { \
                this->result_.store(false); \
                if (this->stopOnSubtestFail_) return; \
        } \
} while(0);

#define UT_SLEEP_US(arg1) \
sleepUs(arg1);
 \
// =====================================================================================================================
