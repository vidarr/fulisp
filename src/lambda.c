/*
 * (C) 2012 Michael J. Beer
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
#include "lambda.h"
#include <assert.h>
#include "cons.h"

#define MODULE_NAME "lambda.c"

#ifdef DEBUG_LAMBDA
#include "debugging.h"
#else
#include "no_debugging.h"
#endif

struct Expression *lambdaCreate(struct Expression *env,
                                struct Expression *expr) {
    struct Lambda *lambda;
    struct Expression *iter;
    struct Expression *args;
    struct Expression *beforeLast, *current;
    struct Expression *rest = NIL;

    assert(env);

    if (!expr || !EXPR_OF_TYPE(expr, EXPR_CONS)) {
        return NIL;
    }
    current = args = intCar(env, expr);
    beforeLast = NIL;

    if ((intCar(env, args) == NIL) && (intCdr(env, args) == NIL)) {
        /* LAMBDA does not expect ANY arguments */
        args = NIL;
        current = NIL;
    }

    while (EXPR_OF_TYPE(current, EXPR_CONS)) {
        DEBUG_PRINT_EXPR(env, beforeLast);
        DEBUG_PRINT_EXPR(env, current);
        iter = intCar(env, current);
        if (!iter || !EXPR_OF_TYPE(iter, EXPR_SYMBOL)) {
            ERROR(ERR_UNEXPECTED_TYPE,
                  "Argument list of lambda expression "
                  "does not contain symbols only");
            return NIL;
        }
        if (iter == REST) {
            /* TODO: Consider copying the argument list, as it is altered in
             * here...  */
            current = intCdr(env, current);
            if (!EXPR_OF_TYPE(current, EXPR_CONS) ||
                !(rest = intCar(env, current)) || EXPR_IS_NIL(rest) ||
                !EXPR_OF_TYPE(
                    rest,
                    EXPR_SYMBOL)) { /* || !(terminal = intCdr(env, current)) ||
                                       !EXPR_IS_NIL(terminal)) { */
                ERROR(ERR_SYNTAX_ERROR, " Expected SYMBOL after &REST");
                return NIL;
            }
            if (EXPR_IS_NIL(beforeLast))
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
    DEBUG_PRINT("expr is  ");
    DEBUG_PRINT_EXPR(env, expr);
    DEBUG_PRINT("args is  ");
    DEBUG_PRINT_EXPR(env, args);
    DEBUG_PRINT("rest is  ");
    DEBUG_PRINT_EXPR(env, rest);
    DEBUG_PRINT("\n");
    lambda = (struct Lambda *)SAFE_MALLOC(sizeof(struct Lambda));
    lambda->rest = expressionAssign(env, rest);
    lambda->argList = expressionAssign(env, args);
    lambda->body = expressionAssign(env, intCar(env, intCdr(env, expr)));
    lambda->env = expressionAssign(env, env);
    DEBUG_PRINT("Created lambda ...\n");
    return EXPRESSION_CREATE_ATOM(env, EXPR_LAMBDA, lambda);
}

struct Expression *lambdaInvoke(struct Expression *oldEnv,
                                struct Lambda *lambda,
                                struct Expression *args) {
    struct Expression *sym, *val, *exprBuf;

    struct Expression *retval = NIL;

    /* 1st step: create new environment with old environment as parent */
    struct Expression *env =
        environmentCreate(lambda->env, ENVIRONMENT_GET_MEMORY(oldEnv));
    assert(lambda);
    /* 2nd step: Fill in arguments */
    sym = lambda->argList;
    val = args;
    DEBUG_PRINT("sym ");
    DEBUG_PRINT_EXPR(env, sym);
    DEBUG_PRINT_EXPR(env, val);
    while (!EXPR_IS_NIL(sym) && !EXPR_IS_NIL(val)) {
        if (!EXPR_OF_TYPE(sym, EXPR_CONS) || !EXPR_OF_TYPE(val, EXPR_CONS)) {
            ERROR(ERR_UNEXPECTED_TYPE, "Argument list is flawed");
            expressionDispose(lambda->env, env);
            goto ret_from_func;
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

    if (!EXPR_IS_NIL(lambda->rest)) {
        if (EXPR_IS_NIL(val)) {
            ENVIRONMENT_ADD_SYMBOL(env, lambda->rest, NIL);
        } else {
            ENVIRONMENT_ADD_SYMBOL(env, lambda->rest, val);
        }
    } else {
        IF_SAFETY_CODE(if (!EXPR_IS_NIL(val)) {
            expressionDispose(lambda->env, env);
            ERROR(ERR_UNEVEN_SYM_VAL, "too many arguments given!");
            goto ret_from_func;
        });
    }

    if (!EXPR_IS_NIL(sym)) {
        expressionDispose(lambda->env, env);
        ERROR(ERR_UNEVEN_SYM_VAL, "too few arguments given!");
        goto ret_from_func;
    }
    /* 3rd step: Evaluate body within new environment */
    retval = eval(env, lambda->body);
    /* 4th step: Get rid of new environment */

ret_from_func:

    expressionDispose(lambda->env, env);
    return retval;
}

void lambdaDispose(struct Expression *env, struct Lambda *lambda) {
    DEBUG_PRINT("lamdaDispose called");
    if (lambda) {
        assert(lambda->argList && lambda->env && lambda->body);
        expressionDispose(lambda->env, lambda->argList);
        expressionDispose(lambda->env, lambda->body);
        expressionDispose(env, lambda->env);
        free(lambda);
    } else
        DEBUG_PRINT("lambda is already NIL");
}
