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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

/**
 * Accessing cons cells
 */

#ifndef __CONS_H__
#define __CONS_H__

#include "config.h"
#include "safety.h"

#include "stdlib.h"
#include <stdio.h>
#include <string.h>
#include "expression.h"
#include "error.h"
#include "lisp_internals.h"
#include "streams.h"
#include "nativefunctions.h"
#include "memory.h"



/*****************************************************************************
 *                                   MACROS
 *****************************************************************************/


/**
 * Iterate over list of cons cells and execute code on every single car (eqv. of
 * Common Lisp's "mapcar").
 * Iterates over the list, assigns first car to runVar, then executes "code",
 * then assigns next car to runVar and goes on like this until the end of the
 * list has been reached. "code" can use "runVar" to subsequently access every
 * car.
 * @param env current environment
 * @param list consed list 
 * @param runVar variable to use as running variable. Must be of type "struct Expression *"
 * @param code the code to map onto the list's entries
 */
#define ITERATE_LIST(env, list, runVar, code) { \
    struct Expression *next; \
    runVar = list; \
    while(runVar != NIL) { \
        if(!EXPR_OF_TYPE(runVar, EXPR_CONS)) { \
            ERROR(ERR_UNEXPECTED_TYPE, "list is not regular"); \
            return NIL; \
        } \
        next = intCdr(env, runVar); \
        runVar = intCar(env, runVar); \
        code; \
        runVar = next; \
    } \
}



/*****************************************************************************
 *                                   MACROS
 *****************************************************************************/


/**
 * Creates a cons cell 
 * @param expr list containing two elements: car and cdr of new cons cell
 * @return a new expression resembling the cons cell
 */
struct Expression *cons(struct Expression *env, struct Expression *expr);

/**
 * Returnes the CAR of a Cons cell or NIL if something goes wrong
 * Does not increase the ref counter!
 * @param args a cons cell
 * @return the CAR of args or NIL
 */
struct Expression *intCar(struct Expression *env, struct Expression *args);

/**
 * Returnes the CAR of a Cons cell or NIL if something goes wrong
 * @param args a cons cell
 * @return the CAR of args or NIL
 */
struct Expression *car(struct Expression *env, struct Expression *args); 

/**
 * Returnes the CDR of a Cons cell or NIL if something goes wrong
 * Does not increase the ref counter!
 * @param args a cons cell
 * @return the CDR of args or NIL
 */
struct Expression *intCdr(struct Expression *env, struct Expression *args);

/**
 * Returnes the CDR of a Cons cell or NIL if something goes wrong
 * @param args a cons cell
 * @return the CDR of args or NIL
 */
struct Expression *cdr(struct Expression *env, struct Expression *args);


/**
 * Takes two arguments. Second argument needs to be a list with two elements.
 * First element  needs to be a cons cell. Second one can
 * be of any kind. Tries to set the car of the first element to the second
 * argument. 
 * @param env Current environment
 * @param expr list with two elements. 
 */
struct Expression *setCar(struct Expression *env, struct Expression *expr);


/**
 * Takes two arguments. Second argument needs to be a list with two elements.
 * First element  needs to be a cons cell. Second one can
 * be of any kind. Tries to set the cdr of the first element to the second
 * argument. 
 * @param env Current environment
 * @param expr list with two elements. 
 */
struct Expression *setCdr(struct Expression *env, struct Expression *expr);


struct Cons *intCons(struct Expression *env, struct Expression *car, struct Expression *cdr);


#endif

