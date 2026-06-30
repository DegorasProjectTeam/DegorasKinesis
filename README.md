<a id="readme-top"></a>

<!-- PROJECT SHIELDS -->
[![C++17][cpp-shield]][cpp-url]
[![CMake][cmake-shield]][cmake-url]
[![License: GPL v3][license-shield]][license-url]
[![Platform][platform-shield]][platform-url]

<!-- PROJECT TITLE -->
<div align="center">
  <h1 align="center">LibThorlabsKinesis</h1>

  <p align="center">
    An extensible C++17 library for controlling Thorlabs Kinesis motion devices.
    <br />
    <a href="#about-the-project"><strong>Explore the docs »</strong></a>
    <br />
    <br />
    <a href="#usage">View Usage</a>
    &middot;
    <a href="https://github.com/DegorasProjectTeam/LibThorlabsKinesis/issues">Report Bug</a>
    &middot;
    <a href="https://github.com/DegorasProjectTeam/LibThorlabsKinesis/issues">Request Feature</a>
  </p>
</div>

<!-- TABLE OF CONTENTS -->
<details>
  <summary>Table of Contents</summary>
  <ol>
    <li>
      <a href="#about-the-project">About The Project</a>
      <ul>
        <li><a href="#architecture">Architecture</a></li>
        <li><a href="#built-with">Built With</a></li>
      </ul>
    </li>
    <li>
      <a href="#getting-started">Getting Started</a>
      <ul>
        <li><a href="#prerequisites">Prerequisites</a></li>
        <li><a href="#build">Build</a></li>
      </ul>
    </li>
    <li><a href="#usage">Usage</a></li>
    <li><a href="#testing">Testing</a></li>
    <li><a href="#roadmap">Roadmap</a></li>
    <li><a href="#license">License</a></li>
    <li><a href="#contact">Contact</a></li>
    <li><a href="#acknowledgments">Acknowledgments</a></li>
  </ol>
</details>

<!-- ABOUT THE PROJECT -->
## About The Project

LibThorlabsKinesis is a production-quality, extensible C++17 library for controlling Thorlabs Kinesis
motion devices. It grows out of a validated proof of concept (the M30X / M30XY benchtop DC servo
drivers) and is rebuilt as a reusable library that additional Kinesis device families can join without
architectural changes.

It currently ships drivers for the single-axis **M30X** and the dual-axis **M30XY** stages behind a
hardware-agnostic motion interface, with deterministic resource ownership, a thread-safe per-device
locking model, an explicit error model, and an asynchronous status-polling/callback facility.

### Architecture

The library is organised in three layers. The layer-boundary rule: the vendor SDK header is included
**only** in Layer 2, so a new device on a different Kinesis module is added as a new Layer-2 adapter plus
a thin Layer-3 personality — by composition, never a shared base class — reusing Layer 1 unchanged.

| Layer | Folder | Knows about | Examples |
|-------|--------|-------------|----------|
| 1 — generic infrastructure | `Common/` | nothing vendor-specific | `OperationResult`, `DeviceError`, motion vocabulary, `StatusPoller<StatusT>`, `waitForCondition`, JSON helpers |
| 2 — per-SDK-module adapter | `DCServo/` | one Kinesis module's C API | `DCServoChannel`, `decodeDCServoStatus`, discovery, `KinesisSimulatorSession` |
| 3 — device personalities | `Devices/` | one device's identity & axis count | `M30X` (1 channel), `M30XY` (2 channels), behind `IMotionDevice` |

Headers are consumed either individually as `#include "LibThorlabsKinesis/<Layer>/<file>.h"`, or a whole
layer at once via a module aggregator: `#include <LibThorlabsKinesis/Modules/Devices>` (also `Common`,
`DCServo`).

### Built With

* C++17
* CMake (>= 3.21) and Ninja, driven by CMake Presets
* MSYS2 MinGW (UCRT64) toolchain — GCC
* Thorlabs Kinesis SDK (Benchtop DC Servo), vendored under `thirdparty/`

<p align="right">(<a href="#readme-top">back to top</a>)</p>

<!-- GETTING STARTED -->
## Getting Started

### Prerequisites

* An MSYS2 MinGW prefix providing GCC, Ninja and (optionally) windres. UCRT64 is the reference prefix.
* CMake >= 3.21.
* The `MINGW_ROOT` environment variable pointing at the prefix (e.g. `E:/msys64/ucrt64`). The provided
  `CMakeUserPresets.json` sets this for the local machine — adjust it for yours.

### Build

The CMake project root is the inner `LibThorlabsKinesis/` directory. Configure and build with a preset:

```sh
cd LibThorlabsKinesis
cmake --preset local-mingw-dynamic-deb
cmake --build --preset local-mingw-dynamic-deb
```

Project presets in `CMakePresets.json` cover `mingw-{dynamic,static}-{deb,rel}`; the `local-*` user presets
just add your `MINGW_ROOT`. Build options: `LIBTHORLABSKINESIS_BUILD_SHARED` (default ON),
`LIBTHORLABSKINESIS_BUILD_TESTING`, `LIBTHORLABSKINESIS_BUILD_EXAMPLES`.

Artifacts land in `build/<preset>/bin/` at the repository root. The build stages, next to the binaries,
both the vendored Thorlabs DLLs and the MinGW C++ runtime (`libstdc++-6.dll`, `libgcc_s_seh-1.dll`,
`libwinpthread-1.dll`), so the executables run without the toolchain on `PATH`.

