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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,
 * USA.
 */

#include "garbage_collector.h"
#include "hash.h"

#define MODULE_NAME "garbage_collector.c"

#ifdef DEBUG_GARBAGE_COLLECTOR
#include "debugging.h"
#else
#include "no_debugging.h"
#endif

#if GARBAGE_COLLECTOR == GC_MARK_AND_SWEEP

/******************************************************************************
 *                                   MACROS
 ******************************************************************************/

#define MARK_EXPR(expr) SET_MARKER_BIT(expr)
#define UNMARK_EXPR(expr) UNSET_MARKER_BIT(expr)

#define SET_MARKER_BIT(expr)        \
    {                               \
        int type = (expr)->type;    \
        type |= EXPR_GC_MARKER_BIT; \
        (expr)->type = type;        \
    }

#define UNSET_MARKER_BIT(expr)       \
    {                                \
        int type = (expr)->type;     \
        type &= ~EXPR_GC_MARKER_BIT; \
        (expr)->type = type;         \
    }

#define IS_MARKER_BIT_SET(expr) ((expr)->type & EXPR_GC_MARKER_BIT)

#define IS_MARKED(expr) IS_MARKER_BIT_SET(expr)
/******************************************************************************
 *                                 PROTOTYPES
 ******************************************************************************/

static struct Expression *gcIntMarkHash(struct Expression *env,
                                        struct HashTable *hash);

static struct Expression *gcIntMarkEnv(struct Expression *env);

static struct Expression *gcIntMarkExpression(struct Expression *env,
                                              struct Expression *expr);

static struct Expression *gcIntSweep(struct Expression *exprEnv);

static void gcIntUnmarkAll(struct Environment *env);

struct Expression *gcMarkAndSweep(struct Expression *env) {

    struct Environment *environ;

    assert(env);
    ENSURE_ENVIRONMENT(env);

    environ = EXPRESSION_ENVIRONMENT(env);
    environ->gcInfo.noMarkedExpr = 0;
    environ->gcInfo.noReclaimedExpr = 0;
    DEBUG_PRINT("Marking...\n");

    /* Perhaps unnecessary ... */
    gcIntUnmarkAll(environ);
    DEBUG_PRINT("Marking current Expr\n");
    gcIntMarkExpression(env, environ->current);
    DEBUG_PRINT("Marking Global environment\n");
    gcIntMarkExpression(env, env);
    DEBUG_PRINT("Sweeping...\n");
    gcIntSweep(env);
    DEBUG_PRINT_PARAM("Still in use: %lu\n", environ->gcInfo.noMarkedExpr);
    DEBUG_PRINT_PARAM("Reclaimed   : %lu\n", environ->gcInfo.noReclaimedExpr);
    return T;
}

struct Expression *gcMarkExpression(struct Expression *env,
                                    struct Expression *expr) {
    assert(env);
    ENSURE_ENVIRONMENT(env);

    return gcIntMarkExpression(env, expr);
}

struct Expression *gcInitEnvironment(struct Environment *environ) {
    assert(environ);
    environ->current = NULL;
    return T;
}

/******************************************************************************
 * M A R K
 *****************************************************************************/

static void gcIntUnmarkAllFromBlock(struct Expression *block) {
    struct Expression *expr;
    size_t indexExpr;
    expr = block;
    for (indexExpr = 0; indexExpr < MEMORY_BLOCK_SIZE; indexExpr++) {
        UNMARK_EXPR(expr);
        expr++;
    }
}

static void gcIntUnmarkAll(struct Environment *env) {
    struct ExpressionBlock *block;
    struct Memory *memory;

    assert(env);

    memory = env->memory;
    assert(env);
    block = memory->exprBlocks;
    assert(block);
#ifdef MEMORY_AUTOEXTEND
    while (block != NULL) {
        gcIntUnmarkAllFromBlock(block->memory);
        block = block->nextBlock;
    }
#else
    gcIntUnmarkAllFromBlock(block->memory);
#endif
}

static struct Expression *gcIntMarkHash(struct Expression *env,
                                        struct HashTable *hash) {
    /* Highly inefficient - better have some iterator over the hash content */
    char **current;
    char **keys;
    struct Expression *expr;

    assert(env);

    keys = hashTableKeys(hash);
    for (current = keys; *current != NULL; current++) {
        expr = hashTableGet(hash, *current);
        DEBUG_PRINT_PARAM("Marking LOOKUP entry for key %s\n", *current);
        gcIntMarkExpression(env, expr);
    }
    free(keys);
    return T;
}

