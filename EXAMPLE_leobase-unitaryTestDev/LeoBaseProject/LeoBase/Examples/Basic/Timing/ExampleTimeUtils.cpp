/*
 *      Copyright(C) Milethos Technologies SLU. 2025
 *      All rights reserved. Reproduction in whole or in
 *      part is prohibited without the written consent of
 *      the copyright holder.
 */

// C++ INCLUDES
#include <iostream>
#include <thread>

// LEOBASE INCLUDES
#include <LeoBase/Aliases/Timing>
#include <LeoBase/Aliases/System>
#include <LeoBase/Aliases/Helpers>

// ---------------------------------------------------------------------------------------------------------------------

// LeoBase namespaces.
using namespace leobase::helpers;
using namespace leobase::timing::utils;
using namespace leobase::timing::types;

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Main entry point of the program `ExampleTimeUtils`.
 */
int main()
{
    // Initial log.
    std::cout << strings::generateExampleTitle("Example TimeUtils");

    // ----------------------------------------------------------------------------------------------------
    // GETTING CLOCKS OFFSTETS.

    std::cout << "[INFO] Computing steady-to-system clock offset..." << std::endl;
    Ns steady_offset = computeOffsetSteadyToSystem();
    std::cout << "  - Steady to System Offset: " << steady_offset.count() << " ns" << std::endl;
    std::cout << "[SUCCESS] Offset computed." << std::endl << std::endl;

    std::cout << "[INFO] Computing high-res-to-system clock offset..." << std::endl;
    Ns highres_offset = computeOffsetHighResToSystem();
    std::cout << "  - High-Res to System Offset: " << highres_offset.count() << " ns" << std::endl;
    std::cout << "[SUCCESS] Offset computed." << std::endl << std::endl;

    // ----------------------------------------------------------------------------------------------------
    // DURATION CONVERSION.

    std::cout << "[INFO] Measuring approximate time difference..." << std::endl;
    auto start_time = currentTimepoint<StdyClock>();
    std::this_thread::sleep_for(std::chrono::milliseconds(28));
    auto end_time = currentTimepoint<StdyClock>();
    Ns measured_duration = end_time - start_time;
    std::cout << "  - Approximate measured duration: " << measured_duration.count() << " ns" << std::endl;
    std::cout << "[SUCCESS] Measured duration." << std::endl << std::endl;

    std::cout << "[INFO] Converting measured duration into different resolutions..." << std::endl;
    std::cout << "  - Duration in seconds: " << convertDuration(measured_duration, TimeResolution::SECONDS) << " s" << std::endl;
    std::cout << "  - Duration in milliseconds: " << convertDuration(measured_duration, TimeResolution::MILLISECONDS) << " ms" << std::endl;
    std::cout << "  - Duration in microseconds: " << convertDuration(measured_duration, TimeResolution::MICROSECONDS) << " us" << std::endl;
    std::cout << "  - Duration in nanoseconds: " << convertDuration(measured_duration, TimeResolution::NANOSECONDS) << " ns" << std::endl;
    std::cout << "[SUCCESS] Converted duration into multiple resolutions." << std::endl << std::endl;

    // ----------------------------------------------------------------------------------------------------
    // GETTING CURRENT TIME ISO8601.

    std::cout << "[INFO] Retrieving current timestamp in ISO 8601 format (SECONDS, UTC, with trailing zeros)..." << std::endl;
    std::string now_iso_s = currentDatetimeIso8601(TimeResolution::SECONDS, true, false);
    std::cout << "  - Current time (UTC, ISO 8601): " << now_iso_s << std::endl;
    std::cout << "[SUCCESS] Retrieved current timestamp." << std::endl << std::endl;

    std::cout << "[INFO] Retrieving current timestamp in ISO 8601 format (SECONDS, UTC, without trailing zeros)..." << std::endl;
    std::string now_iso_s_no_trail = currentDatetimeIso8601(TimeResolution::SECONDS, true, true);
    std::cout << "  - Current time (UTC, ISO 8601): " << now_iso_s_no_trail << std::endl;
    std::cout << "[SUCCESS] Retrieved current timestamp." << std::endl << std::endl;

    std::cout << "[INFO] Retrieving current timestamp in ISO 8601 format (MILLISECONDS, UTC, with trailing zeros)..." << std::endl;
    std::string now_iso_ms = currentDatetimeIso8601(TimeResolution::MILLISECONDS, true, false);
    std::cout << "  - Current time (UTC, ISO 8601): " << now_iso_ms << std::endl;
    std::cout << "[SUCCESS] Retrieved current timestamp." << std::endl << std::endl;

    std::cout << "[INFO] Retrieving current timestamp in ISO 8601 format (MILLISECONDS, UTC, without trailing zeros)..." << std::endl;
    std::string now_iso_ms_no_trail = currentDatetimeIso8601(TimeResolution::MILLISECONDS, true, true);
    std::cout << "  - Current time (UTC, ISO 8601): " << now_iso_ms_no_trail << std::endl;
    std::cout << "[SUCCESS] Retrieved current timestamp." << std::endl << std::endl;

    std::cout << "[INFO] Retrieving current timestamp in ISO 8601 format (MICROSECONDS, UTC, with trailing zeros)..." << std::endl;
    std::string now_iso_us = currentDatetimeIso8601(TimeResolution::MICROSECONDS, true, false);
    std::cout << "  - Current time (UTC, ISO 8601): " << now_iso_us << std::endl;
    std::cout << "[SUCCESS] Retrieved current timestamp." << std::endl << std::endl;

    std::cout << "[INFO] Retrieving current timestamp in ISO 8601 format (MICROSECONDS, UTC, without trailing zeros)..." << std::endl;
    std::string now_iso_us_no_trail = currentDatetimeIso8601(TimeResolution::MICROSECONDS, true, true);
    std::cout << "  - Current time (UTC, ISO 8601): " << now_iso_us_no_trail << std::endl;
    std::cout << "[SUCCESS] Retrieved current timestamp." << std::endl << std::endl;

    std::cout << "[INFO] Retrieving current timestamp in ISO 8601 format (NANOSECONDS, UTC, with trailing zeros)..." << std::endl;
    std::string now_iso_ns = currentDatetimeIso8601(TimeResolution::NANOSECONDS, true, false);
    std::cout << "  - Current time (UTC, ISO 8601): " << now_iso_ns << std::endl;
    std::cout << "[SUCCESS] Retrieved current timestamp." << std::endl << std::endl;

    std::cout << "[INFO] Retrieving current timestamp in ISO 8601 format (NANOSECONDS, UTC, without trailing zeros)..." << std::endl;
    std::string now_iso_ns_no_trail = currentDatetimeIso8601(TimeResolution::NANOSECONDS, true, true);
    std::cout << "  - Current time (UTC, ISO 8601): " << now_iso_ns_no_trail << std::endl;
    std::cout << "[SUCCESS] Retrieved current timestamp." << std::endl << std::endl;

    std::cout << "[INFO] Retrieving current timestamp in ISO 8601 format (MILLISECONDS, Local Time, without trailing zeros)..." << std::endl;
    std::string now_iso_local = currentDatetimeIso8601(TimeResolution::MILLISECONDS, false, true);
    std::cout << "  - Current time (Local, ISO 8601): " << now_iso_local << std::endl;
    std::cout << "[SUCCESS] Retrieved current timestamp." << std::endl << std::endl;

    std::cout << "[INFO] Retrieving current timestamp in ISO 8601 format (NANOSECONDS, UTC, without trailing zeros, HIGH-RES CLOCK)..." << std::endl;
    std::string now_iso_ns_no_trail_hres = currentDatetimeIso8601(TimeResolution::NANOSECONDS, true, true, ClockType::HIGH_RES);
    std::cout << "  - Current time (UTC, ISO 8601, High-Res Clock): " << now_iso_ns_no_trail_hres << std::endl;
    std::cout << "[SUCCESS] Retrieved current timestamp using high-resolution clock." << std::endl << std::endl;

    // ----------------------------------------------------------------------------------------------------
    // CONVERTING A TIMESTAMP TO ISO 8601 FORMAT
    std::cout << "[INFO] Formatting a custom <time_point> as ISO 8601..." << std::endl;
    auto now_tp = std::chrono::high_resolution_clock::now();
    std::string formatted_time = timepointToIso8601(now_tp, TimeResolution::MICROSECONDS, true, true);
    std::cout << "  - Formatted time: " << formatted_time << std::endl;
    std::cout << "[SUCCESS] Formatted time successfully." << std::endl;
    std::cout << std::endl;

    //----------------------------------------------------------------------------------------------------
    // Final log.
    std::cout << strings::generateExampleEnd();
    leobase::system::utils::userInputPause("Press <ENTER> to exit...");

    // Final return.
    return 0;
}

// ---------------------------------------------------------------------------------------------------------------------
