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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <limits.h>

#define VERSION "0.4.0"
#define COPYRIGHT "Copyright (C) 2010, 2012, 2014  "                          \
                  "Michael J. Beer <michael.josef.beer@googlemail.com>"

/* #define DEBUG */
/* #define DEBUG_READER */
/* #define DEBUG_EVAL */
/* #define DEVUG_ENVIRONMENT*/
/* #define DEBUG_NATIVE_FUNCTIONS */
/* #define DEBUG_CONS */
/* #define DEBUG_EXPRESSION */
/* #define DEBUG_SYMBOL_TABLE */
/* #define DEBUG_STACK */
/* #define DEBUG_LAMBDA */
/* #define DEBUG_GARBAGE_COLLECTOR */
/* #define DEBUG_TYPES */

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
/* #define EXIT_ON_ERROR */

/**
  * Should be benchmarked?
  */
#define BENCHMARK

/**
 * Should subsequent errors mask older errors? I.e. if one error occured, thus
 * "lispError != ERR_OK", should another error overwrite error reporting
 * variables? If set, this won't happen.
 */
#define NO_MASK_ERROR


#define READ_BUFFER_SIZE               (int)(16000)
#define PRINT_BUFFER_SIZE              100
#define SYMBOL_TABLE_SIZE              20
#define CALL_STACK_SIZE                200


/*****************************************************************************
 *                                   MEMORY
 *****************************************************************************/


/**
 * size of one mem block in expressions/cons structs
 */
#define MEMORY_BLOCK_SIZE              20000

/**
 * Use preallocated memory instead of malloc/free whenever an expression is
 * created
 */
#define MEMORY_USE_PREALLOCATION 

/**
 * Should the memory be expanded automatically?
 * Will only have effect if MEMORY_USE_PREALLOCATION is set.
 */
/* #define MEMORY_AUTOEXTEND */

/**
 * Should native functions be handled strictly? 
 * Strictly spoken, this is only necessary on architectures where
 * sizeof(void *) != sizeof(NativeFunc *) holds.
 */
#define STRICT_NATIVE_FUNCS

/** 
 * Which kind of garbage collection to use. 
 * This constant could be set to any of the values GC_*.
 */
#define GARBAGE_COLLECTOR GC_MARK_AND_SWEEP 

/**
 * Kind of format to use for expressions
 */
#define EXPRESSION_FORMAT TrueLies



/*******************************************************************************
 *                        TYPES OF GARBAGE COLLECTION
 *******************************************************************************/



/**
 * Reference counting garbage collector
 */
#define GC_REFERENCE_COUNTING 1

/**
 * Reference counting garbage collector
 */
#define GC_MARK_AND_SWEEP     2


/******************************************************************************
 *                   CHECK WHETHER SET OPTIONS ARE POSSIBLE
 ******************************************************************************/



#ifdef BENCHMARK 

#   include <time.h>

#   ifndef CLOCK_PROCESS_CPUTIME_ID

/*        Unfortunately, #warning is not standard c */
/* #      warning "Disabling benchmark - system does not provide for necessary functions" */
#      undef   BENCHMARK

#   endif /* CLOCK_PROCESS_CPUTIME_ID */

#endif /* BENCHMARK */




#endif /* __CONFIG_H__ */

