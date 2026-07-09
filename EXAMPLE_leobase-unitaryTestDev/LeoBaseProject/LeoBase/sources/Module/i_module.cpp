/*
 *      Copyright(C) Milethos Technologies SLU. 2025
 *      All rights reserved. Reproduction in whole or in
 *      part is prohibited without the written consent of
 *      the copyright holder.
 */

// LEOBASE INCLUDES
#include "LeoBase/Module/i_module.h"

// LEOBASE MACROS
LEOBASE_MODULE_BEGIN(module)

ModuleStatus IModule::setup(IParamsPtr)
{
    return ModuleStatus::NOT_IMPLEMENTED;
}

ModuleStatus IModule::start(IParamsPtr)
{
    return ModuleStatus::NOT_IMPLEMENTED;
}

ModuleStatus IModule::stop(IParamsPtr)
{
    return ModuleStatus::NOT_IMPLEMENTED;
}

ModuleStatus IModule::setValue(IParamsPtr, IValuesPtr)
{
    return ModuleStatus::NOT_IMPLEMENTED;
}

ModuleStatus IModule::getValue(IParamsPtr, IValuesPtr&)
{
    return ModuleStatus::NOT_IMPLEMENTED;
}

ModuleStatus IModule::pushEvent(messaging::IEventPtr)
{
    return ModuleStatus::NOT_IMPLEMENTED;
}

bool IModule::update(const messaging::IEventPtr &) noexcept
{
    return false;
}

ModuleStatus IModule::process(IParamsPtr)
{
    return ModuleStatus::NOT_IMPLEMENTED;
}

bool IModule::isReady() const
{
    return false;
}

ModuleStatus IModule::getImpl(IImplementationPtr)
{
    return ModuleStatus::NOT_IMPLEMENTED;
}

// ---------------------------------------------------------------------------------------------------------------------

LEOBASE_MODULE_END
