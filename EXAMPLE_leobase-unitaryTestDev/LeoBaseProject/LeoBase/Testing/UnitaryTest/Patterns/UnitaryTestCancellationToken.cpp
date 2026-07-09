/*
 * Copyright(C) Milethos Technologies SLU. 2026
 * All rights reserved. Reproduction in whole or in
 * part is prohibited without the written consent of
 * the copyright holder.
*/

#include <LeoBase/Aliases/Patterns>
#include <LeoBase/Aliases/Testing>
#include <LeoBase/Aliases/Exceptions>

// Ensure that the exceptions Alias actually includes yours.
// If it still fails, uncomment the following line and set the real path:
#include <LeoBase/Exceptions/canceled_exception.h>

#include "LeoBase/Patterns/cancellation_token.h"
#include "LeoBase/Testing/unitary/unitary_test_macros.h"

using namespace leobase;
using namespace leobase::testing;
using namespace leobase::patterns;
using namespace leobase::exceptions;

// --- Initial State Tests ---
UT_DECLARE_TEST(InitialState_IsNotCancelled);

// --- CancellationToken Tests ---
UT_DECLARE_TEST(Cancel_SetsIsCancelledToTrue);
UT_DECLARE_TEST(Cancel_MultipleCallsAreIdempotent);

// --- Exceptions & Edge Cases ---
UT_DECLARE_TEST(ThrowIfCanceled_ThrowsWhenCanceled);
UT_DECLARE_TEST(ThrowIfCanceled_DoesNotThrowWhenSafe);


// =======================================================================

UT_DEFINE_TEST(InitialState_IsNotCancelled)
{
    CancellationToken token;
    UT_EXPECTED_FALSE(token.isCanceled());
}

UT_DEFINE_TEST(Cancel_SetsIsCancelledToTrue)
{
    CancellationToken token;
    token.cancel();
    UT_EXPECTED_TRUE(token.isCanceled());
}

/**
 * @brief Checks that calling cancel() multiple times does not break anything (idempotence).
 */
UT_DEFINE_TEST(Cancel_MultipleCallsAreIdempotent)
{
    CancellationToken token;
    token.cancel();
    token.cancel();
    token.cancel();
    UT_EXPECTED_TRUE(token.isCanceled());
}

/**
 * @brief Checks that throwIfCanceled() throws a CanceledException if the token is canceled.
 */
UT_DEFINE_TEST(ThrowIfCanceled_ThrowsWhenCanceled)
{
    CancellationToken token;
    token.cancel();

    // Fixed: Since we are already using "using namespace leobase::exceptions",
    // we only need to write the class name.
    // NOTE: If it still fails, check if your framework spells it with a double L (CancelledException).
    UT_EXPECT_THROW(token.throwIfCanceled(), CanceledException);
}

/**
 * @brief Tests that throwIfCanceled() does NOT throw anything if the token is active.
 */
UT_DEFINE_TEST(ThrowIfCanceled_DoesNotThrowWhenSafe)
{
    CancellationToken token;

    bool exceptionThrown = false;
    try
    {
        token.throwIfCanceled();
    }
    catch (...)
    {
        exceptionThrown = true;
    }

    UT_EXPECTED_FALSE(exceptionThrown);
    //UT_EXPECT_NO_THROW(token.throwIfCanceled());
}

UT_START_SESSION("LeoBase CancellationToken Unitary Tests")

UT_REGISTER_TEST(Patterns, CancellationToken, InitialState_IsNotCancelled);
UT_REGISTER_TEST(Patterns, CancellationToken, Cancel_SetsIsCancelledToTrue);
UT_REGISTER_TEST(Patterns, CancellationToken, Cancel_MultipleCallsAreIdempotent);
UT_REGISTER_TEST(Patterns, CancellationToken, ThrowIfCanceled_ThrowsWhenCanceled);
UT_REGISTER_TEST(Patterns, CancellationToken, ThrowIfCanceled_DoesNotThrowWhenSafe);

UT_RUN_TESTS();

UT_FINISH_SESSION()

//
