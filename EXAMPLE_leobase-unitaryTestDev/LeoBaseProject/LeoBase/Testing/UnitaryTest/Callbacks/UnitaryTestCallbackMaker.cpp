/*
 * Copyright(C) Milethos Technologies SLU. 2026
 * All rights reserved. Reproduction in whole or in
 * part is prohibited without the written consent of
 * the copyright holder.
 */

//UnitaryTestCallbackMaker.cpp

//
// C++ INCLUDES
#include <functional>
#include <string>
#include <memory>

// LEOBASE INCLUDES

#include <Leobase/Aliases/Testing>
#include <LeoBase/Callbacks/callback_maker.h>
#include <LeoBase/Testing/unitary/unitary_test_macros.h>


using namespace leobase;
using namespace leobase::testing;

// ---------------------------------------------------------------------------------------------------------------------
// 0. Dummy functions to test

void dummyVoidFunction()
{
    // Just a basic void function to ensure it doesn't crash
}

int dummyMathFunction(int a, int b)
{
    return a * b;
}

void dummyReferenceFunction(std::string& outStr, const std::string& appendStr)
{
    outStr += appendStr;
}

// Dummy class for testing

class DummyTarget
{
public:
    int internalState = 0;

    // 1. Basic void method that modifies state
    void modifyState()
    {
        internalState = 42;
    }

    // 2. Method with arguments and return value that uses internal state
    int calculateSum(int a, int b)
    {
        return a + b + internalState;
    }

    // 3. Method using reference parameters
    void appendToString(std::string& baseString, const std::string& suffix)
    {
        baseString += suffix;
        internalState++; // Keep track of how many times it was called
    }
};

//DummyGeneric class
class DummyGenericTarget
{
public:
    int counter = 0;

    // 1. Non-const method returning void
    void increment()
    {
        counter++;
    }

    // 2. Non-const method returning a value
    int getNext()
    {
        return ++counter;
    }

    // 3. CONST method returning a value
    std::string getStatus() const
    {
        return "Counter is " + std::to_string(counter);
    }

};


//Dummy Any Target class

class DummyAnyTarget
{
public:
    int signalCount = 0;

    // 1. Standard void method
    void emitSignal()
    {
        signalCount++;
    }

    // 2. Const method returning a value
    int getSignalCount() const
    {
        return signalCount;
    }
};

class DummyAnyGenericTarget
{
public:
    int value = 0;

    void reset() { value = 0; }
    int getValue() { return value; }
    bool isZero() const { return value == 0; } // Const method
};

//Dummy functions to MakeAnyCallbacks
int anyFreeMultiply(int a, int b)
{
    return a * b;
}

void anyFreeModifyString(std::string& target, const std::string& suffix)
{
    target += suffix;
}

// Dummy Functor for testing operator()
struct DummyFunctor {
    int operator()(int x) const { return x * 2; }
};

// Dummy Class for overloaded methods
class DummyOverloaded {
public:
    int process(int x) { return x + 1; }
    int process(int x, int y) { return x + y; }
};


class BaseDummyClass
{
public:
    virtual ~BaseDummyClass() = default;

    BaseDummyClass() : content("BaseClass"){}


    virtual std::string getClassName() const
    {
        return content;
    }
protected:
    std::string content;
};


class ComplexDummyClass : public BaseDummyClass
{
public:

    ComplexDummyClass():id("1234")
    {
        content = "ComplexClass";
    }

    std::string getClassName() const override
    {
        return content;
    }

    // New method to test move-only arguments (std::unique_ptr)
    void consumeData(std::unique_ptr<int> data)
    {
        if (data) {
            lastValue = *data;
        }
    }

    int getLastValue() const { return lastValue; }

private:
    std::string id;
    int lastValue;
};

// ---------------------------------------------------------------------------------------------------------------------
// 1. Test Declarations

// inline static auto makeCallback(Callable&& callable)
UT_DECLARE_TEST(CallbackMaker_ValidLambda_NoCapture);
UT_DECLARE_TEST(CallbackMaker_ValidLambda_WithCapture);
UT_DECLARE_TEST(CallbackMaker_BindRestriction_Logic);

