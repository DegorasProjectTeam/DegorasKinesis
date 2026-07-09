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
#include "LeoBase/Patterns/non_copyable.h"
#include "LeoBase/Testing/unitary/unitary_test_macros.h"

using namespace leobase;
using namespace leobase::testing;
using namespace leobase::patterns;

// =======================================================================
// HELPER CLASSES
// =======================================================================
namespace {
    /**
     * @brief Dummy class to test the NonCopyable behavior.
     */
    class DummyNonCopyable : public NonCopyable 
    {
    public:
        int value{0};

        DummyNonCopyable() = default;
        explicit DummyNonCopyable(int v) : value(v) {}
        
        // Default move semantics so we can test them
        DummyNonCopyable(DummyNonCopyable&&) noexcept = default;
        DummyNonCopyable& operator=(DummyNonCopyable&&) noexcept = default;
        
        ~DummyNonCopyable() = default;
    };
}

// =======================================================================
// DECLARATIONS
// =======================================================================

UT_DECLARE_TEST(IsNotCopyConstructible);
UT_DECLARE_TEST(IsNotCopyAssignable);
UT_DECLARE_TEST(IsMoveConstructible);
UT_DECLARE_TEST(IsMoveAssignable);
UT_DECLARE_TEST(MoveOperationsTransferState);

// =======================================================================
// DEFINITIONS
// =======================================================================

/**
 * @brief Verifies that a derived class cannot be copy-constructed.
 */
UT_DEFINE_TEST(IsNotCopyConstructible)
{
    // Evaluates to false at compile-time thanks to the deleted copy constructor
    bool isCopyConstructible = std::is_copy_constructible<DummyNonCopyable>::value;
    UT_EXPECTED_FALSE(isCopyConstructible);
}

/**
 * @brief Verifies that a derived class cannot be copy-assigned.
 */
UT_DEFINE_TEST(IsNotCopyAssignable)
{
    // Evaluates to false at compile-time thanks to the deleted copy assignment
    bool isCopyAssignable = std::is_copy_assignable<DummyNonCopyable>::value;
    UT_EXPECTED_FALSE(isCopyAssignable);
}

/**
 * @brief Verifies that a derived class retains move-constructibility.
 */
UT_DEFINE_TEST(IsMoveConstructible)
{
    bool isMoveConstructible = std::is_move_constructible<DummyNonCopyable>::value;
    UT_EXPECTED_TRUE(isMoveConstructible);
}

/**
 * @brief Verifies that a derived class retains move-assignability.
 */
UT_DEFINE_TEST(IsMoveAssignable)
{
    bool isMoveAssignable = std::is_move_assignable<DummyNonCopyable>::value;
    UT_EXPECTED_TRUE(isMoveAssignable);
}

/**
 * @brief Tests runtime move construction and move assignment behavior.
 */
UT_DEFINE_TEST(MoveOperationsTransferState)
{
    // Test Move Construction
    DummyNonCopyable original(42);
    DummyNonCopyable moved(std::move(original));
    
    UT_EXPECTED_EQ(moved.value, 42);

    // Test Move Assignment
    DummyNonCopyable target(0);
    target = std::move(moved);
    
    UT_EXPECTED_EQ(target.value, 42);
}

// =======================================================================
// SESSION
// =======================================================================

UT_START_SESSION("LeoBase Patterns NonCopyable Unitary Tests")

UT_REGISTER_TEST(Patterns, NonCopyable, IsNotCopyConstructible);
UT_REGISTER_TEST(Patterns, NonCopyable, IsNotCopyAssignable);
UT_REGISTER_TEST(Patterns, NonCopyable, IsMoveConstructible);
UT_REGISTER_TEST(Patterns, NonCopyable, IsMoveAssignable);
UT_REGISTER_TEST(Patterns, NonCopyable, MoveOperationsTransferState);

UT_RUN_TESTS();

UT_FINISH_SESSION()
