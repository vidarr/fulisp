/*
 * (C) 2012 Michael J. Beer
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
    

#include "lambda.h"
#include "cons.h"
#include <assert.h>



#define MODULE_NAME "lambda.c"

#ifdef DEBUG_LAMBDA
#    include "debugging.h"
#else
#    include "no_debugging.h"
#endif


struct Expression *lambdaCreate(struct Expression *env, struct Expression *expr) {
    struct Lambda *lambda;
    struct Expression *iter;
    struct Expression *args;
    struct Expression *beforeLast, *current;
    struct Expression *rest = NIL;

    assert(env);

    if(!expr || !EXPR_OF_TYPE(expr, EXPR_CONS)) {
        return NIL;
    }
    current = args = intCar(env, expr);
    beforeLast = NIL;

    while(EXPR_OF_TYPE(current, EXPR_CONS)) {
        DEBUG_PRINT_EXPR(env, beforeLast);
        DEBUG_PRINT_EXPR(env, current);
        iter = intCar(env, current);
        if(!iter || !EXPR_OF_TYPE(iter, EXPR_SYMBOL)) {
        ERROR(ERR_UNEXPECTED_TYPE, "Argument list of lambda expression " \
            "does not contain symbols only");
            return NIL;
        } 
        if(iter == REST) {
            /* TODO: Consider copying the argument list, as it is altered in here...  */
            current = intCdr(env, current);
            if(!EXPR_OF_TYPE(current, EXPR_CONS) || !(rest = intCar(env, current)) || EXPR_IS_NIL(rest) || 
               !EXPR_OF_TYPE(rest, EXPR_SYMBOL)) { /* || !(terminal = intCdr(env, current)) || !EXPR_IS_NIL(terminal)) { */
                ERROR(ERR_SYNTAX_ERROR, " Expected SYMBOL after &REST");
                return NIL;
            } 
            if(EXPR_IS_NIL(beforeLast)) 
                args = NIL;
            else {
                expressionDispose(env, intCdr(env, beforeLast));
                EXPRESSION_SET_CDR(beforeLast, NIL);
            }
            break;
        }
        beforeLast = EXPR_IS_NIL(beforeLast) ? args : intCdr(env, beforeLast);
        current = intCdr(env, current);
    }
    DEBUG_PRINT("expr is  "); DEBUG_PRINT_EXPR(env, expr); 
    DEBUG_PRINT("args is  "); DEBUG_PRINT_EXPR(env, args); 
    DEBUG_PRINT("rest is  "); DEBUG_PRINT_EXPR(env, rest);
    DEBUG_PRINT("\n");
    lambda          = (struct Lambda *)malloc(sizeof(struct Lambda));
    lambda->rest    = expressionAssign(env, rest);
    lambda->argList = expressionAssign(env, args);
    lambda->body    = expressionAssign(env, intCar(env, intCdr(env, expr)));
    lambda->env     = expressionAssign(env, env);
    DEBUG_PRINT("Created lambda ...");
    return EXPRESSION_CREATE_ATOM(env, EXPR_LAMBDA, lambda);
}


struct Expression *lambdaInvoke(struct Expression *oldEnv, struct Lambda *lambda, struct Expression *args) {
    struct Expression *sym, *val, *exprBuf;

    /* 1st step: create new environment with old environment as parent */
    struct Expression *env = environmentCreate(lambda->env, ENVIRONMENT_GET_MEMORY(oldEnv));
    assert(lambda);
    /* 2nd step: Fill in arguments */
    sym = lambda->argList;
    val = args;
    DEBUG_PRINT("sym ");
    DEBUG_PRINT_EXPR(env, sym);
    DEBUG_PRINT_EXPR(env, val);
    while(sym != NIL && val != NIL) {
        if(!EXPR_OF_TYPE(sym, EXPR_CONS) || !EXPR_OF_TYPE(val, EXPR_CONS)) {
            ERROR(ERR_UNEXPECTED_TYPE, "Argument list is flawed");
            expressionDispose(lambda->env, env);
            return NIL;
        }
        /* Arguments have to be evaluated within old environment! */
        exprBuf = eval(oldEnv, intCar(env, val)); 
        ENVIRONMENT_ADD_SYMBOL(env, intCar(env, sym), exprBuf);
        expressionDispose(env, exprBuf);
        sym = intCdr(env, sym);
        val = intCdr(env, val);
    }

    DEBUG_PRINT("lambdaInvoke(): val is ");
    DEBUG_PRINT_EXPR(env, val);
    DEBUG_PRINT("sym is   ");
    DEBUG_PRINT_EXPR(env, sym);

    if(!EXPR_IS_NIL(lambda->rest)) {
        if(EXPR_IS_NIL(val)) { 
            ENVIRONMENT_ADD_SYMBOL(env, lambda->rest, NIL);
        } else { 
            ENVIRONMENT_ADD_SYMBOL(env, lambda->rest, val);
        }
    } else {
        IF_SAFETY_CODE( \
            if(val != NIL) { \
                expressionDispose(lambda->env, env); \
                ERROR(ERR_UNEVEN_SYM_VAL, "too many arguments given!"); \
                return NIL; \
            } \
        );
    }

    if(sym != NIL) {
        expressionDispose(lambda->env, env); 
        ERROR(ERR_UNEVEN_SYM_VAL, "too few arguments given!");
        return NIL;
    }
    /* 3rd step: Evaluate body within new environment */
    val = eval(env, lambda->body);
    /* 4th step: Get rid of new environment */
    expressionDispose(lambda->env, env);
    return val;
}


void lambdaDispose(struct Lambda *lambda) {
    DEBUG_PRINT("lamdaDispose called");
    if(lambda) {
        assert(lambda->argList && lambda->env && lambda->body);
        expressionDispose(lambda->env, lambda->env);
        expressionDispose(lambda->env, lambda->argList);
        expressionDispose(lambda->env, lambda->body);
        free(lambda);
    } else 
        DEBUG_PRINT("lambda is already NIL");
}

