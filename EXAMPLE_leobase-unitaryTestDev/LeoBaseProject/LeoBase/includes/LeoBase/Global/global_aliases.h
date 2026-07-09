/*
 *      Copyright(C) Milethos Technologies SLU. 2025
 *      All rights reserved. Reproduction in whole or in
 *      part is prohibited without the written consent of
 *      the copyright holder.
 */

#pragma once

// C++ INCLUDES
#include <optional>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <list>
#include <variant>

// LEOBASE INCLUDES
#include "LeoBase/Global/global_macros.h"

// LEOBASE MACROS
LEOBASE_BEGIN_NAMESPACE

// ---------------------------------------------------------------------------------------------------------------------
// Basic optional alias.

using OptionalStr     = std::optional<std::string>;        ///< Represents an optional string.
using OptionalBool    = std::optional<bool>;               ///< Represents an optional boolean value.
using OptionalLong    = std::optional<long>;               ///< Represents an optional long integer.
using OptionalLLong   = std::optional<long long>;          ///< Represents an optional long integer.
using OptionalInt     = std::optional<int>;                ///< Represents an optional integer.
using OptionalFloat   = std::optional<float>;              ///< Represents an optional floating-point value.
using OptionalDouble  = std::optional<double>;             ///< Represents an optional double-precision floating-point value.
using OptionalLDouble = std::optional<long double>;        ///< Represents an optional double-precision floating-point value.
using OptionalSizeT   = std::optional<size_t>;             ///< Represents an optional size type.
using OptionalU       = std::optional<unsigned>;           ///< Represents an optional unsigned.
using OptionalULong   = std::optional<unsigned long>;      ///< Represents an optional unsigned long.
using OptionalULLong  = std::optional<unsigned long long>; ///< Represents an optional unsigned long long integer.

// ---------------------------------------------------------------------------------------------------------------------
// Basic vector alias.

using StrV     = std::vector<std::string>;        ///< Represents a vector of string.
using BoolV    = std::vector<bool>;               ///< Represents a vector of boolean value.
using LongV    = std::vector<long>;               ///< Represents a vector of long integer.
using LLongV   = std::vector<long long>;          ///< Represents a vector of long integer.
using IntV     = std::vector<int>;                ///< Represents a vector of integer.
using FloatV   = std::vector<float>;              ///< Represents a vector of floating-point value.
using DoubleV  = std::vector<double>;             ///< Represents a vector of double-precision floating-point value.
using LDoubleV = std::vector<long double>;        ///< Represents a vector of double-precision floating-point value.
using SizeTV   = std::vector<size_t>;             ///< Represents a vector of  size type.
using UV       = std::vector<unsigned>;           ///< Represents a vector of unsigned.
using ULongV   = std::vector<unsigned long>;      ///< Represents a vector of unsigned long.
using ULLongV  = std::vector<unsigned long long>; ///< Represents a vector of unsigned long long integer.

// ---------------------------------------------------------------------------------------------------------------------
// Templated optional.

template<typename T>
using OptionalTpl = std::optional<T>; ///< Represents a simple templated optional.

// ---------------------------------------------------------------------------------------------------------------------
// Templated containers optional alias.

/// Represents an optional vector of type `T`.
template<typename T>
using OptionalV = std::optional<std::vector<T>>;

/// Represents an optional map with key type `K` and value type `V`.
template<typename K, typename V>
using OptionalMap = std::optional<std::map<K, V>>;

/// Represents an optional map with key type `K` and value type `V`.
template<typename K, typename V>
using OptionalMMap = std::optional<std::multimap<K, V>>;

/// Represents an optional set of type `T`.
template<typename T>
using OptionalSet = std::optional<std::set<T>>;

/// Represents an optional list of type `T`.
template<typename T>
using OptionalList = std::optional<std::list<T>>;

// ---------------------------------------------------------------------------------------------------------------------
// Specific vector optional alias.

using OptionalStrVec = OptionalV<std::string>; ///< Represents an optional vector of std::string type.

// ---------------------------------------------------------------------------------------------------------------------
// Variant containers.

using StreamableVariant = std::variant<
    std::string,
    const char*,
    char,
    bool,
    short,
    int,
    long,
    long long,
    unsigned short,
    unsigned int,
    unsigned long,
    unsigned long long,
    float,
    double,
    long double,
    std::nullptr_t>;

// ---------------------------------------------------------------------------------------------------------------------
// Usefull containers.

using StreamableFieldValuePair = std::pair<std::string, std::optional<StreamableVariant>>;

using StreamableFieldValuePairV = std::vector<StreamableFieldValuePair>;

// ---------------------------------------------------------------------------------------------------------------------

LEOBASE_END_NAMESPACE
