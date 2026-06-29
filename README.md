# LibThorlabsKinesis

Production-quality, extensible C++17 library for controlling Thorlabs Kinesis motion
devices. It grows out of a validated proof of concept (the `Proof_ThorlabsM30XY_Project`
M30X / M30XY drivers) but is rebuilt as a reusable library that new Kinesis device
families can join without architectural changes.

## Architecture (three layers)

| Layer | Knows about | Examples |
|-------|-------------|----------|
| **1 — generic infrastructure** | nothing vendor-specific | `OperationResult`, `DeviceError`, motion vocabulary, `StatusPoller<StatusT>`, `waitForCondition` |
| **2 — per-SDK-module adapter** | one Kinesis module's C API | `DCServoChannel`, `decodeDCServoStatus`, discovery, simulator session (Benchtop.DCServo) |
| **3 — device personalities** | one device's identity & axis count | `M30X` (1 channel), `M30XY` (2 channels), behind `IMotionDevice` |

**The layer-boundary rule:** the vendor header
(`<Thorlabs/...Benchtop.DCServo.h>`) is included **only** in Layer-2 translation units.
A future device on a different Kinesis module (`SC_*` / `CC_*` / `BMC_*`) is added as a
new Layer-2 adapter plus a thin Layer-3 personality — **by composition, never a shared
base class** — reusing Layer 1 unchanged.

## Build (Windows, MSYS2 UCRT64)

Toolchain: GCC (UCRT64), CMake, Ninja. From a shell with `E:\msys64\ucrt64\bin` on `PATH`:

```sh
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build build
```

Artifacts land in `build/bin/` (shared library + executables; the vendored Thorlabs DLLs
are staged alongside them automatically).

## Tests

There is no test framework dependency. Non-hardware checks are plain `assert()`-based
executables under `applications/` (e.g. `AppErrorMappingTesting`), matching the proof of
concept's "testing is an app" convention. Run them directly from `build/bin/`.
Hardware/simulator tests are clearly marked and safety-gated.
