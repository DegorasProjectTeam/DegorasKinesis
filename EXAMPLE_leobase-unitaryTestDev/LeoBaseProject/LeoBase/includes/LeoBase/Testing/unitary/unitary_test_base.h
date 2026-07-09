/*
 *      Copyright(C) Milethos Technologies SLU. 2025
 *      All rights reserved. Reproduction in whole or in
 *      part is prohibited without the written consent of
 *      the copyright holder.
 */

#pragma once

// C++ INCLUDES
#include <atomic>
#include <iostream>
#include <vector>
#include <functional>
#include <sstream>
#include <type_traits>
#include <utility>
#include <chrono>
#include <iomanip>

// LEOBASE INCLUDES
#include "LeoBase/Aliases/Global"
#include "LeoBase/Helpers/strings_utils.h"

// LEOBASE MACROS
LEOBASE_MODULE_BEGIN(testing)

// ---------------------------------------------------------------------------------------------------------------------

using CheckResultsV = std::vector<std::tuple<unsigned, bool, std::string>>;

class LEOBASE_EXPORT UnitaryTestBase
{

protected:

    UnitaryTestBase(const std::string& name);

public:

    virtual void runTest() = 0;

    virtual ~UnitaryTestBase();

    void setForceStreamData(bool enable);

    void setStopOnSubtestFail(bool enable);

    bool forceFail();

    bool forcePass();

    bool expectTrue(bool result);

    bool expectFalse(bool result);

    bool expectEQ(const std::string& str1, const std::string& str2);

    bool expectNE(const std::string& str1, const std::string& str2);

    bool expectEQ(const char* str1, const char* str2);

    bool expectNE(const char* str1, const char* str2);

    void sleepUs(long long us) const;

    //Exception throw statements
    bool checkAnyThrow(std::function<void()> statement);

    bool checkNoThrow(std::function<void()> statement);


    template<typename TException, typename TCallable>
    bool expectThrow(TCallable&& callable, const char* statement_str, const char* exception_str)
    {
        bool result = false;
        std::string log;
        std::cout<<"                           ";
        std::cout <<"- Expecting throw: " << exception_str << std::endl;

        try
        {
            std::forward<TCallable>(callable)();
            result = false;
            log = std::string("No thrown. Expected: ") + exception_str + " | Statement: " + statement_str;
        }
        catch(const TException& e)
        {
            result = true;
            log = std::string("Caught expected: ") + exception_str + " | Statement: " + statement_str;

        }
        this->updateCheckResults(result, log);
        return result;
    }

    //AnyThrow

    template<typename TCallable>
    bool expectAnyThrow(TCallable&& callable)
    {
        bool result = false;
        std::string log;
        try {
            std::forward<TCallable>(callable)();
            result = false;
            log = std::string("No thrown. Expected ANY exception");
        } catch (...) {
            result = true;
            log = std::string("Caught exception (as expected)");
        }
        this->updateCheckResults(result,log);
        return result;
    }

    //NoThrow

    template<typename TCallable>
    bool expectNoThrow(TCallable&& callable)
    {
        bool result = false;
        std::string log;
        try {
            std::forward<TCallable>(callable)();
            result = true; //Doesn't throws an exception
            log = "No thrown (Success)";
        } catch (...) {
            result = false;
            log = "Unexpected exception thrown in checkNoThrow";
        }
        this->updateCheckResults(result,log);
        return result;
    }


    template <typename T>
    typename std::enable_if_t<std::is_class_v<T>, bool>
    expectEQ(const T& obj1, const T& obj2)
    {
        std::cout << std::setw(27) << " ";
        std::cout << "- Comparing struct/class equality" << std::endl;
        std::cout << std::flush;

        bool result = (obj1 == obj2);
        this->updateCheckResults(result, obj1, obj2);
        return result;
    }

    template <typename T>
    typename std::enable_if_t<std::is_class_v<T>, bool>
    expectNE(const T& obj1, const T& obj2)
    {
        std::cout << std::setw(27) << " ";
        std::cout << "- Comparing struct/class inequality" << std::endl;
        std::cout << std::flush;

        bool result = (obj1 != obj2);
        this->updateCheckResults(result, obj1, obj2);
        return result;
    }

    template<typename T>
    typename std::enable_if_t<std::is_integral_v<T>, bool>
    expectEQ(const T& arg1, const T& arg2)
    {
        std::cout << std::setw(27) << " ";
        std::cout << "- Comparing integrals equality"<<std::endl;
        std::cout << std::flush;
        bool result = (arg1 == arg2);
        this->updateCheckResults(result, arg1, arg2);
        return result;
    }

