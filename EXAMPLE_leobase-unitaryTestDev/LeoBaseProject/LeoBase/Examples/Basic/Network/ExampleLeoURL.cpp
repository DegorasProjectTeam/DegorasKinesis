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
#include <LeoBase/Aliases/Network>

// ---------------------------------------------------------------------------------------------------------------------

// LeoBase namespaces.
using namespace leobase::helpers;
using namespace leobase::system;
using namespace leobase::network;

// ---------------------------------------------------------------------------------------------------------------------

void printResult(const std::string& test_name, bool condition)
{
    std::cout << test_name << ": " << (condition ? "[PASSED]" : "[FAILED]") << std::endl;
}

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Main entry point of the program `ExampleLeoURL`.
 */
int main()
{
    // Initial log.
    std::cout << strings::generateExampleTitle("Example LeoURL");

    // ----------------------------------------------------------------------------------------------------
    // Testing LeoURL.

    std::string url1 = "sftp://user:pass@192.168.8.169:22/home/visualizer/angel/file.mp3?param=value";
    std::string url2 = "https://example.com/path/to/resource";
    std::string url3 = "ftp://ftp.example.com/public/data.zip";
    std::string url4 = "ftp.example.com/public/data.zip";

    LeoURL parsed_url1(url1);
    LeoURL parsed_url2(url2);
    LeoURL parsed_url3(url3);
    LeoURL parsed_url4(url4);

    // Show data.
    std::cout << "URL 1 PARTS: " << std::endl;
    std::cout << parsed_url1.toJsonStr(2) << std::endl << std::endl;
    std::cout << "URL 2 PARTS: " << std::endl;
    std::cout << parsed_url2.toJsonStr(2) << std::endl << std::endl;
    std::cout << "URL 3 PARTS: " << std::endl;
    std::cout << parsed_url3.toJsonStr(2) << std::endl << std::endl;
    std::cout << "URL 4 PARTS: " << std::endl;
    std::cout << parsed_url4.toJsonStr(2) << std::endl << std::endl;

    // Expected parsed components
    printResult("Parsing Protocol (sftp)", parsed_url1.getScheme() == "sftp");
    printResult("Parsing Username (user)", parsed_url1.getUsername() == "user");
    printResult("Parsing Password (pass)", parsed_url1.getPassword() == "pass");
    printResult("Parsing Hostname (192.168.8.169)", parsed_url1.getHostname() == "192.168.8.169");
    printResult("Parsing Port (22)", parsed_url1.getPort().value() == 22);
    printResult("Parsing Path (/home/visualizer/angel/file.mp3)", parsed_url1.getPath() == "/home/visualizer/angel/file.mp3");
    printResult("Parsing Parameters (?param=value)", parsed_url1.getQuery() == "param=value");

    // Constructed URL should match original
    printResult("Constructed URL Matches Original", parsed_url1.toString() == url1);
    printResult("Constructed URL Matches Original", parsed_url2.toString() == url2);
    printResult("Constructed URL Matches Original", parsed_url3.toString() == url3);

    // Test another URL without credentials and port
    printResult("Parsing Protocol (https)", parsed_url2.getScheme() == "https");
    printResult("Parsing Hostname (example.com)", parsed_url2.getHostname() == "example.com");
    printResult("Parsing Path (/path/to/resource)", parsed_url2.getPath() == "/path/to/resource");

    // Test FTP URL
    printResult("Parsing FTP Protocol", parsed_url3.getScheme() == "ftp");
    printResult("Parsing FTP Hostname", parsed_url3.getHostname() == "ftp.example.com");
    printResult("Parsing FTP Path", parsed_url3.getPath() == "/public/data.zip");

    //----------------------------------------------------------------------------------------------------
    // Final log.
    std::cout << strings::generateExampleEnd();
    leobase::system::utils::userInputPause("Press <ENTER> to exit...");

    // Final return.
    return 0;
}

// ---------------------------------------------------------------------------------------------------------------------
