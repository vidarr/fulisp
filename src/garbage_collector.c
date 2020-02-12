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
#include "garbage_collector.h"
#include "hash.h"

#define MODULE_NAME "garbage_collector.c"

#ifdef DEBUG_GARBAGE_COLLECTOR
#include "debugging.h"
#else
#include "no_debugging.h"
#endif

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
#define IS_SET_DONT_FREE(expr) ((expr)->type & EXPR_DONT_FREE)

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

void outOfMemory(void) ;

struct Expression *gcMarkAndSweep(struct Expression *env) {

    struct Environment *environ;

    assert(env);
    ENSURE_ENVIRONMENT(env);

    environ = EXPRESSION_ENVIRONMENT(env);
    assert(0 != environ);

    environ->gcInfo.noMarkedExpr = 0;
    environ->gcInfo.noReclaimedExpr = 0;

    DEBUG_PRINT("Resetting memory\n");

    assert(0 != environ->memory);
    resetMemory(environ->memory);

    dumpFreeExpressions(environ->memory);

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

    dumpFreeExpressions(environ->memory);

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
    struct Expression *expr = 0;
    size_t indexExpr;

    for (indexExpr = 0; indexExpr < MEMORY_BLOCK_SIZE; indexExpr++) {
        expr = &block[indexExpr];
        UNMARK_EXPR(expr);
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
    while (block != NULL) {
        gcIntUnmarkAllFromBlock(block->memory);
        block = block->nextBlock;
    }
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
    char buffer[4096];
    struct Expression *expr;
    size_t indexExpr;
    size_t noReclaimed = 0;
    expr = block;
    for (indexExpr = 0; indexExpr < MEMORY_BLOCK_SIZE; indexExpr++) {

        expr = &block[indexExpr];

        if (IS_SET_DONT_FREE(expr)) continue;
        if (IS_MARKED(expr)) continue;

        expressionRelease(env, expr);

        noReclaimed++;
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
    while (block != NULL) {
        noReclaimedExpressions +=
            gcIntReclaimExpressionsFromBlock(env, memory, block->memory);
        block = block->nextBlock;
    }
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
#undef IS_SET_DONT_FREE
