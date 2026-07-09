/*
 *      Copyright(C) Milethos Technologies SLU. 2025
 *      All rights reserved. Reproduction in whole or in
 *      part is prohibited without the written consent of
 *      the copyright holder.
 */

#include <algorithm>
#include <random>
#include <stdexcept>
#include <sstream>
#include <cstring>

// LEOBASE INCLUDES
#include "LeoBase/Helpers/strings_utils.h"

// LEOBASE MACROS
LEOBASE_MODULE_BEGIN(helpers)
BEGIN_NAMESPACE(strings)

// ---------------------------------------------------------------------------------------------------------------------

std::string toUpper(const std::string& str)
{
    std::string result;
    std::transform(str.begin(), str.end(), std::back_inserter(result), ::toupper);
    return result;
}

std::string& toUpperInPlace(std::string& s)
{
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c){ return std::toupper(c); });
    return s;
}

std::string& toLowerInPlace(std::string& s)
{
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c){ return std::tolower(c); });
    return s;
}

std::string toLower(const std::string& str)
{   
    std::string result;
    std::transform(str.begin(), str.end(), std::back_inserter(result), ::tolower);
    return result;
}

std::string ltrim(const std::string& str, const std::string& prefix)
{
    if (str.empty() || prefix.empty())
        return str;

    if (str.find(prefix) == 0) // Check if it starts with prefix
        return str.substr(prefix.length());

    return str;
}

std::string& ltrimInPlace(std::string& str, const std::string& prefix)
{
    if (str.empty() || prefix.empty())
        return str;

    if (str.find(prefix) == 0) // If str starts with prefix
        str.erase(0, prefix.length());

    return str;
}

std::string rtrim(const std::string& str, const std::string& suffix)
{
    if (str.empty() || suffix.empty())
        return str;

    if (str.length() >= suffix.length() && str.compare(str.length() - suffix.length(), suffix.length(), suffix) == 0)
        return str.substr(0, str.length() - suffix.length());

    return str;
}

std::string& rtrimInPlace(std::string& str, const std::string& suffix)
{
    if (str.empty() || suffix.empty())
        return str;

    if (str.length() >= suffix.length() && str.compare(str.length() - suffix.length(), suffix.length(), suffix) == 0)
        str.erase(str.length() - suffix.length());

    return str;
}

std::string trim(const std::string& str, const std::string& chars)
{
    std::string result = str;
    return ltrimInPlace(rtrimInPlace(result, chars), chars);
}

std::string& trimInPlace(std::string& str, const std::string& chars)
{
    return ltrimInPlace(rtrimInPlace(str, chars), chars);
}

std::string generateLoremIpsum(unsigned paragraphs, unsigned sentences, unsigned  words)
{
    static const std::vector<std::string> lorem_ipsum_words = {
        "Lorem", "ipsum", "dolor", "sit", "amet", "consectetur", "adipiscing", "elit",
        "sed", "do", "eiusmod", "tempor", "incididunt", "ut", "labore", "et", "dolore",
        "magna", "aliqua", "Ut", "enim", "ad", "minim", "veniam", "quis", "nostrud",
        "exercitation", "ullamco", "laboris", "nisi", "aliquip", "ex", "ea", "commodo",
        "consequat", "Duis", "aute", "irure", "in", "reprehenderit", "voluptate",
        "velit", "esse", "cillum", "eu", "fugiat", "nulla", "pariatur", "Excepteur",
        "sint", "occaecat", "cupidatat", "non", "proident", "sunt", "culpa", "qui",
        "officia", "deserunt", "mollit", "anim", "id", "est", "laborum"};

    std::string result;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, lorem_ipsum_words.size() - 1);
    for (unsigned p = 0; p < paragraphs; ++p)
    {
        for (unsigned s = 0; s < sentences; ++s)
        {
            for (unsigned w = 0; w < words; ++w)
                result += lorem_ipsum_words[dis(gen)] + " ";
            result += ". ";
        }
        result += "\n\n";
    }
    return result;
}

std::string generateRandom(size_t len)
{
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    std::stringstream ss;
    for (size_t i = 0; i < len; ++i)
        ss <<  alphanum[rand() % (sizeof(alphanum) - 1)];
    return ss.str();
}

std::string generateExampleTitle(const std::string& title, size_t width)
{
    std::ostringstream example_title;
    example_title << strings::fill("=", width) << std::endl;
    example_title << strings::centerText(title, "==", width, ' ') << std::endl;
    example_title << strings::fill("=", width) << std::endl;
    example_title << "Starting example..." << std::endl << std::endl;
    return example_title.str();
}

std::string generateExampleEnd(size_t width)
{
    std::ostringstream end;
    end << strings::fill("-", width) << std::endl;
    end << "Example finished!" << std::endl;
    end << strings::fill("-", width) << std::endl << std::endl;
    return end.str();
}

std::string fill(const std::string& fill_str, size_t width)
{
    if(width == 0 || fill_str.empty())
        return std::string();

    std::string result;
    int fill_size = width / fill_str.size();
    int remainder = width % fill_str.size();
    for (int i = 0; i < fill_size; ++i)
        result += fill_str;
    if (remainder > 0)
        result += fill_str.substr(0, remainder);
    return result;
}

std::string replace(const std::string& str, const std::string& target,
                    const std::string& repl, bool first_only)
{
    if (target.empty() || str.empty())
        return str;

    std::string result;
    size_t last_pos = 0;
    size_t pos = str.find(target);

    while (pos != std::string::npos)
    {
        result.append(str, last_pos, pos - last_pos);
        result.append(repl);
        last_pos = pos + target.length();

        if (first_only)
        {
            result.append(str, last_pos);
            return result;
        }

        pos = str.find(target, last_pos);
    }

    result.append(str, last_pos);
    return result;
}

