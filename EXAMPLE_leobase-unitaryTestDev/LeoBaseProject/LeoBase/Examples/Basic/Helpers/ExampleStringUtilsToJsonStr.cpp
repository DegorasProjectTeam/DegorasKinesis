/*
 *      Copyright(C) Milethos Technologies SLU. 2025
 *      All rights reserved. Reproduction in whole or in
 *      part is prohibited without the written consent of
 *      the copyright holder.
 */

// C++ INCLUDES
#include <iostream>

// LEOBASE INCLUDES
#include <LeoBase/Aliases/System>
#include <LeoBase/Aliases/Helpers>

// ---------------------------------------------------------------------------------------------------------------------

// LeoBase namespaces.
using namespace leobase;
using namespace leobase::helpers;

// ---------------------------------------------------------------------------------------------------------------------

struct SimpleWorkData
{
    SimpleWorkData(const std::string& task, int priority) :
        taskName(std::move(task)),
        priorityLevel(priority)
    {}

    std::string toJsonStr(size_t indent_size = 0, size_t level = 0) const
    {
        StreamableFieldValuePairV json_data =
        {
            {"task_name", this->taskName},
            {"priority", this->priorityLevel}
        };
        return helpers::strings::generateJsonStr(json_data, indent_size, level);
    }

    std::string taskName;
    int priorityLevel;
};

struct SimpleTransferData
{
    SimpleTransferData(const SimpleWorkData& data, const std::string& src,
                       const std::string& dst, bool completed) :
        data(data),
        source(src),
        destination(dst),
        isCompleted(completed)
    {}

    std::string toJsonStr(size_t indent_size = 0, size_t level = 0) const
    {
        StreamableFieldValuePairV json_data =
        {
            {"data", this->data.toJsonStr(indent_size, level+1)},
            {"source", this->source},
            {"destination", this->destination},
            {"completed", this->isCompleted}
        };
        return helpers::strings::generateJsonStr(json_data, indent_size, level);
    }

    SimpleWorkData data;
    std::string source;
    std::string destination;
    bool isCompleted;
};

struct BaseStruct
{
    BaseStruct() :
        baseString("This is the base string."),
        baseNumber(12)
    {}

    std::string toJsonStr(size_t indent_size = 0, size_t level = 0) const
    {
        StreamableFieldValuePairV json_data =
            {
                {"base_string", this->baseString},
                {"base_number", this->baseNumber}
            };
        return helpers::strings::generateJsonStr(json_data, indent_size, level, false);
    }

    std::string baseString;
    int baseNumber;
};

struct HierachyStruct : public BaseStruct
{
    HierachyStruct() :
        BaseStruct(),
        highLevelString("This is the high level string."),
        highLevelNumber(99)
    {}

    std::string toJsonStr(size_t indent_size = 0, size_t level = 0) const
    {
        StreamableFieldValuePairV json_data =
            {
                {"", BaseStruct::toJsonStr(indent_size, level)},
                {"high_level_string", this->highLevelString},
                {"high_level_number", this->highLevelNumber}
            };
        return helpers::strings::generateJsonStr(json_data, indent_size, level);
    }

    std::string highLevelString;
    int highLevelNumber;
};

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Main entry point of the program `StringUtilsToJsonStr`.
 */
int main()
{
    // Initial log.
    std::cout << strings::generateExampleTitle("Example StringUtilsToJsonStr");

    // ----------------------------------------------------------------------------------------------------
    // Simple JSON Example.

    StreamableFieldValuePairV simple_json_data =
    {
        {"name", "John Doe"},
        {"age", 30},
        {"is_student", false},
        {"height", 1.78},
        {"email", "johndoe@example.com"},
        {"phone", std::nullptr_t{}}
    };

    std::cout << "Simple JSON Output:" << std::endl;
    std::cout << strings::generateJsonStr(simple_json_data, 4, 0) << std::endl << std::endl;
    std::cout << strings::generateJsonStr(simple_json_data, 0, 0) << std::endl << std::endl;
    std::cout << strings::generateJsonStr(simple_json_data, 4, 0, false) << std::endl << std::endl;
    std::cout << strings::generateJsonStr(simple_json_data, 0, 0, false) << std::endl << std::endl;

    // ----------------------------------------------------------------------------------------------------
    // Multi-Level JSON Example.

    StreamableFieldValuePairV address_data =
        {
            {"street", "123 Main St"},
            {"city", "Springfield"},
            {"zip", 12345}
        };

    StreamableFieldValuePairV complex_json_data =
        {
            {"id", 1001},
            {"username", "admin"},
            {"roles", "admin, editor"},
            {"metadata", strings::generateJsonStr(address_data, 4, 1)},
            {"verified", true}
        };

    std::cout << "Multi-Level JSON Output:" << std::endl;
    std::cout << strings::generateJsonStr(complex_json_data, 4, 0) << std::endl << std::endl;

    // ----------------------------------------------------------------------------------------------------
    // Example with structs.

    // Example Work Data
    SimpleWorkData workData("File Cleanup", 3);
    std::cout << "WorkData JSON Output:" << std::endl;
    std::cout << workData.toJsonStr(4, 0) << std::endl << std::endl;

    // Example Transfer Data
    SimpleTransferData transferData(workData, "local.txt", "remote.txt", true);
    std::cout << "TransferData JSON Output:" << std::endl;
    std::cout << transferData.toJsonStr(4, 0) << std::endl << std::endl;

    // Example Transfer Data
    std::cout << "TransferData JSON Output:" << std::endl;
    std::cout << transferData.toJsonStr(0, 0) << std::endl << std::endl;

    // ----------------------------------------------------------------------------------------------------
    // Example with HierachyStruct.

    BaseStruct bStruct;
    std::cout << "BaseStruct JSON Output:" << std::endl;
    std::cout << bStruct.toJsonStr(4, 0) << std::endl << std::endl;

    HierachyStruct hStruct;
    std::cout << "HierachyStruct JSON Output:" << std::endl;
    std::cout << hStruct.toJsonStr(4, 0) << std::endl << std::endl;

    //----------------------------------------------------------------------------------------------------
    // Final log.
    std::cout << strings::generateExampleEnd();
    leobase::system::utils::userInputPause("Press <ENTER> to exit...");

    // Final return.
    return 0;
}

// ---------------------------------------------------------------------------------------------------------------------
