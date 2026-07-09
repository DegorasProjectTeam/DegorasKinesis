/*
 * Copyright(C) Milethos Technologies SLU. 2026
 * All rights reserved. Reproduction in whole or in
 * part is prohibited without the written consent of
 * the copyright holder.
 */

 // C++ INCLUDES
#include <type_traits>
#include <utility>

// LeoBase INCLUDES
#include <LeoBase/Aliases/Testing>
#include "LeoBase/Patterns/non_movable.h"
#include "LeoBase/Testing/unitary/unitary_test_macros.h"

using namespace leobase;
using namespace leobase::testing;
using namespace leobase::patterns;


// =======================================================================
// HELPER CLASSES
// =======================================================================
namespace {
    /**
     * @brief Dummy class to test the NonMovable behavior.
     */
    class DummyNonMovable : public NonMovable 
    {
    public:
        DummyNonMovable() = default;
        ~DummyNonMovable() = default;
    };
}


// =======================================================================
// DECLARATIONS
// =======================================================================

UT_DECLARE_TEST(IsNotCopyConstructible);
UT_DECLARE_TEST(IsNotCopyAssignable);
UT_DECLARE_TEST(IsNotMoveConstructible);
UT_DECLARE_TEST(IsNotMoveAssignable);

// =======================================================================
// DEFINITIONS
// =======================================================================

/**
 * @brief Verifies that a derived class cannot be copy-constructed.
 */

UT_DEFINE_TEST(IsNotCopyConstructible)
{
    // Evaluates to false at compile-time thanks to the deleted copy constructor
    bool isCopyConstructible = std::is_copy_constructible<DummyNonMovable>::value;
    UT_EXPECTED_FALSE(isCopyConstructible);
}

/**
 * @brief Verifies that a derived class cannot be copy-assigned.
 */
UT_DEFINE_TEST(IsNotCopyAssignable)
{
    // Evaluates to false at compile-time thanks to the deleted copy assignment operator
    bool isCopyAssignable = std::is_copy_assignable<DummyNonMovable>::value;
    UT_EXPECTED_FALSE(isCopyAssignable);
}

/**
 * @brief Verifies that a derived class cannot be move-constructed.
 */
UT_DEFINE_TEST(IsNotMoveConstructible)
{
    // Evaluates to false at compile-time thanks to the deleted move constructor
    bool isMoveConstructible = std::is_move_constructible<DummyNonMovable>::value;
    UT_EXPECTED_FALSE(isMoveConstructible);
}

/**
 * @brief Verifies that a derived class cannot be move-assigned.
 */
UT_DEFINE_TEST(IsNotMoveAssignable)
{
    // Evaluates to false at compile-time thanks to the deleted move assignment operator
    bool isMoveAssignable = std::is_move_assignable<DummyNonMovable>::value;
    UT_EXPECTED_FALSE(isMoveAssignable);
}

// =======================================================================
// SESION
// =======================================================================

UT_START_SESSION("LeoBase Patterns NonMovable Unitary Tests")

UT_REGISTER_TEST(Patterns, NonMovable, IsNotCopyConstructible);
UT_REGISTER_TEST(Patterns, NonMovable, IsNotCopyAssignable);
UT_REGISTER_TEST(Patterns, NonMovable, IsNotMoveConstructible);
UT_REGISTER_TEST(Patterns, NonMovable, IsNotMoveAssignable);

UT_RUN_TESTS();

UT_FINISH_SESSION()