std::string& replaceInPlace(std::string& str, const std::string& target,
                            const std::string& repl, bool first_only)
{
    if (target.empty() || str.empty())
        return str;

    size_t pos = str.find(target);
    if (pos == std::string::npos)
        return str;

    do
    {
        str.replace(pos, target.length(), repl);
        if (first_only)
            break;

        pos = str.find(target, pos + repl.length());
    } while (pos != std::string::npos);

    return str;
}

std::string centerText(const std::string &text, size_t width, char fillChar)
{
    if (text.length() >= width)
        return text;

    size_t padding = width - text.length();
    size_t leftPadding = padding / 2;
    size_t rightPadding = padding - leftPadding;

    return std::string(leftPadding, fillChar) + text + std::string(rightPadding, fillChar);
}

std::string centerText(const std::string &text, const std::string &bounds, size_t width, char fillChar)
{
    // Ensure width is greater than the bounds size + text length
    if (width <= bounds.size() + text.length())
        return bounds + text + bounds;

    // Calculate padding space excluding bounds
    size_t usableWidth = width - (2 * bounds.size());
    size_t padding = usableWidth - text.length();
    size_t leftPadding = padding / 2;
    size_t rightPadding = padding - leftPadding;

    // Build the centered text
    return bounds + std::string(leftPadding, fillChar) + text + std::string(rightPadding, fillChar) + bounds;
}

std::string generateJsonStr(const StreamableFieldValuePairV& data, size_t indent_size,
                            size_t level, bool braces)
{
    std::string indent = helpers::strings::fill(" ", indent_size);
    std::string lvl = helpers::strings::fill(" ", level * indent_size);
    std::string nline = !indent.empty() ? "\n" : "";
    std::string sep = !indent.empty() ? ": " : ":";
    std::ostringstream json;

    if(!nline.empty() && level != 0 && braces)
        json << nline << lvl << "{" << nline;
    else if (braces)
        json << lvl << "{" << nline;

    for (size_t i = 0; i < data.size(); ++i)
    {
        const auto& [name, val] = data[i];

        if(!name.empty())
            json << lvl << indent << "\"" << name << "\"" << sep;

        if (val.has_value())
        {
            std::string name_str = name;
            std::visit([&json, &name_str](const auto& value)
            {
                using ValueType = std::decay_t<decltype(value)>;
                if constexpr (std::is_same_v<ValueType, std::string>)
                {
                    if (name_str.empty() || (!value.empty() &&
                        startsWithIgnoring(value, "{", {' ', '\n'}) &&
                        endsWithIgnoring(value, "}", {' ', '\n'})))
                    {
                        json << value;
                    }
                    else
                        json << "\"" << value << "\"";
                }
                else if constexpr (std::is_same_v<ValueType, const char*> ||
                                   std::is_same_v<ValueType, char>)
                {
                    json << "\"" << value << "\"";
                }
                else if constexpr (std::is_same_v<ValueType, std::nullptr_t>)
                {
                    json << "null";
                }
                else if constexpr (std::is_same_v<ValueType, bool>)
                {
                    json << (value ? "true" : "false");
                }
                else
                {
                    json << value;
                }
            }, val.value());
        }
        else
        {
            json << "null";
        }

        if (i < data.size() - 1)
        {
            json << ",";
        }

        if(i != data.size() - 1 || (i == data.size() - 1 && braces))
            json << nline;
    }
    if(braces)
        json << lvl << "}";
    return json.str();
}

bool startsWith(const std::string& str, const std::string& prefix)
{
    return (str.size() >= prefix.size()) && std::equal(prefix.begin(), prefix.end(), str.begin());
}

bool endsWith(const std::string& str, const std::string& suffix)
{
    return (str.size() >= suffix.size()) && std::equal(suffix.rbegin(), suffix.rend(), str.rbegin());
}

bool isIgnoredChar(char ch, const std::unordered_set<char>& ignoredChars)
{
    return ignoredChars.find(ch) != ignoredChars.end();
}

bool startsWithIgnoring(const std::string& str, const std::string& prefix,
                        const std::unordered_set<char>& ignored)
{
    auto str_it = str.begin();
    auto prefix_it = prefix.begin();

    while (str_it != str.end() && prefix_it != prefix.end()) {
        // Skip ignored characters in both strings
        while (str_it != str.end() && isIgnoredChar(*str_it, ignored)) ++str_it;
        while (prefix_it != prefix.end() && isIgnoredChar(*prefix_it, ignored)) ++prefix_it;

        // If the prefix is fully matched, return true
        if (prefix_it == prefix.end()) return true;
        if (str_it == str.end() || *str_it != *prefix_it) return false;

        ++str_it;
        ++prefix_it;
    }

    return prefix_it == prefix.end();
}

bool endsWithIgnoring(const std::string& str, const std::string& suffix,
                      const std::unordered_set<char>& ignored)
{
    auto str_it = str.rbegin();
    auto suffix_it = suffix.rbegin();

    while (str_it != str.rend() && suffix_it != suffix.rend()) {
        // Skip ignored characters in both strings
        while (str_it != str.rend() && isIgnoredChar(*str_it, ignored)) ++str_it;
        while (suffix_it != suffix.rend() && isIgnoredChar(*suffix_it, ignored)) ++suffix_it;

        // If the suffix is fully matched, return true
        if (suffix_it == suffix.rend()) return true;
        if (str_it == str.rend() || *str_it != *suffix_it) return false;

        ++str_it;
        ++suffix_it;
    }

    return suffix_it == suffix.rend();
}

// ---------------------------------------------------------------------------------------------------------------------

END_NAMESPACE(strings)
LEOBASE_MODULE_END
