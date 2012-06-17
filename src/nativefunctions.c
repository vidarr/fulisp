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
#    include "debugging.h"
#else
#    include "no_debugging.h"
#endif


/*****************************************************************************
 *                                  MACROS
 *****************************************************************************/


/**
 * Takes expr, checks wether its a list, then assigns car the car and cdr the
 * cdr of the list
 * @param env environment to use
 * @param expr list to split
 * @param car 
 * @param cdr
 */
#define SECURE_CAR(env, expr, car, cdr) { \
    if(!expr || !EXPR_OF_TYPE(expr, EXPR_CONS)) {  \
        ERROR(ERR_UNEXPECTED_TYPE, "Flawed argument list");  \
        return NULL;  \
    };  \
    car = intCar(env, expr);  \
    if(!car) {  \
        ERROR(ERR_UNEXPECTED_TYPE, "Flawed argument list");  \
        return NULL;  \
    };  \
    cdr = intCdr(env, expr);  \
    if(!cdr || !EXPR_OF_TYPE(cdr, EXPR_CONS)) {  \
        ERROR(ERR_UNEXPECTED_TYPE, "Flawed argument list");  \
        return NULL;  \
    };  \
}


/**
 * Checks wether expr is a list. Assigns sym the first and val the second
 * element of the list.
 * If SAFETY_CODE is set, then it is checked if expr is a list of exactly two
 * elements.
 */
#define ASSIGN_2_PARAMS(env, expr, sym, val) { \
    SECURE_CAR(env, expr, sym, val); \
    IF_SAFETY_CODE(  \
            if(!EXPR_IS_NIL(intCdr(env, val))) {  \
            ERROR(ERR_UNEXPECTED_VAL, "2  parameters expected"); \
            return NULL; \
            }); \
    val = intCar(env, val); \
}

/* #define ASSIGN_2_PARAMS(env, expr, sym, val) { \ */
/*     if(!expr || !EXPR_OF_TYPE(expr, EXPR_CONS)) { \ */
/*         ERROR(ERR_UNEXPECTED_TYPE, "Flawed argument list"); \ */
/*         return NULL; \ */
/*     }; \ */
/*     sym = intCar(env, expr); \ */
/*     if(!sym) { \ */
/*         ERROR(ERR_UNEXPECTED_TYPE, "Flawed argument list"); \ */
/*         return NULL; \ */
/*     }; \ */
/*     val = intCdr(env, expr); \ */
/*     if(!val || !EXPR_OF_TYPE(val, EXPR_CONS)) { \ */
/*         ERROR(ERR_UNEXPECTED_TYPE, "Flawed argument list"); \ */
/*         return NULL; \ */
/*     }; \ */
/*     IF_SAFETY_CODE(  \ */
/*             if(!EXPR_IS_NIL(intCdr(env, val))) {  \ */
/*             ERROR(ERR_UNEXPECTED_VAL, "2  parameters expected"); \ */
/*             return NULL; \ */
/*             }); \ */
/*     val = intCar(env, val); \ */
/* } */
/*  */

struct Expression *quote(struct Expression *env, struct Expression *expr) {
    return expressionAssign(env, expr);
}


struct Expression *set(struct Expression *env, struct Expression *expr) {
    struct Expression *sym, *val;
    assert(env && expr);

    DEBUG_PRINT("Entering set");

    ASSIGN_2_PARAMS(env, expr, sym, val);
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

    ASSIGN_2_PARAMS(env, expr, sym, val);
    if(!EXPR_OF_TYPE(sym, EXPR_SYMBOL)) { 
        ERROR(ERR_UNEXPECTED_TYPE, "set expects SYMBOL VALUE as argument"); 
        return NULL; 
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
            ASSIGN_2_PARAMS(env, iter, condition, body); \
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
 *                            COMPARISON OPERATORS
 *****************************************************************************/



struct Expression *numEqual(struct Expression *env, struct Expression *expr) {
    struct Expression *car = NULL;
    struct Expression *first = NULL;
    float res = 0;

    DEBUG_PRINT("Entering numEqual\n");

    SECURE_CAR(env, expr, first, expr);
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

