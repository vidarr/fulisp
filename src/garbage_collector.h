/*
 * (C) 2014 Michael J. Beer
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

#define GC_INIT_EXPR_INFO(initcounter) __GC_INIT_EXPR_INFO(initcounter)

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

#if !defined(GARBAGE_COLLECTOR)

#error("GARBAGE_COLLECTOR has not been defined!")

#elif GARBAGE_COLLECTOR == GC_REFERENCE_COUNTING

struct ExprGcInfo {
    unsigned int counter;
};

#define __GC_DEFINE_ENV_INFO
#define __GC_DEFINE_EXPR_INFO struct ExprGcInfo gcInfo;
#define __GC_INIT_EXPR_INFO(initcounter) \
    , { initcounter }
#define __GC_RUN(env) T
#define __GC_INIT_EXPRESSION(env, expr) \
    do {                                \
        (expr)->gcInfo.counter = 0;     \
    } while (0)
#define __GC_INIT_ENVIRONMENT(env) \
    while (0) {                    \
    }
#define __GC_GET_REF_COUNT(env, expr) ((expr)->gcInfo.counter)
#define __GC_DEC_REF_COUNT(env, expr) ((expr)->gcInfo.counter--)
#define __GC_INC_REF_COUNT(env, expr) ((expr)->gcInfo.counter++)

#elif GARBAGE_COLLECTOR == GC_MARK_AND_SWEEP

struct EnvGcInfo {
    size_t noReclaimedExpr; /* Number of expressions reclaimed dur. last run */
    size_t noMarkedExpr;    /* Number of expressions in use during last run */
};

#define __GC_DEFINE_ENV_INFO struct EnvGcInfo gcInfo;
#define __GC_DEFINE_EXPR_INFO
#define __GC_INIT_EXPR_INFO(initcounter)
#define __GC_RUN(env) gcMarkAndSweep(env)
#define __GC_INIT_EXPRESSION(env, expr) gcMarkExpression(env, expr)

#define __GC_INIT_ENVIRONMENT(env) gcInitEnvironment(env)
#define __GC_GET_REF_COUNT(env, expr) 1
#define __GC_DEC_REF_COUNT(env, expr) \
    while (0) {                       \
    }
#define __GC_INC_REF_COUNT(env, expr) \
    while (0) {                       \
    }

struct Expression *gcInitExpression(struct Expression *env,
                                    struct Expression *expr);

struct Expression *gcMarkAndSweep(struct Expression *env);

struct Expression *gcMarkExpression(struct Expression *env,
                                    struct Expression *expr);

struct Expression *gcInitEnvironment(struct Environment *environ);

#else

#error("GARBAGE_COLLECTOR not set!")

#endif

#include "environment.h"

#endif
