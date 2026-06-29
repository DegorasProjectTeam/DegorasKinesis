/*
 *  Copyright (C) 2018-2026 Degoras Project Team
 *
 *  This file is part of a small-scale research or utility tool built atop
 *  the Degoras Project infrastructure, released under the MIT License.
 *
 *  SPDX-License-Identifier: MIT
 *
 *  See the LICENSE file in the root directory for full license details.
 */

#pragma once

// EXPORT
// ---------------------------------------------------------------------------------------------------------------------
#if ((defined __WIN32__) || (defined _WIN32)) && (!defined LIBTHORLABSKINESIS_STATIC)
    #ifdef LIBTHORLABSKINESIS_LIBRARY
        #define LIBTHORLABSKINESIS_EXPORT	__declspec(dllexport)
    #else
        #define LIBTHORLABSKINESIS_EXPORT	__declspec(dllimport)
    #endif
#else
    # define LIBTHORLABSKINESIS_EXPORT
#endif
// ---------------------------------------------------------------------------------------------------------------------
