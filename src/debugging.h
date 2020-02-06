/*
 * (C) 2010 Michael J. Beer
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,
 * USA.
 */

#ifndef __DEBUGGING_H__
#define __DEBUGGING_H__

#include "printnativefunctions.h"
#include "stdio.h"

#ifdef DEBUG

#define IF_DEBUG(x) x

#define DEBUG_PRINT(msg)                                 \
    {                                                    \
        fprintf(stderr, "%s: %i: ", __FILE__, __LINE__); \
        fprintf(stderr, msg);                            \
    }

#define DEBUG_PRINT_PARAM(msg, param)                    \
    {                                                    \
        fprintf(stderr, "%s: %i: ", __FILE__, __LINE__); \
        fprintf(stderr, msg, param);                     \
    }

#define DEBUG_PRINT_EXPR(env, x)                                    \
    {                                                               \
        char *buf = SAFE_STRING_NEW(3200);                          \
        fprintf(stderr, "%s: %i: Expr is %s\n", __FILE__, __LINE__, \
                expressionToString(env, buf, 3200, x));             \
        free(buf);                                                  \
    }

#define DEBUG_PRINT_NATIVE_FUNC(x)                                     \
    {                                                                  \
        size_t bufLen = 3 * sizeof(NativeFunction *);                  \
        char *str = SAFE_STRING_NEW(bufLen);                           \
        NATIVE_FUNC_TO_STR(x, str, bufLen);                            \
        if (SAFE_STRING_IS_TAINTED(str, bufLen)) {                     \
            ERROR(ERR_BUFFER_OVERFLOW, "Canary has been overwritten"); \
        } else {                                                       \
            DEBUG_PRINT_PARAM("FCT: %s\n", str);                       \
        }                                                              \
        free(str);                                                     \
    };

#else

#define IF_DEBUG(x)

#define DEBUG_PRINT(msg)

#define DEBUG_PRINT_PARAM(msg, param)

#define DEBUG_PRINT_EXPR(env, x)

#define DEBUG_PRINT_NATIVE_FUNC(x)

#endif

#endif
