/*
 * (C) 2010 Michael J. Beer
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

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "expression.h"
#include "print.h"
#include "config.h"
#include "environment.h"

#define MODULE_NAME "expression.c"

#ifdef DEBUG_EXPRESSION
#   include "debugging.h"
#else
#   include "no_debugging.h"
#endif



void expressionDispose(struct Expression *env, struct Expression *expr) {
    if(!expr || EXPR_IS_NIL(expr) || expr == T) return;
    if(!EXPR_IS_VALID(expr)) {
        MEMORY_DISPOSE_EXPRESSION(ENVIRONMENT_GET_MEMORY(env), expr);
        return;
    }
    /* ALERT: Does not work with non-valid expressions - occur together with
     * parsing of conses */
    DEBUG_PRINT_PARAM("Dispose: Old counter : %u\n", expr->counter);
    DEBUG_PRINT_PARAM("   type %u \n", (int)EXPRESSION_TYPE(expr));
    DEBUG_PRINT_EXPR(env, expr);

    if(expr->counter-- > 0) return;
    expressionForceDispose(env, expr);
}


void expressionForceDispose(struct Expression *env, struct Expression *expr) {
#   ifdef MEMORY_USE_PREALLOCATION
    struct Memory *mem;
#   endif
   
    DEBUG_PRINT_PARAM("Disposing type %u \n", (int)EXPRESSION_TYPE(expr));

#   ifdef MEMORY_USE_PREALLOCATION
    mem = ENVIRONMENT_GET_MEMORY(env);
#   endif
    if(EXPR_IS_POINTER(expr)) {
        if(EXPR_OF_TYPE(expr, EXPR_CONS)) {
            DEBUG_PRINT("   expr is a cons cell - recursive disposing...\n");
            if(EXPRESSION_CAR(expr)) expressionDispose(env, EXPRESSION_CAR(expr));
            if(EXPRESSION_CDR(expr)) expressionDispose(env, EXPRESSION_CDR(expr));
            /* free(expr->data.cons); */
            __EXPRESSION_DISPOSE_CONS_STRUCT(mem, expr);
        } else if(EXPR_OF_TYPE(expr, EXPR_ENVIRONMENT)) {
            environmentDispose(env, EXPRESSION_ENVIRONMENT(expr));
        } else if(EXPR_OF_TYPE(expr, EXPR_STRING) || 
                EXPR_OF_TYPE(expr, EXPR_SYMBOL)) {
            free(EXPRESSION_STRING(expr)); 
        } else if (EXPR_OF_TYPE(expr, EXPR_LAMBDA)) {
            lambdaDispose(EXPRESSION_LAMBDA(expr));
        }
    }
    /* free(expr); */
    MEMORY_DISPOSE_EXPRESSION(mem, expr);
}


struct Expression *expressionCreate(struct Expression *env, unsigned char type,
        void *content, void *extension) {
    struct Expression *expr;
    /* expr = malloc(sizeof(struct Expression)); */
    MEMORY_GET_EXPRESSION(ENVIRONMENT_GET_MEMORY(env), expr);
    expr->counter = 0;
    expr->type = type;
    if(EXPR_IS_CONS(expr)) {
        /* Allocate mem for struct Cons */
#       if EXPRESSION_FORMAT == EXPRESSION_FORMAT_EXPANDED
            struct Cons *cons;
            MEMORY_GET_CONS(ENVIRONMENT_GET_MEMORY(env), cons); 
            __EXPRESSION_CONS(expr) = cons;
#       endif
        expressionAssign(env, content);
        expressionAssign(env, extension);
        __EXPRESSION_SET_CAR(expr, content);
        __EXPRESSION_SET_CDR(expr, extension);
    } else if(EXPR_IS_POINTER(expr)) {
        __EXPRESSION_STRING(expr)              = (char *)content;
    } else {
        switch(type) {
            case EXPR_INTEGER:
                __EXPRESSION_INTEGER(expr)     = *((int *)content);
                break;
            case EXPR_FLOAT:
                __EXPRESSION_FLOATING(expr)    = *((float *)content);
                break;
            case EXPR_CHARACTER:
                __EXPRESSION_CHARACTER(expr)   = *((char *)content);
                break;
            case EXPR_NATIVE_FUNC:
#ifndef STRICT_NATIVE_FUNCS
                __EXPRESSION_NATIVE_FUNC(expr) = 
                    ((struct Expression *(*)(struct Expression *, struct Expression *))content);
#else 
                ERROR(ERR_UNEXPECTED_TYPE,
                        "Cannot create native function expression via expressionCreate. " \
                        "Use expressionCreateNativeFunc instead!");
                return NIL;
                
#endif
                break;
            case EXPR_NO_TYPE:
                __EXPRESSION_STRING(expr)      = 0;
        };
    }

    DEBUG_PRINT("Created new expression\n");
    return expr;
}