    template<typename T>
    typename std::enable_if_t<std::is_integral_v<T>, bool>
    expectNE(const T& arg1, const T& arg2)
    {
        std::cout << std::setw(27) << " ";
        std::cout << "- Comparing integrals inequality"<<std::endl;
        std::cout << std::flush;
        bool result = (arg1 != arg2);
        this->updateCheckResults(result, arg1, arg2);
        return result;
    }

    template<typename T>
    typename std::enable_if_t<std::is_floating_point_v<T>, bool>
    expectEQ(const T& arg1, const T& arg2, const T& tolerance = std::numeric_limits<T>::epsilon())
    {
        std::cout << std::setw(27) << " ";
        std::cout <<"- Comparing floats equality" << std::endl;
        std::cout << std::flush;
        bool result = std::abs(arg1 - arg2) <= tolerance;
        this->updateCheckResults(result, arg1, arg2);
        return result;
    }

    template<typename T>
    typename std::enable_if_t<std::is_floating_point_v<T>, bool>
    expectNE(const T& arg1, const T& arg2, const T& tolerance = std::numeric_limits<T>::epsilon())
    {
        std::cout << std::setw(27) << " ";
        std::cout <<"- Comparing floats inequality" << std::endl;
        std::cout << std::flush;
        bool result = std::abs(arg1 - arg2) > tolerance;
        this->updateCheckResults(result, arg1, arg2);
        return std::abs(arg1 - arg2) > tolerance;
    }

    // Duration types equality.
    template<typename Rep, typename Period>
    bool expectEQ(const std::chrono::duration<Rep, Period>& dur1, const std::chrono::duration<Rep, Period>& dur2)
    {
        std::cout << std::setw(27) << " ";
        std::cout <<"- Comparing durations equality" << std::endl;
        std::cout << std::flush;
        bool result = (dur1 == dur2);
        this->updateCheckResults(result, dur1, dur2);
        return result;
    }

    // Duration types inequality.
    template<typename Rep, typename Period>
    bool expectNE(const std::chrono::duration<Rep, Period>& dur1, const std::chrono::duration<Rep, Period>& dur2)
    {
        std::cout << std::setw(27) << " ";
        std::cout <<"- Comparing durations inequality" << std::endl;
        std::cout << std::flush;
        bool result = (dur1 != dur2);
        this->updateCheckResults(result, dur1, dur2);
        return result;
    }

    template<typename T>
    typename std::enable_if_t<!std::is_floating_point_v<T>, bool>
    expectEQ(const std::vector<T>& v1, const std::vector<T>& v2)
    {
        std::cout << std::setw(27) << " ";
        std::cout <<"- Comparing vector equality (non floating point)" << std::endl;
        std::cout << std::flush;

        if (v1.size() != v2.size())
        {
            this->updateCheckResults(false, v1, v2);
            return false;
        }

        for (size_t i = 0; i < v1.size(); ++i)
        {
            if (v1[i] != v2[i])
            {
                this->updateCheckResults(false, v1, v2);
                return false;
            }
        }
        this->updateCheckResults(true, v1, v2);
        return true;
    }

    template<typename T>
    typename std::enable_if_t<!std::is_floating_point_v<T>, bool>
    expectNE(const std::vector<T>& v1, const std::vector<T>& v2)
    {
        std::cout << std::setw(27) << " ";
        std::cout <<"- Comparing vector inequality (non floating point)" << std::endl;
        std::cout << std::flush;

        if (v1.size() != v2.size())
        {
            this->updateCheckResults(true, v1, v2);
            return true;
        }

        for (size_t i = 0; i < v1.size(); ++i)
        {
            if (v1[i] != v2[i])
            {
                this->updateCheckResults(true, v1, v2);
                return true;
            }
        }
        this->updateCheckResults(false, v1, v2);
        return false;
    }

    template<typename T>
    typename std::enable_if_t<
        std::is_floating_point_v<T>, bool>
    expectEQ(const std::vector<T>& v1, const std::vector<T>& v2,  const T& tol = std::numeric_limits<T>::epsilon())
    {
        std::cout << std::setw(27) << " ";
        std::cout <<"- Comparing vector equality (floating point)" << std::endl;
        std::cout << std::flush;

        if (v1.size() != v2.size())
        {
            this->updateCheckResults(false, v1, v2);
            return false;
        }

        for (size_t i = 0; i < v1.size(); ++i)
        {
            if (std::abs(v1[i] - v2[i]) > tol)
            {
                this->updateCheckResults(false, v1, v2);
                return false;
            }
        }
        this->updateCheckResults(true, v1, v2);
        return true;
    }

