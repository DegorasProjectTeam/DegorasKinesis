/*
 *  DegorasKinesis - An extensible C++ library for controlling Thorlabs Kinesis motion devices.
 *
 *  Developed as free software by and for the Spanish Navy Observatory SLR station (SFEL) in San Fernando.
 *
 *  Copyright (C) 2024-2026 Degoras Project Team
 *                          < Ángel Vera Herrera, avera@roa.es - angelvh.engr@gmail.com >
 *                          < Jesús Relinque Madroñal, jrelinque@roa.es >
 *
 *  This program is free software: you can redistribute it and/or modify it under the terms of the GNU General
 *  Public License as published by the Free Software Foundation, either version 3 of the License, or (at your
 *  option) any later version.
 *
 *  This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the
 *  implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 *  for more details.
 *
 *  You should have received a copy of the GNU General Public License along with this program. If not, see
 *  <https://www.gnu.org/licenses/>.
 *
 *  SPDX-License-Identifier: GPL-3.0-or-later
 */

// C++ INCLUDES
#include <iostream>

// PROJECT INCLUDES (module aggregators)
#include <DegorasKinesis/Modules/DCServo>
#include <DegorasKinesis/Modules/Devices>


// Example: enumerate the connected Thorlabs Kinesis devices and print their serial numbers. Works against the
// Kinesis Simulator or real hardware; no device is opened.

using namespace dpkin;
using namespace dpkin::kinesis;
using dpkin::types::OperationResult;

namespace
{
void report(const std::string& label, OperationResult result, const types::ThorlabsSNList& serials)
{
    std::cout << label << ": " << types::toString(result) << " (" << serials.size() << " found)";
    for (const std::string& serial : serials)
        std::cout << " [" << serial << "]";
    std::cout << "\n";
}
}

int main()
{
    // The simulator session is optional and harmless against real hardware.
    KinesisSimulatorSession sim;
    std::cout << "Kinesis simulator: " << toString(sim.result()) << "\n";

    types::ThorlabsSNList m30xy;
    types::ThorlabsSNList m30x;
    const OperationResult r_m30xy = M30XY::getDeviceList(m30xy);
    const OperationResult r_m30x = M30X::getDeviceList(m30x);

    report("M30XY (type 101)", r_m30xy, m30xy);
    report("M30X  (type 105)", r_m30x, m30x);

    if (m30xy.empty() && m30x.empty())
        std::cout << "No devices found. Start the Kinesis Simulator (or connect hardware) and retry.\n";

    return 0;
}
