/*
 *      Copyright(C) Milethos Technologies SLU. 2026
 *      All rights reserved. Reproduction in whole or in
 *      part is prohibited without the written consent of
 *      the copyright holder.
 */

#pragma once

// C++ INCLUDES
#include <memory>

// LEOBASE INCLUDES
#include "LeoBase/Aliases/Global"
#include "LeoBase/Patterns/non_copyable.h"

// LEOBASE MACROS
LEOBASE_MODULE_BEGIN(module)

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Interface for parameter passing in modules.
 *
 * This abstract class serves as a base for parameter objects
 * that will be used in various module functions.
 */

// Commented for testing porpouses
// class LEOBASE_EXPORT IParams : public patterns::NonCopyable
// {

// public:

//     // TODO NOPERMITIR INSTANCIAS QUE NO ESTEN ALMACENDAS EN SHARED OR UNIQUE.
//     // TODO LAS EXCEPCIONES LAS PODRIAMOS UNIFICAR Y QUE TODO LO QUE LANCE EXCEPCIONES
//     //      HEREDE DE UNA CLASE PADRE A LA QUE SE LE PASE UN SCOPE PARA GENERAR LOS
//     //      MENSAJES DE MANERA AUTOMATICA.

//     /**
//      * @brief Default constructor.
//      */
//     IParams() = default;

//     /**
//      * @brief Move constructor.
//      * @param other Another IParams instance to move from.
//      */
//     IParams(IParams&& other) noexcept = default;

//     /**
//      * @brief Move assignment operator.
//      * @param other Another IParams instance to move from.
//      * @return Reference to this object.
//      */
//     IParams& operator=(IParams&& other) noexcept = default;

//     /**
//      * @brief Virtual destructor to allow cleanup in derived classes.
//      */
//     virtual ~IParams() = default;

//     /**
//      * @brief Creates a shared pointer to a derived type of IParams.
//      * @tparam Derived The derived class type that inherits from IParams.
//      * @tparam Args Parameter pack for the constructor arguments.
//      * @param args Arguments to forward to the constructor of the derived type.
//      * @return A shared pointer to the created instance.
//      */
//     template <typename Derived, typename... Args>
//     static std::shared_ptr<Derived> makeShared(Args&&... args)
//     {
//         static_assert(std::is_base_of<IParams, Derived>::value,
//                       "[LeoBase,Module,IParams::makeShared] Derived must inherit from IParams.");
//         return std::make_shared<Derived>(std::forward<Args>(args)...);
//     }

//     /**
//      * @brief Creates a unique pointer to a derived type of IParams.
//      * @tparam Derived The derived class type that inherits from IParams.
//      * @tparam Args Parameter pack for the constructor arguments.
//      * @param args Arguments to forward to the constructor of the derived type.
//      * @return A unique pointer to the created instance.
//      */
//     template <typename Derived, typename... Args>
//     static std::unique_ptr<Derived> makeUnique(Args&&... args)
//     {
//         static_assert(std::is_base_of<IParams, Derived>::value,
//                       "[LeoBase,Module,IParams::makeUnique] Derived must inherit from IParams.");
//         return std::make_unique<Derived>(std::forward<Args>(args)...);
//     }

// };



//TO TEST: -> Applying Passkey idiom pattern ::

class LEOBASE_EXPORT IParams : public patterns::NonCopyable
{
public:
    class ConstructorToken {
    private:
        friend class IParams;
        ConstructorToken() = default;
    };

    /**
     * @brief Default constructor.
     */
    IParams() = default;

    /**
     * @brief Move constructor.
     * @param other Another IParams instance to move from.
     */
    IParams(IParams&& other) noexcept = default;

    /**
     * @brief Move assignment operator.
     * @param other Another IParams instance to move from.
     * @return Reference to this object.
     */
    IParams& operator=(IParams&& other) noexcept = default;

    /**
     * @brief Virtual destructor to allow cleanup in derived classes.
     */
    virtual ~IParams() = default;


    /**
     * @brief Creates a shared pointer to a derived type of IParams.
     * @tparam Derived The derived class type that inherits from IParams.
     * @tparam Args Parameter pack for the constructor arguments.
     * @param args Arguments to forward to the constructor of the derived type.
     * @return A shared pointer to the created instance.
     */
    template <typename Derived, typename... Args>
    static std::shared_ptr<Derived> makeShared(Args&&... args)
    {
        static_assert(std::is_base_of<IParams, Derived>::value,
                      "[LeoBase,Module,IParams::makeShared] Derived must inherit from IParams.");
        //Passing the token to the Derived's constructor
        return std::make_shared<Derived>(ConstructorToken{}, std::forward<Args>(args)...);
    }



    /**
     * @brief Creates a unique pointer to a derived type of IParams.
     * @tparam Derived The derived class type that inherits from IParams.
     * @tparam Args Parameter pack for the constructor arguments.
     * @param args Arguments to forward to the constructor of the derived type.
     * @return A unique pointer to the created instance.
     */
    template <typename Derived, typename... Args>
    static std::unique_ptr<Derived> makeUnique(Args&&... args)
    {
        static_assert(std::is_base_of<IParams, Derived>::value,
                      "[LeoBase,Module,IParams::makeUnique] Derived must inherit from IParams.");
        return std::make_unique<Derived>(ConstructorToken{},std::forward<Args>(args)...);
    }


};

/// Shared pointer type alias for IParams.
using IParamsPtr = std::shared_ptr<IParams>;

/// Unique pointer type alias for IParams.
using IParamsUniquePtr = std::unique_ptr<IParams>;

/// Weak pointer type alias for IParams.
using IParamsWeakPtr = std::weak_ptr<IParams>;

// ---------------------------------------------------------------------------------------------------------------------

LEOBASE_MODULE_END
