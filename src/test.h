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
 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */
  

#ifndef __TEST_H__
#define __TEST_H__

#include "config.h"

#define DECLARE_TEST(name) \
    printf("\nTesting name \n\n");

/**
 * Indicates successful test result.
 */
#define TEST_PASSED 0
/** 
 * Indicates failed test result.
 */
#define TEST_FAILED 1

/**
 * Tests condition, outputs a message according to result of test and returns
 * result.
 * @param cond Condition, i.e. an integer value where == 1 ( = failed) or == 0 (succeeded)
 * @param name Name of the test, used in output message
 */
int test(int cond, char *name);

/**
 * Print out warning message
 */
void testWarn(char *msg);

/**
 * Print out neutral message
 */
void testMessage(char *msg);

#endif