//inline static std::function<ReturnT(Args...)>
//makeCallback(ReturnT(*freeFunction)(Args...))

UT_DECLARE_TEST(CallbackMaker_FreeFunc_VoidNoArgs);
UT_DECLARE_TEST(CallbackMaker_FreeFunc_ReturnValue);
UT_DECLARE_TEST(CallbackMaker_FreeFunc_PassByReference);

//template<typename ClassT, typename FuncT>
//inline static auto makeCallback(ClassT* object, FuncT memberFunction)

UT_DECLARE_TEST(CallbackMaker_GenericMember_VoidNoArgs);
UT_DECLARE_TEST(CallbackMaker_GenericMember_ReturnNoArgs);
UT_DECLARE_TEST(CallbackMaker_GenericMember_ConstMethod);

//template<typename ClassT, typename FuncT>
//inline static auto makeCallback(ClassT* object, FuncT memberFunction)

UT_DECLARE_TEST(CallbackMaker_MemberFunc_StateMutation);
UT_DECLARE_TEST(CallbackMaker_MemberFunc_ReturnValueAndArgs);
UT_DECLARE_TEST(CallbackMaker_MemberFunc_PassByReference);

//template <typename ReturnT, typename... Args>
//inline static CallbackPair makeAnyCallback(ReturnT(*freeFunction)(Args...))

UT_DECLARE_TEST(CallbackMaker_Any_TypeInformation);
UT_DECLARE_TEST(CallbackMaker_Any_VoidExecution);
UT_DECLARE_TEST(CallbackMaker_Any_ReturnConstExecution);

//template <typename ReturnT>
//inline static CallbackPair makeAnyCallback(ReturnT(*freeFunction)())

UT_DECLARE_TEST(CallbackMaker_AnyFree_TypeInformation);
UT_DECLARE_TEST(CallbackMaker_AnyFree_Execution);
UT_DECLARE_TEST(CallbackMaker_AnyFree_ReferenceForwarding);

//template<typename ClassT, typename FuncT>
//inline static CallbackPair makeAnyCallback(ClassT* object, FuncT memberFunction)

UT_DECLARE_TEST(CallbackMaker_AnyGenericMember_TypeInfo);
UT_DECLARE_TEST(CallbackMaker_AnyGenericMember_Execution);
UT_DECLARE_TEST(CallbackMaker_AnyGenericMember_ConstSupport);

// --- Advanced Edge Cases & Robustness ---
UT_DECLARE_TEST(CallbackMaker_MutableLambda);

// --- Complex Class & Polymorphism ---
UT_DECLARE_TEST(CallbackMaker_Complex_Polymorphism);

// Test polymorphism using std::shared_ptr
UT_DECLARE_TEST(CallbackMaker_Complex_SharedPtr_Polymorphism);

// Test perfect forwarding with move-only types (std::unique_ptr)
UT_DECLARE_TEST(CallbackMaker_Complex_MoveOnly_Forwarding);


// ---------------------------------------------------------------------------------------------------------------------
// 2. Test Definitions

/**
 * @brief Tests that makeCallback works correctly with a simple lambda (no captures).
 */
UT_DEFINE_TEST(CallbackMaker_ValidLambda_NoCapture)
{
    auto myLambda = [](int a, int b) -> int {
        return a + b;
    };

    // Generate the callback
    auto cb = leobase::callbacks::CallbackMaker::makeCallback(myLambda);

    // Verify that it can be invoked and returns the correct result
    UT_EXPECTED_TRUE(cb(5, 7) == 12);
}

/**
 * @brief Tests that makeCallback works correctly with a lambda that captures environment variables.
 */
UT_DEFINE_TEST(CallbackMaker_ValidLambda_WithCapture)
{
    std::string prefix = "Result: ";
    int multiplier = 10;

    auto myLambda = [&prefix, multiplier](int val) -> std::string {
        return prefix + std::to_string(val * multiplier);
    };

    // Generate the callback
    auto cb = leobase::callbacks::CallbackMaker::makeCallback(myLambda);

    // Verify the captured state and result
    UT_EXPECTED_TRUE(cb(5) == "Result: 50");
}

