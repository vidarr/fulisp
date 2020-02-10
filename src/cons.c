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

#include "cons.h"

#define MODULE_NAME "cons.c"

#ifdef DEBUG_CONS
#include "debugging.h"
#else
#include "no_debugging.h"
#endif

struct Expression *cons(struct Expression *env, struct Expression *expr) {
    struct Expression *car, *cdr;
    INIT_NATIVE_FUNCTION("cons", env, expr);
    SECURE_CAR(env, expr, car, "First argument of CONS is flawed");
    SECURE_CDR(env, expr, cdr, "Second argument of CONS is flawed");
    SECURE_CAR(env, cdr, expr, "First argument of CONS is flawed");
    IF_SAFETY_CODE(
        struct Expression * cddr;
        SECURE_CDR(env, cdr, cddr, "Second argument of CONS is flawed");
        if (!EXPR_IS_NIL(cddr)) {
            ERROR(ERR_SYNTAX_ERROR, "CONS takes exactly 2 arguments");
            return NIL;
        });
    car = eval(env, car);
    expr = eval(env, expr);
    expressionAssign(env, car);
    expressionAssign(env, expr);
    return EXPRESSION_CREATE_CONS(env, car, expr);
}

struct Expression *intCar(struct Expression *env, struct Expression *args) {
    assert(env && args);
    if (EXPR_IS_NIL(args)) return NIL;
    if (EXPR_OF_TYPE(args, EXPR_CONS)) {
        return EXPRESSION_CAR(args);
    } else {
        ERROR(ERR_UNEXPECTED_VAL, "car: Expected Cons, got other");
        return NIL;
    };
}

struct Expression *car(struct Expression *env, struct Expression *expr) {
    struct Expression *car;
    INIT_NATIVE_FUNCTION("car", env, expr);
    if (EXPR_IS_NIL(expr)) return NIL;
    expr = intCar(env, expr);
    expr = eval(env, expr);
    if (EXPR_IS_NIL(expr)) return NIL;
    SECURE_CAR(env, expr, car, "Flawed argument list");
    expressionAssign(env, car);
    expressionDispose(env, expr);
    DEBUG_PRINT("Leaving car()\n");
    return car;
}

struct Expression *intCdr(struct Expression *env, struct Expression *args) {
    assert(env && args);
    if (EXPR_OF_TYPE(args, EXPR_CONS)) {
        return EXPRESSION_CDR(args);
    } else {
        ERROR(ERR_UNEXPECTED_VAL, "cdr: Expected Cons, got other");
        return NIL;
    };
}

struct Expression *cdr(struct Expression *env, struct Expression *expr) {
    struct Expression *cdr;

    INIT_NATIVE_FUNCTION("cdr", env, expr);

    if (EXPR_IS_NIL(expr)) return NIL;
    expr = intCar(env, expr);
    expr = eval(env, expr);
    if (EXPR_IS_NIL(expr)) return NIL;
    SECURE_CDR(env, expr, cdr, "Flawed argument list");
    expressionAssign(env, cdr);
    expressionDispose(env, expr);
    DEBUG_PRINT("Leaving cdr\n");
    return cdr;
}

struct Expression *setCar(struct Expression *env, struct Expression *expr) {
    struct Expression *consCell, *carExpr;

    INIT_NATIVE_FUNCTION("setCar", env, expr);

    ASSIGN_2_PARAMS(env, expr, consCell, carExpr,
                    "Argument list of CONS is flawed");

    DEBUG_PRINT("consCell is ");
    DEBUG_PRINT_EXPR(env, consCell);
    DEBUG_PRINT("\ncarExpr is ");
    DEBUG_PRINT_EXPR(env, carExpr);

    consCell = eval(env, consCell);
    if (!EXPR_OF_TYPE(consCell, EXPR_CONS)) {
        ERROR(ERR_UNEXPECTED_VAL, "setCar: Expected Cons, got other");
        expressionDispose(env, consCell);
        return NIL;
    }

    DEBUG_PRINT("\ncarExpr is ");
    DEBUG_PRINT_EXPR(env, carExpr);

    carExpr = eval(env, carExpr);
    if (EXPRESSION_CAR(consCell))
        expressionDispose(env, EXPRESSION_CAR(consCell));
    EXPRESSION_SET_CAR(consCell, carExpr);

    DEBUG_PRINT("\nconsCell is ");
    DEBUG_PRINT_EXPR(env, consCell);

    return consCell;
}

struct Expression *setCdr(struct Expression *env, struct Expression *expr) {
    struct Expression *consCell, *cdrExpr;

    INIT_NATIVE_FUNCTION("setCdr", env, expr);

    ASSIGN_2_PARAMS(env, expr, consCell, cdrExpr,
                    "Argument list of CONS is flawed");

    DEBUG_PRINT("consCell is ");
    DEBUG_PRINT_EXPR(env, consCell);
    DEBUG_PRINT("\ncdrExpr is ");
    DEBUG_PRINT_EXPR(env, cdrExpr);

    consCell = eval(env, consCell);
    if (!EXPR_OF_TYPE(consCell, EXPR_CONS)) {
        ERROR(ERR_UNEXPECTED_VAL, "setCdr: Expected Cons, got other");
        expressionDispose(env, consCell);
        return NIL;
    }

    DEBUG_PRINT("\ncdrExpr is ");
    DEBUG_PRINT_EXPR(env, cdrExpr);

    cdrExpr = eval(env, cdrExpr);
    if (EXPRESSION_CDR(consCell))
        expressionDispose(env, EXPRESSION_CDR(consCell));
    EXPRESSION_SET_CDR(consCell, cdrExpr);

    DEBUG_PRINT("\nconsCell is ");
    DEBUG_PRINT_EXPR(env, consCell);

    return consCell;
}
