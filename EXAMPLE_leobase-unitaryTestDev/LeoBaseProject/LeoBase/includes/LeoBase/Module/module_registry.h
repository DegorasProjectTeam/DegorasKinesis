/*
 *      Copyright(C) Milethos Technologies SLU. 2025
 *      All rights reserved. Reproduction in whole or in
 *      part is prohibited without the written consent of
 *      the copyright holder.
 */

#pragma once

// C++ INCLUDES
#include <map>
#include <mutex>

// LEOBASE INCLUDES
#include "LeoBase/Aliases/Global"
#include "LeoBase/Patterns/non_copyable.h"
#include "LeoBase/Module/i_module.h"

// LEOBASE MACROS
LEOBASE_MODULE_BEGIN(module)

// ---------------------------------------------------------------------------------------------------------------------

/// Alias for module IDs.
using ModuleID = std::uint8_t;

/// Alias for module map.
using ModuleMap = std::map<ModuleID, IModulePtr>;

/**
 * @brief The ModuleRegistry class
 *
 * Maintains a registry of available modules for the application.
 * Provides a thread-safe interface to register, retrieve, and remove modules.
 */
class LEOBASE_EXPORT ModuleRegistry: public patterns::NonCopyable
{
public:
    /**
     * @brief Provides access to the singleton instance of ModuleRegistry.
     *
     * This method ensures a thread-safe, lazy initialization of the instance.
     * @return Reference to the singleton instance.
     */
    static ModuleRegistry& instance();

    /**
     * @brief Registers a module with the given identifier.
     *
     * This method allows registering a module using a provided identifier.
     * The module will be stored in the registry and can be retrieved later.
     *
     * @tparam T The type of the module ID, which is converted to ModuleID.
     * @param id The unique identifier of the module.
     * @param module The module instance to register.
     */
    template<typename T>
    void registerModule(const T& id, IModulePtr module)
    {
        this->registerModuleInternal(static_cast<ModuleID>(id), module);
    }

    /**
     * @brief Removes a module from the registry.
     *
     * This method removes a registered module based on its identifier.
     *
     * @tparam T The type of the module ID, which is converted to ModuleID.
     * @param id The unique identifier of the module.
     */
    template<typename T>
    void removeModule(const T& id)
    {
        this->removeModuleInternal(static_cast<ModuleID>(id));
    }

    /**
     * @brief Retrieves a module by its ID.
     *
     * This method fetches a registered module based on its identifier.
     *
     * @tparam T The type of the module ID, which is converted to ModuleID.
     * @param id The unique identifier of the module.
     * @return A shared pointer to the module if found, or nullptr if not found.
     */
    template<typename T>
    IModulePtr getModule(T id)
    {
        return this->getModuleInternal(static_cast<ModuleID>(id));
    }

    /**
     * @brief Retrieves the module map.
     *
     * Provides access to the underlying module map for read-only operations.
     *
     * @return A constant reference to the module map.
     */
    const ModuleMap& getModuleMap() const;

    /**
     * @brief Default destructor for ModuleRegistry.
     */
    ~ModuleRegistry() = default;

protected:

    /**
     * @brief Default constructor for ModuleRegistry.
     */
    ModuleRegistry() = default;

    /**
     * @brief Registers a module with the given identifier.
     *
     * If a module with the same identifier already exists, it will be replaced
     * by the new module.
     *
     * @param id The unique identifier of the module.
     * @param module The module instance to register.
     */
    void registerModuleInternal(ModuleID id, IModulePtr module);

    /**
     * @brief Removes a module from the registry.
     *
     * If the module ID exists in the registry, it will be removed.
     *
     * @param id The unique identifier for the module.
     */
    void removeModuleInternal(ModuleID id);

    /**
     * @brief Retrieves a module by its ID.
     *
     * Looks up a module in the registry by its unique identifier.
     *
     * @param id The unique identifier of the module.
     * @return A shared pointer to the module if found, or nullptr if not found.
     */
    IModulePtr getModuleInternal(const ModuleID& id) const;

    // Internal variables and containers.
    ModuleMap module_map_;   ///< Map storing registered modules.
    mutable std::mutex mtx_; ///< Mutex to ensure thread safety.
};

// ---------------------------------------------------------------------------------------------------------------------

LEOBASE_MODULE_END
