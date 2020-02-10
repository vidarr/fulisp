/*
 * (C) 2010 Michael J. Beer
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
#include "safety.h"

#define __OLD_MALLOC malloc

#undef malloc
#undef strlen
#undef strcpy
#undef strcmp
#undef sprintf

#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "error.h"

int safetyLevel = SAFETY_HIGH;

/*============================================================================
 * Safe string functions
 *===========================================================================*/

#ifdef GENERATE_SAFETY_CODE

void *safeMalloc(size_t noBytes) {
    void *newMem = calloc(1, noBytes);
    if (newMem == NULL) {
        ERROR(ERR_INSUFFICTIENT_MEMORY, "Could not allocate further memory");
    }
    return newMem;
}

#define malloc __OLD_MALLOC /* Prevent further use of malloc(3) */
#undef __OLD_MALLOC

char *stringNew(size_t length) {
    char *cString = NULL;

    assert(length >= 0);

    cString = SAFE_MALLOC(sizeof(char) * (length + 1));
    if (cString == NULL) {
        return NULL;
    }
    cString[length] = '\0'; /* Place CANARY */
    return cString;
}

int stringIsTainted(char *str, size_t length) {
    int result = 0;

    assert(str);

    if (str[length] != '\0') { /* Check CANARY */
        WARNING(ERR_UNEXPECTED_END_OF_STRING, "String has been tainted");
        str[length - 1] = '\0';
        result = 1;
    }
    return result;
}

#endif

#ifdef GENERATE_SAFETY_CODE

int safeSprintf(char *str, size_t bufLen, const char *format, ...) {
    int retVal;
    va_list args;

    /*
     * This function comes in a couple of flavours, all of them are not
     * 100% safe as the string printed is possibly trunated.
     * However, as long as  GENERATE_SAFE_CODE is defined at least
     * buffer overflows will be detected and reported.
     */

#if __STDC_VERSION__ >= 199901L

    /* This version prevents memory corruption but possibly truncates */
    va_start(args, format);
    retVal = vsnprintf(str, bufLen, format, args);
    va_end(args);
    if (retVal >= bufLen) {
        ERROR(ERR_BUFFER_OVERFLOW, "While printing to string");
        retVal = -1;
    }

#else /* __STDC_VERSION__ */

    /* This version wont prevent possible memory corruptions but detect them
     * using a CANARY
     * and report them back */

    if (str[bufLen] != '\0') {
        WARNING(ERR_UNEXPECTED_END_OF_STRING, "String has been tainted");
        str[bufLen] = '\0';
    }
    va_start(args, format);
    retVal = vsprintf(str, format, args);
    va_end(args);
    if (SAFE_STRING_IS_TAINTED(str, bufLen)) {
        ERROR(ERR_BUFFER_OVERFLOW,
              "Canary has been overwritten "
              "while invoking sprintf(3)");
        retVal = -1;
    } else {
        WARNING(WRN_UNSAFE_CODE,
                "Using unsafe sprintf(3) ( snprintf(3) unavailable ?)");
    }

#endif /* __STDC_VERSION__ */

    return retVal;
}

#endif /* GENERATE_SAFETY_CODE */

size_t safeStrlen(const char *str, size_t maxLen) {
    size_t len = 0;
    while (str[len] != '\0') {
        if (!(len < maxLen)) {
            fprintf(stderr, "safeStrlen: String exceeds max length\n");
            abort();
        }
        len++;
    }
    return len;
}

char *safeStrcpy(char *dest, const char *src, size_t maxLen) {
    size_t len = 0;
    while (src[len] != '\0') {
        if (!(len < maxLen - 1)) {
            fprintf(stderr, "safeStrcpy: String exceeds max length\n");
            abort();
        }
        len++;
        dest[len] = src[len];
    }
    dest[len + 1] = '\0';
    return dest;
}
