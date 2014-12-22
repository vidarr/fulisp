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

#include "garbage_collector.h"
#include "hash.h"

#define MODULE_NAME "garbage_collector.c"

#ifdef DEBUG_GARBAGE_COLLECTOR
#include "debugging.h"
#else
#include "no_debugging.h"
#endif

#if GARBAGE_COLLECTOR == GC_MARK_AND_SWEEP


static struct Expression * gcIntFlipMarker(struct Environment *env) ;
static struct Expression * gcIntMarkHash(int marker, 
                                         struct Expression *env, 
                                         struct HashTable *hash) ;
static struct Expression * gcIntMarkEnv(int marker, struct Expression *env) ;
static struct Expression * gcIntMarkExpression(int marker, 
                                               struct Expression *env, 
                                               struct Expression *expr) ;
static struct Expression * gcIntSweep(struct Environment *env);
static void                gcIntUnmarkAll(int marker, struct Environment *env);

    
struct Expression * gcMarkAndSweep(struct Expression *env) {
    struct Environment * environ;
    int marker;

    assert(env);
    ENSURE_ENVIRONMENT(env);

    environ = EXPRESSION_ENVIRONMENT(env);
    gcIntFlipMarker(environ);
    marker = environ->gcInfo.marker;
    environ->gcInfo.noMarkedExpr = 0;
    environ->gcInfo.noReclaimedExpr = 0;
    DEBUG_PRINT("Marking...\n");
    /* Perhaps unnecessary ... */
    gcIntUnmarkAll(marker, environ); 
    gcIntMarkExpression(marker, env, environ->current);
    gcIntMarkExpression(marker, env, env);
    DEBUG_PRINT("Sweeping...\n");
    gcIntSweep(environ);
    DEBUG_PRINT_PARAM("Still in use: %lu\n", environ->gcInfo.noMarkedExpr);
    DEBUG_PRINT_PARAM("Reclaimed   : %lu\n", environ->gcInfo.noReclaimedExpr);
    return T;
}


struct Expression * gcMarkExpression(struct Expression *env, 
                                     struct Expression *expr) {
    struct Environment *environ;
    int marker;

    assert(env);
    ENSURE_ENVIRONMENT(env);

    environ = EXPRESSION_ENVIRONMENT(env);
    marker = environ->gcInfo.marker;
    return gcIntMarkExpression(marker, env, expr);
}


struct Expression *gcInitEnvironment(struct Environment *environ) {
    assert(environ);
    environ->gcInfo.marker = 0;
    environ->current = NULL;
    return T;
}


/******************************************************************************
  I N T E R N A L S
 ******************************************************************************/


#define MARK_EXPR(marker, expr) { \
    if(marker) { \
        SET_MARKER_BIT(expr); \
    } else { \
        UNSET_MARKER_BIT(expr); \
    } \
}

#define UNMARK_EXPR(marker, expr) { \
    if(marker) { \
        UNSET_MARKER_BIT(expr); \
    } else { \
        SET_MARKER_BIT(expr); \
    } \
}

#define SET_MARKER_BIT(expr)  { \
    int type = (expr)->type; \
    type |= EXPR_GC_MARKER_BIT; \
    (expr)->type = type; \
}

#define UNSET_MARKER_BIT(expr)  { \
    int type = (expr)->type; \
    type &= ~EXPR_GC_MARKER_BIT; \
    (expr)->type = type; \
}

#define IS_MARKER_BIT_SET(expr) \
    ((expr)->type & EXPR_GC_MARKER_BIT)
    
#define IS_MARKED(marker, expr) \
    (( marker) ? \
     IS_MARKER_BIT_SET(expr) \
     : \
     ! IS_MARKER_BIT_SET(expr) \
    )


/******************************************************************************
 * M A R K   
 *****************************************************************************/


static void gcIntUnmarkAllFromBlock(int marker, struct Expression *block) {
    struct Expression *expr;
    size_t indexExpr; 
    expr = block;
    for(indexExpr = 0; indexExpr < MEMORY_BLOCK_SIZE; indexExpr++) {
        UNMARK_EXPR(marker, expr);
        expr++;
    }
}


static void gcIntUnmarkAll(int marker, struct Environment *env) {
    struct ExpressionBlock *block;
    struct Memory * memory;

    assert(env);

    memory = env->memory;
    assert(env);
    block = memory->exprBlocks;
    assert(block);
#   ifdef MEMORY_AUTOEXTEND
    while(block != NULL) {
        gcIntUnmarkAllFromBlock(marker, block->memory);
        block = block->nextBlock;
    }
#   else
    gcIntUnmarkAllFromBlock(marker, block->memory);
#   endif
}


