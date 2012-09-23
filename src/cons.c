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
    expressionAssign(env, car);
    expressionAssign(env, expr);
    return EXPRESSION_CREATE_CONS(env, car, expr);
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


struct Expression *setCar(struct Expression *env, struct Expression *expr) {
    struct Expression *consCell, *carExpr;

    DEBUG_PRINT("Entering setCar\n");

    INIT_NATIVE_FUNCTION(env, expr);
    ASSIGN_2_PARAMS(env, expr, consCell, carExpr, "Argument list of CONS is flawed");

    DEBUG_PRINT("consCell is ");
    DEBUG_PRINT_EXPR(env, consCell);
    DEBUG_PRINT("\ncarExpr is ");
    DEBUG_PRINT_EXPR(env, carExpr);

    consCell = eval(env, consCell);
    if(!EXPR_OF_TYPE(consCell, EXPR_CONS)) {
            ERROR(ERR_UNEXPECTED_VAL, "setCar: Expected Cons, got other");
            expressionDispose(env, consCell);
            return NIL;
    }

    DEBUG_PRINT("\ncarExpr is ");
    DEBUG_PRINT_EXPR(env, carExpr);

    carExpr = eval(env, carExpr);
    if(EXPRESSION_CAR(consCell)) expressionDispose(env, EXPRESSION_CAR(consCell));
    EXPRESSION_SET_CAR(consCell, carExpr);

    DEBUG_PRINT("\nconsCell is ");
    DEBUG_PRINT_EXPR(env, consCell);

    return consCell;
}


struct Expression *setCdr(struct Expression * env, struct Expression *expr) {
    struct Expression *consCell, *cdrExpr;

    DEBUG_PRINT("Entering setCar\n");

    INIT_NATIVE_FUNCTION(env, expr);
    ASSIGN_2_PARAMS(env, expr, consCell, cdrExpr, "Argument list of CONS is flawed");

    DEBUG_PRINT("consCell is ");
    DEBUG_PRINT_EXPR(env, consCell);
    DEBUG_PRINT("\ncdrExpr is ");
    DEBUG_PRINT_EXPR(env, cdrExpr);

    consCell = eval(env, consCell);
    if(!EXPR_OF_TYPE(consCell, EXPR_CONS)) {
            ERROR(ERR_UNEXPECTED_VAL, "setCar: Expected Cons, got other");
            expressionDispose(env, consCell);
            return NIL;
    }

    DEBUG_PRINT("\ncdrExpr is ");
    DEBUG_PRINT_EXPR(env, cdrExpr);

    cdrExpr = eval(env, cdrExpr);
    if(EXPRESSION_CDR(consCell)) expressionDispose(env, EXPRESSION_CDR(consCell));
    EXPRESSION_SET_CDR(consCell, cdrExpr);

    DEBUG_PRINT("\nconsCell is ");
    DEBUG_PRINT_EXPR(env, consCell);

    return consCell;
}

