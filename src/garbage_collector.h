/*
 * (C) 2014 Michael J. Beer
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

#ifndef __GARBAGE_COLLECTOR_H__
#define __GARBAGE_COLLECTOR_H__

#include "config.h"

struct Expression;
struct Environment;

/**
 * struct in the environment reserved for the GC to store whatever data it 
 * requires. Actual appearance depends on GC used.
 */
#define GC_DEFINE_ENV_INFO __GC_DEFINE_ENV_INFO

/**
 * struct in each expression reserved for the GC to store whatever data it 
 * requires. Actual appearance depends on GC used.
 */
#define GC_DEFINE_EXPR_INFO __GC_DEFINE_EXPR_INFO

#define GC_INIT_EXPR_INFO(initcounter)   __GC_INIT_EXPR_INFO(initcounter)

/**
 * Invoke the garbage collector
 */
#define GC_RUN(env) __GC_RUN(env)

/**
 * Initialize expression after creation
 */
#define GC_INIT_EXPRESSION(env, expr) __GC_INIT_EXPRESSION(env, expr)

/**
 * Initialize expression after creation
 */
#define GC_INIT_ENVIRONMENT(env) __GC_INIT_ENVIRONMENT(env)

/**
 * Return current ref counter of expr 
 */
#define GC_GET_REF_COUNT(env, expr) __GC_GET_REF_COUNT(env, expr)

/**
 * Decrease ref counter of expr
 */
#define GC_DEC_REF_COUNT(env, expr) __GC_DEC_REF_COUNT(env, expr)

/**
 * Increase ref counter of expr
 */
#define GC_INC_REF_COUNT(env, expr) __GC_INC_REF_COUNT(env, expr)

/******************************************************************************
 * Implementation details
 ******************************************************************************/


#if ! defined (GARBAGE_COLLECTOR)

#    error("GARBAGE_COLLECTOR has not been defined!")

# elif      GARBAGE_COLLECTOR == GC_REFERENCE_COUNTING

struct ExprGcInfo {
    unsigned int counter;
};

#    define __GC_DEFINE_ENV_INFO
#    define __GC_DEFINE_EXPR_INFO            struct ExprGcInfo gcInfo;
#    define __GC_INIT_EXPR_INFO(initcounter) ,{initcounter}
#    define __GC_RUN(env)                    T
#    define __GC_INIT_EXPRESSION(env, expr)  ((expr)->gcInfo.counter = 0)
#    define __GC_GET_REF_COUNT(env, expr)    ((expr)->gcInfo.counter)
#    define __GC_DEC_REF_COUNT(env, expr)    ((expr)->gcInfo.counter--)
#    define __GC_INC_REF_COUNT(env, expr)    ((expr)->gcInfo.counter++)

#elif       GARBAGE_COLLECTOR == GC_MARK_AND_SWEEP

#    if EXPRESSION_FORMAT == EXPRESSION_FORMAT_EXPANDED

#        error("MARK_AND_SWEEP does not support EXPRESSION_FORMAT_EXPANDED")

#    endif

struct EnvGcInfo {
    size_t noReclaimedExpr; /* Number of expressions reclaimed dur. last run */
    size_t noMarkedExpr;    /* Number of expressions in use during last run */
};

#    define __GC_DEFINE_ENV_INFO             struct EnvGcInfo gcInfo;
#    define __GC_DEFINE_EXPR_INFO 
#    define __GC_INIT_EXPR_INFO(initcounter)
#    define __GC_RUN(env)                    gcMarkAndSweep(env)
#    define __GC_INIT_EXPRESSION(env, expr)  gcMarkExpression(env, expr)
#    define __GC_INIT_ENVIRONMENT(env)       gcInitEnvironment(env)
#    define __GC_GET_REF_COUNT(env, expr)    1
#    define __GC_DEC_REF_COUNT(env, expr)    while(0) {}
#    define __GC_INC_REF_COUNT(env, expr)    while(0) {}


struct Expression *gcMarkAndSweep   (struct Expression *env);

struct Expression *gcMarkExpression (struct Expression *env, 
        struct Expression *expr);

struct Expression *gcInitEnvironment(struct Environment *environ);

#else

#   error("GARBAGE_COLLECTOR not set!")

#endif


#include "environment.h"


#endif
