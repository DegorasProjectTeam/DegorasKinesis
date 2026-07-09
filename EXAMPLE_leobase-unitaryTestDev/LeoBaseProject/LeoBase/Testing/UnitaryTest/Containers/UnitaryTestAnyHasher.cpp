/*
 * Copyright(C) Milethos Technologies SLU. 2026
 * All rights reserved. Reproduction in whole or in
 * part is prohibited without the written consent of
 * the copyright holder.
 */

// AnyHasher UnitaryTest

// C++ INCLUDES
#include <cstdint>
#include <string>
#include <any>

// LEOBASE INCLUDES
#include <LeoBase/Aliases/Testing>
#include <LeoBase/Aliases/Containers>

#include <LeoBase/Containers/any_hasher.h>
#include <LeoBase/Exceptions/leobase_exception.h>


// ---------------------------------------------------------------------------------------------------------------------

using namespace leobase;
using namespace leobase::containers;

// 1. Test Declaration
UT_DECLARE_TEST(HashConsistencySameValue)
UT_DECLARE_TEST(HashDifferentValues)
UT_DECLARE_TEST(HashDifferentTypesSameValue)
UT_DECLARE_TEST(HashUnsupportedType)

// 2. Define Tests

/**
 * @brief Verifies that hashing the same value multiple times produces the same hash result.
 */
UT_DEFINE_TEST(HashConsistencySameValue)
{
    AnyHasher hasher;
    std::any val = int(42);

    std::size_t hash1 = hasher(val);
    std::size_t hash2 = hasher(val);

    // Hash must be deterministic for the same input
    UT_EXPECTED_TRUE(hash1 == hash2);
}

/**
 * @brief Verifies that different values (of the same type) produce different hash results.
 */
UT_DEFINE_TEST(HashDifferentValues)
{
    AnyHasher hasher;

    std::size_t h1 = hasher(std::any(int(10)));
    std::size_t h2 = hasher(std::any(int(20)));
    std::size_t h3 = hasher(std::any(std::string("A")));
    std::size_t h4 = hasher(std::any(std::string("B")));

    UT_EXPECTED_TRUE(h1 != h2);
    UT_EXPECTED_TRUE(h3 != h4);
}

/**
 * @brief Verifies that different types with the same "logical value" produce different hashes.
 * In LeoBase, hash is based on typeid().hash_code() + value.
 */
UT_DEFINE_TEST(HashDifferentTypesSameValue)
{
    AnyHasher hasher;

    // Even if both are '5', the internal type hash makes them unique
    std::size_t h_int = hasher(std::any(int(5)));
    std::size_t h_short = hasher(std::any(short(5)));

    UT_EXPECTED_TRUE(h_int != h_short);
}

/**
 * @brief Verifies that types not present in the supported list throw a LeoBaseException.
 */
UT_DEFINE_TEST(HashUnsupportedType)
{
    AnyHasher hasher;

    try {
        // float is not supported in the current implementation
        float unsupported = 1.23f;
        hasher(std::any(unsupported));

        UT_FORCE_FAIL(); // Failure if no exception is thrown
    } catch (const leobase::exceptions::LeoBaseException& e) {
        // Success: Exception caught as expected
        UT_EXPECTED_TRUE(true);
    } catch (...) {
        // Failure: Wrong exception type
        UT_FORCE_FAIL();
    }
}

// 3. Main session execution
UT_START_SESSION("AnyHasher Test Session")

UT_FORCE_STOP_IF_SUBTEST_FAIL(false);

UT_REGISTER_TEST(Containers, Utilities, HashConsistencySameValue);
UT_REGISTER_TEST(Containers, Utilities, HashDifferentValues);
UT_REGISTER_TEST(Containers, Utilities, HashDifferentTypesSameValue);
UT_REGISTER_TEST(Containers, Utilities, HashUnsupportedType);

UT_RUN_TESTS();

UT_FINISH_SESSION()