/**
 * @brief Verifies the internal logic of the static_assert to ensure that
 * std::is_bind_expression_v correctly detects std::bind expressions and allows lambdas to pass.
 */
UT_DEFINE_TEST(CallbackMaker_BindRestriction_Logic)
{
    // 1. Valid Case (Lambda)
    auto myLambda = [](int x) { return x; };
    using LambdaType = std::decay_t<decltype(myLambda)>;

    // Verify that the trait evaluates to FALSE (not a bind, would pass the static_assert)
    bool isLambdaABind = std::is_bind_expression_v<LambdaType>;
    UT_EXPECTED_FALSE(isLambdaABind);


    // 2. Invalid Case (std::bind)
    auto dummyFunc = [](int a, int b) { return a + b; };
    auto myBind = std::bind(dummyFunc, 10, std::placeholders::_1); //TODO: DELETE AUTO
    using BindType = std::decay_t<decltype(myBind)>;

    // Verify that the trait evaluates to TRUE (is a bind, would trigger the static_assert)
    bool isBindABind = std::is_bind_expression_v<BindType>;
    UT_EXPECTED_TRUE(isBindABind);
}

/**
 * @brief Tests wrapping a free function that takes no arguments and returns void.
 */
UT_DEFINE_TEST(CallbackMaker_FreeFunc_VoidNoArgs)
{
    // We pass the function pointer directly
    auto cb = leobase::callbacks::CallbackMaker::makeCallback(dummyVoidFunction);

    // It should execute without throwing any exceptions
    UT_EXPECT_NO_THROW(cb());
}

/**
 * @brief Tests wrapping a free function that takes parameters and returns a value.
 */
UT_DEFINE_TEST(CallbackMaker_FreeFunc_ReturnValue)
{
    auto cb = leobase::callbacks::CallbackMaker::makeCallback(dummyMathFunction);

    // Verify the math logic holds up through the callback execution
    UT_EXPECTED_TRUE(cb(4, 5) == 20);
    UT_EXPECTED_TRUE(cb(-2, 3) == -6);
    //TODO: REVISAR POR EQUALL

}

/**
 * @brief Tests wrapping a free function with reference parameters to ensure
 * std::forward perfectly forwards the references inside the lambda.
 */
UT_DEFINE_TEST(CallbackMaker_FreeFunc_PassByReference)
{
    auto cb = leobase::callbacks::CallbackMaker::makeCallback(dummyReferenceFunction);

    std::string testString = "Hello";

    // If std::forward works correctly, this will modify the original 'testString'
    cb(testString, " World");

    UT_EXPECTED_TRUE(testString == "Hello World");
}


/**
 * @brief Tests wrapping a generic non-const member function that returns void and takes no arguments.
 */
UT_DEFINE_TEST(CallbackMaker_GenericMember_VoidNoArgs)
{
    DummyGenericTarget target;

    // Generate the callback using the generic FuncT overload
    auto cb = leobase::callbacks::CallbackMaker::makeCallback(&target, &DummyGenericTarget::increment);

    UT_EXPECTED_TRUE(target.counter == 0);

    // Execute callback
    cb();

    UT_EXPECTED_TRUE(target.counter == 1);
}

/**
 * @brief Tests wrapping a generic non-const member function that returns a value.
 */
UT_DEFINE_TEST(CallbackMaker_GenericMember_ReturnNoArgs)
{
    DummyGenericTarget target;
    target.counter = 10;

    auto cb = leobase::callbacks::CallbackMaker::makeCallback(&target, &DummyGenericTarget::getNext);

    // Should increment the counter to 11 and return the new value
    int result = cb();

    UT_EXPECTED_TRUE(result == 11);
    UT_EXPECTED_TRUE(target.counter == 11);
}

/**
 * @brief Tests wrapping a CONST member function. The generic FuncT deduction
 * seamlessly handles const without needing a dedicated overload.
 */
