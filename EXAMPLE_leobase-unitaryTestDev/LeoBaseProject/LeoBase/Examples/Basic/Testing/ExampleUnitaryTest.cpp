/*
 * Copyright(C) Milethos Technologies SLU. 2026
 * All rights reserved. Reproduction in whole or in
 * part is prohibited without the written consent of
 * the copyright holder.
 */

// C++ INCLUDES
#include <iostream>
#include <thread>
#include <string>
#include <vector>       // Added
#include <array>        // Added
#include <chrono>       // Added
#include <functional>   // Added
#include <stdexcept>    // Added

// LEOBASE INCLUDES
#include <LeoBase/Aliases/Testing>
#include <LeoBase/Aliases/Helpers>
#include <LeoBase/Aliases/System>

// ---------------------------------------------------------------------------------------------------------------------

struct Person
{
    std::string name;
    int age;

    bool operator==(const Person& other) const
    {
        return (name == other.name) && (age == other.age);
    }

    bool operator!=(const Person& other) const
    {
        return !(*this == other);
    }
};

bool isEven(int x)
{
    return x % 2 == 0;
}

bool nonEmptyStr(const std::string& str)
{
    return !str.empty();
}

// --- HELPER FUNCTIONS FOR EXCEPTION TESTS (ADDED) ---
void throwRuntime() { throw std::runtime_error("Test intentional error"); }
void throwLogic() { throw std::logic_error("Test logic error"); }
void throwCustomInt() { throw 404; }
void noThrowFunc() { int a = 1; (void)a; }
// --------------------------------------------------

// ---------------------------------------------------------------------------------------------------------------------

// Declare unit tests.
UT_DECLARE_TEST(BasicAssertionsPass)
UT_DECLARE_TEST(BasicAssertionsFail)
UT_DECLARE_TEST(ForcedPass)
UT_DECLARE_TEST(ForcedFail)
UT_DECLARE_TEST(SleepUs)
//
UT_DECLARE_TEST(StructComparison)
UT_DECLARE_TEST(VectorComparison)
UT_DECLARE_TEST(ArrayComparison)
UT_DECLARE_TEST(DurationComparison)
UT_DECLARE_TEST(CustomCheck)
UT_DECLARE_TEST(ExceptionHandling)

// Other modules examples.
UT_DECLARE_TEST(ToUpper)

// Exception test
UT_DECLARE_TEST(ExceptionSpecificThrow)
UT_DECLARE_TEST(ExceptionAnyThrow)
UT_DECLARE_TEST(ExceptionNoThrow)
UT_DECLARE_TEST(ExceptionCodeBlocks)

// ---------------------------------------------------------------------------------------------------------------------

UT_DEFINE_TEST(BasicAssertionsPass)
{
    int x = 5, y = 5, z = 10;
    double a = 1.001, b = 1.002, epsilon = 0.01;
    std::string str1 = "LeoBase", str2 = "LeoBase", str3 = "UnitTest";

    // Should pass.
    UT_EXPECTED_EQ(x, y)
    UT_EXPECTED_NE(x, z)
    UT_EXPECTED_EQ_F(a, b, epsilon)

    // Should pass.
    UT_EXPECTED_EQ(str1, str2)
    UT_EXPECTED_NE(str1, str3)
}

UT_DEFINE_TEST(BasicAssertionsFail)
{
    int x = 5, y = 1;
    double a = 2.001, b = 4.002, epsilon = 0.01;
    std::string str1 = "LeoBase", str2 = "Satan";

    // Should fail.
    UT_EXPECTED_EQ(x, y)
    UT_EXPECTED_NE(x, x)
    UT_EXPECTED_EQ_F(a, b, epsilon)
    UT_EXPECTED_EQ(str1, str2)
}

UT_DEFINE_TEST(ForcedPass)
{
    UT_EXPECTED_EQ(1,2);
    UT_FORCE_PASS();
}

