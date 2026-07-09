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
#include <LeoBase/Aliases/Callbacks>
#include <LeoBase/Aliases/Helpers>

// ---------------------------------------------------------------------------------------------------------------------

// LeoBase namespaces.
using namespace leobase::helpers;
using leobase::callbacks::CallbackHandler;
using leobase::callbacks::CallbackMaker;

// ---------------------------------------------------------------------------------------------------------------------

void freeFunction1()
{
    std::cout << "Free function 1 called." << std::endl;
}

void freeFunction2(int x, const std::string& str)
{
    std::cout << "Free function 2 called with values: " << x << " - " << str << std::endl;
}

std::string  freeFunction3()
{
    return("Free function 3 return.");
}

double freeAddFunction(double x, double y)
{
    return x+y;
}

void freeAddFunction(double x, double y, double& z)
{
    z = x+y;
}

class Example
{
public:

    void memberFunction1()
    {
        std::cout << "Member function 1 called." << std::endl;
    }

    void memberFunction2(int x, const std::string& str)
    {
        std::cout << "Member function 2 called with values: " << x << " - " << str << std::endl;
    }

    std::string  memberFunction3()
    {
        return("Member function 3 return.");
    }

    std::string memberFunction4(std::string& str)
    {
        str = "Hola Mundo";
        return("Member function 4 return.");
    }

    static std::string memberFunction5()
    {
        return("Member function 5 (static) return.");
    }

    double memberAddFunction(double x, double y)
    {
        return x+y;
    }

    void memberAddFunction(double x, double y, double& z)
    {
        z = x+y;
    }
};

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Main entry point of the program `ExampleCallbackHandler_1`.
 */
