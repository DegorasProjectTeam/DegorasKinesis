/*
 *      Copyright(C) Milethos Technologies SLU. 2025
 *      All rights reserved. Reproduction in whole or in
 *      part is prohibited without the written consent of
 *      the copyright holder.
 */

#pragma once

// C++ INCLUDES
#include <iterator>
#include <ostream>
#include <vector>
#include <map>

// LEOBASE INCLUDES
#include "LeoBase/Global/global_macros.h"

// LEOBASE MACROS
LEOBASE_MODULE_BEGIN(containers)
BEGIN_NAMESPACE(operators)
// =====================================================================================================================
// FORWARD DECLARATION (TO TEST)
// =====================================================================================================================

template <class T>
std::ostream& operator<<(std::ostream& out, const std::vector<T>& v);

template <class A, class B>
std::ostream& operator<<(std::ostream& out, const std::pair<A, B>& p);

template <class K, class V>
std::ostream& operator<<(std::ostream& out, const std::map<K, V>& m);

template <class K, class V>
std::ostream& operator<<(std::ostream& out, const std::multimap<K, V>& mm);

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Overload of `operator<<` to print `std::vector<T>` in human-readable format.
 * @tparam T Type of the elements inside the vector.
 * @param out Output stream.
 * @param v Vector to print.
 * @return Modified output stream.
 */
template <class T>
std::ostream& operator<<(std::ostream& out, const std::vector<T>& v)
{
    out << '[';
    for (size_t i = 0; i < v.size(); ++i)
    {
        out << v[i];
        if (i != v.size() - 1)
            out << ", ";
    }
    out << ']';
    return out;
}

/**
 * @brief Overload of `operator<<` to print `std::pair<A, B>` in human-readable format.
 * @tparam A Type of the first element.
 * @tparam B Type of the second element.
 * @param out Output stream.
 * @param p Pair to print.
 * @return Modified output stream.
 */
template <class A, class B>
std::ostream& operator<<(std::ostream& out, const std::pair<A, B>& p)
{
    out << "(" << p.first << ", " << p.second << ")";
    return out;
}

/**
 * @brief Overload of `operator<<` to print `std::map<K, V>` in human-readable format.
 * @tparam K Type of the keys.
 * @tparam V Type of the values.
 * @param out Output stream.
 * @param m Map to print.
 * @return Modified output stream.
 */
template <class K, class V>
std::ostream& operator<<(std::ostream& out, const std::map<K, V>& m)
{
    out << "{";
    for (auto it = m.begin(); it != m.end(); ++it)
    {
        out << it->first << ": " << it->second;
        if (std::next(it) != m.end())
            out << ", ";
    }
    out << "}";
    return out;
}

/**
 * @brief Overload of `operator<<` to print `std::multimap<K, V>` in human-readable format.
 * @tparam K Type of the keys.
 * @tparam V Type of the values.
 * @param out Output stream.
 * @param mm Multimap to print.
 * @return Modified output stream.
 */
template <class K, class V>
std::ostream& operator<<(std::ostream& out, const std::multimap<K, V>& mm)
{
    out << "{";
    for (auto it = mm.begin(); it != mm.end(); ++it)
    {
        out << it->first << ": " << it->second;
        if (std::next(it) != mm.end())
            out << ", ";
    }
    out << "}";
    return out;
}

// ---------------------------------------------------------------------------------------------------------------------

END_NAMESPACE(operators)
LEOBASE_MODULE_END
