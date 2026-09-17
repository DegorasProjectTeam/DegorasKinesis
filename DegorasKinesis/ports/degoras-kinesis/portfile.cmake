# Overlay port for DegorasKinesis.
#
# REF and SHA512 are REAL as of v0.1.0: the tag exists and the hash below is of the archive GitHub generated for
# it. Both must move together on every release -- REF follows ${VERSION} from vcpkg.json, and the hash has to be
# recomputed afterwards, because GitHub cannot produce the archive until the tag is pushed. Getting one without
# the other is the failure this port shipped with until now: SHA512 0 against a tag that did not exist.
#
# To develop against a working tree instead of a release, see the overlay instructions in docs/PACKAGING.md.
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
    SHA512 c86fcb9221ac853ee1607de85d8dd3a963f3ced1070ef34af1f015e04d0f9cac5d76a882c315ec68e07a9eaa1d7caef573853b5d8b525c9f09c556e8b65a6741
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
