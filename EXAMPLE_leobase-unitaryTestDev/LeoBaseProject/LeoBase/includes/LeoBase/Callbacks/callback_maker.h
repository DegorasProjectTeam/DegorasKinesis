/*
 *      Copyright(C) Milethos Technologies SLU. 2025
 *      All rights reserved. Reproduction in whole or in
 *      part is prohibited without the written consent of
 *      the copyright holder.
 */

#pragma once

// C++ INCLUDES
#include <functional>
#include <any>
#include <type_traits>
#include <utility>
#include <typeindex>

// LEOBASE INCLUDES
#include "LeoBase/Aliases/Global"
#include "LeoBase/Callbacks/callback_types.h"
#include "LeoBase/Patterns/non_instantiable.h"

// LEOBASE MACROS
LEOBASE_MODULE_BEGIN(callbacks)

// ---------------------------------------------------------------------------------------------------------------------

class LEOBASE_EXPORT CallbackMaker final : public patterns::NonInstantiable
{

public:

    template <typename Callable>
    inline static auto makeCallback(Callable&& callable)
    {
        static_assert(!std::is_bind_expression_v<std::decay_t<Callable>>,
                      "[LeoBase,Callbacks,CallbackMaker::makeCallback] "
                      "std::bind expressions are not supported. Use lambdas instead.");

        using FunctionType = typename function_signature<std::decay_t<Callable>>::type;
        return FunctionType(std::forward<Callable>(callable));
    }

    template <typename ReturnT, typename... Args>
    inline static std::function<ReturnT(Args...)> makeCallback(ReturnT(*freeFunction)(Args...))
    {
        return std::function<ReturnT(Args...)>(
            [freeFunction](Args... args) -> ReturnT
            {
                return freeFunction(std::forward<Args>(args)...);
            }
            );
    }

    template<typename ClassT, typename FuncT>
    inline static auto makeCallback(ClassT* object, FuncT memberFunction)
    {
        using ReturnT = decltype((object->*memberFunction)());
        return std::function<ReturnT()>(
            [object, memberFunction](auto&&... args) -> ReturnT
            {
                return (object->*memberFunction)(std::forward<decltype(args)>(args)...);
            }
            );
    }

    template <typename ReturnT, typename... Args, typename ClassT>
    inline static std::function<ReturnT(Args...)> makeCallback(ClassT* object, ReturnT(ClassT::*memberFunction)(Args...))
    {
        return std::function<ReturnT(Args...)>(
            [object, memberFunction](Args... args) -> ReturnT
            {
                return (object->*memberFunction)(std::forward<Args>(args)...);
            }
            );
    }

    template <typename Callable>
    inline static CallbackPair makeAnyCallback(Callable&& callable)
    {
        static_assert(!std::is_bind_expression_v<std::decay_t<Callable>>,
                      "[LeoBase,Callbacks,CallbackMaker::makeAnyCallback] "
                      "std::bind expressions are not supported. Use lambdas instead.");

        using FunctionType = typename leobase::function_signature<std::decay_t<Callable>>::type;
        auto callback = FunctionType(std::forward<Callable>(callable));

        return { std::any(callback), std::type_index(typeid(FunctionType)) };
    }

    // For free functions with no parameters
    template <typename ReturnT>
    inline static CallbackPair makeAnyCallback(ReturnT(*freeFunction)())
    {
        using FunctionType = std::function<ReturnT()>;

        auto callback = FunctionType(
            [freeFunction]() -> ReturnT
            {
                return freeFunction();
            });

        return { std::any(callback), std::type_index(typeid(FunctionType)) };
    }

    // For free functions with parameters
    template <typename ReturnT, typename... Args>
    inline static CallbackPair makeAnyCallback(ReturnT(*freeFunction)(Args...))
    {
        using FunctionType = std::function<ReturnT(Args...)>;

        auto callback = FunctionType(
            [freeFunction](Args... args) -> ReturnT
            {
                return freeFunction(std::forward<Args>(args)...);
            });

        return { std::any(callback), std::type_index(typeid(FunctionType)) };
    }

    template<typename ClassT, typename FuncT>
    inline static CallbackPair makeAnyCallback(ClassT* object, FuncT memberFunction)
    {
        using ReturnT = decltype((object->*memberFunction)());
        using FunctionType = std::function<ReturnT()>;

        auto callback = FunctionType(
            [object, memberFunction](auto&&... args) -> ReturnT
            {
                return (object->*memberFunction)(std::forward<decltype(args)>(args)...);
            });

        return { std::any(callback), std::type_index(typeid(FunctionType)) };
    }

    template <typename ReturnT, typename... Args, typename ClassT>
    inline static CallbackPair makeAnyCallback(ClassT* object, ReturnT(ClassT::*memberFunction)(Args...))
    {
        using FunctionType = std::function<ReturnT(Args...)>;

        auto callback = FunctionType(
            [object, memberFunction](Args... args) -> ReturnT
            {
                return (object->*memberFunction)(std::forward<Args>(args)...);
            });

        return { std::any(callback), std::type_index(typeid(FunctionType)) };
    }
};

// ---------------------------------------------------------------------------------------------------------------------

LEOBASE_MODULE_END
