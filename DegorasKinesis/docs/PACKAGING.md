# Packaging, Install & Deployment

DegorasKinesis installs a single prefix-relative layout (via `GNUInstallDirs`) that serves **both** a standard
standalone install and a **vcpkg** overlay port. This document covers both.

> **Platform / ABI:** the library controls the proprietary, **Windows-only** Thorlabs Kinesis SDK and its public API
> passes C++ standard-library types across the library boundary. It is therefore built and consumed with a
> **MinGW/UCRT64** toolchain, and a consumer must use a compatible one. It cannot be consumed from an MSVC build.

---

## 1. Standalone install (no vcpkg)

Configure, build and install with the provided presets (they use the bundled MinGW toolchain and require the
`MINGW_ROOT` environment variable, e.g. `E:/msys64/ucrt64`):

```sh
cd DegorasKinesis
cmake --preset local-mingw-dynamic-rel
cmake --build --preset local-mingw-dynamic-rel
cmake --install ../build/local-mingw-dynamic-rel --prefix /path/to/prefix
```

Installed layout (relative to the prefix):

| Path | Contents |
|------|----------|
| `bin/` | `DegorasKinesis.dll` and (opt-out) the vendored Thorlabs runtime DLLs |
| `lib/` | import/static library |
| `include/DegorasKinesis/` | public headers |
| `lib/cmake/DegorasKinesis/` | `DegorasKinesisConfig.cmake`, `…ConfigVersion.cmake`, `DegorasKinesisTargets*.cmake`, `FindThorlabsKinesis.cmake` |
| `share/DegorasKinesis/` | `copyright`, `usage` |

### Install options

| Option | Default | Effect |
|--------|---------|--------|
| `DEGORASKINESIS_INSTALL` | ON if top-level | Master switch for all install/export rules |
| `DEGORASKINESIS_BUILD_SHARED` | follows `BUILD_SHARED_LIBS`, else ON | Shared vs static |
| `DEGORASKINESIS_INSTALL_THORLABS_RUNTIME` | ON | Install the vendored Thorlabs `*.dll` into `bin/` |
| `DEGORASKINESIS_BUILD_DOCS` | OFF | Install `README.md` into the doc dir |
| `DEGORASKINESIS_INSTALL_EXAMPLES` | OFF | Install the example sources |
| `DEGORASKINESIS_BUILD_TOOLS` | OFF | Build/install CLI tools (none yet) |
| `DEGORASKINESIS_BUILD_TESTING` | ON | Build tests and register them with CTest |

Run the tests with CTest from the build tree: `ctest --output-on-failure` (the long-running monitors and the
hardware-gated test are intentionally not registered).

## 2. Consuming the installed package

```cmake
find_package(DegorasKinesis CONFIG REQUIRED)
target_link_libraries(my_app PRIVATE Degoras::Kinesis)
```

Point CMake at the prefix with `-DCMAKE_PREFIX_PATH=/path/to/prefix` and use the same MinGW toolchain. A **shared**
consumer needs nothing else at configure time (the Thorlabs dependency is sealed in the DLL); a **static** consumer
additionally re-resolves Thorlabs via `find_dependency(ThorlabsKinesis)` — set `THORLABS_KINESIS_ROOT` if the SDK is
not auto-detected. At run time the Thorlabs `*.dll` must be discoverable (installed alongside, on `PATH`, or via the
Thorlabs Kinesis software).

## 3. vcpkg (overlay / private registry)

Because the Thorlabs SDK is proprietary and vendored, this is an **overlay / private-registry** port, never an
official curated one. Files are provided:

- `cmake/triplets/x64-mingw-dynamic.cmake` and `x64-mingw-static.cmake` — chainload the MinGW toolchain.
- `ports/degoras-kinesis/{vcpkg.json,portfile.cmake}` — the port.

### Manifest or classic mode

```sh
# classic mode
vcpkg install degoras-kinesis \
    --overlay-ports=DegorasKinesis/ports \
    --overlay-triplets=DegorasKinesis/cmake/triplets \
    --triplet x64-mingw-dynamic
```

```json
// manifest mode (vcpkg.json in your project), then `vcpkg install`
{ "dependencies": ["degoras-kinesis"] }
```
…invoked with the same `--overlay-ports` / `--overlay-triplets` (or configured in `vcpkg-configuration.json`), and
`MINGW_ROOT` exported in the environment.

The port runs `vcpkg_cmake_configure` → `vcpkg_cmake_install` → `vcpkg_cmake_config_fixup(PACKAGE_NAME degoraskinesis
CONFIG_PATH lib/cmake/DegorasKinesis)`, which relocates the CMake package files to `share/degoraskinesis/` and merges
the debug/release trees — producing the standard vcpkg installed layout (`bin`, `lib`, `include`, `share`,
`debug/bin`, `debug/lib`, …). Consumers then use the exact same `find_package(DegorasKinesis)` / `Degoras::Kinesis`.

### Publishing note

`portfile.cmake` uses `vcpkg_from_github` with a placeholder `REF`/`SHA512`. When the repository is published, set
`REF` to the release tag and `SHA512` to the archive hash (vcpkg prints the expected value on the first configure);
no other change is needed. Until then, point the port at a local checkout for testing.

### Why not an official vcpkg port?

The curated registry forbids vendored/prebuilt proprietary binaries and requires from-source, redistributable,
CI-built dependencies. The Thorlabs Kinesis SDK is closed, Windows-only, and under an EULA, so an official port is
not feasible; an overlay or private registry is the supported route.
