/**
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,
 * USA.
 */


#include "nativefunctions.h"
#include "expression.h"
#include "error.h"
#include "safety.h"


#define MODULE_NAME "nativefunctions.c"

#ifdef DEBUG_NATIVE_FUNCTIONS
#include "debugging.h"
#else
#include "no_debugging.h"
#endif



struct Expression *quote(struct Expression *env, struct Expression *expr) {
    return expressionAssign(env, expr);
}


struct Expression *add(struct Expression *env, struct Expression *expr) {
    struct Expression *car = 0;
    float res = 0;
/*    IF_SAFETY_CODE(float buf;); */

    DEBUG_PRINT("Entering add\n");


    while(EXPR_IS_CONS(expr)) {
        /* TODO: Check for conversion errors */
        car = intCar(env, expr);
        car = eval(env, car);
        res += EXPR_TO_FLOAT(car); 
        expressionDispose(env, car);
        DEBUG_PRINT_PARAM("add():   IN while: res = %f\n", res);
        expr = intCdr(env, expr);
    };

    /* expr now should be NIL */
    /* expr = eval(env, expr);  
       res += EXPR_TO_FLOAT(expr);  
       expressionDispose(env, expr);  */

    DEBUG_PRINT_PARAM("ADD: %f\n", res);

    return expressionCreate(env, EXPR_FLOAT, (void *)&res);
}


struct Expression *set(struct Expression *env, struct Expression *expr) {
    struct Expression *sym, *val;
    assert(env && expr);

    DEBUG_PRINT("Entering set");

    if(!expr || !EXPR_OF_TYPE(expr, EXPR_CONS)) {
        ERROR(ERR_UNEXPECTED_TYPE, "set expects SYMBOL VALUE as argument");
        return NIL;
    };
    sym = intCar(env, expr);
    if(!sym || !EXPR_OF_TYPE(sym, EXPR_SYMBOL)) {
        ERROR(ERR_UNEXPECTED_TYPE, "set expects SYMBOL VALUE as argument");
        return NIL;
    };
    val = intCdr(env, expr);
    if(!val || !EXPR_OF_TYPE(val, EXPR_CONS)) {
        ERROR(ERR_UNEXPECTED_TYPE, "set expects SYMBOL VALUE as argument");
        return NIL;
    };
    IF_SAFETY_CODE( \
            if(!EXPR_IS_NIL(intCdr(env, val))) { \
            ERROR(ERR_UNEXPECTED_VAL, "set expects exactly 2" \
                " arguments"); \
            return NIL; \
            });
    val = intCar(env, val);
    val = eval(env, val);
/*    printf("Got sym: %s\n", EXPRESSION_STRING(sym));
    printf("Got val: %s\n", EXPRESSION_STRING(fuPrint(env, val))); */
    DEBUG_PRINT("Setting \n");
    DEBUG_PRINT_EXPR(env, sym);
    DEBUG_PRINT_EXPR(env, val);

    ENVIRONMENT_ADD_SYMBOL(env, sym, val);
    return expressionAssign(env, val);
}

