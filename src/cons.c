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
#include "debugging.h"
#else
#include "no_debugging.h"
#endif


/**
 * Creates the structure covering the two expr pointers */
struct Cons *intCons(struct Environment *env, struct Expression *car, struct Expression *cdr) {
    struct Cons *cons = (struct Cons *)malloc(sizeof(struct Cons));
    cons->car = expressionAssign(env, car);
    cons->cdr = expressionAssign(env, cdr);
    return cons;
}


struct Expression *cons(struct Environment *env, struct Expression *car, struct Expression *cdr) {
    return expressionCreate(env, EXPR_CONS, intCons(env, car, cdr));
}


struct Expression *intCar(struct Environment *env, struct Expression *args) {
    if(EXPR_OF_TYPE(args, EXPR_CONS)) {
        return EXPRESSION_CAR(args);
    } else {
        ERROR(ERR_UNEXPECTED_VAL, "car: Expected Cons, got other");
        return CREATE_SYMBOL(env, "NIL");
    };
}


struct Expression *car(struct Environment *env, struct Expression *args) {
  return expressionAssign(env, intCar(env, args));
}


struct Expression *intCdr(struct Environment *env, struct Expression *args) {
    if(EXPR_OF_TYPE(args, EXPR_CONS)) {
        return EXPRESSION_CDR(args);
    } else {
        ERROR(ERR_UNEXPECTED_VAL, "cdr: Expected Cons, got other");
        return CREATE_SYMBOL(env, "NIL");
    };
}

struct Expression *cdr(struct Environment *env, struct Expression *args) {
  return expressionAssign(env, intCdr(env, args));
}


void setCar(struct Environment *env, struct Expression *cons, struct Expression *car) {
    assert(cons);
    if(!EXPR_OF_TYPE(cons, EXPR_CONS)) {
        ERROR(ERR_UNEXPECTED_VAL, "setCar: Expected Cons, got other");
        return;
    }
    if(EXPRESSION_CAR(cons)) expressionDispose(env, EXPRESSION_CAR(cons));
    cons->data.cons->car = expressionAssign(env, car);
}


void setCdr(struct Environment * env, struct Expression *cons, struct Expression *cdr) {
    assert(cons);
    if(!EXPR_OF_TYPE(cons, EXPR_CONS)) {
        ERROR(ERR_UNEXPECTED_VAL, "setCdr: Expected Cons, got other");
        return;
    }
    if(EXPRESSION_CDR(cons)) expressionDispose(env, EXPRESSION_CDR(cons));
    cons->data.cons->cdr = expressionAssign(env, cdr);
}


