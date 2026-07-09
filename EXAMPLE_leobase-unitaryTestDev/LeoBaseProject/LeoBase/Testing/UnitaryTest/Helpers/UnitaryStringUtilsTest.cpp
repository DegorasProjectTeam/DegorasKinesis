/*
 * Copyright(C) Milethos Technologies SLU. 2026
 * All rights reserved. Reproduction in whole or in
 * part is prohibited without the written consent of
 * the copyright holder.
 */


//Helpers String Utils Testing


// C++ INCLUDES

#include <string>


// LEOBASE INCLUDES
#include <LeoBase/Aliases/Testing>
#include <LeoBase/Helpers/strings_utils.h>
#include <LeoBase/Exceptions/leobase_exception.h>

//
using namespace leobase;
using namespace leobase::testing;
using namespace leobase::helpers::strings;

// 1. Declaración de los Tests

// Tokenizer Tests
UT_DECLARE_TEST(TokenizeBasic)
UT_DECLARE_TEST(TokenizeWithEmpties)
UT_DECLARE_TEST(TokenizeReturnVersion)

// Join Tests
UT_DECLARE_TEST(JoinVector)
UT_DECLARE_TEST(JoinIterators)

// Number Formatting Tests
UT_DECLARE_TEST(NumberToFixStr)
UT_DECLARE_TEST(NumberToStr_Formatting) //
UT_DECLARE_TEST(NumberToMaxDecStr)



UT_DEFINE_TEST(TokenizeBasic)
{
    std::string input = "apple,banana,cherry";
    std::vector<std::string> result;
    // Test simple split
    tokenize(result, input, ",", false);

    UT_EXPECTED_EQ(result.size(), 3ULL);//3ULL because "UT_EXPECTED_EQ" expects 2 of the same type of variables and result.size() gives a long long
    if(result.size() == 3) {
        UT_EXPECTED_EQ(result[0], "apple");
        UT_EXPECTED_EQ(result[1], "banana");
        UT_EXPECTED_EQ(result[2], "cherry");
    }

    // Test with delimiters at ends
    input = ",apple,banana,";
    tokenize(result, input, ",", false); // false = ignore empties
    UT_EXPECTED_EQ(result.size(), 2ULL);
}


UT_DEFINE_TEST(TokenizeWithEmpties)
{
    std::string input = "a,,b,";
    std::vector<std::string> result;

    // Test WITH empties (true)
    // "a", "", "b", ""
    tokenize(result, input, ",", true);

    UT_EXPECTED_EQ(result.size(), 4ULL);
    if(result.size() >= 3) {
        UT_EXPECTED_EQ(result[0], "a");
        UT_EXPECTED_EQ(result[1], "");
        UT_EXPECTED_EQ(result[2], "b");
    }

    // Test WITHOUT empties (false)
    // "a", "b"
    tokenize(result, input, ",", false);
    UT_EXPECTED_EQ(result.size(), 2ULL);
}


UT_DEFINE_TEST(TokenizeReturnVersion)
{
    std::string input = "1|2|3";
    // Testing a std::list to prove the template
    std::list<std::string> result = tokenize<std::list<std::string>>(input, "|", false);

    UT_EXPECTED_EQ(result.size(), 3ULL);
    UT_EXPECTED_EQ(result.front(), "1");
    UT_EXPECTED_EQ(result.back(), "3");
}


UT_DEFINE_TEST(JoinVector)
{
    std::vector<std::string> parts = {"Hello", "World", "2026"};

    std::string joined = join(parts, " ");
    UT_EXPECTED_EQ(joined, "Hello World 2026");

    // Test single element
    std::vector<std::string> single = {"Solo"};
    UT_EXPECTED_EQ(join(single, ","), "Solo");

    // Test empty
    std::vector<std::string> emptyVec;
    UT_EXPECTED_EQ(join(emptyVec, ","), "");
}

UT_DEFINE_TEST(JoinIterators)
{
    std::vector<std::string> parts = {"A", "B", "C", "D"};

    // Join only the middle parts (B, C)
    // begin() + 1 aims to B
    // begin() + 3 aims to D (end exclusive)
    std::string joined = join(parts.begin() + 1, parts.begin() + 3, "-");

    UT_EXPECTED_EQ(joined, "B-C");
}


UT_DEFINE_TEST(NumberToFixStr)
{
    double val = 12.345678;

    // Case 1: Normal precision, default separator (using comma or dot depending on implementation,
    // but the function forces the separator passed in the 'dlm' string)
    // Note: Your function numberToFixStr takes 'std::string dlm'.

    std::string res = numberToFixStr(val, 2, ".");
    // 12.345... -> rounding/truncating to 2 decimals -> "12.35" or "12.34" depending on the stream
    // std::setprecision with fixed affects this. Your function doesn't use explicit std::fixed, it uses default.
    // With default formatting, setprecision refers to the total number of digits (significant figures).

    // If val=12.3456 and prec=4 (total digits) -> 12.35
    std::string res1 = numberToFixStr(12.3456, 4, ".");
    UT_EXPECTED_EQ(res1, "12.35"); // 4 significant digits

    // Case 2: Zero cleanup and custom separator
    // 10.00 -> "10", separator ","
    std::string res2 = numberToFixStr(10.00001, 2, ",");
    // Precision 2 total digits -> 1.0e1 -> "10".
    UT_EXPECTED_EQ(res2, "10");
}


UT_DEFINE_TEST(NumberToStr_Formatting)
{
    // This function uses std::fixed if the bool is true.
    double val = 123.456789;

    // Params: value, prec(stream), dec_places(manual cut), fixed(bool), dlm(char)

    // Case A: Fixed = true, Prec = 6, DecPlaces = 2
    // It should generate "123.456789" internally, and then cut to 2 decimal places -> "123.45"
    // NOTE: If this test fails and returns "123.456789", it confirms the BUG detected earlier.
    std::string res = numberToStr(val, 6, 2, true, '.');

    UT_EXPECTED_EQ(res, "123.45");

    // Case B: Custom separator
    std::string res2 = numberToStr(1.5, 2, 1, true, ',');
    UT_EXPECTED_EQ(res2, "1,5");
}

UT_DEFINE_TEST(NumberToMaxDecStr)
{
    double val = 3.14159;
    char dlm = '.';

    std::string res = numberToMaxDecStr(val, dlm);

    UT_EXPECTED_EQ(res, "3.14159");

    // Cleaning zero tests
    double val2 = 5.000000;
    UT_EXPECTED_EQ(numberToMaxDecStr(val2, '.'), "5");
}

//3.Test execution

UT_START_SESSION("String Helpers Unit Tests")

UT_FORCE_STOP_IF_SUBTEST_FAIL(false);

// Register Tokenizer
UT_REGISTER_TEST(Helpers, Strings, TokenizeBasic);
UT_REGISTER_TEST(Helpers, Strings, TokenizeWithEmpties);
UT_REGISTER_TEST(Helpers, Strings, TokenizeReturnVersion);

// Register Join
UT_REGISTER_TEST(Helpers, Strings, JoinVector);
UT_REGISTER_TEST(Helpers, Strings, JoinIterators);

// Register Formatting
UT_REGISTER_TEST(Helpers, Strings, NumberToFixStr);
UT_REGISTER_TEST(Helpers, Strings, NumberToStr_Formatting);
UT_REGISTER_TEST(Helpers, Strings, NumberToMaxDecStr);

UT_RUN_TESTS();

UT_FINISH_SESSION();
