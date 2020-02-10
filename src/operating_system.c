/*
 * (C) 2020 Michael J. Beer
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

#include "operating_system.h"

#define MODULE_NAME "operating_system.c"

#ifdef DEBUG_OPERATING_SYSTEM
#include "debugging.h"
#else
#include "no_debugging.h"
#endif

#include <string.h>

/*----------------------------------------------------------------------------*/

#if _POSIX_VERSION < 200809L

static size_t strnlen(const char *str, size_t maxlen) {

    size_t i;

    if(0 == str) return 0;

    for(i = 0; i < maxlen; ++i) {
        if(0 == str[i]) return i;
    }

    return maxlen;

}

#endif

/*----------------------------------------------------------------------------*/

FILE * file_open(char const *path, char const *fname, char const *mode) {

    char fpath[PATH_MAX];
    size_t fpathLen = 0;

    memset(fpath, 0, sizeof(fpath));

    if (0 == fname) {
        DEBUG_PRINT("Called without file name");
        return 0;
    }

    fpathLen = strnlen(fname, PATH_MAX) + 1;

    if(0 != path) {
        fpathLen += strnlen(path, PATH_MAX);
        fpathLen += 1; /* Separator between path and fname */
    }

    if(sizeof(fpath) < fpathLen) {
        DEBUG_PRINT("Path is too long");
        return 0;
    }

    if (0 != path) {
        strcat(fpath, path);
        strcat(fpath, "/");
    }

    strcat(fpath, fname);

    return fopen(fpath, mode);

}
