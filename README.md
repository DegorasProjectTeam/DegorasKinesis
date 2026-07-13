<a id="readme-top"></a>

<!-- PROJECT SHIELDS -->
[![C++17][cpp-shield]][cpp-url]
[![CMake][cmake-shield]][cmake-url]
[![License: GPL v3][license-shield]][license-url]
[![Platform][platform-shield]][platform-url]

<!-- PROJECT TITLE -->
<div align="center">
  <h1 align="center">LibDegorasKinesis</h1>

  <p align="center">
    An extensible C++17 library for controlling Thorlabs Kinesis motion devices.
    <br />
    <a href="#about-the-project"><strong>Explore the docs »</strong></a>
    <br />
    <br />
    <a href="#usage">View Usage</a>
    &middot;
    <a href="https://github.com/DegorasProjectTeam/LibDegorasKinesis/issues">Report Bug</a>
    &middot;
    <a href="https://github.com/DegorasProjectTeam/LibDegorasKinesis/issues">Request Feature</a>
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

LibDegorasKinesis is a production-quality, extensible C++17 library for controlling Thorlabs Kinesis
motion devices. It grows out of a validated proof of concept (the M30X / M30XY benchtop DC servo
drivers) and is rebuilt as a reusable library that additional Kinesis device families can join without
architectural changes.

It currently ships drivers for the single-axis **M30X** and the dual-axis **M30XY** benchtop DC-servo stages
and the **K10CR2/M** motorized rotation stage (an integrated-stepper device, in degrees), behind a
hardware-agnostic motion interface, with deterministic resource ownership, a thread-safe per-device
locking model, an explicit error model, and an asynchronous status-polling/callback facility.

### Architecture

The library is organised in layers. The layer-boundary rule: a given vendor SDK module header is included
**only** in its own Layer-2 adapter, so a new device on a different Kinesis module is added as a new Layer-2
adapter plus a thin Layer-3 personality — by composition, never a shared base class — reusing the lower
layers unchanged.

| Layer | Folders | Knows about | Examples |
|-------|---------|-------------|----------|
| 1 — generic infrastructure | `Global/`, `Common/`, `Helpers/` | nothing vendor-specific | export macro; `OperationResult`, `DeviceError`, motion vocabulary; `StatusPoller<StatusT>`, `waitForCondition`, JSON helpers |
| K — shared Kinesis family | `Kinesis/` | the Kinesis-family C API shared across modules | discovery/per-serial locks, `KinesisSimulatorSession`, `MotorStatusFlags` + `decodeMotorStatus` (the shared MOT status word), `categoryFromKinesis`, `enumerateByTypeId` |
| 2 — per-SDK-module adapter | `DCServo/`, `IntStepper/` | one Kinesis module's C API | `DCServoChannel` (Benchtop DC Servo, `BDC_*`); `IntStepperController` (Integrated Stepper, `ISC_*`) |
| 3 — device personalities | `Devices/` | one device's identity & axis count | `M30X` (1 axis), `M30XY` (2 axes), `K10CR2` (1 rotation axis, degrees), behind `IMotionDevice` |

Within Layer 1: `Global/` holds the export macro, `Common/` the motion vocabulary and result/error model, and
`Helpers/` the generic infrastructure. The shared `Kinesis/` layer holds the module-agnostic Kinesis plumbing
reused by every adapter (the status word is identical across the DC-servo and integrated-stepper modules).
Headers are consumed individually as `#include "LibDegorasKinesis/<Folder>/<file>.h"`, or a whole group via a
module aggregator: `#include <LibDegorasKinesis/Modules/Devices>` (also `Common`, `Helpers`, `Kinesis`,
`DCServo`, `IntStepper`).

### Built With

* C++17
* CMake (>= 3.21) and Ninja, driven by CMake Presets
* MSYS2 MinGW (UCRT64) toolchain — GCC
* Thorlabs Kinesis SDK (Benchtop DC Servo + Integrated Stepper Motors), vendored under `thirdparty/`

<p align="right">(<a href="#readme-top">back to top</a>)</p>

<!-- GETTING STARTED -->
## Getting Started

### Prerequisites

* An MSYS2 MinGW prefix providing GCC, Ninja and (optionally) windres. UCRT64 is the reference prefix.
* CMake >= 3.21.
* The `MINGW_ROOT` environment variable pointing at the prefix (e.g. `E:/msys64/ucrt64`). The provided
  `CMakeUserPresets.json` sets this for the local machine — adjust it for yours.

### Build

