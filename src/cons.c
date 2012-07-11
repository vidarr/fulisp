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

#include "cons.h"


#define MODULE_NAME "cons.c"

#ifdef DEBUG_CONS
#    include "debugging.h"
#else
#    include "no_debugging.h"
#endif


/**
 * Creates the structure covering the two expr pointers 
 */
struct Cons *intCons(struct Expression *env, struct Expression *car, struct Expression *cdr) {
    struct Cons *cons;
    assert(env && car && cdr);
    cons = (struct Cons *)malloc(sizeof(struct Cons));
    cons->car = expressionAssign(env, car);
    cons->cdr = expressionAssign(env, cdr);
    return cons;
}


struct Expression *cons(struct Expression *env, struct Expression *expr) {
    struct Expression *car, *cdr;
    DEBUG_PRINT("Entering cons\n");
    INIT_NATIVE_FUNCTION(env, expr);
    SECURE_CAR(env, expr, car, "First argument of CONS is flawed");
    SECURE_CDR(env, expr, cdr, "Second argument of CONS is flawed");
    SECURE_CAR(env, cdr, expr, "First argument of CONS is flawed");
    IF_SAFETY_CODE( \
            struct Expression *cddr;
            SECURE_CDR(env, cdr, cddr, "Second argument of CONS is flawed"); \
            if(!EXPR_IS_NIL(cddr)) { \
               ERROR(ERR_SYNTAX_ERROR, "CONS takes exactly 2 arguments"); \
               return NIL; \
            } \
            );
    car = eval(env, car);
    expr = eval(env, expr);
    return expressionCreate(env, EXPR_CONS, intCons(env, car, expr));
}


struct Expression *intCar(struct Expression *env, struct Expression *args) {
    assert(env && args);
    if(EXPR_IS_NIL(args)) return NIL;
    if(EXPR_OF_TYPE(args, EXPR_CONS)) {
        return EXPRESSION_CAR(args);
    } else {
        ERROR(ERR_UNEXPECTED_VAL, "car: Expected Cons, got other");
        return NIL;
    };
}


struct Expression *car(struct Expression *env, struct Expression *expr) { 
    struct Expression *car; 
    DEBUG_PRINT("Entering car()\n");
    assert(env && expr); 
    if(EXPR_IS_NIL(expr)) return NIL;
    expr = intCar(env, expr);
    expr = eval(env, expr);
    if(EXPR_IS_NIL(expr)) return NIL;
    SECURE_CAR(env, expr, car, "Flawed argument list");  
    expressionAssign(env, car); 
    expressionDispose(env, expr); 
    DEBUG_PRINT("Leaving car()\n");
    return car; 
} 


struct Expression *intCdr(struct Expression *env, struct Expression *args) {
    assert(env && args);
    if(EXPR_OF_TYPE(args, EXPR_CONS)) {
        return EXPRESSION_CDR(args);
    } else {
        ERROR(ERR_UNEXPECTED_VAL, "cdr: Expected Cons, got other");
        return NIL;
    };
}


struct Expression *cdr(struct Expression *env, struct Expression *expr) { 
    struct Expression *cdr; 
    DEBUG_PRINT("Entering cdr\n");
    assert(env && expr); 
    if(EXPR_IS_NIL(expr)) return NIL;
    expr = intCar(env, expr);
    expr = eval(env, expr);  
    if(EXPR_IS_NIL(expr)) return NIL;
    SECURE_CDR(env, expr, cdr, "Flawed argument list"); 
    expressionAssign(env, cdr);
    expressionDispose(env, expr); 
    DEBUG_PRINT("Leaving cdr\n");
    return cdr;
} 


struct Expression *consP(struct Expression *env, struct Expression *expr) {
    DEBUG_PRINT("Entering cdr\n");
    INIT_NATIVE_FUNCTION(env, expr);
    expr = intCar(env, expr);
    expr = eval(env, expr);
    if(EXPR_IS_NIL(expr)) return T;
    DEBUG_PRINT("Leaving cdr\n");
    return EXPR_IS_CONS(expr) ? T : NIL;
}


struct Expression *setCar(struct Expression *env, struct Expression *consCell, struct Expression *car) {
    assert(env && consCell && car);
    if(!EXPR_OF_TYPE(consCell, EXPR_CONS)) {
        /* if(EXPR_IS_NIL(consCell)) {
            consCell = cons(env, expressionAssign(env, car), NIL);
            return consCell;
        } else { */
            ERROR(ERR_UNEXPECTED_VAL, "setCar: Expected Cons, got other");
            return NIL;
        /* } */
    }
    if(EXPRESSION_CAR(consCell)) expressionDispose(env, EXPRESSION_CAR(consCell));
    consCell->data.cons->car = expressionAssign(env, car);
    return consCell;
}


struct Expression *setCdr(struct Expression * env, struct Expression *consCell, struct Expression *cdr) {
    assert(consCell);
    if(!EXPR_OF_TYPE(consCell, EXPR_CONS)) {
        /* if(EXPR_IS_NIL(consCell)) {
            consCell = cons(env, NIL, expressionAssign(env, cdr));
            return consCell;
        } else { */
            ERROR(ERR_UNEXPECTED_VAL, "setCdr: Expected Cons, got other");
            return NIL;
        /* } */
    }
    if(EXPRESSION_CDR(consCell)) expressionDispose(env, EXPRESSION_CDR(consCell));
    consCell->data.cons->cdr = expressionAssign(env, cdr);
    return consCell;
}


