/*
 *      Copyright(C) Milethos Technologies SLU. 2025
 *      All rights reserved. Reproduction in whole or in
 *      part is prohibited without the written consent of
 *      the copyright holder.
 */

#pragma once

// C++ INCLUDES
#include <string>
#include <vector>
#include <unordered_set>

// LEOBASE INCLUDES
#include "LeoBase/Aliases/Global"

// LEOBASE MACROS
LEOBASE_MODULE_BEGIN(helpers)
BEGIN_NAMESPACE(strings)

// ---------------------------------------------------------------------------------------------------------------------

LEOBASE_EXPORT std::string toUpper(const std::string& str);

LEOBASE_EXPORT std::string& toUpperInPlace(std::string& str);

LEOBASE_EXPORT std::string toLower(const std::string& str);

LEOBASE_EXPORT std::string& toLowerInPlace(std::string& str);

LEOBASE_EXPORT std::string ltrim(const std::string& str, const std::string& chars = " ");

LEOBASE_EXPORT std::string& ltrimInPlace(std::string& str, const std::string& chars = " ");

LEOBASE_EXPORT std::string rtrim(const std::string& str, const std::string& chars = " ");

LEOBASE_EXPORT std::string& rtrimInPlace(std::string& str, const std::string& chars = " ");

LEOBASE_EXPORT std::string trim(const std::string& str, const std::string& chars = " ");

LEOBASE_EXPORT std::string& trimInPlace(std::string& str, const std::string& chars = " ");

LEOBASE_EXPORT std::string generateLoremIpsum(size_t paragraphs, size_t sentences, size_t words);

LEOBASE_EXPORT std::string generateRandom(size_t len);

LEOBASE_EXPORT std::string generateExampleTitle(const std::string& title, size_t width = 50);

LEOBASE_EXPORT std::string generateExampleEnd(size_t width = 50);

LEOBASE_EXPORT std::string fill(const std::string& fillStr, size_t width);

/**
 * @brief Replaces occurrences of a substring in a given string and returns a new string.
 *
 * @param str The input string in which replacements will be performed.
 * @param target The substring to be replaced.
 * @param repl The string to replace each occurrence of the target substring.
 * @param firstOnly If true, only the first occurrence of `target` will be replaced.
 * @return A new string with occurrences of `target` replaced by `repl`.
 */
LEOBASE_EXPORT std::string replace(const std::string& str, const std::string& target,
                                   const std::string& repl, bool firstOnly = false);

/**
 * @brief Replaces occurrences of a substring in a given string in-place.
 *
 * @param str The string in which replacements will be performed.
 * @param target The substring to be replaced.
 * @param repl The string to replace each occurrence of the target substring.
 * @param firstOnly If true, only the first occurrence of `target` will be replaced.
 * @return A reference to the modified input string.
 */
LEOBASE_EXPORT std::string& replaceInPlace(std::string& str, const std::string& target,
                                          const std::string& repl, bool firstOnly = false);

/**
 * @brief Centers a given string within a specified width.
 *
 * This function adds padding to both sides of the input string to center it
 * within the given width, using the specified fill character.
 *
 * @param text The input string to center.
 * @param width The total width of the output string.
 * @param fillChar The character used for padding (default: space ' ').
 * @return A centered string of the specified width.
 */
LEOBASE_EXPORT std::string centerText(const std::string& text, size_t width, char fillChar = ' ');

LEOBASE_EXPORT std::string centerText(const std::string& text, const std::string& bounds,
                                      size_t width, char fillChar = ' ');


LEOBASE_EXPORT std::string generateJsonStr(const StreamableFieldValuePairV& data, size_t indentSize,
                                           size_t level, bool braces = true);

LEOBASE_EXPORT bool startsWith(const std::string& str, const std::string& prefix);

LEOBASE_EXPORT bool endsWith(const std::string& str, const std::string& suffix);

LEOBASE_EXPORT bool startsWithIgnoring(const std::string& str, const std::string& prefix,
                                       const std::unordered_set<char>& ignored);

LEOBASE_EXPORT bool endsWithIgnoring(const std::string& str, const std::string& suffix,
                                     const std::unordered_set<char>& ignored);


// TODOS

LEOBASE_EXPORT std::string rmLastLineBreak(const std::string& str);

LEOBASE_EXPORT std::string remNullChar(const std::string& str);





// TODO PORT

std::vector<unsigned> tokenizeVersion(const std::string& str_ver);

int compareVersions(const std::string& str_ver1, const std::string& str_ver2);

// ---------------------------------------------------------------------------------------------------------------------
// TEMPLATE FUNCTIONS

template <class Container>
void tokenize (Container& result, const std::string& s, const std::string& dlm, bool empties = true);

template <class Container>
Container tokenize (const std::string& s, const std::string& dlm, bool empties = true );

template <typename Container>
std::string join(const Container& strings, const std::string& dlm = " ");

template <typename Iter>
std::string join(Iter begin, Iter end, const std::string& dlm = " ");

// Custom number to fixed string conversion.
template<typename T>
std::string numberToFixStr(T x, size_t prec, char dlm = '.');

// Custom number to fixed string conversion.
template<typename T>
std::string numberToStr(T x, size_t prec, size_t dec_places, bool fixed = true, char dlm = '.');

template<typename T>
std::string numberToMaxDecStr(const T& x, char dlm = '.');

template<typename T>
std::string generateJsonParam(size_t indent_size, size_t level, std::string name, T val)
{
    std::string indent = helpers::strings::fill(" ", indent_size);
    std::string lvl = helpers::strings::fill(" ", level*indent_size);
    std::string nline = !indent.empty() ? "\n" : "";
    std::string sep = !indent.empty() ? ": " : ":";
    std::ostringstream json;
    json << level << indent << "\"" << name << "\"" << sep << "\"" << val << "\"," << nline;
    return json.str();
}

// ---------------------------------------------------------------------------------------------------------------------

END_NAMESPACE(strings)
LEOBASE_MODULE_END

// TEMPLATES INCLUDES
#include "LeoBase/Helpers/strings_utils.tpp"

// ---------------------------------------------------------------------------------------------------------------------
