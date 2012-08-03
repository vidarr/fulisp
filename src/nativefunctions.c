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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,
 * USA.
 */


#include "nativefunctions.h"
#include "expression.h"
#include "error.h"
#include "safety.h"


#define MODULE_NAME "nativefunctions.c"

#ifdef DEBUG_NATIVE_FUNCTIONS
#    include "debugging.h"
#else
#    include "no_debugging.h"
#endif




/*****************************************************************************
 *                                  MACROS
 *****************************************************************************/



struct Expression *quote(struct Expression *env, struct Expression *expr) {
    assert(env && expr);
    DEBUG_PRINT_EXPR(env, expr);
    if(EXPR_IS_NIL(expr)) return NIL;
    return expressionAssign(env, intCar(env, expr));
}


struct Expression *set(struct Expression *env, struct Expression *expr) {
    struct Expression *sym, *val;
    assert(env && expr);

    DEBUG_PRINT("Entering set");

    ASSIGN_2_PARAMS(env, expr, sym, val, "2 arguments expected");
    if(!EXPR_OF_TYPE(sym, EXPR_SYMBOL)) { 
        ERROR(ERR_UNEXPECTED_TYPE, "set expects SYMBOL VALUE as argument"); 
        return NULL; 
    }; 
    val = eval(env, val);
    DEBUG_PRINT("Setting \n");
    DEBUG_PRINT_EXPR(env, sym);
    DEBUG_PRINT_EXPR(env, val);

    environmentUpdate(env, sym, val);
    return expressionAssign(env, val);
}


struct Expression *define(struct Expression *env, struct Expression *expr) {
    struct Expression *sym, *val;
    assert(env && expr);

    DEBUG_PRINT("Entering define");

    ASSIGN_2_PARAMS(env, expr, sym, val, "2 arguments expected");
    if(!EXPR_OF_TYPE(sym, EXPR_SYMBOL)) { 
        ERROR(ERR_UNEXPECTED_TYPE, "set expects SYMBOL VALUE as argument"); 
        return NIL; 
    }; 
    val = eval(env, val);

    DEBUG_PRINT("Setting \n");
    DEBUG_PRINT_EXPR(env, sym);
    DEBUG_PRINT_EXPR(env, val);

    ENVIRONMENT_ADD_SYMBOL(env, sym, val);
    return expressionAssign(env, val);
}


struct Expression *cond(struct Expression *env, struct Expression *expr) {
    struct Expression *iter, *condition, *body;

    assert(env && expr);

    ITERATE_LIST(env, expr, iter, { \
            ASSIGN_2_PARAMS(env, iter, condition, body, "2 arguments expected"); \
            condition = eval(env, condition); \
            IF_SAFETY_CODE(if(!condition) return NIL;) \
            if(!EXPR_IS_NIL(condition)) { \
                body = eval(env, body); \
                IF_SAFETY_CODE(if(!body) return NIL;); \
                return expressionAssign(env, body); \
            };
        };); 
    return NIL;
}



/*****************************************************************************
 *                           MATHEMATICAL FUNCTIONS
 *****************************************************************************/



struct Expression *add(struct Expression *env, struct Expression *expr) {
    struct Expression *car = 0;
    float res = 0;

    DEBUG_PRINT("Entering add\n");

    ITERATE_LIST(env, expr, car, { \
        /* TODO: Check for conversion errors */ \
        car = eval(env, car); \
        res += EXPR_TO_FLOAT(car);  \
        expressionDispose(env, car); \
        DEBUG_PRINT_PARAM("add():   IN while: res = %f\n", res); \
    });

    DEBUG_PRINT_PARAM("ADD: %f\n", res);

    return expressionCreate(env, EXPR_FLOAT, (void *)&res);
}


struct Expression *mul(struct Expression *env, struct Expression *expr) {
    struct Expression *car = NULL;
    float res = 1;

    DEBUG_PRINT("Entering mul\n");

    ITERATE_LIST(env, expr, car, { \
        /* TODO: Check for conversion errors */ \
        car = eval(env, car); \
        res *= EXPR_TO_FLOAT(car);  \
        expressionDispose(env, car); \
        DEBUG_PRINT_PARAM("mul():   IN while: res = %f\n", res); \
    });

    DEBUG_PRINT_PARAM("MUL: %f\n", res);

    return expressionCreate(env, EXPR_FLOAT, (void *)&res);
}



/*****************************************************************************
 *                              LOGIC FUNCTIONS 
 *****************************************************************************/



struct Expression *and(struct Expression *env, struct Expression *expr) {
    struct Expression *car;
    INIT_NATIVE_FUNCTION(env, expr);
    ITERATE_LIST(env, expr, car, { \
       car = eval(env, car); \
       if(EXPR_IS_NIL(car)) return NIL; \
    });
    return T;
}


struct Expression *or(struct Expression *env, struct Expression *expr) {
    struct Expression *car;
    INIT_NATIVE_FUNCTION(env, expr);
    ITERATE_LIST(env, expr, car, { \
       car = eval(env, car); \
       if(!EXPR_IS_NIL(car)) return T; \
    });
    return NIL;
}

struct Expression *not(struct Expression *env, struct Expression *expr) {
    struct Expression *car;
    INIT_NATIVE_FUNCTION(env, expr);
    if(EXPR_IS_NIL(expr)) return T;
    car = eval(env, intCar(env, expr));
    return EXPR_IS_NIL(car) ? T : NIL;
}



/*****************************************************************************
 *                            COMPARISON OPERATORS
 *****************************************************************************/



struct Expression *numEqual(struct Expression *env, struct Expression *expr) {
    struct Expression *car = NULL;
    struct Expression *first = NULL;
    float res = 0;

    DEBUG_PRINT("Entering numEqual\n");

    SECURE_CARCDR(env, expr, first, expr, "Flawed argument list");
    first = eval(env, first);
    if(first == NIL) {
        return first;
    }
    res = EXPR_TO_FLOAT(first);
    expressionDispose(env, first);
    if(!NO_ERROR) return NIL;
    ITERATE_LIST(env, expr, car, { \
        car = eval(env, car); \
        if((res != EXPR_TO_FLOAT(car)) || !NO_ERROR) { \
            expressionDispose(env, car); \
            return NIL;  \
        } \
        expressionDispose(env, car); \
        DEBUG_PRINT_PARAM("numEqual():   IN while: res = %f\n", res); \
    });

    DEBUG_PRINT_PARAM("numEqual: %f\n", res);

    return T;
}


struct Expression *numSmaller(struct Expression *env, struct Expression *expr) {
    struct Expression *car = NULL;
    struct Expression *first = NULL;
    float res = 0, nextRes = 0;

    DEBUG_PRINT("Entering numSmaller\n");

    SECURE_CARCDR(env, expr, first, expr, "Flawed argument list");
    first = eval(env, first);
    if(first == NIL) {
        return first;
    }
    res = EXPR_TO_FLOAT(first);
    expressionDispose(env, first);
    if(!NO_ERROR) return NIL;

    ITERATE_LIST(env, expr, car, { \
        car = eval(env, car); \
        nextRes = EXPR_TO_FLOAT(car); \
        expressionDispose(env, car); \
        if((res >= nextRes) || !NO_ERROR) { \
            return NIL;  \
        } \
        res = nextRes; \
        DEBUG_PRINT_PARAM("numSmaller:   IN while: res = %f\n", res); \
    });

    DEBUG_PRINT("numSmaller: returning T\n");

    return T;
}
