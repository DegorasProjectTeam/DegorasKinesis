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
└── LibThorlabsKinesis/                   (CMake project root)
    ├── CMakeLists.txt  ├── CMakePresets.json  ├── CMakeUserPresets.json
    ├── cmake/toolchains/dp_windows_mingw.cmake
    ├── includes/LibThorlabsKinesis/{Common,DCServo,Devices}/   + libthorlabskinesis_global.h
    ├── sources/{Common,DCServo,Devices}/
    ├── testing/{Common,DCServo,Devices}/      assert-based, no-framework self-checks
    ├── examples/                              consumer usage demos
    └── thirdparty/Thorlabs/                   vendored Kinesis SDK
```

Public headers are consumed as `#include "LibThorlabsKinesis/<Layer>/<file>.h"`.

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

Artifacts (shared library + executables, with the vendored Thorlabs DLLs staged alongside)
land in `build/<preset>/bin/`.

## Tests & examples

No test-framework dependency: the `testing/` executables are plain `assert()`-based
checks (run them directly). The pure/logic checks need no hardware; the device-level
checks run against the **Thorlabs Kinesis Simulator** (and self-skip if it is absent).
`AppM30XYHardwareTesting` is gated behind `--i-have-hardware` and performs only small,
conservative moves — read its safety notice before running on real optics.
