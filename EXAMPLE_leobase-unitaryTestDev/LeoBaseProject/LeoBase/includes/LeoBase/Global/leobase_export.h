/*
 *      Copyright(C) Milethos Technologies SLU. 2025
 *      All rights reserved. Reproduction in whole or in
 *      part is prohibited without the written consent of
 *      the copyright holder.
 */

#pragma once

// EXPORT
// ---------------------------------------------------------------------------------------------------------------------
#if ((defined __WIN32__) || (defined _WIN32)) && (!defined LEOBASE_STATIC)
    #ifdef LEOBASE_LIBRARY
        #define LEOBASE_EXPORT	__declspec(dllexport)
    #else
        #define LEOBASE_EXPORT	__declspec(dllimport)
    #endif
#else
    # define LEOBASE_EXPORT
#endif
// ---------------------------------------------------------------------------------------------------------------------
