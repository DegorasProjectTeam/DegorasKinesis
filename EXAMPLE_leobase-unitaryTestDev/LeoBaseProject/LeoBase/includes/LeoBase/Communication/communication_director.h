/*
 *      Copyright(C) Milethos Technologies SLU. 2025
 *      All rights reserved. Reproduction in whole or in
 *      part is prohibited without the written consent of
 *      the copyright holder.
 */

#pragma once

// LEOBASE INCLUDES
#include "LeoBase/Aliases/Global"
#include "LeoBase/Patterns/non_copyable.h"
#include "LeoBase/Communication/i_communication_builder.h"

// LEOBASE MACROS
LEOBASE_MODULE_BEGIN(communication)

// ---------------------------------------------------------------------------------------------------------------------

class LEOBASE_EXPORT CommunicationDirector : public patterns::NonCopyable
{
public:

    void setCommunicationBuilder(ICommunicationBuilderPtr builder);

    ICommunicationBuilderPtr getCommunicationBuilder();

protected:

    bool buildAll();

private:

    ICommunicationBuilderPtr builder_;

};

// ---------------------------------------------------------------------------------------------------------------------

LEOBASE_MODULE_END
