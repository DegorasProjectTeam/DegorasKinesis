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

// ASSERTIONS ARE THE POINT OF A TEST, so NDEBUG must not be allowed to delete them. A Release build defines it and
// assert() then expands to NOTHING AT ALL -- the whole expression, not just the abort, so any call written inside an
// assert stops happening too. Measured on this suite before the fix: objdump found ZERO references to assert in all
// nine Release test objects, i.e. every one of the 195 checks was dead, and the suite reported success without
// evaluating any of them. After the fix the same objects carry 197 references and the suite still passes 9/9 in both
// configurations -- so nothing had been failing behind the dead layer here; it simply was not being checked.
//
// BOTH LINES ARE REQUIRED, in this order, above the first #include. The bare #undef is not enough: if anything has
// already pulled in <cassert> while NDEBUG was defined, assert is already expanded away and stays dead. Including
// the header again re-arms it, because assert.h undefines and redefines the macro on every inclusion and <cassert>
// deliberately has no include guard.
#undef NDEBUG
#include <cassert>

// C++ INCLUDES
#include <cassert>
#include <cmath>
#include <iostream>
#include <string>

// PROJECT INCLUDES (module aggregators)
#include <DegorasKinesis/Modules/Common>
#include <DegorasKinesis/Modules/DCServo>
#include <DegorasKinesis/Modules/Devices>


using namespace dpkin;
using namespace dpkin::types;

// ---------------------------------------------------------------------------------------------------------------------
// No-hardware self-check for JSON serialisation: every serialisable type round-trips through toJsonStr/fromJsonStr,
// in both compact and pretty forms, and the pretty form parses identically to the compact form.
// ---------------------------------------------------------------------------------------------------------------------

namespace
{

bool dEq(double a, double b) { return std::abs(a - b) < 1e-9; }

bool flagsEq(const kinesis::MotorStatusFlags& a, const kinesis::MotorStatusFlags& b)
{
    return a.limit_cw == b.limit_cw && a.limit_ccw == b.limit_ccw
        && a.sw_limit_cw == b.sw_limit_cw && a.sw_limit_ccw == b.sw_limit_ccw
        && a.moving_cw == b.moving_cw && a.moving_ccw == b.moving_ccw
        && a.jogging_cw == b.jogging_cw && a.jogging_ccw == b.jogging_ccw
        && a.motor_connected == b.motor_connected && a.homing == b.homing && a.homed == b.homed
        && a.active == b.active && a.enabled == b.enabled && a.digital_in == b.digital_in;
}

void testFlags()
{
    kinesis::MotorStatusFlags f;
    f.homed = true; f.enabled = true; f.moving_cw = true; f.motor_connected = true;
    f.digital_in[1] = true; f.digital_in[4] = true;

    assert(flagsEq(f, kinesis::MotorStatusFlags::fromJsonStr(f.toJsonStr())));

    const std::string pretty = f.toJsonStr(true);
    assert(pretty.find('\n') != std::string::npos);                 // pretty is multi-line
    assert(f.toJsonStr(false).find('\n') == std::string::npos);     // compact is single-line
    assert(flagsEq(f, kinesis::MotorStatusFlags::fromJsonStr(pretty)));   // pretty parses identically
}

void testJogParameters()
{
    JogParameters p;
    p.mode = JogMode::SINGLE_STEP;
    p.step_size = 2.5;
    p.vel_profile.min = 0.5; p.vel_profile.max = 4.0; p.vel_profile.acc = 1.25;
    p.stop_mode = StopMode::PROFILED;

    const JogParameters rt = JogParameters::fromJsonStr(p.toJsonStr());
    assert(rt.mode == p.mode && rt.stop_mode == p.stop_mode && dEq(rt.step_size, p.step_size)
           && dEq(rt.vel_profile.min, p.vel_profile.min) && dEq(rt.vel_profile.max, p.vel_profile.max)
           && dEq(rt.vel_profile.acc, p.vel_profile.acc));

    const JogParameters rtp = JogParameters::fromJsonStr(p.toJsonStr(true));
    assert(rtp.mode == p.mode && dEq(rtp.vel_profile.acc, p.vel_profile.acc));
}

void testM30XY()
{
    M30XYChannelStatus c(Channel::Y_CHANNEL);
    c.valid = true; c.pos_raw = 12345; c.real_pos = 8.5;
    c.flags.homed = true; c.flags.jogging_ccw = true; c.flags.digital_in[2] = true;

    const M30XYChannelStatus crt = M30XYChannelStatus::fromJsonStr(c.toJsonStr());
    assert(crt.channel == c.channel && crt.valid == c.valid && crt.pos_raw == c.pos_raw
           && dEq(crt.real_pos, c.real_pos) && flagsEq(crt.flags, c.flags));

    M30XYDeviceStatus d;
    d.serial_no = "101000002"; d.connected = true;
    d.chann_x = M30XYChannelStatus(Channel::X_CHANNEL); d.chann_x.valid = true; d.chann_x.real_pos = 1.5;
    d.chann_x.flags.homed = true;
    d.chann_y = M30XYChannelStatus(Channel::Y_CHANNEL); d.chann_y.valid = true; d.chann_y.real_pos = 2.5;
    d.chann_y.flags.enabled = true;

    const M30XYDeviceStatus drt = M30XYDeviceStatus::fromJsonStr(d.toJsonStr(true));   // via pretty
    assert(drt.serial_no == d.serial_no && drt.connected == d.connected);
    assert(drt.chann_x.channel == Channel::X_CHANNEL && dEq(drt.chann_x.real_pos, 1.5) && drt.chann_x.flags.homed);
    assert(drt.chann_y.channel == Channel::Y_CHANNEL && dEq(drt.chann_y.real_pos, 2.5) && drt.chann_y.flags.enabled);
}

void testM30X()
{
    M30XDeviceStatus d;
    d.serial_no = "105000002"; d.connected = true;
    d.chann.valid = true; d.chann.real_pos = 3.5; d.chann.flags.homed = true; d.chann.flags.limit_cw = true;

    const M30XDeviceStatus rt = M30XDeviceStatus::fromJsonStr(d.toJsonStr());
    assert(rt.serial_no == d.serial_no && rt.connected && rt.chann.valid
           && dEq(rt.chann.real_pos, 3.5) && rt.chann.flags.homed && rt.chann.flags.limit_cw);

    const M30XDeviceStatus rtp = M30XDeviceStatus::fromJsonStr(d.toJsonStr(true));
    assert(rtp.chann.flags.homed && rtp.serial_no == d.serial_no);
}

} // namespace

int main()
{
    testFlags();
    testJogParameters();
    testM30XY();
    testM30X();

    std::cout << "UT_Json: ALL CHECKS PASSED" << std::endl;
    return 0;
}