UT_DEFINE_TEST(CallbackMaker_GenericMember_ConstMethod)
{
    DummyGenericTarget target;
    target.counter = 5;

    // Notice we are passing a pointer to a const method.
    // The previous explicit template would fail here, but this generic one succeeds
    auto cb = leobase::callbacks::CallbackMaker::makeCallback(&target, &DummyGenericTarget::getStatus);

    std::string result = cb();

    UT_EXPECTED_TRUE(result == "Counter is 5");
}


/**
 * @brief Tests wrapping a member function with no arguments, ensuring it correctly
 * targets the specific object instance provided.
 */
UT_DEFINE_TEST(CallbackMaker_MemberFunc_StateMutation)
{
    DummyTarget instanceA;
    DummyTarget instanceB;

    // Create a callback tied specifically to instanceA
    auto cb = leobase::callbacks::CallbackMaker::makeCallback(&instanceA, &DummyTarget::modifyState);

    // Initial state check
    UT_EXPECTED_TRUE(instanceA.internalState == 0);
    UT_EXPECTED_TRUE(instanceB.internalState == 0);

    // Execute callback
    UT_EXPECT_NO_THROW(cb());

    // Verify instanceA was modified, but instanceB was left alone
    UT_EXPECTED_TRUE(instanceA.internalState == 42);
    UT_EXPECTED_TRUE(instanceB.internalState == 0);
}

/**
 * @brief Tests passing arguments to a member function and receiving a return value,
 * ensuring the function has access to the object's member variables.
 */
UT_DEFINE_TEST(CallbackMaker_MemberFunc_ReturnValueAndArgs)
{
    DummyTarget myTarget;
    myTarget.internalState = 10; // Set initial state

    auto cb = leobase::callbacks::CallbackMaker::makeCallback(&myTarget, &DummyTarget::calculateSum);

    // The method does: a + b + internalState
    // 5 + 5 + 10 = 20
    UT_EXPECTED_TRUE(cb(5, 5) == 20);

    // If we change the state directly, the callback should reflect it on the next call
    myTarget.internalState = 100;
    UT_EXPECTED_TRUE(cb(5, 5) == 110);
}

/**
 * @brief Tests perfect forwarding of references within a member function callback.
 */
UT_DEFINE_TEST(CallbackMaker_MemberFunc_PassByReference)
{
    DummyTarget myTarget;
    auto cb = leobase::callbacks::CallbackMaker::makeCallback(&myTarget, &DummyTarget::appendToString);

    std::string text = "C++";

    // Execute the callback
    cb(text, " is awesome");

    // Verify the reference was properly modified
    UT_EXPECTED_TRUE(text == "C++ is awesome");
    // Verify the internal state of the object was updated
    UT_EXPECTED_TRUE(myTarget.internalState == 1);
}

/**
 * @brief Tests that makeAnyCallback correctly populates the CallbackPair
 * with a valid std::any and the correct std::type_index.
 */
UT_DEFINE_TEST(CallbackMaker_Any_TypeInformation)
{
    DummyAnyTarget target;

    // We use C++17 structured bindings to unpack the CallbackPair automatically
    auto [erasedCallback, typeIdx] = leobase::callbacks::CallbackMaker::makeAnyCallback(&target, &DummyAnyTarget::emitSignal);

    // 1. Verify that the std::any actually holds a value
    UT_EXPECTED_TRUE(erasedCallback.has_value());

    // 2. Verify that the type_index matches std::function<void()>
    std::type_index expectedType = std::type_index(typeid(std::function<void()>));
    UT_EXPECTED_TRUE(typeIdx == expectedType); //TODO UT_EXPECT_EQUALL
}

/**
 * @brief Tests extracting the void callback from std::any and executing it.
 */
UT_DEFINE_TEST(CallbackMaker_Any_VoidExecution)
{
    DummyAnyTarget target;
    auto [erasedCallback, typeIdx] = leobase::callbacks::CallbackMaker::makeAnyCallback(&target, &DummyAnyTarget::emitSignal);

    // Extract the concrete std::function from the std::any
    // If the type is wrong, std::any_cast will throw std::bad_any_cast
    auto executableCallback = std::any_cast<std::function<void()>>(erasedCallback);

    UT_EXPECTED_TRUE(target.signalCount == 0);

    // Execute the extracted callback
    executableCallback();

    UT_EXPECTED_TRUE(target.signalCount == 1);
}

