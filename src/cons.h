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

#include "stdlib.h"
#include <stdio.h>
#include <string.h>
#include "expression.h"
#include "error.h"
#include "lisp_internals.h"
#include "streams.h"

/**
 * Creates a cons cell 
 * @param car the car to asign to the cell
 * @param cdr the car to asign to the cell
 * @return a new expression resembling the cons cell
 */
struct Expression *cons(struct Expression *env, struct Expression *car, struct Expression *cons);

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


void setCar(struct Expression *env, struct Expression *cons, struct Expression *car);


void setCdr(struct Expression *env, struct Expression *cons, struct Expression *cdr);


struct Cons *intCons(struct Expression *env, struct Expression *car, struct Expression *cdr);


#endif