    template <typename T, size_t N>
    typename std::enable_if_t<!std::is_floating_point_v<T>, bool>
    expectEQ(const std::array<T, N>& arr1, const std::array<T, N>& arr2)
    {
        std::cout << std::setw(27) << " ";
        std::cout <<"- Comparing array equality (non floating point)" << std::endl;
        std::cout << std::flush;

        for (size_t i = 0; i < N; ++i)
        {
            if (arr1[i] != arr2[i])
            {
                this->updateCheckResults(false, arr1, arr2);
                return false;
            }
        }
        this->updateCheckResults(true, arr1, arr2);
        return true;
    }

    template <typename T, size_t N>
    typename std::enable_if_t<!std::is_floating_point_v<T>, bool>
    expectNE(const std::array<T, N>& arr1, const std::array<T, N>& arr2)
    {
        std::cout << std::setw(27) << " ";
        std::cout <<"- Comparing array inequality (non floating point)" << std::endl;
        std::cout << std::flush;

        for (size_t i = 0; i < N; ++i)
        {
            if (arr1[i] != arr2[i])
            {
                this->updateCheckResults(true, arr1, arr2);
                return true;
            }
        }
        this->updateCheckResults(false, arr1, arr2);
        return false;
    }

    template <typename T, size_t N>
    typename std::enable_if_t<std::is_floating_point_v<T>, bool>
    expectEQ(const std::array<T, N>& arr1, const std::array<T, N>& arr2,
             const T& tol = std::numeric_limits<T>::epsilon())
    {
        std::cout << std::setw(27) << " ";
        std::cout <<"- Comparing array equality (floating point)" << std::endl;
        std::cout << std::flush;

        // TODO PARALELIZAR COMPROBACIONES ARRAYS/MATRICES/ETC

        for (size_t i = 0; i < N; ++i)
        {
            if (std::abs(arr1[i] - arr2[i]) > tol)
            {
                this->updateCheckResults(false, arr1, arr2);
                return false;
            }
        }
        this->updateCheckResults(true, arr1, arr2);
        return true;
    }

    // Custom checks.
    template<typename... Args>
    bool customCheck(const std::function<bool(const Args&...)>& checkFunction, const Args&... args)
    {
        std::cout << std::setw(27) << " ";
        std::cout<<"- Customized function evaluation"<<std::endl;
        std::cout << std::flush;
        bool result = checkFunction(args...);
        this->updateCheckResults(result, args...);
        return result;
    }

    // Custom checks.
    template<typename... Args>
    bool customCheck(const std::function<bool(Args...)>& checkFunction, Args... args)
    {
        std::cout << std::setw(27) << " ";
        std::cout<<"- Customized function evaluation"<<std::endl;
        std::cout << std::flush;
        bool result = checkFunction(args...);
        this->updateCheckResults(result, args...);
        return result;
    }

    // Public members.
    std::string testName_;
    std::atomic_bool result_;
    bool forceStreamData_;
    std::atomic_bool stopOnSubtestFail_;
    unsigned currentCheckN_;
    CheckResultsV checkResultsV_;

private:

    template<typename T> static std::enable_if_t<!is_streamable_v<T>, std::string>
    valueToString(const T&)
    {
        return "<NON STREAMABLE>";
    }

    // Conversion to string for streamable types .
    template<typename T>
    static std::enable_if_t<is_streamable_v<T> && !is_floating_v<T>, std::string>
    valueToString(const T& value)
    {
        std::ostringstream os;
        os << value;
        return os.str();
    }

    // Specialization for floating-point types using numberToMaxDecStr
    template<typename T> static std::enable_if_t<is_floating_v<T>, std::string>
    valueToString(const T& value)
    {
        return helpers::strings::numberToMaxDecStr(value);
    }

    // Specialization for std::chrono::time_point types
    template<typename Clock, typename Duration>
    std::string valueToString(const std::chrono::time_point<Clock, Duration>& tp)
    {
        return valueToString(tp.time_since_epoch());
    }

    // Specialization for std::chrono::duration types
    template<typename Rep, typename Period>
    std::string valueToString(const std::chrono::duration<Rep, Period>& dur)
    {
        return std::to_string(dur.count()) + " [" + std::to_string(Period::num) +
               "/" + std::to_string(Period::den) + "]s";
    }

    template<typename... Args>
    void updateCheckResults(bool res, Args&&... args)
    {
        std::string combined_msg;
        this->currentCheckN_++;

        if(!res || this->forceStreamData_)
        {
            if constexpr (sizeof...(args) > 0)
            {
                std::ostringstream os;
                (..., (os << valueToString(std::forward<Args>(args)) << " | " ));
                combined_msg = os.str();
                helpers::strings::rtrimInPlace(combined_msg, "|");
            }
        }

        // Store the results.
        this->checkResultsV_.emplace_back(currentCheckN_, res, combined_msg);
    }
};

// ---------------------------------------------------------------------------------------------------------------------

LEOBASE_MODULE_END
