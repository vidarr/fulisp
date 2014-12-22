/*
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
#include "cons.h"

#define MODULE_NAME "environment.c"

#ifdef DEBUG_ENVIRONMENT
#   include "debugging.h"
#else
#   include "no_debugging.h"
#endif



struct Expression *environmentCreate(struct Expression *parent, struct Memory *mem) {
    struct Environment *env = (struct Environment *)malloc(sizeof(struct Environment));
    env->lookup = hashTableCreate(SYMBOL_TABLE_SIZE, stdHashFunction);
    if(parent && !EXPR_IS_NIL(parent)) {
        DEBUG_PRINT_PARAM("Counter of parent before expressionAssign: %i\n", parent->counter);
        if(!EXPR_OF_TYPE(parent, EXPR_ENVIRONMENT)) {
            ERROR(ERR_UNEXPECTED_TYPE, "Expected environment");
            return NIL;
        }
        expressionAssign(parent, parent);
        DEBUG_PRINT_PARAM("Counter of parent after expressionAssign: %i\n", parent->counter);
    }
    env->parent = parent;
    env->memory = mem;
    /* TODO: As soon as Memory structure will be used, replace by different call to a
     * functon using only Memory, not Environment */
    /* expressionCreate(env, EXPR_ENVIRONMENT, env); */
    return createEnvironmentExpression(env);
}


struct Expression *environmentCreateStdEnv(struct Memory *mem) {
    struct Expression *env = environmentCreate(NULL, mem);
    ADD_NATIVE_FUNCTION_EXPRESSION(env, "QUOTE",            quote);
    ADD_NATIVE_FUNCTION_EXPRESSION(env, "CAR",              car);
    ADD_NATIVE_FUNCTION_EXPRESSION(env, "CDR",              cdr);
    ADD_NATIVE_FUNCTION_EXPRESSION(env, "SET-CAR!",         setCar);
    ADD_NATIVE_FUNCTION_EXPRESSION(env, "SET-CDR!",         setCdr); 
    ADD_NATIVE_FUNCTION_EXPRESSION(env, "CONS",             cons); 
    ADD_NATIVE_FUNCTION_EXPRESSION(env, "CONS?",            consP);
    ADD_NATIVE_FUNCTION_EXPRESSION(env, "LAMBDA",           lambdaCreate);
    ADD_NATIVE_FUNCTION_EXPRESSION(env, "SET!",             set);
    ADD_NATIVE_FUNCTION_EXPRESSION(env, "DEFINE",           define);
    ADD_NATIVE_FUNCTION_EXPRESSION(env, "BEGIN",            begin);
    ADD_NATIVE_FUNCTION_EXPRESSION(env, "COND",             cond);
    ADD_NATIVE_FUNCTION_EXPRESSION(env, "OR",               or);
    ADD_NATIVE_FUNCTION_EXPRESSION(env, "AND",              and);
    ADD_NATIVE_FUNCTION_EXPRESSION(env, "NOT",              not);
    ADD_NATIVE_FUNCTION_EXPRESSION(env, "+",                add);
    ADD_NATIVE_FUNCTION_EXPRESSION(env, "*",                mul);
    ADD_NATIVE_FUNCTION_EXPRESSION(env, "/",                divide);
    ADD_NATIVE_FUNCTION_EXPRESSION(env, "=",                numEqual);
    ADD_NATIVE_FUNCTION_EXPRESSION(env, "<",                numSmaller);
    ADD_NATIVE_FUNCTION_EXPRESSION(env, "INTEGER?",         integerP);
    ADD_NATIVE_FUNCTION_EXPRESSION(env, "FLOAT?",           floatP);
    ADD_NATIVE_FUNCTION_EXPRESSION(env, "CHARACTER?",       characterP);
    ADD_NATIVE_FUNCTION_EXPRESSION(env, "NATIVE-FUNCTION?", nativeFunctionP);
    ADD_NATIVE_FUNCTION_EXPRESSION(env, "STRING?",          stringP);
    ADD_NATIVE_FUNCTION_EXPRESSION(env, "SYMBOL?",          symbolP);
    ADD_NATIVE_FUNCTION_EXPRESSION(env, "LAMBDA?",          lambdaP);
    ADD_NATIVE_FUNCTION_EXPRESSION(env, "ENVIRONMENT?",     environmentP);
    ADD_NATIVE_FUNCTION_EXPRESSION(env, "GC-RUN",           gcRun);

    ENVIRONMENT_ADD_STRING(env, ((NIL)->data.string), NIL); 
    ENVIRONMENT_ADD_STRING(env, ((T)->data.string), T); 
    return env;
}


void environmentDispose(struct Expression *surrEnv, struct Environment *env) {
    /* Dispose symbol table */
    /* TODO: Dispose all expressions within the symboltable */
    int i = 0;
    char **keys;
    if(!env) {
        DEBUG_PRINT("environmentDispose: env is NIL");
        return;
    }
    keys  = hashTableKeys(env->lookup);
    DEBUG_PRINT("Disposing environment...\n");
    while(keys[i] != 0) {
        DEBUG_PRINT_PARAM("Key going to be disposed: %s", keys[i]);
        expressionDispose(surrEnv, hashTableDelete(env->lookup, keys[i++]));
    };
    free(keys);
    if(ENVIRONMENT_GET_PARENT(env)) 
        expressionDispose(surrEnv,  env->parent);
    hashTableDispose(env->lookup);
    free(env);
}


struct Expression *environmentLookup(struct Expression *env, struct Expression *sym) {
    struct Expression *expr;
    struct Expression *parent;
    ENSURE_ENVIRONMENT(env);
    expr = hashTableGet((EXPRESSION_ENVIRONMENT(env))->lookup, EXPRESSION_STRING(sym));
    DEBUG_PRINT_PARAM("Symbol was %s\n", expr ? " found" : " not found");
    if(!expr && 
            (parent = EXPRESSION_ENVIRONMENT(env)->parent) &&
            !EXPR_IS_NIL(parent)) {
        return
            environmentLookup((EXPRESSION_ENVIRONMENT(env))->parent, sym);
    }
    return expressionAssign(env, expr);
}


struct Expression *environmentUpdate(struct Expression *env, struct Expression *sym, struct Expression *expr) {
    struct Expression *oldVal;
    ENSURE_ENVIRONMENT(env);
    oldVal = hashTableGet((EXPRESSION_ENVIRONMENT(env))->lookup, EXPRESSION_STRING(sym));
    DEBUG_PRINT_PARAM("Symbol was %s\n", expr ? " found" : " not found");
    if(!oldVal) {
        if(!EXPR_IS_NIL(ENVIRONMENT_GET_PARENT(EXPRESSION_ENVIRONMENT(env)))) {
            return
                environmentUpdate(ENVIRONMENT_GET_PARENT(EXPRESSION_ENVIRONMENT(env)), sym, expr);
        } else {
            return NULL;
        }
    }
    ENVIRONMENT_ADD_SYMBOL(env, sym, expr);
    return expr;
}

