/*
 *      Copyright(C) Milethos Technologies SLU. 2025
 *      All rights reserved. Reproduction in whole or in
 *      part is prohibited without the written consent of
 *      the copyright holder.
 */

// C++ INCLUDES
#include <iostream>
#include <chrono>
#include <thread>

// LEOBASE INCLUDES
#include <LeoBase/Aliases/System>
#include <LeoBase/Aliases/Callbacks>
#include <LeoBase/Aliases/Helpers>

// ---------------------------------------------------------------------------------------------------------------------

// LeoBase namespaces.
using namespace leobase::helpers;
using leobase::callbacks::CallbackHandler;
using leobase::callbacks::CallbackMaker;
using leobase::callbacks::CallbackPair;

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
 * @brief Main entry point of the program `ExampleCallbackHandler_2`.
 */
int main()
{
    // Initial log.
    std::cout << strings::generateExampleTitle("Example CallbackHandler 2");

    //----------------------------------------------------------------------------------------------------

    // Handler and example object.
    CallbackHandler handler;
    Example example_obj;

    //----------------------------------------------------------------------------------------------------

    // Example 1: <CallbackHandler> usages for free functions using makeCallback.
    std::cout << "Example 1: <CallbackHandler> usages for free functions using makeCallback." << std::endl;

    // Register free function without parameters.
    handler.registerCallback("freeFunc1_example1", CallbackMaker::makeCallback(&freeFunction1));
    handler.invokeSync("freeFunc1_example1");

    // Register free function with parameters.
    using freeFunc2_example1_type = std::function<void(int, const std::string&)>;
    handler.registerCallback("freeFunc2_example1", CallbackMaker::makeCallback(&freeFunction2));
    handler.invokeSync<freeFunc2_example1_type>("freeFunc2_example1", 42, "Hello Callback");

    // Register free function with return value.
    using freeFunc3_example1_type = std::function<std::string()>;
    handler.registerCallback("freeFunc3_example1", CallbackMaker::makeCallback(&freeFunction3));
    std::cout << handler.invokeSync<freeFunc3_example1_type>("freeFunc3_example1") << std::endl;

    // Register overloaded free function.
    using freeAdd1_example1_type = std::function<double(double, double)>;
    handler.registerCallback("freeAdd1_example1",
                             CallbackMaker::makeCallback<double, double, double>(&freeAddFunction));
    double res_1 = handler.invokeSync<freeAdd1_example1_type>("freeAdd1_example1", 5.5, 2.5);
    std::cout << "Result of freeAdd1_example1: " << res_1 << std::endl;

    // Register overloaded free function.
    using freeAdd2_example1_type = std::function<void(double, double, double&)>;
    handler.registerCallback("freeAdd2_example1",
                             CallbackMaker::makeCallback<void, double, double, double&>(&freeAddFunction));
    double res_2;
    handler.invokeSync<freeAdd2_example1_type>("freeAdd2_example1", 10.5, 20.5, res_2);
    std::cout << "Result of freeAdd2_example1: " << res_2 << std::endl;

    //----------------------------------------------------------------------------------------------------

    // Example 2: <CallbackHandler> usages for member functions using makeCallback.
    std::cout << std::endl << "Example 2: <CallbackHandler> usages for member functions using makeCallback." << std::endl;

    // Register member function without parameters.
    handler.registerCallback("memberFunc1_example2", CallbackMaker::makeCallback(&example_obj, &Example::memberFunction1));
    handler.invokeSync("memberFunc1_example2");

    // Register member function with parameters.
    using memberFunc2_example2_type = std::function<void(int, const std::string&)>;
    handler.registerCallback("memberFunc2_example2", CallbackMaker::makeCallback(&example_obj, &Example::memberFunction2));
    handler.invokeSync<memberFunc2_example2_type>("memberFunc2_example2", 42, "Hello Callback");

    // Register member function with return value.
    using memberFunc3_example2_type = std::function<std::string()>;
    handler.registerCallback("memberFunc3_example2", CallbackMaker::makeCallback(&example_obj, &Example::memberFunction3));
    std::cout << handler.invokeSync<memberFunc3_example2_type>("memberFunc3_example2") << std::endl;

    // Register overloaded member function.
    using memberAdd1_example2_type = std::function<double(double, double)>;
    handler.registerCallback("memberAdd1_example2",
                             CallbackMaker::makeCallback<double, double, double>(&example_obj, &Example::memberAddFunction));
    double res_3 = handler.invokeSync<memberAdd1_example2_type>("memberAdd1_example2", 1.5, 6.5);
    std::cout << "Result of memberAdd1_example2: " << res_3 << std::endl;

    // Register overloaded free function.
    using memberAdd2_example2_type = std::function<void(double, double, double&)>;
    handler.registerCallback("memberAdd2_example2",
                             CallbackMaker::makeCallback<void, double, double, double&>(&freeAddFunction));
    double res_4;
    handler.invokeSync<memberAdd2_example2_type>("memberAdd2_example2", 10.5, 20.5, res_4);
    std::cout << "Result of memberAdd2_example2: " << res_2 << std::endl;

    //----------------------------------------------------------------------------------------------------

    // Example 3: <CallbackHandler> usages for member functions using makeAnyCallback.
    std::cout << std::endl << "Example 3: <CallbackHandler> usages for functions using makeAnyCallback." << std::endl;

    // Example of identifiers.
    enum class FuncCallbacks
    {
        MemberFunc1,
        MemberFunc2,
        FreeFunc1,
        FreeFunc2,
        LambdaFunc1,
        LambdaFunc2,
        LambdaFunc3,
        LambdaFunc4,
        LambdaMemberFunc2Placeholder,
        MemberAddFunc1
    };

    // Prepare an intermediate map for delayed registration.
    std::unordered_map<FuncCallbacks, CallbackPair> intermediate_map;

    // Register member function without parameters in the intermediate map.
    intermediate_map.emplace(FuncCallbacks::MemberFunc1,
                             CallbackMaker::makeAnyCallback(&example_obj, &Example::memberFunction1));

    // Register member function without parameters in the intermediate map.
    intermediate_map.emplace(FuncCallbacks::MemberFunc2,
                             CallbackMaker::makeAnyCallback(&example_obj, &Example::memberFunction2));

    // Register free function without parameters in the intermediate map.
    intermediate_map.emplace(FuncCallbacks::FreeFunc1,
                             CallbackMaker::makeAnyCallback(&freeFunction1));

    // Register free function without parameters in the intermediate map.
    intermediate_map.emplace(FuncCallbacks::FreeFunc2,
                             CallbackMaker::makeAnyCallback(&freeFunction2));

    // Register lambda function without parameters.
    intermediate_map.emplace(FuncCallbacks::LambdaFunc1,
                             CallbackMaker::makeAnyCallback([]()
                                {std::cout << "Lambda function 1 called." << std::endl;}));

    // Register lambda function with parameters.
    auto lambda1 = [](int x, const std::string& str) {
        std::cout << "Lambda function 2 called with values: " << x << ", " << str << std::endl;};
    intermediate_map.emplace(FuncCallbacks::LambdaFunc2, CallbackMaker::makeAnyCallback(lambda1));

    // Register lambda function with return type.
    auto lambda2 = [](double a, double b) -> double {return a * b;};
    intermediate_map.emplace(FuncCallbacks::LambdaFunc3, CallbackMaker::makeAnyCallback(lambda2));

    // Register lambda function with return type.
    auto lambda3 = [&example_obj](int a) {return example_obj.memberFunction2(a, "Placeholder");};
    intermediate_map.emplace(FuncCallbacks::LambdaMemberFunc2Placeholder, CallbackMaker::makeAnyCallback(lambda3));

    // Register lambda function with sleep.
    auto lambda4 = [](const std::chrono::milliseconds& ms) -> std::string {
        std::this_thread::sleep_for(ms);
        return std::string("Done"); };
    intermediate_map.emplace(FuncCallbacks::LambdaFunc4, CallbackMaker::makeAnyCallback(lambda4));

    // Register member function with overload.
    auto cb_aux = CallbackMaker::makeAnyCallback<double, double, double>(&example_obj, &Example::memberAddFunction);
    intermediate_map.emplace(FuncCallbacks::MemberAddFunc1, cb_aux);

    // Register in the final handler all functions in the intermediate map.
    handler.registerCallbacks(intermediate_map);

    // Function types.
    using MemberFunc2Type = std::function<void(int, const std::string&)>;
    using MemberAddFunc1 = std::function<double(double, double)>;
    using FreeFunc2Type = std::function<void(int, const std::string&)>;
    using LambdaFunc2Type = std::function<void(int, const std::string&)>;
    using LambdaFunc3Type = std::function<double(double, double)>;
    using LambdaFunc4Type = std::function<std::string(const std::chrono::milliseconds&)>;
    using LambdaMemberFunc2PlaceholderType = std::function<void(int)>;;

    // Invoke the callbacks in the handler (sync mode).
    handler.invokeSync(FuncCallbacks::MemberFunc1);
    handler.invokeSync<MemberFunc2Type>(FuncCallbacks::MemberFunc2, 42, "Hello Callback");
    handler.invokeSync(FuncCallbacks::FreeFunc1);
    handler.invokeSync<FreeFunc2Type>(FuncCallbacks::FreeFunc2, 42, "Hello Callback");
    handler.invokeSync(FuncCallbacks::LambdaFunc1);
    handler.invokeSync<LambdaFunc2Type>(FuncCallbacks::LambdaFunc2, 123, "Lambda Callback");
    double result = handler.invokeSync<LambdaFunc3Type>(FuncCallbacks::LambdaFunc3, 5.0, 2.0);
    std::cout << "Result of LambdaFunc3: " << result << std::endl;
    handler.invokeSync<LambdaMemberFunc2PlaceholderType>(FuncCallbacks::LambdaMemberFunc2Placeholder, 21);
    result = handler.invokeSync<MemberAddFunc1>(FuncCallbacks::MemberAddFunc1, 12, 1.8);
    std::cout << "Result of MemberAddFunc1: " << result << std::endl;

    // Invoke the callbacks in the handler (async mode).
    std::future<double> res_fut = handler.invokeAsync<LambdaFunc3Type>(FuncCallbacks::LambdaFunc3, 5.0, 2.0);
    res_fut.wait();
    std::cout << "Result of LambdaFunc3 async: " << res_fut.get() << std::endl;

    // Invoke the callbacks in the handler (async mode) with sleep.
    std::future<std::string> res_sleep = handler.invokeAsync<LambdaFunc4Type>(FuncCallbacks::LambdaFunc4, std::chrono::milliseconds(1000));
    std::cout << "Waiting the result of LambdaFunc4 sleep. " << std::endl;
    res_sleep.wait();
    std::cout << "Result of LambdaFunc4 sleep: " << res_sleep.get() << std::endl;

    //----------------------------------------------------------------------------------------------------
    // Final log.
    std::cout << strings::generateExampleEnd();
    leobase::system::utils::userInputPause("Press <ENTER> to exit...");

    // Final return.
    return 0;
}

// ---------------------------------------------------------------------------------------------------------------------
