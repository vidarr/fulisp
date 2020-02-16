/*
 * (C) 2011 Michael J. Beer
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
            res = expr;
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
            res = first;
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
        }
    }
    environ = EXPRESSION_ENVIRONMENT(env);
    environ->current = res;
    return res;
}
