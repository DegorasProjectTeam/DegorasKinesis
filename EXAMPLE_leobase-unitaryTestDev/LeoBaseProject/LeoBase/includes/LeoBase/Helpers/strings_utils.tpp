/*
 *      Copyright(C) Milethos Technologies SLU. 2025
 *      All rights reserved. Reproduction in whole or in
 *      part is prohibited without the written consent of
 *      the copyright holder.
 */

#pragma once

// C++ INCLUDES
#include <iostream>
#include <string>
#include <limits>
#include <sstream>
#include <iomanip>
#include <vector>
#include <algorithm>

// LEOBASE INCLUDES
#include "LeoBase/Global/global_macros.h"

// LEOBASE MACROS
LEOBASE_MODULE_BEGIN(helpers)
BEGIN_NAMESPACE(strings)

// ---------------------------------------------------------------------------------------------------------------------

template <class Container>
void tokenize (Container& result, const std::string& s, const std::string& dlm, bool empties)
{
    result.clear();
    size_t current;
    size_t next = std::numeric_limits<size_t>::max();
    do
    {
        if (!empties)
        {
            next = s.find_first_not_of(dlm, next + 1);

            if (next != std::string::npos)
            {
                current = next;
                next = s.find_first_of(dlm, current);
                result.push_back( s.substr( current, next - current));
            }
        }
        else
        {
            current = next + 1;
            next = s.find_first_of(dlm, current);
            result.push_back(s.substr( current, next - current));
        }
    }
    while (next != std::string::npos);
}

template <class Container>
Container tokenize (const std::string& s, const std::string& dlm, bool empties)
{
    Container result;
    tokenize(result, s, dlm, empties);
    return result;
}

template <typename Container>
std::string join(const Container& strings, const std::string& dlm)
{
    std::ostringstream oss;
    auto it = strings.begin();
    if (it != strings.end()) {
        oss << *it;
        ++it;
    }
    for (; it != strings.end(); ++it) {
        oss << dlm << *it;
    }
    return oss.str();
}


template <typename Iter>
std::string join(Iter begin, Iter end, const std::string& dlm)
{
    std::ostringstream result;
    if (begin != end)
        result << *begin++;
    while (begin != end)
        result << dlm << *begin++;
    return result.str();
}

template<typename T>
std::string numberToFixStr(T x, size_t prec, std::string dlm)
{
    std::ostringstream strout;
    strout << std::showpoint << std::setprecision(prec) << x;
    std::string str = strout.str();
    size_t end = str.find_last_not_of('0') + 1;
    str.erase(end);

    if(str.back() == '.')
        str.pop_back();

    // Replace the decimal separator. This avoids std locals issues.
    std::replace(str.begin(), str.end(), '.', dlm.front());
    std::replace(str.begin(), str.end(), ',', dlm.front());

    return str;
}

template<typename T>
std::string numberToStr(T x, size_t prec, size_t dec_places, bool fixed, char dlm)
{
    std::string final_str;
    std::ostringstream strout;
    std::vector<std::string> aux;
    std::string dplac;
    if(fixed)
        strout<<std::fixed;
    strout << std::showpoint << std::setprecision(prec) << x ;
    std::string str = strout.str() ;
    tokenize(aux, str, ".", true);
    if(aux.size()==2 && aux[1].size() > dec_places)
    {
        dplac = aux[1].substr(0, dec_places);
        final_str = aux[0]+"."+dplac;
    }
    else
    {
        final_str = str;
    }
    //TO DELETE:


    // // Replace the decimal separator. This avoids std locals issues.
    // std::replace(str.begin(), str.end(), '.', dlm);
    // std::replace(str.begin(), str.end(), ',', dlm);
    // // Return the str.
    // return str;

    std::replace(final_str.begin(), final_str.end(), '.', dlm);
    std::replace(final_str.begin(), final_str.end(), ',', dlm);

    return final_str;
}

template<typename T>
std::string numberToMaxDecStr(const T& x, char dlm)
{
    // Container.
    std::ostringstream strout;
    // Set precision to maximum possible for the type
    strout << std::showpoint << std::setprecision(std::numeric_limits<T>::digits10) << x;
    // Convert to string
    std::string str = strout.str();
    // Remove trailing zeros and potential trailing decimal point
    size_t end = str.find_last_not_of('0') + 1;
    if (str[end - 1] == '.')
        --end;
    str.erase(end);
    // Replace the decimal separator. This avoids std locals issues.
    std::replace(str.begin(), str.end(), '.', dlm);
    std::replace(str.begin(), str.end(), ',', dlm);
    // Return the str.
    return str;
}

// ---------------------------------------------------------------------------------------------------------------------

END_NAMESPACE(strings)
LEOBASE_MODULE_END
