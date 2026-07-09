/*
 *      Copyright(C) Milethos Technologies SLU. 2025
 *      All rights reserved. Reproduction in whole or in
 *      part is prohibited without the written consent of
 *      the copyright holder.
 */

#pragma once

// C++ INCLUDES
#include <memory>
#include <cstdint>

// LEOBASE INCLUDES
#include "LeoBase/Aliases/Global"
#include "LeoBase/Patterns/Observer.h"
#include "LeoBase/Messaging/i_event.h"
#include "LeoBase/Module/i_implementation.h"
#include "LeoBase/Module/i_params.h"
#include "LeoBase/Module/i_values.h"

// LEOBASE MACROS
LEOBASE_MODULE_BEGIN(module)

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Enumeration representing the module's status.
 */
enum class ModuleStatus : std::uint8_t
{
    NOT_IMPLEMENTED,         ///< Functionality not implemented.
    MODULE_PARAMETER_ERROR,  ///< Incorrect parameters provided.
    MODULE_ALREADY_SETUP,    ///< Module is already set up.
    MODULE_NOT_SETUP,        ///< Module is not set up.
    MODULE_ERROR,            ///< A general error occurred.
    MODULE_OK,               ///< Operation successful.
    MODULE_NOT_READY         ///< Module is not ready.
};

/**
 * @brief Interface for module components within the system.
 *
 * Provides a standard interface for managing and interacting with system modules.
 */
class LEOBASE_EXPORT IModule: public patterns::Observer<messaging::IEventPtr>,
                              public patterns::NonCopyable
{

public:

    /**
     * @brief Virtual destructor to allow proper cleanup of derived classes.
     */
    virtual ~IModule() = default;
    /**
     * @brief Sets up the module with provided parameters.
     * @param params Configuration parameters.
     * @return Default: ModuleStatus::NOT_IMPLEMENTED.
     */
    virtual ModuleStatus setup(IParamsPtr params);

    /**
     * @brief Starts the module.
     * @param params Parameters required for starting the module.
     * @return Default: ModuleStatus::NOT_IMPLEMENTED.
     */
    virtual ModuleStatus start(IParamsPtr params);

    /**
     * @brief Stops the module.
     * @param params Parameters for stopping the module.
     * @return Default: ModuleStatus::NOT_IMPLEMENTED.
     */
    virtual ModuleStatus stop(IParamsPtr params);

    /**
     * @brief Sets a value within the module.
     * @param params Key or context parameters.
     * @param values Values to be set.
     * @return Default: ModuleStatus::NOT_IMPLEMENTED.
     */
    virtual ModuleStatus setValue(IParamsPtr params, IValuesPtr values);

    /**
     * @brief Retrieves values from the module.
     * @param params Key or context parameters.
     * @param values Output parameter for retrieved values.
     * @return Default: ModuleStatus::NOT_IMPLEMENTED.
     */
    virtual ModuleStatus getValue(IParamsPtr params, IValuesPtr& values);

    /**
     * @brief Pushes an asynchronous event into the module.
     * @param event The event to be processed asynchronously.
     * @return Default: ModuleStatus::NOT_IMPLEMENTED.
     */
    virtual ModuleStatus pushEvent(messaging::IEventPtr event);

    /**
     * @brief Observer method called when an event takes place in the subscribed channel.
     * @param event The received event.
     * @return Default: false.
     */
    virtual bool update(const messaging::IEventPtr& event) noexcept;

    /**
     * @brief Performs synchronous module operations.
     * @param params Operation parameters.
     * @return Default: ModuleStatus::NOT_IMPLEMENTED.
     */
    virtual ModuleStatus process(IParamsPtr params);

    /**
     * @brief Checks whether the module has been properly initialized.
     * @return Default: false.
     */
    virtual bool isReady() const;

    /**
     * @brief Retrieves the module implementation.
     * @param implementation Output parameter for the implementation object.
     * @return Default: ModuleStatus::NOT_IMPLEMENTED.
     */
    virtual ModuleStatus getImpl(IImplementationPtr implementation);
};

/// Shared pointer type alias for IModule.
using IModulePtr = std::shared_ptr<IModule>;

/// Unique pointer type alias for IModule.
using IModuleUniquePtr = std::unique_ptr<IModule>;

/// Weak pointer type alias for IModule.
using IModuleWeakPtr = std::weak_ptr<IModule>;

// ---------------------------------------------------------------------------------------------------------------------

LEOBASE_MODULE_END