/**
 * @brief Tests extracting a const member callback that returns a value from std::any.
 */
UT_DEFINE_TEST(CallbackMaker_Any_ReturnConstExecution)
{
    DummyAnyTarget target;
    target.signalCount = 42; // Set initial state
    //AYW CALLBACK
    auto [erasedCallback, typeIdx] = leobase::callbacks::CallbackMaker::makeAnyCallback(&target, &DummyAnyTarget::getSignalCount);

    // Verify the type_index matches std::function<int()>
    UT_EXPECTED_TRUE(typeIdx == std::type_index(typeid(std::function<int()>)));

    // Extract and execute
    auto executableCallback = std::any_cast<std::function<int()>>(erasedCallback);
    int result = executableCallback();

    UT_EXPECTED_TRUE(result == 42);
}

/**
 * @brief Tests that makeAnyCallback generates the correct std::type_index
 * for a free function with multiple parameters.
 */
UT_DEFINE_TEST(CallbackMaker_AnyFree_TypeInformation)
{
    // Generate the type-erased callback pair
    auto [erasedCallback, typeIdx] = leobase::callbacks::CallbackMaker::makeAnyCallback(anyFreeMultiply);

    // 1. Verify the any object is populated
    UT_EXPECTED_TRUE(erasedCallback.has_value());

    // 2. Verify the type index exactly matches std::function<int(int, int)>
    std::type_index expectedType = std::type_index(typeid(std::function<int(int, int)>));
    UT_EXPECTED_TRUE(typeIdx == expectedType);
}

/**
 * @brief Tests extracting a free function with parameters from std::any and executing it.
 */
UT_DEFINE_TEST(CallbackMaker_AnyFree_Execution)
{
    auto [erasedCallback, typeIdx] = leobase::callbacks::CallbackMaker::makeAnyCallback(anyFreeMultiply);

    // Extract the callable
    // If the signature does not match exactly, std::any_cast will throw
    auto executableCallback = std::any_cast<std::function<int(int, int)>>(erasedCallback);

    // Execute and verify the math logic holds
    int result = executableCallback(6, 7);
    UT_EXPECTED_TRUE(result == 42);
}

/**
 * @brief Tests extracting a free function that takes parameters by reference,
 * ensuring std::forward works perfectly across the type-erasure boundary.
 */
UT_DEFINE_TEST(CallbackMaker_AnyFree_ReferenceForwarding)
{
    auto [erasedCallback, typeIdx] = leobase::callbacks::CallbackMaker::makeAnyCallback(anyFreeModifyString);

    // The signature expects a reference and a const reference
    using ExpectedFuncType = std::function<void(std::string&, const std::string&)>;

    UT_EXPECTED_TRUE(typeIdx == std::type_index(typeid(ExpectedFuncType)));

    // Extract the callable
    auto executableCallback = std::any_cast<ExpectedFuncType>(erasedCallback);

    // Set up test data
    std::string base = "Type ";

    // Execute
    executableCallback(base, "Erasure");

    // Verify the reference was properly modified
    UT_EXPECTED_TRUE(base == "Type Erasure");
}

/**
 * @brief Verifies that the CallbackPair correctly identifies the signature
 * as std::function<void()> even when deduced via generic FuncT.
 */
UT_DEFINE_TEST(CallbackMaker_AnyGenericMember_TypeInfo)
{
    DummyAnyGenericTarget target;
    auto [erased, typeIdx] = leobase::callbacks::CallbackMaker::makeAnyCallback(&target, &DummyAnyGenericTarget::reset);

    // Verify type information
    std::type_index expected = std::type_index(typeid(std::function<void()>));
    bool typesMatch = (typeIdx == expected);
    UT_EXPECTED_TRUE(typesMatch);
    UT_EXPECTED_TRUE(erased.has_value());
}