int main()
{
    // Initial log.
    std::cout << strings::generateExampleTitle("Example CallbackHandler 1");

    //----------------------------------------------------------------------------------------------------

    // Example 1: <makeCallback> usages for free functions.
    std::cout << "Example 1: <makeCallback> usages for free functions." << std::endl;

    // Callback free function without i/o params and void return type.
    auto clbk_free_1 = CallbackMaker::makeCallback(&freeFunction1);
    clbk_free_1();

    // Callback free function with input params and void return type.
    auto clbk_free_2 = CallbackMaker::makeCallback(&freeFunction2);
    clbk_free_2(45, "Satan");

    //Callback free function without i/o params and return type.
    auto clbk_free_3 = CallbackMaker::makeCallback(&freeFunction3);
    std::cout << clbk_free_3() << std::endl;

    // Callback free overload function.
    auto clbk_free_4 = CallbackMaker::makeCallback<double, double, double>(&freeAddFunction);
    std::cout << "Result of freeAddFunction: " << clbk_free_4(2.24, 1.19) << std::endl;

    // Callback free overload function.
    auto clbk_free_5 = CallbackMaker::makeCallback<void, double, double, double&>(&freeAddFunction);
    double result_free;
    clbk_free_5(4.52342, 43.8726, result_free);
    std::cout << "Result of freeAddFunction (with output): " << result_free << std::endl;

    //----------------------------------------------------------------------------------------------------

    // Example 2: <makeCallback> usages for member functions.
    std::cout << std::endl<<"Example 2: <makeCallback> usages for member functions." << std::endl;

    // Example object.
    Example example_obj;

    // Callback member function without i/o params and void return type.
    auto clbk_member_1 = CallbackMaker::makeCallback(&example_obj, &Example::memberFunction1);
    clbk_member_1();

    // Callback member function with input params and void return type.
    auto clbk_member_2 = CallbackMaker::makeCallback(&example_obj, &Example::memberFunction2);
    clbk_member_2(42, "Hello");

    //Callback member function without i/o params and return type.
    auto clbk_member_3 = CallbackMaker::makeCallback(&example_obj, &Example::memberFunction3);
    std::cout << clbk_member_3() << std::endl;

    // Callback member function with output params and return type.
    std::string str = "example";
    auto clbk_member_4 = CallbackMaker::makeCallback(&example_obj, &Example::memberFunction4);
    std::cout << clbk_member_4(str) << " Modified str: " << str << std::endl;

    // Callback static member function.
    auto clbk_member_5 = CallbackMaker::makeCallback(&Example::memberFunction5);
    std::cout << clbk_member_5() << str << std::endl;

    // Callback member overload function.
    auto callback6 = CallbackMaker::makeCallback<double, double, double>(&example_obj, &Example::memberAddFunction);
    std::cout << "Result of memberAddFunction: " << callback6(5.5, 2.5) << std::endl;

    // Callback member overload function.
    auto clbk_member_7 = CallbackMaker::makeCallback<void, double, double, double&>(&example_obj, &Example::memberAddFunction);
    double result;
    clbk_member_7(10.5, 20.5, result);
    std::cout << "Result of memberAddFunction (with output): " << result << std::endl;

    //----------------------------------------------------------------------------------------------------

    // Example 3: <makeCallback> usages using lambdas as binding for free functions.
    std::cout << std::endl<<"Example 3: <makeCallback> usages using lambdas as binding for free functions." << std::endl;

    // Free function with no binding.
    auto clbk_bound_free_1 = CallbackMaker::makeCallback(
        [](int x, const std::string& str) { freeFunction2(x, str); });
    clbk_bound_free_1(416, "Hello World Bound");

    // Free function binding first parameter.
    auto clbk_bound_free_2 = CallbackMaker::makeCallback(
        [](const std::string& str) { freeFunction2(100, str); });
    clbk_bound_free_2("Hello World Bound");

    // Free function binding both parameter.
    auto clbk_bound_free_3 = CallbackMaker::makeCallback(
        []() { freeFunction2(200, "Hello World Bound"); });
    clbk_bound_free_3();

    // Free function with overload binding both parameter.
    auto clbk_bound_free_4 = CallbackMaker::makeCallback(
        []() -> double { return freeAddFunction(200.0, 23.6); });
    std::cout << "Result of freeAddFunction: " << clbk_bound_free_4() << std::endl;

    // Free function with overload with no binding.
    auto clbk_bound_free_5 = CallbackMaker::makeCallback(
        [](double a, double b, double& result) { freeAddFunction(a, b, result); });
    double result_binding;
    clbk_bound_free_5(12.5, 223.56, result_binding);
    std::cout << "Result of freeAddFunction (with output): " << result_binding << std::endl;

    //----------------------------------------------------------------------------------------------------

    // Example 4: <makeCallback> usages using lambdas as binding for member functions.
    std::cout << std::endl<<"Example 4: <makeCallback> usages using lambdas as binding for member functions." << std::endl;

    // Member function with no binding.
    auto clbk_bound_member_1 = CallbackMaker::makeCallback(
        [&example_obj]() { example_obj.memberFunction1(); });
    clbk_bound_member_1();

    // Member function binding first parameter.
    auto clbk_bound_member_2 = CallbackMaker::makeCallback(
        [&example_obj](const std::string& str) { example_obj.memberFunction2(500, str); });
    clbk_bound_member_2("Hello Member Bound");

    // Member function binding both parameters.
    auto clbk_bound_member_3 = CallbackMaker::makeCallback(
        [&example_obj]() { example_obj.memberFunction2(700, "Fully Bound Member"); });
    clbk_bound_member_3();

    // Member function with return value.
    auto clbk_bound_member_4 = CallbackMaker::makeCallback(
        [&example_obj]() -> std::string { return example_obj.memberFunction3(); });
    std::cout << clbk_bound_member_4() << std::endl;

    // Member function with input/output parameters.
    std::string str_original = "Initial String";
    auto clbk_bound_member_5 = CallbackMaker::makeCallback(
        [&example_obj](std::string& str) -> std::string { return example_obj.memberFunction4(str); });
    std::cout << clbk_bound_member_5(str_original) << " | Modified str: " << str_original << std::endl;

    // Static member function.
    auto clbk_bound_member_6 = CallbackMaker::makeCallback(
        []() { return Example::memberFunction5(); });
    std::cout << clbk_bound_member_6() << std::endl;

    //----------------------------------------------------------------------------------------------------

    // Example 5: <makeCallback> usages using std::bind for functions.
    std::cout << std::endl<<"Example 5: <makeCallback> using std::bind for functions." << std::endl;

    // Example 1: Free function with no binding.
    std::function<void(int, const std::string&)> clbk_bind_free_1 =
        std::bind(&freeFunction2, std::placeholders::_1, std::placeholders::_2);
    clbk_bind_free_1(416, "Hello World Bound");

    // Example 2: Free function binding the first parameter.
    std::function<void(const std::string&)> clbk_bind_free_2 =
        std::bind(&freeFunction2, 100, std::placeholders::_1);
    clbk_bind_free_2("Hello World Bound");

    // Example 3: Free function binding both parameters.
    std::function<void()> clbk_bind_free_3 =
        std::bind(&freeFunction2, 200, "Hello World Bound");
    clbk_bind_free_3();

    // Example 4: Free function with overload binding both parameters.
    std::function<double()> clbk_bind_free_4 =
        std::bind(static_cast<double(*)(double, double)>(&freeAddFunction), 200, 23.6);
    std::cout << "Result of freeAddFunction: " << clbk_bind_free_4() << std::endl;

    // Example 5: Free function with overload and no binding (with output).
    std::function<void(double, double, double&)> clbk_bind_free_5 =
        std::bind(static_cast<void(*)(double, double, double&)>(&freeAddFunction),
                  std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    clbk_bind_free_5(12.5, 223.56, result_binding);
    std::cout << "Result of freeAddFunction (with output): " << result_binding << std::endl;

    //----------------------------------------------------------------------------------------------------

    // Example 6: <makeCallback> usages using lambda functions.
    std::cout << std::endl<<"Example 6: <makeCallback> usages using lambda functions." << std::endl;

    // Lambda function with no parameters and void return type
    auto clbk_lambda_1 = CallbackMaker::makeCallback(
        []() {std::cout << "Lambda function 1 called." << std::endl;});
    clbk_lambda_1();

    // Lambda function with input parameters and void return type
    auto clbk_lambda_2 = CallbackMaker::makeCallback(
        [](int a, const std::string& str) {
            std::cout << "Lambda function 2 called with values: " << a << " - " << str << std::endl;
        });
    clbk_lambda_2(99, "Lambda Example");

    // Lambda function with return value
    auto clbk_lambda_3 = CallbackMaker::makeCallback(
        []() -> std::string {return "Lambda function 3 return value.";});
    std::cout << "Result of lambda 3: " << clbk_lambda_3() << std::endl;

    // Lambda function capturing external variables
    int multiplier = 2;
    auto clbk_lambda_4 = CallbackMaker::makeCallback(
        [multiplier](int x) -> int {return x * multiplier;});
    std::cout << "Result of lambda 4 (captured variable): " << clbk_lambda_4(10) << std::endl;

    // Lambda function with input/output parameters
    auto clbk_lambda_5 = CallbackMaker::makeCallback(
        [](std::string& input) -> std::string {
            input = "Modified inside lambda";
            return "Lambda function 5 processed.";
        });
    std::string lambda_input = "Original value";
    std::cout << clbk_lambda_5(lambda_input) << " | Modified str: " << lambda_input << std::endl;

    // Lambda function with a complex operation
    auto clbk_lambda_6 = CallbackMaker::makeCallback(
        [](double a, double b) -> double {return a * b + 10;});
    std::cout << "Result of lambda 6 (complex operation): " << clbk_lambda_6(3.5, 4.2) << std::endl;

    // Lambda function capturing by reference and modifying variable
    int counter = 0;
    auto clbk_lambda_7 = CallbackMaker::makeCallback(
        [&counter]() {
            counter++;
            std::cout << "Lambda function 7 called, counter: " << counter << std::endl;
        });
    clbk_lambda_7();
    clbk_lambda_7();
    clbk_lambda_7();

    //----------------------------------------------------------------------------------------------------
    // Final log.
    std::cout << strings::generateExampleEnd();
    leobase::system::utils::userInputPause("Press <ENTER> to exit...");

    // Final return.
    return 0;
}

// ---------------------------------------------------------------------------------------------------------------------