UT_DEFINE_TEST(ForcedFail)
{
    UT_EXPECTED_EQ(1,1);
    UT_FORCE_FAIL();
}

UT_DEFINE_TEST(SleepUs)
{
    std::cout << "[INFO] Sleeping for 500 microseconds..." << std::endl;
    UT_SLEEP_US(5000ll)
    std::cout << "[SUCCESS] Wake up after sleep." << std::endl;
}

UT_DEFINE_TEST(StructComparison)
{
    Person p1{"Alice", 30};
    Person p2{"Alice", 30};
    Person p3{"Bob", 40};

    // Should pass.
    UT_EXPECTED_EQ(p1, p2)
    UT_EXPECTED_NE(p1, p3)
}

UT_DEFINE_TEST(VectorComparison)
{
    std::vector<int> v1 = {1, -2, 3};
    std::vector<int> v2 = {1, -2, 3};
    std::vector<int> v3 = {1, 2, 4};

    std::vector<std::string> vs1 = {"Hola", "Caracola0"};
    std::vector<std::string> vs2 = {"Hola", "Caracola0"};
    std::vector<std::string> vs3 = {"Hola", "Adios"};

    // Should pass.
    UT_EXPECTED_EQ(v1, v2)
    UT_EXPECTED_EQ(v1, v2)

    // Should pass.
    UT_EXPECTED_EQ(vs1, vs2)
    UT_EXPECTED_NE(vs1, vs3)
}

UT_DEFINE_TEST(ArrayComparison)
{
    std::array<unsigned, 3> arr1 = {1, 2, 3};
    std::array<unsigned, 3> arr2 = {1, 2, 3};
    std::array<unsigned, 3> arr3 = {6, 4, 1};

    // Should pass.
    UT_EXPECTED_EQ(arr1, arr2)
    UT_EXPECTED_NE(arr1, arr3)
}

UT_DEFINE_TEST(DurationComparison)
{
    std::chrono::milliseconds time1(100);
    std::chrono::milliseconds time2(100);
    std::chrono::milliseconds time3(150);

    // Should pass.
    UT_EXPECTED_EQ(time1, time2)
    UT_EXPECTED_NE(time1, time3)
}

UT_DEFINE_TEST(CustomCheck)
{
    const std::function<bool(int)> isEvenEncapsulated = isEven;
    const std::function<bool(const std::string&)> nonEmptyStrEncapsulated = nonEmptyStr;

    int even = 4;
    int not_even = 5;
    std::string non_empty = "non_empty";
    std::string empty = "";

    // Should fail.
    UT_CUSTOM_CHECK(isEvenEncapsulated, not_even)
    UT_CUSTOM_CHECK(nonEmptyStrEncapsulated, empty)

    // Should pass.
    UT_CUSTOM_CHECK(isEvenEncapsulated, even)
    UT_CUSTOM_CHECK(nonEmptyStrEncapsulated, non_empty)
}

UT_DEFINE_TEST(ExceptionHandling)
{
    try
    {
        throw std::runtime_error("Intentional test exception!");

        UT_FORCE_PASS()
    }
    catch (const std::exception& e)
    {
        std::cerr << "[ERROR] Exception caught: " << e.what() << std::endl;
        UT_FORCE_PASS()
    }
}


// ---------------------------------------------------------------------------------------------------------------------

// Define the test
UT_DEFINE_TEST(ToUpper)
{
    using namespace leobase::helpers::strings;

    // Basic test cases
    UT_EXPECTED_EQ(toUpper("hello"), "HELLO")
    UT_EXPECTED_EQ(toUpper("WORLD"), "WORLD")
    UT_EXPECTED_EQ(toUpper("LeObAsE"), "LEOBASE")
    UT_EXPECTED_EQ(toUpper("123"), "123")
    UT_EXPECTED_EQ(toUpper("hello world!"), "HELLO WORLD!")
    UT_EXPECTED_EQ(toUpper(""), "")

    // Edge case: Special characters
    UT_EXPECTED_EQ(toUpper("áéíóúñ"), "ÁÉÍÓÚÑ")

    // Force test pass (optional)
    // UT_FORCE_PASS()
}

