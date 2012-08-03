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
#include "debugging.h"
#else
#include "no_debugging.h"
#endif


char nil[] = "NIL"; 

struct Expression expressionNil = { 
    {nil}, 
    0, 
    EXPR_SYMBOL 
}; 


/* struct Cons *nilCons; */
/*  */
/* struct Expression expressionNil = { */
/*     {nilCons}, */
/*     0, */
/*     EXPR_CONS */
/* }; */
/*  */
/* struct Cons *nilCons = { */
/*     &expressionNil, &expressionNil */
/* }; */


char true[] = "T";
struct Expression expressionT = {
    {true},
    0,
    EXPR_SYMBOL
};


/* void expressionInitialize(void) { */
/*     expressionNil.data.string = nil; */
/*     expressionNil.counter = 0; */
/*     expressionNil.type = EXPR_SYMBOL; */
/* } */


void expressionDispose(struct Expression *env, struct Expression *expr) {
    if(!expr || EXPR_IS_NIL(expr) || expr == T) return;
    if(!EXPR_IS_VALID(expr)) {
        free(expr); 
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
    DEBUG_PRINT_PARAM("Disposing type %u \n", (int)EXPRESSION_TYPE(expr));

    if(EXPR_IS_POINTER(expr)) {
        if(EXPR_OF_TYPE(expr, EXPR_CONS)) {
            DEBUG_PRINT("   expr is a cons cell - recursive disposing...\n");
            if(EXPRESSION_CAR(expr)) expressionDispose(env, EXPRESSION_CAR(expr));
            if(EXPRESSION_CDR(expr)) expressionDispose(env, EXPRESSION_CDR(expr));
            free(expr->data.cons);
        } else if(EXPR_OF_TYPE(expr, EXPR_ENVIRONMENT)) {
            environmentDispose(env, EXPRESSION_ENVIRONMENT(expr));
        } else if(EXPR_OF_TYPE(expr, EXPR_STRING) || 
                EXPR_OF_TYPE(expr, EXPR_SYMBOL)) {
            free(EXPRESSION_STRING(expr));
        } else if (EXPR_OF_TYPE(expr, EXPR_LAMBDA)) {
            lambdaDispose(EXPRESSION_LAMBDA(expr));
        }
    }
    free(expr);
    expr = 0;
}


struct Expression *expressionCreate(struct Expression *env, unsigned char type, void *content) {
    struct Expression *expr = malloc(sizeof(struct Expression));
    expr->counter = 0;
    expr->type = type;
    if(EXPR_IS_POINTER(expr)) {
        expr->data.string = (char *)content;
    } else {
        switch(type) {
            case EXPR_INTEGER:
                expr->data.integer = *((int *)content);
                break;
            case EXPR_FLOAT:
                expr->data.floating = *((float *)content);
                break;
            case EXPR_CHARACTER:
                expr->data.character = *((char *)content);
                break;
            case EXPR_NATIVE_FUNC:
#ifndef STRICT_NATIVE_FUNCS
                expr->data.nativeFunc = 
                    ((struct Expression *(*)(struct Expression *, struct Expression *))content);
#else 
                ERROR(ERR_UNEXPECTED_TYPE,
                        "Cannot create native function expression via expressionCreate. Use expressionCreateNativeFunc instead!");
                return NIL;
                
#endif
                break;
            case EXPR_NO_TYPE:
                expr->data.string = 0;
        };
    }

    DEBUG_PRINT("Created new expression\n");
    return expr;
}


struct Expression *expressionCreateNativeFunc(struct Expression *env, NativeFunction *content) {
    struct Expression *expr = malloc(sizeof(struct Expression));
    expr->counter = 0;
    expr->type = EXPR_NATIVE_FUNC;
    expr->data.nativeFunc = content;
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
    return expressionCreate(env, EXPR_STRING, buf);
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
    return expressionCreate(env, EXPR_SYMBOL, buf);
}


struct Expression *expressionAssign(struct Expression *env, struct Expression *expr) {
    if(expr) 
        expr->counter++;
    return expr;
}

