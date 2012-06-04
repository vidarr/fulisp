/**
 * (C) 2011 Michael J. Beer
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

#include "environment.h"
#include "nativefunctions.h"
/* #include "stdio.h" */



/**
 * Create a new expression containing a native function and insert it into the
 * lookup table of an environment
 * @param env the environment to instert to
 * @param string name of the function 
 * @param func the function to add
 */
#define ADD_NATIVE_FUNCTION_EXPRESSION(env, string, func) \
    ENVIRONMENT_ADD_STRING((env), (string), expressionCreateNativeFunc((env), (func)))



struct Expression *environmentCreate(struct Expression *parent) {
    struct Environment *env = (struct Environment *)malloc(sizeof(struct Environment));
    env->lookup = hashTableCreate(SYMBOL_TABLE_SIZE, stdHashFunction);
    if(parent) {
        if(EXPR_OF_TYPE(parent, EXPR_ENVIRONMENT)) {
            ERROR(ERR_UNEXPECTED_TYPE, "Expected environment");
            return NIL;
        };
        expressionAssign(parent, parent);
    }
    env->parent = parent;
    return expressionCreate(parent, EXPR_ENVIRONMENT, env);
}


struct Expression *environmentCreateStdEnv(void) {
    struct Expression *env = environmentCreate(0);
    ADD_NATIVE_FUNCTION_EXPRESSION(env, "QUOTE", quote);
    ADD_NATIVE_FUNCTION_EXPRESSION(env, "+", add);
    ENVIRONMENT_ADD_STRING(env, "NIL", 0); 
/*    expr = expressionCreate(env, EXPR_NATIVE_FUNC, (void *)quote);
    ENVIRONMENT_ADD_STRING(env, "QUOTE", expr); */
    return env;
}


void environmentDispose(struct Expression *surrEnv, struct Environment *env) {
    /* Dispose symbol table */
    /* TODO: Dispose all expressions within the symboltable */
    int i = 0;
    char **keys = hashTableKeys(env->lookup);
    while(keys[i] != 0) {
        fprintf(stderr, "Key going to be disposed: %s", keys[i]);
        expressionDispose(surrEnv, hashTableDelete(env->lookup, keys[i++]));
    };
    free(keys);
    if(env->parent) 
        expressionDispose(surrEnv,  env->parent);
    hashTableDispose(env->lookup);
    free(env);
}


struct Expression *environmentLookup(struct Expression *env, struct Expression *sym) {
    struct Expression *expr;
    ENSURE_ENVIRONMENT(env);
    expr = hashTableGet((EXPRESSION_ENVIRONMENT(env))->lookup, EXPRESSION_STRING(sym));
    if(!expr && EXPRESSION_ENVIRONMENT(env)->parent) {
        return
            environmentLookup((EXPRESSION_ENVIRONMENT(env))->parent, sym);
    }
    return expr;
}


/* struct Expression *environmentExpressionLookup(struct Expression *env, struct
        Expression *sym) {
    ENSURE_ENVIRONMENT(env);
    return environmentLookup(EXPRESSION_ENVIRONMENT(env), sym);
} */