// 1. Test for specific exception types
UT_DEFINE_TEST(ExceptionSpecificThrow)
{
    // Should pass: Correct types
    UT_EXPECT_THROW(throwRuntime(), std::runtime_error)
    UT_EXPECT_THROW(throwLogic(), std::logic_error)

    // Should fail: Wrong exception type
    UT_EXPECT_THROW(throwRuntime(), std::logic_error)
}

// 2. Test for any exception being thrown
UT_DEFINE_TEST(ExceptionAnyThrow)
    {
        // Should pass: Both throw something
        UT_EXPECT_ANY_THROW(throwRuntime())
        UT_EXPECT_ANY_THROW(throwCustomInt())

        // Should fail: Does not throw
        UT_EXPECT_ANY_THROW(noThrowFunc())
    }

// 3. Test that code executes without exceptions
UT_DEFINE_TEST(ExceptionNoThrow)
    {
        // Should pass: Clean execution
        UT_EXPECT_NO_THROW(noThrowFunc())

        // Should fail: Throws an error
        UT_EXPECT_NO_THROW(throwRuntime())
    }

// 4. Test complex logic inside code blocks {}
UT_DEFINE_TEST(ExceptionCodeBlocks)
    {
        // Verifying standard library behavior
        UT_EXPECT_THROW({
            std::vector<int> v;
            v.at(100); // Throws out_of_range
        }, std::out_of_range)

        // Verifying complex initialization does not throw
        UT_EXPECT_NO_THROW({
            std::string s = "LeoBase";
            s += "_Testing";
            if(s.empty()) throw std::runtime_error("Empty");
        })
    }


// ---------------------------------------------------------------------------------------------------------------------

// UNITARY TEST EXECUTION

// Start the Unit Test Session.
UT_START_SESSION("LeoBase Full Unitary Test Suite")

    // Configuration.
    UT_FORCE_SHOW_RESULTS(TRUE)
    UT_FORCE_STOP_IF_SUBTEST_FAIL(FALSE)

    // Register the tests for the module.
    UT_REGISTER_TEST(Unitary, Basic, BasicAssertionsPass)
    UT_REGISTER_TEST(Unitary, Basic, BasicAssertionsFail)
    UT_REGISTER_TEST(Unitary, Basic, ForcedPass)
    UT_REGISTER_TEST(Unitary, Basic, ForcedFail)
    UT_REGISTER_TEST(Unitary, Basic, SleepUs)
    UT_REGISTER_TEST(Unitary, Structs, StructComparison)
    UT_REGISTER_TEST(Unitary, Containers, VectorComparison)
    UT_REGISTER_TEST(Unitary, Containers, ArrayComparison)
    UT_REGISTER_TEST(Unitary, Containers, DurationComparison)
    UT_REGISTER_TEST(Unitary, Functions, CustomCheck)
    UT_REGISTER_TEST(Unitary, Functions, ExceptionHandling)

    // Other module for example purposes.
    UT_REGISTER_TEST(Helpers, Strings, ToUpper)

    // NEW TESTS REGISTRATION (ADDED)
    UT_REGISTER_TEST(Unitary, Exceptions, ExceptionSpecificThrow)
    UT_REGISTER_TEST(Unitary, Exceptions, ExceptionAnyThrow)
    UT_REGISTER_TEST(Unitary, Exceptions, ExceptionNoThrow)
    UT_REGISTER_TEST(Unitary, Exceptions, ExceptionCodeBlocks)

    // Run unit tests.
    UT_RUN_TESTS();

    // Finish the session.
    UT_FINISH_SESSION();
