/*
 * Copyright(C) Milethos Technologies SLU. 2026
 * All rights reserved. Reproduction in whole or in
 * part is prohibited without the written consent of
 * the copyright holder.
 */

// C++ INCLUDES
#include <type_traits>

// LeoBase INCLUDES
#include <LeoBase/Aliases/Testing>
#include "LeoBase/Patterns/non_instantiable.h"
#include "LeoBase/Testing/unitary/unitary_test_macros.h"

using namespace leobase;
using namespace leobase::testing;
using namespace leobase::patterns;

// =======================================================================
// HELPER CLASSES
// =======================================================================
namespace {
    /**
     * @brief Dummy utility class to test the NonInstantiable behavior.
     * It simulates a class that should only contain static methods.
     */

    //We dispense with the assignment constructor and operator= since the class is not instantiable at all, so they are irrelevant.
    class DummyNonInstantiable : public NonInstantiable 
    {
    public:
        static int doSomething() { return 23; }
    };
}


// =======================================================================
// DECLARATIONS
// =======================================================================

UT_DECLARE_TEST(IsNotDefaultConstructable);
UT_DECLARE_TEST(IsNotCopyConstructible);
UT_DECLARE_TEST(IsNotCopyAssignable);

// =======================================================================
// DEFINITIONS
// =======================================================================

/**
 * @brief Verifies that a derived class cannot be instantiated using the default constructor.
 */
UT_DEFINE_TEST(IsNotDefaultConstructable)
{
    // Evaluates to false at compile-time thanks to the deleted default constructor
    bool isConstructible = std::is_default_constructible<DummyNonInstantiable>::value;
    UT_EXPECTED_FALSE(isConstructible);
}

/**
 * @brief Verifies that a derived class cannot be copy-constructed.
 */
UT_DEFINE_TEST(IsNotCopyConstructible)
{
    bool isCopyConstructible = std::is_copy_constructible<DummyNonInstantiable>::value;
    UT_EXPECTED_FALSE(isCopyConstructible);
}

/**
 * @brief Verifies that a derived class cannot be copy-assigned.
 */
UT_DEFINE_TEST(IsNotCopyAssignable)
{
    bool isCopyAssignable = std::is_copy_assignable<DummyNonInstantiable>::value;
    UT_EXPECTED_FALSE(isCopyAssignable);
}

// =======================================================================
// SESSION 
// =======================================================================
UT_START_SESSION("LeoBase Patterns NonInstantiable Unitary Tests")

UT_REGISTER_TEST(Patterns, NonInstantiable, IsNotDefaultConstructable);
UT_REGISTER_TEST(Patterns, NonInstantiable, IsNotCopyConstructible);
UT_REGISTER_TEST(Patterns, NonInstantiable, IsNotCopyAssignable);

UT_RUN_TESTS();

UT_FINISH_SESSION()
