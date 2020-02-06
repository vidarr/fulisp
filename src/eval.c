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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,
 * USA.
 */

#include "eval.h"

#define MODULE_NAME "eval.c"

#ifdef DEBUG_EVAL
#include "debugging.h"
#else
#include "no_debugging.h"
#endif

/******************************************************************************
                                PUBLIC FUNCTIONS
 ******************************************************************************/

struct Expression *eval(struct Expression *env, struct Expression *expr) {
    struct Expression *res;
    struct Environment *environ;

    assert(env && expr);
    ENSURE_ENVIRONMENT(env);

    DEBUG_PRINT("Evaluating \n");
    DEBUG_PRINT_EXPR(env, expr);

    if (!EXPR_IS_CONS(expr)) {
        if (!EXPR_OF_TYPE(expr, EXPR_SYMBOL) || EXPR_IS_NIL(expr)) {
            DEBUG_PRINT("eval(): No symbol - evaluate to itself\n");
            res = expressionAssign(env, expr);
        } else {
            DEBUG_PRINT("eval(): Symbol - resolving it...\n");
            res = ENVIRONMENT_SYMBOL_LOOKUP(env, expr);
            if (!res) {
                ERROR(ERR_UNRESOLVABLE, "Could not resolve symbol");
                res = NIL; /* ENVIRONMENT_STRING_LOOKUP(env, "NIL"); */
            }
        }
    } else {
        /* All lists are function calls - argument lists are quoted and thus
         * given to the native function 'quote' returning the list */
        struct Expression *first = intCar(env, expr);
        DEBUG_PRINT("eval(): Got CONS cell\n");
        DEBUG_PRINT_EXPR(env, first);
        /* In case of being a or something returning a function it should be
         * resolved before being applied */
        first = eval(env, first);
        DEBUG_PRINT("eval(): Evaluated to\n");
        DEBUG_PRINT_EXPR(env, first);
        if (!first) {
            /* Should be unreachable code ... */
            ERROR(ERR_UNRESOLVABLE, "Could not resolve symbol");
            first = NIL; /* ENVIRONMENT_STRING_LOOKUP(env, "NIL"); */
            res = expressionAssign(env, first);
        } else {
            switch (EXPRESSION_TYPE(first)) {
                case EXPR_NATIVE_FUNC:
                    res = EXPRESSION_NATIVE_FUNC(first)(env, intCdr(env, expr));
                    break;
                case EXPR_LAMBDA:
                    res = lambdaInvoke(env, EXPRESSION_LAMBDA(first),
                                       intCdr(env, expr));
                    break;
                case EXPR_ENVIRONMENT:
                    res = first;
                    break;
                default:
                    ERROR(ERR_UNEXPECTED_VAL,
                          "eval(): First element in list is not a function!");
                    res = NIL; /* ENVIRONMENT_STRING_LOOKUP (env, "NIL"); */
            };
            expressionDispose(env, first);
        }
    }
    environ = EXPRESSION_ENVIRONMENT(env);
    environ->current = res;
    return res;
}