<p align="right">(<a href="#readme-top">back to top</a>)</p>

<!-- USAGE -->
## Usage

```cpp
#include <LibThorlabsKinesis/Modules/DCServo>   // KinesisSimulatorSession
#include <LibThorlabsKinesis/Modules/Devices>   // M30XY, IMotionDevice, status types

using namespace thorlabs;
using thorlabs::types::Channel;
using thorlabs::types::OperationResult;

int main()
{
    // Optional: connect to the Kinesis Simulator (a no-op against real hardware).
    KinesisSimulatorSession sim;

    types::ThorlabsSNList serials;
    if (M30XY::getDeviceList(serials) != OperationResult::OPERATION_OK || serials.empty())
        return 1;

    M30XY dev(serials.front());                 // no device I/O in the constructor
    if (dev.doConnect() != OperationResult::OPERATION_OK)
        return 1;

    dev.doEnableChannels(true);
    dev.doHomeAll();
    dev.waitForHomed(Channel::X_CHANNEL, std::chrono::seconds(60));

    dev.doMoveAbsolute(Channel::X_CHANNEL, 5.0); // millimetres
    dev.waitForMoveFinished(Channel::X_CHANNEL, std::chrono::seconds(30));

    dev.doDisconnect();                          // also handled by the destructor
}
```

The single-axis `M30X` is used the same way with one channel (`Channel::X_CHANNEL`); a non-existent channel
returns `OperationResult::INVALID_CHANNEL`. See the `examples/` directory for a complete program.

<p align="right">(<a href="#readme-top">back to top</a>)</p>

<!-- TESTING -->
## Testing

There is no test-framework dependency: the `testing/` executables are plain `assert()`-based checks, named
`UT_*` for hardware-free unit tests, `Test_*` for integration tests, and `Example_*` for the demos under
`examples/`. Build them with the project (`LIBTHORLABSKINESIS_BUILD_TESTING=ON`) and run from
`build/<preset>/bin/`.

* `UT_*` — vocabulary/error mapping, status decode, the status poller, and JSON round-trip. No hardware.
* `Test_M30XYSim` / `Test_M30XSim` — full SDK round-trip against the Kinesis Simulator; self-skip if absent.
* `Test_Concurrency` — same-serial aliasing and two-device concurrent stress.
* `Test_M30XYMonitor [seconds]` — connects and continuously prints decoded status while running a scripted
  home / jog / move sequence; you can also drive the stage manually in the simulator and watch it evolve.
* `Test_M30XYHardware` — gated behind `--i-have-hardware`; conservative moves only. Read its safety notice
  before running on real optics.

> [!WARNING]
> The Kinesis Simulator exposes the devices and the full command/status path works, but it cannot be
> assigned a stage (travel range), so it does not physically translate position. The device tests therefore
> assert the SDK contract and only log position. Full physical-motion validation requires real hardware.

<p align="right">(<a href="#readme-top">back to top</a>)</p>

<!-- ROADMAP -->
## Roadmap

- [ ] Install rules + a CMake package config (`find_package(LibThorlabsKinesis)`) for external consumption.
- [ ] Additional Kinesis device families (e.g. stepper / KCube / brushless) as new Layer-2 adapters.
- [ ] Migrate the generic, project-agnostic infrastructure (status poller, wait helper, JSON utilities) into
      LibDegorasBase so it is shared rather than reimplemented per project.

<p align="right">(<a href="#readme-top">back to top</a>)</p>

<!-- LICENSE -->
## License

Distributed under the GNU General Public License v3.0 (or later). See the full text in the
[`LICENSE`](LICENSE) file for details.

<p align="right">(<a href="#readme-top">back to top</a>)</p>

<!-- CONTACT -->
## Contact

Degoras Project Team — Spanish Navy Observatory SLR station (SFEL), San Fernando.

* Ángel Vera Herrera — avera@roa.es · angelvh.engr@gmail.com
* Jesús Relinque Madroñal — jrelinque@roa.es

<p align="right">(<a href="#readme-top">back to top</a>)</p>

<!-- ACKNOWLEDGMENTS -->
## Acknowledgments

* [Thorlabs Kinesis](https://www.thorlabs.com/software_pages/ViewSoftwarePage.cfm?Code=Motion_Control) — the underlying motion-control SDK.
* Real Instituto y Observatorio de la Armada (ROA) and the SFEL SLR station, San Fernando.
* [LibDegorasBase](https://github.com/DegorasProjectTeam/LibDegorasBase) — Degoras Project base library and conventions.
* [Best-README-Template](https://github.com/othneildrew/Best-README-Template).

<p align="right">(<a href="#readme-top">back to top</a>)</p>

<!-- MARKDOWN LINKS & IMAGES -->
[cpp-shield]: https://img.shields.io/badge/C%2B%2B-17-00599C.svg?style=for-the-badge&logo=cplusplus
[cpp-url]: https://en.cppreference.com/w/cpp/17
[cmake-shield]: https://img.shields.io/badge/CMake-3.21%2B-064F8C.svg?style=for-the-badge&logo=cmake&logoColor=white
[cmake-url]: https://cmake.org/
[license-shield]: https://img.shields.io/badge/License-GPLv3-blue.svg?style=for-the-badge
[license-url]: https://www.gnu.org/licenses/gpl-3.0
[platform-shield]: https://img.shields.io/badge/platform-Windows%20|%20MinGW-lightgrey.svg?style=for-the-badge
[platform-url]: #getting-started