/**
 * @brief Tests extracting and executing a non-const member function that returns a value.
 */
UT_DEFINE_TEST(CallbackMaker_AnyGenericMember_Execution)
{
    DummyAnyGenericTarget target;
    target.value = 100;

    auto [erased, typeIdx] = leobase::callbacks::CallbackMaker::makeAnyCallback(&target, &DummyAnyGenericTarget::getValue);

    // Cast back to the deduced signature: std::function<int()>
    auto cb = std::any_cast<std::function<int()>>(erased);

    UT_EXPECTED_TRUE(cb() == 100);
}

/**
 * @brief Tests that the generic any-wrapper correctly handles const member functions.
 */
UT_DEFINE_TEST(CallbackMaker_AnyGenericMember_ConstSupport)
{
    DummyAnyGenericTarget target;
    target.value = 0;

    // Wrapping a CONST method
    auto [erased, typeIdx] = leobase::callbacks::CallbackMaker::makeAnyCallback(&target, &DummyAnyGenericTarget::isZero);

    UT_EXPECTED_TRUE(typeIdx == std::type_index(typeid(std::function<bool()>)));

    auto cb = std::any_cast<std::function<bool()>>(erased);
    UT_EXPECTED_TRUE(cb() == true);
}

// =====================================================================================================================
// ADVANCED EDGE CASES & ROBUSTNESS TESTS
// =====================================================================================================================

/**
 * @brief Tests if makeCallback correctly deduces and wraps a mutable lambda
 * that maintains internal state across multiple invocations.
 */
UT_DEFINE_TEST(CallbackMaker_MutableLambda)
{
    int startVal = 0;
    // Note the 'mutable' keyword
    auto countingLambda = [startVal]() mutable -> int {
        startVal += 5;
        return startVal;
    };

    auto cb = leobase::callbacks::CallbackMaker::makeCallback(countingLambda);

    UT_EXPECTED_TRUE(cb() == 5);
    UT_EXPECTED_TRUE(cb() == 10);
    UT_EXPECTED_TRUE(cb() == 15);
}


/**
 * @brief Tests that makeCallback correctly respects polymorphism and dynamic dispatch
 * when a base class pointer is used to register a callback for a derived class object.
 */
UT_DEFINE_TEST(CallbackMaker_Complex_Polymorphism)
{
    // 1. Create the derived object
    ComplexDummyClass derivedObject;

    // 2. Hide it behind a base class pointer (Upcasting)
    BaseDummyClass* basePtr = &derivedObject;

    // 3. Register the callback using the BASE pointer and the BASE method pointer.
    // The CallbackMaker only sees 'BaseDummyClass' at compile time.
    auto cb = leobase::callbacks::CallbackMaker::makeCallback(basePtr, &BaseDummyClass::getClassName);

    // 4. Execute the callback
    std::string result = cb();

    // 5. Verify dynamic conversion
    // correctly calls to ComplexDummyClass::getClassName() at runtime.

    UT_EXPECTED_EQ(result, "ComplexClass");
}

/**
 * @brief Tests that makeCallback works correctly when the object is managed by a shared_ptr,
 * ensuring that dynamic dispatch (polymorphism) still directs the call to the derived class.
 */
UT_DEFINE_TEST(CallbackMaker_Complex_SharedPtr_Polymorphism)
{
    // 1. Create the object inside a shared_ptr (Upcasting to Base)
    std::shared_ptr<BaseDummyClass> smartPtr = std::make_shared<ComplexDummyClass>();

    // 2. Create the callback using the raw pointer from the shared_ptr
    // Even though we pass a BaseDummyClass*, it should call the Complex override.
    auto cb = leobase::callbacks::CallbackMaker::makeCallback(smartPtr.get(), &BaseDummyClass::getClassName);

    // 3. Execute
    std::string result = cb();

    // 4. Verify result and ownership
    UT_EXPECTED_EQ(result, "ComplexClass");
    UT_EXPECTED_TRUE(smartPtr.use_count() == 1); // Callback should not capture/increment reference count
}

