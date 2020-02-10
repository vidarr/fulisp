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

/**
 * Accessing cons cells
 */

#ifndef __CONS_H__
#define __CONS_H__

#include "config.h"
#include "safety.h"

#include <stdio.h>
#include <string.h>
#include "error.h"
#include "expression.h"
#include "lisp_internals.h"
#include "memory.h"
#include "nativefunctions.h"
#include "stdlib.h"
#include "streams.h"

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
 * @param runVar variable to use as iterator. Must be type "struct Expression *"
 * @param code the code to map onto the list's entries
 */
#define ITERATE_LIST(env, list, runVar, code)                      \
    {                                                              \
        struct Expression *next;                                   \
        runVar = list;                                             \
        while (runVar != NIL) {                                    \
            if (!EXPR_OF_TYPE(runVar, EXPR_CONS)) {                \
                ERROR(ERR_UNEXPECTED_TYPE, "list is not regular"); \
                return NIL;                                        \
            }                                                      \
            next = intCdr(env, runVar);                            \
            runVar = intCar(env, runVar);                          \
            code;                                                  \
            runVar = next;                                         \
        }                                                          \
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

struct Cons *intCons(struct Expression *env, struct Expression *car,
                     struct Expression *cdr);

#endif