static struct Expression * gcIntFlipMarker(struct Environment *env) {
    int marker = env->gcInfo.marker;
    marker ^= EXPR_GC_MARKER_BIT;
    env->gcInfo.marker = marker;
    DEBUG_PRINT_PARAM("marker is now %x\n", env->gcInfo.marker);
    return T;
}


static struct Expression * gcIntMarkHash(int marker, 
                                         struct Expression *env, 
                                         struct HashTable *hash) {
    /* Highly inefficient - better have some iterator over the hash content */
    char ** current;
    char **keys;
    struct Expression * expr;

    assert(env);

    keys = hashTableKeys(hash);
    for(current = keys; *current != NULL; current++) {
        expr = hashTableGet(hash, *current);
        gcIntMarkExpression(marker, env, expr);
    }
    free(keys);
    return T;
}
 

static struct Expression * gcIntMarkEnv(int marker, struct Expression *env) {
    struct Environment *environ = EXPRESSION_ENVIRONMENT(env);
    return gcIntMarkHash(marker, env, environ->lookup);
}


static struct Expression * gcIntMarkExpression(int marker, 
                                               struct Expression *env,  
                                               struct Expression *expr) {
    struct Expression  *element;
    struct Lambda      *lambda;
    struct Environment *environ;

    assert(env);

    if(expr == NULL) {
        DEBUG_PRINT("Expression is NIL\n");
        return T;
    }
    DEBUG_PRINT_EXPR(env, expr);
    if(IS_MARKED(marker, expr)) return T;
    MARK_EXPR(marker, expr);
    environ = EXPRESSION_ENVIRONMENT(env);
    environ->gcInfo.noMarkedExpr++;
    if(EXPR_IS_CONS(expr)) {
        DEBUG_PRINT("Marking CONS cell\n");
        element = intCar(env, expr);
        gcIntMarkExpression(marker, env, element);
        element = intCdr(env, expr);
        gcIntMarkExpression(marker, env, element);
    } else if(EXPR_OF_TYPE(expr, EXPR_LAMBDA)) {
        DEBUG_PRINT("Marking LAMBDA\n");
        lambda = EXPRESSION_LAMBDA(expr);
        assert(lambda);
        gcIntMarkExpression(marker, env, lambda->body);
        gcIntMarkExpression(marker, env, lambda->env);
        gcIntMarkExpression(marker, env, lambda->argList);
        gcIntMarkExpression(marker, env, lambda->rest);
    } else if(EXPR_OF_TYPE(expr, EXPR_ENVIRONMENT)) {
        gcIntMarkEnv(marker, expr);
    };
    return T;
}


/******************************************************************************
 *  S W E E P    P A R T
 ******************************************************************************/


static int gcIntReclaimExpressionsFromBlock(int marker, struct Memory *memory, 
        struct Expression *block) {
    struct Expression *expr;
    size_t indexExpr;
    size_t noReclaimed = 0;
    expr = block;
    for(indexExpr = 0; indexExpr < MEMORY_BLOCK_SIZE; indexExpr++) {
        if(! IS_MARKED(marker, expr)) {
            MEMORY_DISPOSE_EXPRESSION(memory, expr);
            noReclaimed++;
        }
        expr++;
    }
    return noReclaimed;
}


static int gcIntReclaimExpressions(int marker, struct Memory *memory) {
    struct ExpressionBlock *block;
    unsigned int noReclaimedExpressions = 0;

    assert(memory);

    block = memory->exprBlocks;
    assert(block);
#   ifdef MEMORY_AUTOEXTEND
    while(block != NULL) {
        noReclaimedExpressions += gcIntReclaimExpressionsFromBlock(marker, 
                memory, block->memory);
        block = block->nextBlock;
    }
#   else
    noReclaimedExpressions += gcIntReclaimExpressionsFromBlock(marker, 
            memory, block->memory);
#   endif
    return noReclaimedExpressions;
}


static struct Expression * gcIntSweep(struct Environment * environ) {
    struct Memory     *memory;
    int marker = environ->gcInfo.marker;
    memory = environ->memory;
    memory->nextExpr = NULL;
    environ->gcInfo.noReclaimedExpr = gcIntReclaimExpressions(marker, memory);
    return T;
}
    
#undef MARKER_BIT
#undef MARK_EXPR
#undef SET_MARKER_BIT 
#undef UNSET_MARKER_BIT 
#undef IS_MARKER_BIT_SET
#undef IS_MARKED

#endif

