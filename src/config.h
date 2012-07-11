/**
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <limits.h>

#define VERSION "0.1.0"
#define COPYRIGHT "Copyright (C) 2010, 2012  Michael J. Beer <michael.josef.beer@googlemail.com>"

/* #define DEBUG */
/* #define DEBUG_READER */
/* #define DEBUG_EVAL*/
/* #define DEVUG_ENVIRONMENT*/
#define DEBUG_NATIVE_FUNCTIONS
#define DEBUG_CONS
/* #define DEBUG_EXPRESSION*/
/* #define DEBUG_SYMBOL_TABLE*/
/* #define DEBUG_STACK*/
/* #define DEBUG_LAMBDA*/

/**
 * Should hash tables place strings or copies of strings as keys?
 */
#define HASH_TABLE_SAFE_KEYS

/**
 * Should additional error-checking code be generated?
 */
#define GENERATE_SAFETY_CODE

/**
 * Should test use ANSI Escapes to color output?
 */
#define TEST_OUTPUT_ANSI_COLOR

/**
 * Should lisp abort on lisp error?
 */
/* #define EXIT_ON_ERROR  */


/**
 * Should subsequent errors mask older errors? I.e. if one error occured, thus
 * "lispError != ERR_OK", should another error overwrite error reporting
 * variables? If set, this won't happen.
 */
#define NO_MASK_ERROR


#define READ_BUFFER_SIZE (int)(16000)
#define PRINT_BUFFER_SIZE 100
#define SYMBOL_TABLE_SIZE 20
#define CALL_STACK_SIZE 200

/**
 * Should native functions be handled strictly? 
 * Strictly spoken, this is only necessary on architectures where
 * sizeof(void *) != sizeof(NativeFunc *) holds.
 */
#define STRICT_NATIVE_FUNCS

#define GC_REFERENCE_COUNTING 1

/** 
 * Which kind of garbage collection to use. 
 * This constant could be set to any of the values GC_*.
 */
#define GARBAGE_COLLECTOR GC_REFERNCE_COUNTING

#endif

