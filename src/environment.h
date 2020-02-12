/*
 * (C) 2011 Michael J. Beer
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

#ifndef __ENVIRONMENT_H__
#define __ENVIRONMENT_H__

/*******************************************************************************
 *                                 DATA TYPE
 *******************************************************************************/

struct Environment;

#include "error.h"
#include "expression.h"
#include "garbage_collector.h"
#include "hash.h"
#include "lisp_internals.h"
#include "memory.h"

struct Environment {
    struct HashTable *lookup;
    struct Memory *memory;
    struct Expression *parent;
    struct Expression *current;
    GC_DEFINE_ENV_INFO
};

/*******************************************************************************
 * MACROS
 *******************************************************************************/

/**
 * Check if expr is of type environment and if not, set error appropriately and
 * return NIL
 */
#define ENSURE_ENVIRONMENT(expr)                                  \
    IF_SAFETY_CODE(if (!EXPR_OF_TYPE((expr), EXPR_ENVIRONMENT)) { \
        ERROR(ERR_UNEXPECTED_TYPE, "Expected Environment");       \
        return NIL;                                               \
    })

/**
 * Get parent environment of env
 */
#define ENVIRONMENT_GET_PARENT(env) ((env)->parent)

/**
 * Get pointer to the Memory struct
 */
#define ENVIRONMENT_GET_MEMORY(env) ((EXPRESSION_ENVIRONMENT(env))->memory)

/**
 * Try to resolve a symbol within an environment
 * @param env expression containing the environment to use
 * @param sym the symbol to look up
 * @return whatever sym resolves to
 */
#define ENVIRONMENT_SYMBOL_LOOKUP(env, sym) environmentLookup((env), (sym))

/**
 * Try to resolve a string within an environment
 * @param env the environment to use
 * @param sym the string to look up
 * @return whatever sym resolves to
 */
#define ENVIRONMENT_STRING_LOOKUP(env, sym) \
    (hashTableGet((EXPRESSION_ENVIRONMENT(env))->lookup, (sym)))

/**
 * Define a symbol within environment
 * Does not check whether the symbol already exists
 * @param env the environment to define the symbol in
 * @param string the name of the new symbol
 * @param expr the expression to set the symbol to
 * @return old value of the symbol or NULL if previously not set
 */
#define ENVIRONMENT_ADD_STRING(env, string, expr)                          \
    {                                                                      \
        void *old = hashTableSet((EXPRESSION_ENVIRONMENT(env))->lookup,    \
                                 (string), (expr));                        \
        if (old != NULL) expressionDispose(env, (struct Expression *)old); \
        expressionAssign((env), (expr));                                   \
    };

/**
 * Define a symbol within environment
 * Does not check whether the symbol already exists
 * @param env the environment to define the symbol in
 * @param sym the symbol
 * @param expression the epxression to set the symbol to
 * @return old value of the symbol or NULL if previously not set
 */
#define ENVIRONMENT_ADD_SYMBOL(env, sym, expr) \
    ENVIRONMENT_ADD_STRING(env, EXPRESSION_STRING(sym), (expr))

/**
 * Create a new expression containing a native function and insert it into the
 * lookup table of an environment
 * @param env the environment to instert to
 * @param string name of the function
 * @param func the function to add
 */
#define ADD_NATIVE_FUNCTION_EXPRESSION(env, string, func)                 \
    {                                                                     \
        struct Expression *f = expressionCreateNativeFunc((env), (func)); \
        ENVIRONMENT_ADD_STRING((env), (string), f);                       \
        expressionDispose((env), f);                                      \
    }

/*******************************************************************************
                               F U N C T I O N S
 *******************************************************************************/

/**
 * Create new empty environment
 * @param parent either expression containing parent environment or 0
 * @param mem pointer to a Memory struct
 * @return the new environment
 */
struct Expression *environmentCreate(struct Expression *parent,
                                     struct Memory *mem);

/**
 * Create new environment with standard symbols defined
 * @param mem pointer to a Memory struct
 * @return the new environment
 */
struct Expression *environmentCreateStdEnv(struct Memory *mem);

/**
 * Disposes of an environment. <b>If you want to get rid of a environment, use
 * expressionDispose. This function is to be used internally/by
 * expressionDispose only!</b>
 * @param surrEnv surrounding environment
 * @param env the environment to dispose
 */
void environmentDispose(struct Expression *surrEnv, struct Environment *env);

void environmentRelease(struct Expression *surrEnv, struct Environment *env);

/**
 * Remove all exressions from environment
 */
void environmentClear(struct Expression *env);

/**
 * Look up the binding of a symbol within the current environment
 * @param env the environment to look up symbol
 * @param sym the symbol to look up
 * @return expression bound to the symbol or NIL if not found
 */
struct Expression *environmentLookup(struct Expression *env,
                                     struct Expression *sym);

/**
 * Look up the binding of a symbol within the current environment. if found,
 * bind expr to that symbol. If not found, return NULL.
 * @param env the environment to look up symbol
 * @param sym the symbol to look up
 * @param expr Expression to bind to sym
 * @return expression bound to the symbol or NULL if not found
 */
struct Expression *environmentUpdate(struct Expression *env,
                                     struct Expression *sym,
                                     struct Expression *expr);

#endif
