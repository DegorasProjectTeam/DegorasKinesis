/*
 *      Copyright(C) Milethos Technologies SLU. 2026
 *      All rights reserved. Reproduction in whole or in
 *      part is prohibited without the written consent of
 *      the copyright holder.
 */

#pragma once

// C++ INCLUDES
#include <functional>
#include <future>
#include <unordered_map>
#include <any>
#include <mutex>
#include <type_traits>
#include <stdexcept>
#include <utility>
#include <typeindex>

// LEOBASE INCLUDES
#include "LeoBase/Aliases/Global"
#include "LeoBase/Callbacks/callback_types.h"
#include "LeoBase/Patterns/non_copyable.h"
#include "LeoBase/Patterns/non_movable.h"

// LEOBASE MACROS
LEOBASE_MODULE_BEGIN(callbacks)

// ---------------------------------------------------------------------------------------------------------------------

class LEOBASE_EXPORT CallbackHandler: public patterns::NonCopyable,
                                      public patterns::NonMovable
{

public:

    template<typename Id, typename FuncSignature>
    void registerCallback(Id id, std::function<FuncSignature> callback)
    {
        // Lock for thread safety
        std::lock_guard<std::mutex> lock(this->mtx_);

        // Hash the ID for storage
        CallbackId callback_id = std::hash<Id>{}(id);

        // Store the callback in the map
        this->callbackMap_.emplace(
            callback_id,
            std::make_pair(
                std::any(callback),
                std::type_index(typeid(std::function<FuncSignature>))));
    }

    template<typename Id>
    void registerCallback(Id id, const CallbackPair& callbackPair)
    {
        // Lock for thread safety
        std::lock_guard<std::mutex> lock(this->mtx_);

        // Hash the ID for storage
        CallbackId callback_id = std::hash<Id>{}(id);

        // Insert the prepared callback pair into the map
        this->callbackMap_.emplace(callback_id, callbackPair);
    }

    template<typename Id>
    void registerCallbacks(const std::unordered_map<Id, CallbackPair>& map)
    {
        for (const auto& [id, clbk_pair] : map)
            this->registerCallback(id, clbk_pair);
    }

    template <typename FunctionT = std::function<void(void)>, typename Id, typename... Args,
             typename std::enable_if_t<leobase::is_std_function<FunctionT>::value, int> = 0>
    auto invokeSync(Id id, Args&&... args)
    {
        std::lock_guard<std::mutex> lock(this->mtx_);

        CallbackId id_hash = std::hash<Id>{}(id);
        auto it = this->callbackMap_.find(id_hash);

        if (it == this->callbackMap_.end())
            throw std::invalid_argument("CallbackHandler - Callback with the provided ID does not exist.");

        auto& any_obj = it->second.first;
        auto& stored_type = it->second.second;

        if (stored_type == typeid(FunctionT))
        {
            auto& callback = std::any_cast<FunctionT&>(any_obj);
            return callback(std::forward<Args>(args)...);
        }
        else
        {
            throw std::invalid_argument("CallbackHandler - Incorrect callback type.");
        }
    }

    template <typename FunctionT = std::function<void(void)>, typename Id, typename... Args,
             typename std::enable_if_t<leobase::is_std_function<FunctionT>::value, int> = 0>
    auto invokeAsync(Id id, Args&&... args)
    {
        std::lock_guard<std::mutex> lock(this->mtx_);

        CallbackId id_hash = std::hash<Id>{}(id);
        auto it = this->callbackMap_.find(id_hash);

        if (it == this->callbackMap_.end())
            throw std::invalid_argument("CallbackHandler - Callback with the provided ID does not exist.");

        auto& any_obj = it->second.first;
        auto& stored_type = it->second.second;

        if (stored_type == typeid(FunctionT))
        {
            auto& callback = std::any_cast<FunctionT&>(any_obj);
            return std::async(std::launch::async, callback, std::forward<Args>(args)...);
        }
        else
        {
            throw std::invalid_argument("CallbackHandler - Incorrect callback type.");
        }
    }

    template <typename Id>
    void removeCallback(Id id)
    {
        std::lock_guard<std::mutex> lock(this->mtx_);
        auto it = this->callbackMap_.find(std::hash<Id>{}(id));
        if (it != this->callbackMap_.end())
            this->callbackMap_.erase(it);
    }

    template <typename Id>
    bool hasCallback(Id id) const
    {
        std::lock_guard<std::mutex> lock(this->mtx_);
        return this->callbackMap_.find(std::hash<Id>{}(id)) != this->callbackMap_.end();
    }

    /**
     * @brief Remove all registered callbacks.
     */
    void clearCallbacks();

    /**
     * @brief Default constructor.
     */
    CallbackHandler() = default;

    /**
     * @brief Default destructor.
     */
    virtual ~CallbackHandler() = default;

private:

    // Private members.
    CallbackMap callbackMap_;  ///< Map storing callbacks against their ids.
    mutable std::mutex mtx_;   ///< Safety mutex.

};

// ---------------------------------------------------------------------------------------------------------------------

LEOBASE_MODULE_END
