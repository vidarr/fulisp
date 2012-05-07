/**
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
 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "test.h"
#include <stdlib.h>
#include <stdio.h>

#define ESC 27


int test(int cond, char *name) {
    printf("Checking %60s...", name);
    if(cond == 0) {
        printf("%c[0;32;40mPassed%c[0m\n", ESC, ESC);
        return 0;
    } else if (cond > 0) {
        printf("%c[0;31;40mFailed!%c[0m\n", ESC, ESC);
        return 1;
    } else {
        printf("%c[7;31;40mCritical Fail! ABORTING%c[0m\n", ESC, ESC);
        exit(1);
    }
}
