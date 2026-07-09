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
#include <LeoBase/Aliases/Containers>
#include <LeoBase/Aliases/Helpers>

// ---------------------------------------------------------------------------------------------------------------------

// LeoBase namespaces.
using namespace leobase::helpers;
using leobase::containers::AnyUnorderedMap;
using leobase::containers::getValue;
using leobase::containers::tryGetValue;

// ---------------------------------------------------------------------------------------------------------------------

struct CustomData
{
    int id;
    std::string name;

    bool operator==(const CustomData& other) const
    {
        return id == other.id && name == other.name;
    }
};

// Sample enum id type.
using ExampleEnumId = std::uint8_t;

// Sample enum class
enum class ExampleEnum : ExampleEnumId
{
    VALUE_ONE = 1,
    VALUE_TWO = 2
};

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Main entry point of the program `ExampleAnyUnorderedMap`.
 */
int main()
{    
    // Initial log.
    std::cout << strings::generateExampleTitle("Example AnyUnorderedMap");

    //----------------------------------------------------------------------------------------------------
    // Creating and populating AnyUnorderedMap

    AnyUnorderedMap any_map;

    std::cout << "[INFO] Populating AnyUnorderedMap..." << std::endl;

    any_map[1] = 42;
    any_map["message"] = std::string("Hello");
    any_map[2] = std::uint32_t(100);
    any_map["flag"] = true;

    std::cout << "[SUCCESS] Values added to the map." << std::endl;
    std::cout << std::endl;

    //----------------------------------------------------------------------------------------------------
    // Retrieving values using getValue (Exception-Based)

    std::cout << "[INFO] Retrieving values using `getValue` (Exception-based)..." << std::endl;

    try
    {
        std::cout << "  - any_map[1] (int): " << getValue<int>(any_map, 1) << std::endl;
        std::cout << "  - any_map[\"message\"] (std::string): " << getValue<std::string>(any_map, "message") << std::endl;
        std::cout << "  - any_map[2] (std::uint32_t): " << getValue<std::uint32_t>(any_map, 2) << std::endl;
        std::cout << "  - any_map[\"flag\"] (bool): " << (getValue<bool>(any_map, "flag") ? "true" : "false") << std::endl;

        // This will throw an exception (wrong type retrieval)
        std::cout << "  - Attempting to retrieve any_map[\"message\"] as int (will fail)..." << std::endl;
        std::cout << getValue<int>(any_map, "message") << std::endl;

    }
    catch (const std::exception& e)
    {
        std::cout << "[EXCEPTION] " << e.what() << std::endl;
    }

    std::cout << "[INFO] Finished retrieval using `getValue`." << std::endl;
    std::cout << std::endl;

    //----------------------------------------------------------------------------------------------------
    // Retrieving values using tryGetValue (Safe Optional-Based)
    std::cout << "[INFO] Retrieving values using `tryGetValue` (Optional-based)..." << std::endl;

    auto intValue = tryGetValue<int>(any_map, 1);
    auto strValue = tryGetValue<std::string>(any_map, "message");
    auto uintValue = tryGetValue<std::uint32_t>(any_map, 2);
    auto missingValue = tryGetValue<int>(any_map, "missing_key"); // This key does not exist

    if (intValue) std::cout << "  - any_map[1] (int): " << *intValue << std::endl;
    if (strValue) std::cout << "  - any_map[\"message\"] (std::string): " << *strValue << std::endl;
    if (uintValue) std::cout << "  - any_map[2] (std::uint32_t): " << *uintValue << std::endl;
    if (!missingValue) std::cout << "  - any_map[\"missing_key\"] not found (as expected)." << std::endl;

    std::cout << "[INFO] Finished retrieval using `tryGetValue`." << std::endl;
    std::cout << std::endl;

    // ----------------------------------------------------------------------------------------------------
    // Adding more diverse key types
    std::cout << "[INFO] Adding diverse key types to AnyUnorderedMap..." << std::endl;

    std::size_t key_size_t = 12345;
    std::uint8_t key_uint8 = 255;
    std::int16_t key_int16 = -32768;
    ExampleEnumId key_enum = static_cast<ExampleEnumId>(ExampleEnum::VALUE_ONE);

    any_map[key_size_t] = "Stored with size_t key";
    any_map[key_uint8] = 3.1415;
    any_map[key_int16] = std::string("Stored with int16_t key");
    any_map[key_enum] = std::string("Stored with key_enum key");

    std::cout << "[SUCCESS] Additional keys added to AnyUnorderedMap." << std::endl;
    std::cout << std::endl;

    // ----------------------------------------------------------------------------------------------------
    // Retrieving values with diverse keys
    std::cout << "[INFO] Retrieving values using various key types..." << std::endl;

    std::cout << "  - any_map[key_size_t] (std::string): " << getValue<const char *>(any_map, key_size_t) << std::endl;
    std::cout << "  - any_map[key_uint8] (double): " << getValue<double>(any_map, key_uint8) << std::endl;
    std::cout << "  - any_map[key_int16] (std::string): " << getValue<std::string>(any_map, key_int16) << std::endl;
    std::cout << "  - any_map[key_int16] (std::string): " << getValue<std::string>(any_map, key_enum) << std::endl;

    std::cout << "[INFO] Finished retrieval using different key types." << std::endl;
    std::cout << std::endl;

    // ----------------------------------------------------------------------------------------------------
    // Using std::pair as a value

    using CustomPair = std::pair<int, std::string>;

    std::cout << "[INFO] Storing `std::pair<int, std::string>` in AnyUnorderedMap..." << std::endl;

    any_map["pair_key"] = std::make_pair(10, std::string("LeoBase Pair"));

    std::pair<int, std::string> retrieved_pair = getValue<CustomPair>(any_map, "pair_key");

    std::cout << "  - any_map[\"pair_key\"] (std::pair<int, std::string>): ("
              << retrieved_pair.first << ", " << retrieved_pair.second << ")" << std::endl;

    std::cout << "[INFO] Successfully stored and retrieved `std::pair<>`." << std::endl;
    std::cout << std::endl;

    // ----------------------------------------------------------------------------------------------------
    // Using a custom struct as a value

    // Inserting a struct instance into the map
    std::cout << "[INFO] Storing a custom struct in AnyUnorderedMap..." << std::endl;

    CustomData customValue{42, "LeoStruct"};
    any_map["custom_struct"] = customValue;

    // Retrieving the custom struct
    CustomData retrieved_custom = getValue<CustomData>(any_map, "custom_struct");

    std::cout << "  - any_map[\"custom_struct\"] (CustomData): {id: "
              << retrieved_custom.id << ", name: \"" << retrieved_custom.name << "\"}" << std::endl;

    std::cout << "[INFO] Successfully stored and retrieved a custom struct." << std::endl;
    std::cout << std::endl;

    //----------------------------------------------------------------------------------------------------
    // Final log.
    std::cout << strings::generateExampleEnd();
    leobase::system::utils::userInputPause("Press <ENTER> to exit...");

    // Final return.
    return 0;
}

// ---------------------------------------------------------------------------------------------------------------------
