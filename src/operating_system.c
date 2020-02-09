/*
 * (c) 2020 Michael J. Beer
 * this program is free software; you can redistribute it and/or modify
 * it under the terms of the gnu general public license as published by
 * the free software foundation; either version 3 of the license, or
 * (at your option) any later version.
 *
 * this program is distributed in the hope that it will be useful,
 * but without any warranty; without even the implied warranty of
 * merchantability or fitness for a particular purpose.  see the
 * gnu general public license for more details.
 *
 * you should have received a copy of the gnu general public license
 * along with this program; if not, write to the free software
 * foundation, inc., 51 franklin street, fifth floor, boston, ma  02110-1301,
 * usa.
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
