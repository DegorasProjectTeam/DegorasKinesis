/*
 *      Copyright(C) Milethos Technologies SLU. 2025
 *      All rights reserved. Reproduction in whole or in
 *      part is prohibited without the written consent of
 *      the copyright holder.
 */

// LEOBASE INCLUDES
#include "LeoBase/Module/module_registry.h"

// LEOBASE MACROS
LEOBASE_MODULE_BEGIN(module)

ModuleRegistry &ModuleRegistry::instance()
{
    static ModuleRegistry instance;
    return instance;
}

void ModuleRegistry::registerModuleInternal(ModuleID id, IModulePtr module)
{
    if (!module)
        return;

    std::lock_guard<std::mutex> lock(this->mtx_);
    module_map_[id] = module;
}

void ModuleRegistry::removeModuleInternal(ModuleID id)
{
    std::lock_guard<std::mutex> lock(mtx_);
    module_map_.erase(id);
}

const ModuleMap& ModuleRegistry::getModuleMap() const
{
    std::lock_guard<std::mutex> lock(this->mtx_);
    return this->module_map_;
}

IModulePtr ModuleRegistry::getModuleInternal(const ModuleID &id) const
{
    std::lock_guard<std::mutex> lock(this->mtx_);
    auto it = this->module_map_.find(id);
    return (it != this->module_map_.end()) ? it->second : nullptr;
}

// ---------------------------------------------------------------------------------------------------------------------

LEOBASE_MODULE_END