static struct Expression *gcIntMarkEnv(struct Expression *env) {
    struct Environment *environ = EXPRESSION_ENVIRONMENT(env);
    DEBUG_PRINT("MARKING ENV:LOOKUP\n");
    return gcIntMarkHash(env, environ->lookup);
}

static struct Expression *gcIntMarkExpression(struct Expression *env,
                                              struct Expression *expr) {
    struct Expression *element;
    struct Lambda *lambda;
    struct Environment *environ;

    assert(env);

    if (expr == NULL) {
        DEBUG_PRINT("Expression is NIL\n");
        return T;
    }
    DEBUG_PRINT_EXPR(env, expr);
    if (IS_MARKED(expr)) {
        DEBUG_PRINT("Already marked\n");
        return T;
    }
    DEBUG_PRINT("Marking expression\n");
    MARK_EXPR(expr);
    environ = EXPRESSION_ENVIRONMENT(env);
    environ->gcInfo.noMarkedExpr++;
    if (EXPR_IS_CONS(expr)) {
        DEBUG_PRINT("Marking CONS:CAR\n");
        element = intCar(env, expr);
        gcIntMarkExpression(env, element);
        DEBUG_PRINT("Marking CONS:CDR\n");
        element = intCdr(env, expr);
        gcIntMarkExpression(env, element);
    } else if (EXPR_OF_TYPE(expr, EXPR_LAMBDA)) {
        lambda = EXPRESSION_LAMBDA(expr);
        assert(lambda);
        DEBUG_PRINT("Marking LAMBDA:BODY\n");
        gcIntMarkExpression(env, lambda->body);
        DEBUG_PRINT("Marking LAMBDA:ENV\n");
        gcIntMarkExpression(env, lambda->env);
        DEBUG_PRINT("Marking LAMBDA:ARGS\n");
        gcIntMarkExpression(env, lambda->argList);
        DEBUG_PRINT("Marking LAMBDA:REST\n");
        gcIntMarkExpression(env, lambda->rest);
    } else if (EXPR_OF_TYPE(expr, EXPR_ENVIRONMENT)) {
        gcIntMarkEnv(expr);
    };
    DEBUG_PRINT("Done Marking expression \n");
    DEBUG_PRINT_EXPR(env, expr);
    return T;
}

/******************************************************************************
 *  S W E E P    P A R T
 ******************************************************************************/

static int gcIntReclaimExpressionsFromBlock(struct Expression *env,
                                            struct Memory *memory,
                                            struct Expression *block) {
    struct Expression *expr;
    size_t indexExpr;
    size_t noReclaimed = 0;
    expr = block;
    for (indexExpr = 0; indexExpr < MEMORY_BLOCK_SIZE; indexExpr++) {
        if (!IS_MARKED(expr)) {
            expressionForceDispose(env, expr);
            noReclaimed++;
        }
        expr++;
    }
    return noReclaimed;
}

static int gcIntReclaimExpressions(struct Expression *env,
                                   struct Memory *memory) {
    struct ExpressionBlock *block;
    unsigned int noReclaimedExpressions = 0;

    assert(memory);

    block = memory->exprBlocks;
    assert(block);
#ifdef MEMORY_AUTOEXTEND
    while (block != NULL) {
        noReclaimedExpressions +=
            gcIntReclaimExpressionsFromBlock(env, memory, block->memory);
        block = block->nextBlock;
    }
#else
    noReclaimedExpressions +=
        gcIntReclaimExpressionsFromBlock(env, memory, block->memory);
#endif
    return noReclaimedExpressions;
}

static struct Expression *gcIntSweep(struct Expression *env) {

    struct Memory *memory;
    struct Environment *environ;

    assert(env);
    ENSURE_ENVIRONMENT(env);

    environ = EXPRESSION_ENVIRONMENT(env);
    memory = environ->memory;
    memory->nextExpr = NULL;
    environ->gcInfo.noReclaimedExpr = gcIntReclaimExpressions(env, memory);
    return T;
}

#undef MARKER_BIT
#undef MARK_EXPR
#undef SET_MARKER_BIT
#undef UNSET_MARKER_BIT
#undef IS_MARKER_BIT_SET
#undef IS_MARKED

#endif
