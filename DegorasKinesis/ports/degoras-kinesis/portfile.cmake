# Overlay port for DegorasKinesis.
#
# The public repository is not published yet, so REF/SHA512 below are placeholders. Until then, build against a local
# checkout using the overlay instructions in docs/PACKAGING.md (SOURCE_PATH can be pointed at the working tree). Once
# the repo is public, set REF to the release tag and SHA512 to the archive hash (vcpkg prints the expected value on
# the first configure) and this port works unchanged.
#
# The library vendors the proprietary, Windows-only Thorlabs Kinesis SDK (thirdparty/Thorlabs), so this is an
# OVERLAY / private-registry port, never an official curated one. Use a MinGW triplet (see cmake/triplets/).
#
# THE CONSTRAINT IS THE C++ RUNTIME, NOT THE PREFIX. The public API passes std::string, std::vector and
# std::function across the DLL boundary, so the consumer must be built with the SAME C++ standard library as
# this library -- libstdc++ with libstdc++, libc++ with libc++. That is a real and unforgiving constraint, and
# mixing them produces link errors or, worse, corruption at run time.
#
# It is NOT a lock to UCRT64, which is what this note used to claim. Measured: the library builds under
# clang64/libc++ (Clang 22.1.8) with zero diagnostics and passes all nine tests against the Kinesis Simulator,
# exactly as it does under ucrt64/libstdc++. Either prefix is fine; what must not differ is the pairing.

vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO DegorasProjectTeam/DegorasKinesis
    REF "v${VERSION}"
    SHA512 0   # TODO(publish): replace with the real archive SHA512.
    HEAD_REF main
)

# The CMake project root is the INNER directory of the repository.
vcpkg_cmake_configure(
    SOURCE_PATH "${SOURCE_PATH}/DegorasKinesis"
    OPTIONS
        -DDEGORASKINESIS_INSTALL=ON
        -DDEGORASKINESIS_BUILD_TESTING=OFF
        -DDEGORASKINESIS_BUILD_EXAMPLES=OFF
        -DDEGORASKINESIS_INSTALL_THORLABS_RUNTIME=ON
)

vcpkg_cmake_install()

# Relocate the CMake package files from lib/cmake/DegorasKinesis to share/degoraskinesis (vcpkg convention).
vcpkg_cmake_config_fixup(PACKAGE_NAME degoraskinesis CONFIG_PATH lib/cmake/DegorasKinesis)

# vcpkg forbids a debug/include or debug/share tree, and owns copyright placement.
file(REMOVE_RECURSE
    "${CURRENT_PACKAGES_DIR}/debug/include"
    "${CURRENT_PACKAGES_DIR}/debug/share")

# Ship the usage note and the license as the port copyright.
configure_file(
    "${SOURCE_PATH}/DegorasKinesis/cmake/usage"
    "${CURRENT_PACKAGES_DIR}/share/${PORT}/usage"
    COPYONLY)
vcpkg_install_copyright(FILE_LIST "${SOURCE_PATH}/LICENSE")
