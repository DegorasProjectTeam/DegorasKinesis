# LibThorlabsKinesis

Production-quality, extensible **C++17** library for controlling Thorlabs Kinesis motion
devices. It grows out of a validated proof of concept (the `Proof_ThorlabsM30XY_Project`
M30X / M30XY drivers) but is rebuilt as a reusable library that new Kinesis device
families can join without architectural changes.

Licensed under the **GNU General Public License v3.0 or later** (see [LICENSE](LICENSE)).

## Architecture (three layers)

| Layer | Folder | Knows about | Examples |
|-------|--------|-------------|----------|
| **1 — generic infrastructure** | `Common/` | nothing vendor-specific | `OperationResult`, `DeviceError`, motion vocabulary, `StatusPoller<StatusT>`, `waitForCondition` |
| **2 — per-SDK-module adapter** | `DCServo/` | one Kinesis module's C API | `DCServoChannel`, `decodeDCServoStatus`, discovery, `KinesisSimulatorSession` |
| **3 — device personalities** | `Devices/` | one device's identity & axis count | `M30X` (1 channel), `M30XY` (2 channels), behind `IMotionDevice` |

**The layer-boundary rule:** the vendor header (`<Thorlabs/...Benchtop.DCServo.h>`) is
included **only** in Layer-2 translation units. A future device on a different Kinesis
module (`SC_*` / `CC_*` / `BMC_*`) is added as a new Layer-2 adapter plus a thin Layer-3
personality — **by composition, never a shared base class** — reusing Layer 1 unchanged.

## Repository layout

```
LibThorlabsKinesis/                       (this repo)
├── LICENSE  ├── README.md  ├── .gitignore
├── build/<preset>/                       (out-of-tree build output; git-ignored)
└── LibThorlabsKinesis/                   (CMake project root)
    ├── CMakeLists.txt  ├── CMakePresets.json  ├── CMakeUserPresets.json
    ├── cmake/toolchains/dp_windows_mingw.cmake
    ├── includes/LibThorlabsKinesis/
    │   ├── {Common,DCServo,Devices}/      + libthorlabskinesis_global.h
    │   └── Modules/{Common,DCServo,Devices}   whole-layer aggregator headers
    ├── sources/{Common,DCServo,Devices}/
    ├── testing/{Common,DCServo,Devices}/      assert-based, no-framework self-checks
    ├── examples/                              consumer usage demos
    └── thirdparty/Thorlabs/                   vendored Kinesis SDK
```

Headers are consumed either individually as `#include "LibThorlabsKinesis/<Layer>/<file>.h"`, or a whole
layer at once via a **module** aggregator: `#include <LibThorlabsKinesis/Modules/Devices>` (also `Common`,
`DCServo`). Executable naming convention: examples `Example_*`, unit tests `UT_*`, other/integration tests
`Test_*`, and applications `App*`.

## Build (Windows, MSYS2 MinGW)

Toolchain: an MSYS2 prefix (e.g. UCRT64) providing GCC, Ninja and (optionally) windres.
The toolchain file resolves the prefix from the `MINGW_ROOT` environment variable; the
`CMakeUserPresets.json` presets set it to `E:/msys64/ucrt64` — adjust it for your machine.

```sh
cd LibThorlabsKinesis
cmake --preset local-mingw-dynamic-deb        # configure (Shared, Debug)
cmake --build --preset local-mingw-dynamic-deb
```

Project presets (in `CMakePresets.json`) cover `mingw-{dynamic,static}-{deb,rel}`; the
`local-*` user presets just add your `MINGW_ROOT`. Build options:
`LIBTHORLABSKINESIS_BUILD_SHARED` (default ON), `…_BUILD_TESTING`, `…_BUILD_EXAMPLES`.

Artifacts land in `build/<preset>/bin/` at the **repo root** (out of the project tree). The build also stages,
next to the binaries, the vendored Thorlabs DLLs **and** the MinGW C++ runtime (`libstdc++-6.dll`,
`libgcc_s_seh-1.dll`, `libwinpthread-1.dll`), so the executables run without the toolchain on `PATH`.

## Tests & examples

No test-framework dependency: the `testing/` executables are plain `assert()`-based checks (run them
directly). Unit tests (`UT_*`) need no hardware. Integration tests (`Test_*`) run against the **Thorlabs
Kinesis Simulator** and self-skip if it is absent:

- `Test_M30XYSim` / `Test_M30XSim` — full SDK round-trip (connect/enable/home/status/callback/disconnect).
- `Test_Concurrency` — same-serial aliasing (`SERIAL_IN_USE`) + two-serial concurrent stress.
- `Test_M30XYMonitor [seconds]` — connects and continuously prints decoded status while running a scripted
  home / jog FWD-REV / move-relative / move-to sequence; you can also move the stage **manually** in the
  simulator and watch it evolve. Observational (not an assert test).
- `Test_M30XYHardware` — gated behind `--i-have-hardware`; small, conservative moves only. Read its safety
  notice before running on real optics.

The simulator exposes the devices and the full command/status path works, but it cannot be assigned a stage
(travel range), so it does not physically translate position; the device tests therefore assert the SDK
contract and only **log** position. Full physical-motion validation requires real hardware.
