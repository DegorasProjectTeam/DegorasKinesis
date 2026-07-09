/*
 *      Copyright(C) Milethos Technologies SLU. 2025
 *      All rights reserved. Reproduction in whole or in
 *      part is prohibited without the written consent of
 *      the copyright holder.
 */

#pragma once

// C++ INCLUDES
#include <functional>
#include <type_traits>
#include <sstream>
#include <vector>
#include <array>

// LEOBASE INCLUDES
#include "LeoBase/Global/global_macros.h"

LEOBASE_BEGIN_NAMESPACE

// ---------------------------------------------------------------------------------------------------------------------

template <typename T>
struct function_signature;

template <typename Ret, typename... Args>
struct function_signature<Ret(*)(Args...)>
{
    using type = std::function<Ret(Args...)>;
};

template <typename Callable>
struct function_signature
{
private:
    using call_type = decltype(&Callable::operator());
    using traits = function_signature<call_type>;
public:
    using type = typename traits::type;
};

template <typename Ret, typename ClassType, typename... Args>
struct function_signature<Ret(ClassType::*)(Args...) const>
{
    using type = std::function<Ret(Args...)>;
};

template <typename Ret, typename ClassType, typename... Args>
struct function_signature<Ret(ClassType::*)(Args...)>
{
    using type = std::function<Ret(Args...)>;
};

template <typename T>
struct is_std_function : std::false_type {};

template <typename Ret, typename... Args>
struct is_std_function<std::function<Ret(Args...)>> : std::true_type {};

template<typename T, typename = void>
struct underlying_type {using type = T;};

template<typename T>
using underlying_type_t = typename underlying_type<T>::type;

template<typename T>
struct is_floating : std::integral_constant<bool, std::is_floating_point_v<T>>{};

template<typename T>
inline constexpr bool is_floating_v = is_floating<T>::value;

template<typename T>
struct is_numeric : std::integral_constant<bool, std::is_integral_v<T> ||
                                           std::is_floating_point_v<T>>{};

template<typename T>
inline constexpr bool is_numeric_v = is_numeric<T>::value;

template<typename T1, typename T2,  typename = void>
struct both_floating : std::false_type {};

template<typename T1, typename T2>
struct both_floating<T1, T2, std::enable_if_t<
                                 (std::is_floating_point_v<T1>) &&
                                 (std::is_floating_point_v<T2>)>> : std::true_type {};

template<typename T1, typename T2>
inline constexpr bool both_floating_v = both_floating<T1, T2>::value;

template<typename T, typename U, typename = void>
struct same_arithmetic_category : std::false_type {};

template<typename T, typename U>
struct same_arithmetic_category<T, U, std::enable_if_t<
                                          (std::is_floating_point_v<T>) &&
                                          (std::is_floating_point_v<U>)>> : std::true_type {};

template<typename T, typename U>
struct same_arithmetic_category<T, U, std::enable_if_t<
                                          (std::is_integral_v<T>) &&
                                          (std::is_integral_v<U>)>> : std::true_type {};

template<typename T, typename U>
inline constexpr bool same_arithmetic_category_v = same_arithmetic_category<T, U>::value;

template <typename T>
struct is_container : std::false_type {};

template <typename... Args>
struct is_container<std::vector<Args...>> : std::true_type {};

template <typename T, size_t N>
struct is_container<std::array<T, N>> : std::true_type {};

template<typename T, typename = void>
struct is_streamable : std::false_type {};

template<typename T>
struct is_streamable<T, std::void_t<decltype(std::declval<std::ostringstream&>()
                                             << std::declval<T>())>> : std::true_type {};

template<typename T>
inline constexpr bool is_streamable_v = is_streamable<T>::value;

// ---------------------------------------------------------------------------------------------------------------------

LEOBASE_END_NAMESPACE
