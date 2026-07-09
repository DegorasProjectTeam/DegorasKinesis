/*
 *      Copyright(C) Milethos Technologies SLU. 2025
 *      All rights reserved. Reproduction in whole or in
 *      part is prohibited without the written consent of
 *      the copyright holder.
 */

#pragma once

// C++ INCLUDES
#include <algorithm>
#include <memory>
#include <mutex>
#include <functional>

// LEOBASE INCLUDES
#include "LeoBase/Aliases/Global"
#include "LeoBase/Patterns/non_copyable.h"
#include "LeoBase/Patterns/non_movable.h"

// LEOBASE MACROS
LEOBASE_MODULE_BEGIN(patterns)

// ---------------------------------------------------------------------------------------------------------------------

template <typename ContextType = void>
class ContextHandlerImp : public patterns::NonCopyable
{
protected:

    using ContextPtr = std::unique_ptr<ContextType>;

    virtual void initContext() = 0;
    virtual void stopContext() = 0;

    inline static ContextPtr context_;

    template <typename T>
    friend class ContextHandler;
};

template <typename ContextType = void>
using ContextHandlerImpUniquePtr = std::unique_ptr<ContextHandlerImp<ContextType>>;

template <typename ContextType = void>
class ContextHandler : public patterns::NonCopyable,
                       public patterns::NonMovable
{
public:

    inline static bool isInitialized()
    {
        std::lock_guard<std::mutex> lock(ContextHandler::mtx_);
        return !ContextHandler::instances_.empty();
    }

    inline static unsigned nInstances()
    {
        std::lock_guard<std::mutex> lock(ContextHandler::mtx_);
        return ContextHandler::instances_.size();
    }

    template <typename T = ContextType, typename = std::enable_if_t<!std::is_void_v<T>>>
    const typename ContextHandlerImp<T>::ContextPtr& getContext()
    {
        return handlerImp_->context_;
    }

protected:

    using ContextHandlerRef = std::reference_wrapper<ContextHandler>;

    inline ContextHandler(ContextHandlerImpUniquePtr<ContextType> ctx_imp) :
        handlerImp_(std::move(ctx_imp))
    {
        std::lock_guard<std::mutex> lock(ContextHandler::mtx_);

        if (ContextHandler::instances_.empty())
            this->handlerImp_->initContext();

        ContextHandler::instances_.push_back(std::ref(*this));
    }

    inline virtual ~ContextHandler()
    {
        std::lock_guard<std::mutex> lock(ContextHandler::mtx_);

        instances_.erase(
            std::remove_if(
                ContextHandler::instances_.begin(),
                ContextHandler::instances_.end(),
                [this](const ContextHandlerRef& ref){ return &ref.get() == this; }),
                ContextHandler::instances_.end());

        if (ContextHandler::instances_.empty())
            this->handlerImp_->stopContext();
    }

private:

    ContextHandlerImpUniquePtr<ContextType> handlerImp_;
    inline static std::mutex mtx_;
    inline static std::vector<ContextHandlerRef> instances_;
};

// ---------------------------------------------------------------------------------------------------------------------

LEOBASE_MODULE_END
