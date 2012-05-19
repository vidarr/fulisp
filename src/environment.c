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
#include "stdio.h"



/**
 * Create a new expression containing a native function and insert it into the
 * lookup table of an environment
 * @param env the environment to instert to
 * @param string name of the function 
 * @param func the function to add
 */
#define ADD_NATIVE_FUNCTION_EXPRESSION(env, string, func) \
    ENVIRONMENT_ADD_STRING((env), (string), expressionCreateNativeFunc((env), (func)))



struct Environment *environmentCreate(void) {
    struct Environment *env = (struct Environment *)malloc(sizeof(struct Environment));
    env->lookup = hashTableCreate(SYMBOL_TABLE_SIZE, stdHashFunction);
    return env;
}



struct Environment *environmentCreateStdEnv(void) {
    struct Environment *env = environmentCreate();
    ADD_NATIVE_FUNCTION_EXPRESSION(env, "QUOTE", quote);
    ADD_NATIVE_FUNCTION_EXPRESSION(env, "+", add);
    ENVIRONMENT_ADD_STRING(env, "NIL", 0); 
/*    expr = expressionCreate(env, EXPR_NATIVE_FUNC, (void *)quote);
    ENVIRONMENT_ADD_STRING(env, "QUOTE", expr); */
    return env;
}



void environmentDispose(struct Environment *env) {
    /* Dispose symbol table */
    /* TODO: Dispose all expressions within the symboltable */
    int i = 0;
    char **keys = hashTableKeys(env->lookup);
    while(keys[i] != 0) {
        fprintf(stderr, "Key going to be disposed: %s", keys[i]);
        expressionDispose(env, hashTableDelete(env->lookup, keys[i++]));
    };
    free(keys);

    hashTableDispose(env->lookup);
    free(env);
}

