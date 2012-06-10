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

    assert(env && expr);

    DEBUG_PRINT("Evaluating ");
    DEBUG_PRINT_EXPR(env, expr);

    if(!EXPR_IS_CONS(expr)) {
        if(!EXPR_OF_TYPE(expr, EXPR_SYMBOL)) {
            DEBUG_PRINT("EVAL: No symbol - evaluate to itself\n");
            return expressionAssign(env, expr);
        } else {
            DEBUG_PRINT("EVAL: Symbol - resolving it...\n");
            res = ENVIRONMENT_SYMBOL_LOOKUP(env, expr);
            if(!res) {
                ERROR(ERR_UNRESOLVABLE, "Could not resolve symbol");
                res = NIL; /* ENVIRONMENT_STRING_LOOKUP(env, "NIL"); */
            }  
            return res;
        } 
    }
    else {
        /* All lists are function calls - argument lists are quoted and thus
         * given to the native function 'quote' returning the list */
        struct Expression *first = intCar(env, expr);
        DEBUG_PRINT_EXPR(env, first);
        /* In case of being a or something returning a function it should be resolved before being applied */
        first = eval(env, first);
        if(!first) {
            /* Should be unreachable code ... */
            ERROR(ERR_UNRESOLVABLE, "Could not resolve symbol");
            first = NIL; /* ENVIRONMENT_STRING_LOOKUP(env, "NIL"); */
            return expressionAssign(env, first);
        };
        switch(EXPRESSION_TYPE(first)) {
            case EXPR_NATIVE_FUNC:
                res = EXPRESSION_NATIVE_FUNC(first)(env, intCdr(env, expr));
                break;
            case EXPR_LAMBDA:
                res = lambdaInvoke(env, EXPRESSION_LAMBDA(first), intCdr(env, expr));
                break;
            default:
                ERROR(ERR_UNEXPECTED_VAL, "eval(): First element in list is not a function!");
                res = NIL; /* ENVIRONMENT_STRING_LOOKUP (env, "NIL"); */
        };
        expressionDispose(env, first);
        return res;
    }
}

