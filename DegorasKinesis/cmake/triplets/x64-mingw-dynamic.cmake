# vcpkg community-style triplet: 64-bit MinGW, dynamic linkage.
#
# This library's public API passes C++ standard-library types across the boundary, so it must be built with the same
# MinGW/UCRT64 toolchain as its consumers. This triplet chainloads the project's MinGW toolchain file. Requires the
# MINGW_ROOT environment variable (e.g. E:/msys64/ucrt64); it is passed through to the build.
#
# Use with: --overlay-triplets=<this dir> --triplet x64-mingw-dynamic   (see docs/PACKAGING.md)

set(VCPKG_TARGET_ARCHITECTURE x64)
set(VCPKG_CRT_LINKAGE dynamic)
set(VCPKG_LIBRARY_LINKAGE dynamic)

set(VCPKG_CMAKE_SYSTEM_NAME "")   # native Windows (not a cross build)
set(VCPKG_ENV_PASSTHROUGH MINGW_ROOT)
set(VCPKG_CHAINLOAD_TOOLCHAIN_FILE "${CMAKE_CURRENT_LIST_DIR}/../toolchains/dp_windows_mingw.cmake")

# The vendored Thorlabs SDK ships DLLs without matching (MinGW-style) import libs; allow them in the package.
set(VCPKG_POLICY_DLLS_WITHOUT_LIBS enabled)
