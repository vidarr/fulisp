/*
 * (C) 2010, 2012 Michael J. Beer
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

#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <limits.h>

#include "../.version.h"

#define COPYRIGHT                      \
    "Copyright (C) 2010, 2012, 2014  " \
    "Michael J. Beer <michael.josef.beer@googlemail.com>"

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
 * Where to look for files to import by default
 */
#define DEFAULT_LISP_LIBRARY_PATH "."

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

#define READ_BUFFER_SIZE (int)(16000)
#define PRINT_BUFFER_SIZE 100
#define SYMBOL_TABLE_SIZE 20
#define CALL_STACK_SIZE 200

/*****************************************************************************
 *                                   MEMORY
 *****************************************************************************/

/**
 * size of one mem block in expressions/cons structs
 */
#define MEMORY_BLOCK_SIZE 20000

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
/* #define GARBAGE_COLLECTOR GC_REFERENCE_COUNTING */
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
#define GC_MARK_AND_SWEEP 2

/******************************************************************************
 *                   CHECK WHETHER SET OPTIONS ARE POSSIBLE
 ******************************************************************************/

#if !defined(GARBAGE_COLLECTOR)

#error("GARBAGE_COLLECTOR has not been defined!")

#elif GARBAGE_COLLECTOR == GC_MARK_AND_SWEEP

#ifndef MEMORY_USE_PREALLOCATION

#error("MARK_AND_SWEEP requires MEMORY_USE_PREALLOCATION");

#endif /* MEMORY_USE_PREALLOCATION */

#endif /* GARBAGE_COLLECTION */

/*----------------------------------------------------------------------------*/

#ifdef BENCHMARK

#include <time.h>

#ifndef CLOCK_PROCESS_CPUTIME_ID

/*        Unfortunately, #warning is not standard c */
/* #      warning "Disabling benchmark - system does not provide for necessary
 * functions" */
#undef BENCHMARK

#endif /* CLOCK_PROCESS_CPUTIME_ID */

#endif /* BENCHMARK */

#endif /* __CONFIG_H__ */