struct Expression *expressionCreateNativeFunc(struct Expression *env, NativeFunction *content) {
    struct Expression *expr;
    /* expr = malloc(sizeof(struct Expression)); */
    MEMORY_GET_EXPRESSION(ENVIRONMENT_GET_MEMORY(env), expr);
    expr->counter = 0;
    expr->type = EXPR_NATIVE_FUNC;
    EXPRESSION_NATIVE_FUNC(expr) = content;
    DEBUG_PRINT("Created new expression containing native function.\n");
    return expr;
}


struct Expression *expressionCreateString(struct Expression *env, char *str) {
    char *buf;
    assert (str);

    /*TODO: As soon as the global lookup for symbols is installed,
      look up str in it and assign the data-slot a pointer to the entry in the
      table */
    buf = malloc(sizeof(char) * (strlen(str) + 1));
    strcpy(buf, str);
    return EXPRESSION_CREATE_ATOM(env, EXPR_STRING, buf);
}


struct Expression *createSymbol(struct Expression *env, char *str) {
    char *buf;
    assert (str);

    DEBUG_PRINT_PARAM("createSymbol(): Got '%s'\n", str);

    /*TODO: As soon as the global lookup for symbols is installed,
      look up str in it and assign the data-slot a pointer to the entry in the
      table */
    buf = malloc(sizeof(char) * (strlen(str) + 1));
    strcpy(buf, str);
    DEBUG_PRINT_PARAM("createSymbol(): Copied to  %s\n", buf);
    return EXPRESSION_CREATE_ATOM(env, EXPR_SYMBOL, buf);
}


struct Expression *expressionAssign(struct Expression *env, struct Expression *expr) {
    if(expr) 
        expr->counter++;
    return expr;
}


struct Expression *createEnvironmentExpression(struct Environment *env) {
    struct Expression *expr;
    MEMORY_GET_EXPRESSION(env->memory, expr);
    expr->counter = 0;
    expr->type = EXPR_ENVIRONMENT;
    EXPRESSION_ENVIRONMENT(expr) = env;
    return expr;
}


char nil[] = "NIL"; 

char true[] = "T";

char rest[] = "&REST";

#if EXPRESSION_FORMAT == EXPRESSION_FORMAT_EXPANDED

struct Expression expressionNil = { 
    EXPR_SYMBOL,
    0, 
    {nil} 
}; 

struct Expression expressionT = {
    EXPR_SYMBOL,
    0,
    {true}
};

struct Expression expressionRest = {
    EXPR_SYMBOL,
    1,
    {rest}
};


#elif EXPRESSION_FORMAT == EXPRESSION_FORMAT_PACKED


struct Expression expressionNil = { 
    EXPR_SYMBOL,
    0, 
    {nil}, 
    {NULL}
}; 

struct Expression expressionT = {
    EXPR_SYMBOL,
    0,
    {true},
    {NULL} 
};

struct Expression expressionRest = {
    EXPR_SYMBOL,
    1,
    {rest},
    {NULL} 
};


#else  /* expression format */


#   error("EXPRESSION_FORMAT not recognized");


#endif /* expression format */

