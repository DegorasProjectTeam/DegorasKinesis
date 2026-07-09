/*
 *      Copyright(C) Milethos Technologies SLU. 2025
 *      All rights reserved. Reproduction in whole or in
 *      part is prohibited without the written consent of
 *      the copyright holder.
 */

#pragma once

// ---------------------------------------------------------------------------------------------------------------------

// Macro to begin a namespace
#define BEGIN_NAMESPACE(ns) namespace ns {

// Macro to end a namespace
#define END_NAMESPACE(ns) }

// Macro to create nested namespaces dynamically
#define BEGIN_NESTED_NAMESPACE(ns1, ns2) namespace ns1 { namespace ns2 {
#define END_NESTED_NAMESPACE END_NAMESPACE END_NAMESPACE

// Specific usage for LeoBase namespaces
#define LEOBASE_BEGIN_NAMESPACE BEGIN_NAMESPACE(leobase)
#define LEOBASE_END_NAMESPACE END_NAMESPACE(leobase)

// Generic macro to handle module-specific namespaces
#define MODULE_BEGIN_NAMESPACE(module) BEGIN_NAMESPACE(module)
#define MODULE_END_NAMESPACE(module) END_NAMESPACE(module)

// Macro for nested LeoBase module namespaces
#define LEOBASE_MODULE_BEGIN(module) \
LEOBASE_BEGIN_NAMESPACE MODULE_BEGIN_NAMESPACE(module)
#define LEOBASE_MODULE_END \
MODULE_END_NAMESPACE() LEOBASE_END_NAMESPACE

// ---------------------------------------------------------------------------------------------------------------------
