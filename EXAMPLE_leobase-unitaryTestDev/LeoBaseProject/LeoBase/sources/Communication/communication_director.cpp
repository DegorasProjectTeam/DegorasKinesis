/*
 *      Copyright(C) Milethos Technologies SLU. 2025
 *      All rights reserved. Reproduction in whole or in
 *      part is prohibited without the written consent of
 *      the copyright holder.
 */

// LEOBASE INCLUDES
#include "LeoBase/Communication/communication_director.h"
#include "LeoBase/Global/global_definitions.h"
#include "LeoBase/Exceptions/leobase_exception.h"

// LEOBASE MACROS
LEOBASE_MODULE_BEGIN(communication)

// ---------------------------------------------------------------------------------------------------------------------

void CommunicationDirector::setCommunicationBuilder(ICommunicationBuilderPtr builder)
{
    this->builder_ = std::move(builder);
}



ICommunicationBuilderPtr CommunicationDirector::getCommunicationBuilder()
{
    return builder_;
}



bool CommunicationDirector::buildAll()
{
    if (!this->builder_)
    {
        throw exceptions::LeoBaseException(
            kModN, "CommunicationDirector::buildAll","Builder is not set.");
    }

    return this->builder_->produceAll();
}

// ---------------------------------------------------------------------------------------------------------------------

LEOBASE_MODULE_END
