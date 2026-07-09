/*
 *      Copyright(C) Milethos Technologies SLU. 2025
 *      All rights reserved. Reproduction in whole or in
 *      part is prohibited without the written consent of
 *      the copyright holder.
 */

#pragma once

// C++ INCLUDES
#include <memory>

// LEOBASE INCLUDES
#include "LeoBase/Aliases/Global"
#include "LeoBase/Module/module_registry.h"

// LEOBASE MACROS
LEOBASE_MODULE_BEGIN(module)

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Proxy class for automatic registration of a module with the registry.
 *
 * This class ensures that a module is automatically registered when an instance
 * is created and removed from the registry upon destruction.
 *
 * @tparam T The module type to be registered.
 */
template<class T>
class LEOBASE_EXPORT RegisterModuleProxy : public patterns::NonCopyable
{
public:

    /**
     * @brief Constructs the proxy and registers the module in the registry.
     *
     * @param id The unique identifier for the module.
     */
    explicit RegisterModuleProxy(const ModuleID &id) :
        id_(id),
        module_(std::make_shared<T>())
    {
        ModuleRegistry::instance().registerModule(id_, this->module_);
    }

    /**
     * @brief Constructs the proxy with arguments and registers the module.
     *
     * @tparam Args Parameter pack for the constructor arguments.
     * @param id The unique identifier for the module.
     * @param args Arguments to forward to the constructor of the module.
     */
    template <typename... Args>
    explicit RegisterModuleProxy(const ModuleID &id, Args&&... args)
        : id_(id),
        module_(std::make_shared<T>(std::forward<Args>(args)...))
    {
        ModuleRegistry::instance().registerModule(id_, this->module_);
    }

    /**
     * @brief Constructs the proxy with an existing shared pointer to a module.
     *
     * @param id The unique identifier for the module.
     * @param module A shared pointer to an existing module.
     */
    explicit RegisterModuleProxy(const ModuleID &id, std::shared_ptr<T> module)
        : id_(id),
        module_(std::move(module))
    {
        ModuleRegistry::instance().registerModule(id_, this->module_);
    }


    /**
     * @brief Constructs the proxy with an existing module reference.
     *
     * @param id The unique identifier for the module.
     * @param module A reference to an existing module.
     */
    explicit RegisterModuleProxy(const ModuleID &id, T& module)
        : id_(id),
        module_(std::make_shared<T>(module))
    {
        ModuleRegistry::instance().registerModule(id_, this->module_);
    }

    /**
     * @brief Destructor that unregisters the module from the registry.
     */
    ~RegisterModuleProxy()
    {
        ModuleRegistry::instance().removeModule(id_);
    }

    /**
     * @brief Retrieves the registered module instance.
     *
     * @return A raw pointer to the registered module.
     */
    T* get()
    {
        return this->module_.get();
    }

    /**
     * @brief Retrieves the registered module instance as a shared pointer.
     *
     * Provides shared ownership, allowing the caller to safely hold the module
     * without worrying about premature destruction.
     *
     * @return A shared pointer to the registered module.
     */
    std::shared_ptr<T> getShared()
    {
        return this->module_;
    }

protected:

    ModuleID id_;               ///< The module identifier.
    std::shared_ptr<T> module_; ///< The module instance.
};

// ---------------------------------------------------------------------------------------------------------------------

LEOBASE_MODULE_END
