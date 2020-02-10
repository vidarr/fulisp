/*
 * (C) 2012 Michael J. Beer
 * All rights reserved.
 *
 * Redistribution  and use in source and binary forms, with or with‐
 * out modification, are permitted provided that the following  con‐
 * ditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above  copy‐
 * right  notice,  this  list  of  conditions and the following dis‐
 * claimer in the documentation and/or other materials provided with
 * the distribution.
 *
 * 3.  Neither the name of the copyright holder nor the names of its
 * contributors may be used to endorse or promote  products  derived
 * from this software without specific prior written permission.
 *
 * THIS  SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBU‐
 * TORS "AS IS" AND ANY EXPRESS OR  IMPLIED  WARRANTIES,  INCLUDING,
 * BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS FOR A PARTICULAR PURPOSE  ARE  DISCLAIMED.  IN  NO  EVENT
 * SHALL  THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DI‐
 * RECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR  CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS IN‐
 * TERRUPTION)  HOWEVER  CAUSED  AND  ON  ANY  THEORY  OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING  NEGLI‐
 * GENCE  OR  OTHERWISE)  ARISING  IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __ERROR_H__
#define __ERROR_H__
#include <stdlib.h>

/**
 * Check whether there has been no error
 */
#define NO_ERROR (lispError == ERR_OK)

/**
 * Signal an error
 */
#ifdef NO_MASK_ERROR

#define ERROR_BASIC(e, m)         \
    {                             \
        if (NO_ERROR) {           \
            lispFile = __FILE__;  \
            lispLine = __LINE__;  \
            lispError = e;        \
            lispErrorMessage = m; \
        }                         \
    }

#else

#define ERROR_BASIC(e, m)     \
    {                         \
        lispFile = __FILE__;  \
        lispLine = __LINE__;  \
        lispError = e;        \
        lispErrorMessage = m; \
    }

#endif

#ifdef EXIT_ON_ERROR
#define ERROR(e, m)                                                         \
    {                                                                       \
        ERROR_BASIC(e, m);                                                  \
        fprintf(stderr, "\n%s:%u - %u %s\n", lispFile, lispLine, lispError, \
                lispErrorMessage);                                          \
        abort();                                                            \
    }
#else
#define ERROR(e, m) ERROR_BASIC(e, m)
#endif

#define ERROR_RESET           \
    {                         \
        lispError = 0;        \
        lispErrorMessage = 0; \
    };

/**
 * Signal a warning
 */
#define WARNING(w, m)           \
    {                           \
        lispFile = __FILE__;    \
        lispLine = __LINE__;    \
        lispWarning = w;        \
        lispWarningMessage = m; \
    }

/**
 * Signal an unrecoverable error - abort the process
 */
#define PANIC(p, m)                                      \
    {                                                    \
        fprintf(stderr, "fuLisp Panic %3i: %s\n", p, m); \
        exit(1);                                         \
    }

/**
 * The error codes
 */
#define ERR_OK 0
#define ERR_NIL_VALUE 1
#define ERR_SYNTAX_ERROR 2
#define ERR_BUFFER_OVERFLOW 100
#define ERR_LIMIT_EXCESS 110
#define ERR_UNEXPECTED_TYPE 200
#define ERR_UNEXPECTED_VAL 300
#define ERR_EXPECTED_FUNCTION 310
#define ERR_UNEXPECTED_END_OF_STRING 500
#define ERR_UNBALANCED_PARENTHESES 600
#define ERR_UNEVEN_SYM_VAL 700
#define ERR_UNRESOLVABLE 1100
#define ERR_STACK_OVERFLOW 1210
#define ERR_STACK_UNDERFLOW 1211
#define ERR_UNIMPLEMENTED 2000
#define ERR_DIVISION_BY_ZERO 3000
#define ERR_INSUFFICTIENT_MEMORY 4000
#define ERR_CONVERSION_FAILED 5000

#define ERR_NO_RESOURCE 6001

#define WRN_BEGIN_WRN_CODE 10000
#define WRN_UNSAFE_CODE WRN_BEGIN_WRN_CODE + 10

extern int lispError;
extern char *lispErrorMessage;
extern int lispWarning;
extern char *lispWarningMessage;

extern char *lispFile;
extern int lispLine;

#endif
