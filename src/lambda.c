/**
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
#include <assert.h>



#define MODULE_NAME "lambda.c"

#ifdef DEBUG_LAMBDA
#    include "debugging.h"
#else
#    include "no_debugging.h"
#endif


struct Expression *lambdaCreate(struct Expression *env, struct Expression *expr) {
    struct Lambda *lambda;

    assert(env);

    if(!expr || !EXPR_OF_TYPE(expr, EXPR_CONS)) {
        return 0;
    }
    /* TODO: Check if all arguments are symbols */
    DEBUG_PRINT_EXPR(env, expr);
    lambda = (struct Lambda *)malloc(sizeof(struct Lambda));
    lambda->argList = expressionAssign(env, intCar(env, expr));
    lambda->body = expressionAssign(env, intCar(env, intCdr(env, expr)));
    lambda->env =  expressionAssign(env, env);
    DEBUG_PRINT("Created lambda ...");
    return expressionCreate(env, EXPR_LAMBDA ,lambda);
}


struct Expression *lambdaInvoke(struct Lambda *lambda, struct Expression *args) {
    struct Expression *sym, *val, *exprBuf;

    /* 1st step: create new environment with old environment as parent */
    struct Expression *env = environmentCreate(lambda->env);
    assert(lambda);
    /* 2nd step: Fill in arguments */
    sym = lambda->argList;
    val = args;
    DEBUG_PRINT("sym ");
    DEBUG_PRINT_EXPR(env, sym);
    DEBUG_PRINT_EXPR(env, val);
    while(sym != NIL && args != NIL) {
        if(!EXPR_OF_TYPE(sym, EXPR_CONS) || !EXPR_OF_TYPE(val, EXPR_CONS)) {
            ERROR(ERR_UNEXPECTED_TYPE, "Argument list is flawed");
            expressionDispose(lambda->env, env);
            return NIL;
        }
        /* exprBuf = intCar(env, val); */
        /* DEBUG_PRINT_PARAM("Counter of exprBuf before eval: %i\n",
         * exprBuf->counter); */
        /* exprBuf = eval(env, exprBuf);  
           DEBUG_PRINT_PARAM("Counter of exprBuf after eval: %i\n",
         * exprBuf->counter); */
        exprBuf = eval(env, intCar(env, val)); 
        ENVIRONMENT_ADD_SYMBOL(env, intCar(env, sym), exprBuf);
        expressionDispose(env, exprBuf);
        sym = intCdr(env, sym);
        val = intCdr(env, val);
    }
    IF_SAFETY_CODE( \
            if(val != NIL) { \
                expressionDispose(lambda->env, env); \
                ERROR(ERR_UNEVEN_SYM_VAL, "too many arguments given!"); \
                return NIL; \
            } \
        );
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