The CMake project root is the inner `LibDegorasKinesis/` directory. Configure and build with a preset:

```sh
cd LibDegorasKinesis
cmake --preset local-mingw-dynamic-deb
cmake --build --preset local-mingw-dynamic-deb
```

Project presets in `CMakePresets.json` cover `mingw-{dynamic,static}-{deb,rel}`; the `local-*` user presets
just add your `MINGW_ROOT`. Build options: `LIBDEGORASKINESIS_BUILD_SHARED` (default ON),
`LIBDEGORASKINESIS_BUILD_TESTING`, `LIBDEGORASKINESIS_BUILD_EXAMPLES`.

Artifacts land in `build/<preset>/bin/` at the repository root. The build stages, next to the binaries,
both the vendored Thorlabs DLLs and the MinGW C++ runtime (`libstdc++-6.dll`, `libgcc_s_seh-1.dll`,
`libwinpthread-1.dll`), so the executables run without the toolchain on `PATH`.

<p align="right">(<a href="#readme-top">back to top</a>)</p>

<!-- USAGE -->
## Usage

```cpp
#include <LibDegorasKinesis/Modules/Kinesis>   // KinesisSimulatorSession
#include <LibDegorasKinesis/Modules/Devices>   // M30XY, K10CR2, IMotionDevice, status types

using namespace dpkin;
using namespace dpkin::kinesis;                 // KinesisSimulatorSession, discovery, status decode
using dpkin::types::Channel;
using dpkin::types::OperationResult;

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

The single-axis `M30X` and the rotation-stage `K10CR2` are used the same way with one channel
(`Channel::X_CHANNEL`); a non-existent channel returns `OperationResult::INVALID_CHANNEL`. For the K10CR2,
positions and distances are in **degrees** rather than millimetres. `Device::isCompatibleSerial(serial)` checks
whether a serial number belongs to that device type (Thorlabs serials begin with the device type id). See the
`examples/` directory for complete programs (`M30X_control`, `M30XY_control`, `K10CR2_control`); each accepts an
optional device serial on the command line and otherwise uses the first discovered device of that type.

<p align="right">(<a href="#readme-top">back to top</a>)</p>

<!-- TESTING -->
## Testing

There is no test-framework dependency: the `testing/` executables are plain `assert()`-based checks, named
`UT_*` for hardware-free unit tests and `Test_*` for integration tests (build with
`LIBDEGORASKINESIS_BUILD_TESTING=ON`). The `examples/` demos (`Example_*`) build with
`LIBDEGORASKINESIS_BUILD_EXAMPLES=ON`; each is a self-contained subproject (`<name>/CMakeLists.txt` +
`main.cpp`) — the generic `basic_device_discovery`, `basic_device_connection`, `multi_device_control` and the
device-specific `M30X_control`, `M30XY_control`, `K10CR2_control`. Everything runs from `build/<preset>/bin/`.

* `UT_*` — vocabulary/error mapping, status decode, the status poller, and JSON round-trip. No hardware.
* `Test_M30XYSim` / `Test_M30XSim` / `Test_K10CR2Sim [serial]` — full SDK round-trip against the Kinesis Simulator
  (serial `55000002` for the K10CR2); each optionally takes a device serial, else uses the first discovered;
  self-skip if absent.
* `Test_Concurrency` — same-serial aliasing and two-device concurrent stress.
* `Test_M30XYMonitor [seconds] [serial]` / `Test_K10CR2Monitor [seconds] [serial]` — connect and continuously print decoded status
  while running a scripted home / jog / move sequence; you can also drive the stage manually in the simulator and
  watch it evolve.
* `Test_M30XYHardware` — gated behind `--i-have-hardware`; conservative moves only. Read its safety notice
  before running on real optics.

> [!WARNING]
> The Kinesis Simulator exposes the devices and the full command/status path works, but it cannot be
> assigned a stage (travel range), so it does not physically translate position. The device tests therefore
> assert the SDK contract and only log position. Full physical-motion validation requires real hardware.

<p align="right">(<a href="#readme-top">back to top</a>)</p>

<!-- ROADMAP -->
## Roadmap

- [ ] Install rules + a CMake package config (`find_package(LibDegorasKinesis)`) for external consumption.
- [x] Integrated Stepper Motors module (K10CR2/M rotation stage) as the `IntStepper/` Layer-2 adapter.
- [ ] Further Kinesis device families (e.g. KCube / brushless) as new Layer-2 adapters, reusing the shared `Kinesis/` layer.
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
