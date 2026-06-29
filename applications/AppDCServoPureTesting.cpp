/*
 *  Copyright (C) 2018-2026 Degoras Project Team
 *
 *  This file is part of a small-scale research or utility tool built atop
 *  the Degoras Project infrastructure, released under the MIT License.
 *
 *  SPDX-License-Identifier: MIT
 *
 *  See the LICENSE file in the root directory for full license details.
 */

// C++ INCLUDES
#include <bitset>
#include <cassert>
#include <iostream>
#include <mutex>

// PROJECT INCLUDES
#include "dcservo_status.h"
#include "kinesis_api_lock.h"


using thorlabs::dcservo::DCServoStatusFlags;
using thorlabs::dcservo::decodeDCServoStatus;

// ---------------------------------------------------------------------------------------------------------------------
// No-hardware self-check for the Layer-2 PURE pieces (milestone M3): status decode + lock registry.
// ---------------------------------------------------------------------------------------------------------------------

namespace
{

DCServoStatusFlags decode(unsigned long raw)
{
    return decodeDCServoStatus(std::bitset<32>(raw));
}

void testStatusDecodeSingleBits()
{
    // All-zero word: nothing set.
    DCServoStatusFlags z = decode(0);
    assert(!z.homed && !z.homing && !z.isMoving() && !z.isJogging() && !z.enabled && !z.active);
    for (bool d : z.digital_in)
        assert(!d);

    // One bit at a time maps to exactly the right flag (the bit-index audit).
    assert(decode(0x00000001).limit_cw);
    assert(decode(0x00000002).limit_ccw);
    assert(decode(0x00000004).sw_limit_cw);
    assert(decode(0x00000008).sw_limit_ccw);
    assert(decode(0x00000010).moving_cw);
    assert(decode(0x00000020).moving_ccw);
    assert(decode(0x00000040).jogging_cw);
    assert(decode(0x00000080).jogging_ccw);
    assert(decode(0x00000100).motor_connected);
    assert(decode(0x00000200).homing);    // bit 9
    assert(decode(0x00000400).homed);     // bit 10
    assert(decode(0x20000000).active);    // bit 29
    assert(decode(0x80000000).enabled);   // bit 31
}

void testStatusDecodeExclusivityAndHelpers()
{
    // homing and homed are distinct bits (the PoC inline test(9)/test(10)).
    assert(decode(0x00000400).homed && !decode(0x00000400).homing);
    assert(decode(0x00000200).homing && !decode(0x00000200).homed);

    assert(decode(0x00000010).isMoving());
    assert(decode(0x00000020).isMoving());
    assert(!decode(0x00000040).isMoving());
    assert(decode(0x00000040).isJogging());
    assert(decode(0x00000080).isJogging());
    assert(!decode(0x00000010).isJogging());
}

void testStatusDecodeDigitalInputs()
{
    const unsigned long din[6] = {0x00100000, 0x00200000, 0x00400000, 0x00800000, 0x01000000, 0x02000000};
    for (int i = 0; i < 6; ++i)
    {
        DCServoStatusFlags f = decode(din[i]);
        for (int j = 0; j < 6; ++j)
            assert(f.digital_in[static_cast<std::size_t>(j)] == (j == i));
    }
}

void testStatusDecodeCombined()
{
    DCServoStatusFlags f = decode(0x00000400 | 0x80000000 | 0x00000010 | 0x00400000);
    assert(f.homed && f.enabled && f.moving_cw && f.digital_in[2]);
    assert(!f.homing && !f.moving_ccw && !f.digital_in[0] && !f.limit_cw);
}

void testLockRegistry()
{
    std::mutex& a1 = thorlabs::serialMtx("SN-A");
    std::mutex& a2 = thorlabs::serialMtx("SN-A");
    std::mutex& b  = thorlabs::serialMtx("SN-B");
    assert(&a1 == &a2);   // same serial -> same mutex
    assert(&a1 != &b);    // distinct serials -> distinct mutexes

    std::mutex& d1 = thorlabs::discoveryMtx();
    std::mutex& d2 = thorlabs::discoveryMtx();
    assert(&d1 == &d2);
    assert(&d1 != &a1);   // discovery lock is independent of per-serial locks

    // The mutexes are actually lockable (no throw / deadlock on a fresh lock).
    { const std::lock_guard<std::mutex> lk(a1); }
    { const std::lock_guard<std::mutex> lk(d1); }
}

} // namespace

int main()
{
    testStatusDecodeSingleBits();
    testStatusDecodeExclusivityAndHelpers();
    testStatusDecodeDigitalInputs();
    testStatusDecodeCombined();
    testLockRegistry();

    std::cout << "AppDCServoPureTesting: ALL CHECKS PASSED" << std::endl;
    return 0;
}
