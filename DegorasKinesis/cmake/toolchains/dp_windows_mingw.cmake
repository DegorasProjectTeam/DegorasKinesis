#
# Windows toolchain for MSYS2 prefixes.
# Requires environment variable:
#   DEVSYSTEM_TOOLCHAIN_ROOT -> the MSYS2 prefix, e.g. E:/msys64/ucrt64 (or /mingw64, /clang64). The
#                               DegorasSLR environment exports it.
# Honoured when present:
#   DEVSYSTEM_TOOLCHAIN      -> "gcc" or "clang"; picks the compiler driver. See below.
#

set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

if(NOT DEFINED ENV{DEVSYSTEM_TOOLCHAIN_ROOT} OR "$ENV{DEVSYSTEM_TOOLCHAIN_ROOT}" STREQUAL "")
    message(FATAL_ERROR "[CMAKE] dp_windows_mingw.cmake: DEVSYSTEM_TOOLCHAIN_ROOT is not set. It names the "
                        "MSYS2 prefix holding the compiler, e.g. E:/msys64/ucrt64; the DegorasSLR environment "
                        "exports it. It replaced MINGW_ROOT, which named the same directory, so an environment "
                        "generated before the rename wants step 2 of DrivEnv-Win re-run.")
endif()

set(_PFX "$ENV{DEVSYSTEM_TOOLCHAIN_ROOT}")

# COMPILER DRIVER. gcc.exe and g++.exe are right on ucrt64 and mingw64, and they also WORK on clang64, where
# MSYS2 ships them as byte-identical copies of clang -- which is why this named them unconditionally. Those
# copies are exactly what makes the family undiscoverable, though: nothing about the prefix says which compiler
# you are getting, and a configure log reading "GNU" for a clang build is a lie that costs an afternoon. So when
# the environment states the family, believe it and invoke the real driver. Unstated, the old behaviour stands.
set(_CC  "gcc.exe")
set(_CXX "g++.exe")
if("$ENV{DEVSYSTEM_TOOLCHAIN}" STREQUAL "clang" AND EXISTS "${_PFX}/bin/clang.exe")
    set(_CC  "clang.exe")
    set(_CXX "clang++.exe")
endif()

set(CMAKE_C_COMPILER   "${_PFX}/bin/${_CC}"  CACHE FILEPATH "MSYS2 C compiler"   FORCE)
set(CMAKE_CXX_COMPILER "${_PFX}/bin/${_CXX}" CACHE FILEPATH "MSYS2 C++ compiler" FORCE)

# Resource compiler (optional, but common for Windows builds).
if(EXISTS "${_PFX}/bin/windres.exe")
    set(CMAKE_RC_COMPILER "${_PFX}/bin/windres.exe" CACHE FILEPATH "MSYS2 windres" FORCE)
endif()

# Prefer Ninja from the same prefix if available.
if(EXISTS "${_PFX}/bin/ninja.exe")
    set(CMAKE_MAKE_PROGRAM "${_PFX}/bin/ninja.exe" CACHE FILEPATH "Ninja from MSYS2 prefix" FORCE)
endif()

# Prefer config packages, and ensure the prefix is searched first.
set(CMAKE_FIND_PACKAGE_PREFER_CONFIG ON CACHE BOOL "" FORCE)
list(PREPEND CMAKE_PREFIX_PATH "${_PFX}")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE BOOL "" FORCE)
