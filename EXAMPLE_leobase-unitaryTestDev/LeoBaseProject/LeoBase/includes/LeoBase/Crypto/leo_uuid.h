/*
 *      Copyright(C) Milethos Technologies SLU. 2025
 *      All rights reserved. Reproduction in whole or in
 *      part is prohibited without the written consent of
 *      the copyright holder.
 */

#pragma once

// C++ INCLUDES
#include <cstddef>
#include <array>
#include <string>

// LEOBASE INCLUDES
#include "LeoBase/Aliases/Global"

// LEOBASE MACROS
LEOBASE_MODULE_BEGIN(crypto)

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Class to encapsulate the LeoUUID bytes and provide string representation
 */
class LEOBASE_EXPORT LeoUUID
{

public:

    /// LeoUUID bytes size.
    static inline constexpr unsigned kUUIDSize = 16;

    /**
     * @brief Construct a new empty (invalid) LeoUUID object.
     */
    LeoUUID();

    /**
     * @brief Construct a new LeoUUID object from an array of 16 bytes.
     * @param bytes An array of 16 bytes representing the LeoUUID.
     */
    LeoUUID(const std::array<std::byte, kUUIDSize>& bytes);

    LeoUUID(const LeoUUID&) = default;
    LeoUUID(LeoUUID&&) = default;
    LeoUUID& operator=(const LeoUUID&) = default;
    LeoUUID& operator=(LeoUUID&&) = default;

    /**
     * @brief Returns string representation of the LeoUUID
     *
     * This function converts the LeoUUID into a string following the standard representation of a LeoUUID, which consists of
     * 32 hexadecimal digits displayed in five groups separated by hyphens, in the form 8-4-4-4-12 for a total of 36
     * characters (including the hyphens).
     *
     * The string representation is divided as follows:
     * 1. time-low: The first 8 hex digits (4 bytes).
     * 2. time-mid: The next 4 hex digits (2 bytes).
     * 3. time-high-and-version: The next 4 hex digits (2 bytes).
     * 4. clock-seq-and-reserved and clock-seq-low: The next 4 hex digits (2 bytes).
     * 5. node: The last 12 hex digits (6 bytes).
     *
     * An example of a LeoUUID string: 550e8400-e29b-41d4-a716-446655440000
     *
     * This method's implementation is in alignment with RFC 4122: A Universally Unique IDentifier (LeoUUID) URN Namespace,
     * available at: https://www.ietf.org/rfc/rfc4122.txt
     *
     * @return String representation of the LeoUUID
     */
    std::string toRFC4122String() const;

    /**
     * @brief Retrieves a constant reference to the LeoUUID's byte array.
     * @return A constant reference to the LeoUUID's byte array.
     */
    const std::array<std::byte, kUUIDSize>& getBytes() const;

    /**
     * @brief Clears the LeoUUID, resetting all bytes to zero.
     *
     * This function sets all 16 bytes of the LeoUUID to zero, effectively nullifying the LeoUUID. It uses std::fill_n to
     * efficiently perform the operation on the byte array. After calling this method, the LeoUUID will be in a state
     * equivalent to a newly constructed, uninitialized LeoUUID object.
     */
    void clear();

private:

    /// Bytes of the LeoUUID.
    std::array<std::byte, kUUIDSize> bytes_;
};

/**
 * @brief UUID less operator
 * @param a
 * @param b
 * @return true if a is less than b, false otherwise
 */
LEOBASE_EXPORT bool operator<(const LeoUUID& a, const LeoUUID& b);

/**
 * @brief UUID greater operator
 * @param a
 * @param b
 * @return true if a is greater than b, false otherwise
 */
LEOBASE_EXPORT bool operator>(const LeoUUID& a, const LeoUUID& b);

/**
 * @brief UUID less or equal operator
 * @param a
 * @param b
 * @return true if a is less or equal than b, false otherwise
 */
LEOBASE_EXPORT bool operator<=(const LeoUUID& a, const LeoUUID& b);

/**
 * @brief UUID greater or equal operator
 * @param a
 * @param b
 * @return true if a is greater or equal than b, false otherwise
 */
LEOBASE_EXPORT bool operator>=(const LeoUUID& a, const LeoUUID& b);

/**
 * @brief UUID equal operator
 * @param a
 * @param b
 * @return true if a is equal than b, false otherwise
 */
LEOBASE_EXPORT bool operator==(const LeoUUID& a, const LeoUUID& b);

/**
 * @brief UUID not equal operator
 * @param a
 * @param b
 * @return true if a is not equal than b, false otherwise
 */
LEOBASE_EXPORT bool operator!=(const LeoUUID& a, const LeoUUID& b);

// ---------------------------------------------------------------------------------------------------------------------

LEOBASE_MODULE_END
