/*
 * (C) 2010, 2012 Michael J. Beer
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,
 USA.
 */

#include "test.h"
#include <stdio.h>
#include <stdlib.h>

#define PASSED "Passed"

#define FAILED "Failed"

#define CRITICAL "Critical Fail! ABORTING!"

#ifdef TEST_OUTPUT_ANSI_COLOR

#define ESC 27

#define PRINT_OK(str) printf("%c[0;32;40m%s%c[0m\n", ESC, (str), ESC);

#define PRINT_FAIL(str) printf("%c[0;31;40m%s%c[0m\n", ESC, (str), ESC);

#define PRINT_CRITICAL(str) printf("%c[7;31;40m%s%c[0m\n", ESC, (str), ESC);

#else

#define PRINT_OK(str) printf("%s\n", (str));

#define PRINT_FAIL(str) printf("%s\n", (str));

#define PRINT_CRITICAL(str) printf("%s\n", (str));

#endif

int test(int cond, char *name) {
    printf("Checking %60s...", name);
    if (cond == 0) {
        PRINT_OK(PASSED);
        return 0;
    } else if (cond > 0) {
        PRINT_FAIL(FAILED);
        return 1;
    } else {
        PRINT_CRITICAL(CRITICAL);
        exit(1);
    }
}

void testWarn(char *msg) { PRINT_FAIL(msg); }

void testMessage(char *msg) { PRINT_OK(msg); }

#undef PASSED
#undef FAILED
#undef CRITICAL
#undef PRINT_OK
#undef PRINT_FAIL
#undef PRINT_CRITICAL