/**
 * @brief Tests that the callback correctly forwards a move-only argument (std::unique_ptr).
 * This proves the internal use of std::forward is correct and no copies are attempted.
 */
UT_DEFINE_TEST(CallbackMaker_Complex_MoveOnly_Forwarding)
{
    ComplexDummyClass target;

    // 1. Create callback for the 'consumeData' method
    auto cb = leobase::callbacks::CallbackMaker::makeCallback(&target, &ComplexDummyClass::consumeData);

    // 2. Prepare move-only resource
    auto resource = std::make_unique<int>(999);

    // 3. Invoke the callback by moving the resource
    // If CallbackMaker were to try and copy this, the code would not compile.
    cb(std::move(resource));

    // 4. Verify results
    UT_EXPECTED_TRUE(resource == nullptr); // Resource must be moved out
    UT_EXPECTED_EQ(target.getLastValue(), 999);
}


// ---------------------------------------------------------------------------------------------------------------------
// 3. Execution (if you have a separate executable for this, or add it to your main session)

UT_START_SESSION("CallbackMaker Unitary Tests")

UT_REGISTER_TEST(Callbacks, Maker, CallbackMaker_ValidLambda_NoCapture);
UT_REGISTER_TEST(Callbacks, Maker, CallbackMaker_ValidLambda_WithCapture);
UT_REGISTER_TEST(Callbacks, Maker, CallbackMaker_BindRestriction_Logic);
UT_REGISTER_TEST(Callbacks, Maker, CallbackMaker_FreeFunc_VoidNoArgs);
UT_REGISTER_TEST(Callbacks, Maker, CallbackMaker_FreeFunc_ReturnValue);
UT_REGISTER_TEST(Callbacks, Maker, CallbackMaker_FreeFunc_PassByReference);
UT_REGISTER_TEST(Callbacks, Maker, CallbackMaker_GenericMember_VoidNoArgs)
UT_REGISTER_TEST(Callbacks, Maker, CallbackMaker_GenericMember_ReturnNoArgs)
UT_REGISTER_TEST(Callbacks, Maker, CallbackMaker_GenericMember_ConstMethod)
UT_REGISTER_TEST(Callbacks, Maker, CallbackMaker_MemberFunc_StateMutation);
UT_REGISTER_TEST(Callbacks, Maker, CallbackMaker_MemberFunc_ReturnValueAndArgs);
UT_REGISTER_TEST(Callbacks, Maker, CallbackMaker_MemberFunc_PassByReference);
UT_REGISTER_TEST(Callbacks, Maker, CallbackMaker_Any_TypeInformation)
UT_REGISTER_TEST(Callbacks, Maker, CallbackMaker_Any_VoidExecution)
UT_REGISTER_TEST(Callbacks, Maker, CallbackMaker_Any_ReturnConstExecution)
UT_REGISTER_TEST(Callbacks, Maker, CallbackMaker_AnyFree_TypeInformation)
UT_REGISTER_TEST(Callbacks, Maker, CallbackMaker_AnyFree_Execution)
UT_REGISTER_TEST(Callbacks, Maker, CallbackMaker_AnyFree_ReferenceForwarding)
UT_REGISTER_TEST(Callbacks, Maker, CallbackMaker_AnyGenericMember_TypeInfo);
UT_REGISTER_TEST(Callbacks, Maker, CallbackMaker_AnyGenericMember_Execution);
UT_REGISTER_TEST(Callbacks, Maker, CallbackMaker_AnyGenericMember_ConstSupport);
UT_REGISTER_TEST(Callbacks, Maker, CallbackMaker_MutableLambda);
UT_REGISTER_TEST(Callbacks, Maker, CallbackMaker_Complex_Polymorphism);
UT_REGISTER_TEST(Callbacks, Maker, CallbackMaker_Complex_SharedPtr_Polymorphism);
UT_REGISTER_TEST(Callbacks, Maker, CallbackMaker_Complex_MoveOnly_Forwarding);


UT_RUN_TESTS();

UT_FINISH_SESSION()
