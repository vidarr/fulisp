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

#ifndef __NATIVEFUNCTIONS_H__
#define __NATIVEFUNCTIONS_H__

#include "environment.h"
#include "eval.h"
#include "lambda.h"

/*****************************************************************************
 *                                  MACROS
 *****************************************************************************/

/**
 * Performs basic checks on the argument list
 */
#define INIT_NATIVE_FUNCTION(funName, env, expr)                              \
    {                                                                         \
        DEBUG_PRINT_PARAM("Entering %s\n", funName);                          \
        assert(env &&expr);                                                   \
        IF_SAFETY_CODE({                                                      \
            if (!EXPR_IS_NIL(expr) && !EXPR_IS_CONS(expr)) {                  \
                ERROR(ERR_UNEXPECTED_VAL,                                     \
                      "Severe internal error: Argument list is not a list!"); \
                return NIL;                                                   \
            };                                                                \
        });                                                                   \
    }

/**
 * Takes expr, checks wether its a list, then assigns car the car  of the list
 * @param env environment to use
 * @param expr list to split
 * @param car
 * @param emsg Error string if operation fails
 */
#define SECURE_CAR(env, expr, car, emsg)               \
    {                                                  \
        if (!expr || !EXPR_OF_TYPE(expr, EXPR_CONS)) { \
            ERROR(ERR_UNEXPECTED_TYPE, emsg);          \
            return NIL;                                \
        };                                             \
        car = intCar(env, expr);                       \
        if (!car) {                                    \
            ERROR(ERR_UNEXPECTED_TYPE, emsg);          \
            return NIL;                                \
        };                                             \
    }

/**
 * Takes expr, checks wether its a list, then assigns cdr the cdr of the list
 * @param env environment to use
 * @param expr list to split
 * @param cdr
 * @param emsg Error string if operation fails
 */
#define SECURE_CDR(env, expr, cdr, emsg)               \
    {                                                  \
        if (!expr || !EXPR_OF_TYPE(expr, EXPR_CONS)) { \
            ERROR(ERR_UNEXPECTED_TYPE, emsg);          \
            return NIL;                                \
        };                                             \
        cdr = intCdr(env, expr);                       \
        if (!cdr) {                                    \
            ERROR(ERR_UNEXPECTED_TYPE, emsg);          \
            return NIL;                                \
        };                                             \
    }

/**
 * Takes expr, checks wether its a list, then assigns car the car and cdr the
 * cdr of the list
 * @param env environment to use
 * @param expr list to split
 * @param car
 * @param cdr
 * @param emsg Error string if operation fails
 */
#define SECURE_CARCDR(env, expr, car, cdr, emsg)     \
    {                                                \
        SECURE_CAR(env, expr, car, emsg);            \
        cdr = intCdr(env, expr);                     \
        if (!cdr || !EXPR_OF_TYPE(cdr, EXPR_CONS)) { \
            ERROR(ERR_UNEXPECTED_TYPE, emsg);        \
            return NIL;                              \
        };                                           \
    }

/**
 * Checks wether expr is a list. Assigns sym the first and val the second
 * element of the list.
 * If SAFETY_CODE is set, then it is checked if expr is a list of exactly two
 * elements.
 * @param sym struct Expression which will be assigned the first expression
 * @param val struct Expression which will be assigned the second expression
 * @param emsg Error string if operation fails
 */
#define ASSIGN_2_PARAMS(env, expr, sym, val, emsg)           \
    {                                                        \
        SECURE_CARCDR(env, expr, sym, val, emsg);            \
        IF_SAFETY_CODE(if (!EXPR_IS_NIL(intCdr(env, val))) { \
            ERROR(ERR_UNEXPECTED_VAL, emsg);                 \
            return NULL;                                     \
        });                                                  \
        val = intCar(env, val);                              \
    }

/**
 * Checks wether expr is a list. Assigns sym the first and val the second
 * element of the list.
 * If SAFETY_CODE is set, then it is checked if expr is a list of exactly two
 * elements.
 * @param sym struct Expression which will be assigned the first expression
 * @param val struct Expression which will be assigned the second expression
 * @param rest struct Expression which will be assigned the rest of the list
 * @param emsg Error string if operation fails
 */
#define ASSIGN_2_PARAMS_WITH_REST(env, expr, sym, val, rest, emsg) \
    {                                                              \
        SECURE_CARCDR(env, expr, sym, rest, emsg);                 \
        IF_SAFETY_CODE(if (EXPR_IS_NIL(rest)) {                    \
            ERROR(ERR_UNEXPECTED_VAL, emsg);                       \
            return NULL;                                           \
        });                                                        \
        val = intCar(env, rest);                                   \
        rest = intCdr(env, rest);                                  \
    }

/******************************************************************************
                                   FUNCTIONS
 ******************************************************************************/

/**
 * Identity
 * Returnes the expression given to this functions without alteration
 * @parame env the environment to be used
 * @param expr
 * @returns expr
 */
struct Expression *quote(struct Expression *env, struct Expression *expr);

/**
 * Prints out a String representation of the expression given as parameter
 * @param env the environment to be used
 * @param expr an expression to print
 * @returns T
 */
struct Expression *print(struct Expression *env, struct Expression *expr);

/**
 * Bind symbol to a value. Currently, if there is no binding for this symbol
 * yet, a new binding is created, thus explicit call to "define" or something is
 * unneccessary.
 * @param env the environment to be used
 * @param expr a cons list with two entries: first one should be the symbol to
 * bind, second the value to be bound to the symbol.
 * @returns new value bound to the symbol
 */
struct Expression *set(struct Expression *env, struct Expression *expr);

/**
 * Bind a symbol to a value within current environment, if it exists already,
 * create new binding.
 * @param  env environment create binding within
 * @param  expr list of two elements. Car is the symbol to bind to, cadr is the
 *              value to be bound.
 * @return new value
 */
struct Expression *define(struct Expression *env, struct Expression *expr);

/**
 * Takes lists of two elements. Iterates over the lists in the order given. If
 * one list's car evaluates to something not equal to NIL, the cadr is evaluated
 * and returned.
 * @param env Current environment
 * @param expr list of lists of two elements.
 */
struct Expression *cond(struct Expression *env, struct Expression *expr);

struct Expression *begin(struct Expression *env, struct Expression *expr);

/**
 * Enforces run of garbage collector
 */
struct Expression *gcRun(struct Expression *env, struct Expression *expr);

/*****************************************************************************
 *                           MATHEMATICAL FUNCTIONS
 *****************************************************************************/

/**
 * Returns the sum of the arguments given
 * @param env the environment to be used
 * @param a list of numbers
 * @returnes sum of the arguments
 */
struct Expression *add(struct Expression *env, struct Expression *expr);

/**
 * Returnes the product of the arguments given
 * @param env environment to be used
 * @param expr list of numbers to be multiplied together
 * @returns product of the arguments
 */
struct Expression *mul(struct Expression *env, struct Expression *expr);

/**
 * Returnes the result of dividing the car by all the other numbers given as
 * arguments.
 * @param env environment to be used
 * @param expr list of numbers
 * @returns result of division
 */
struct Expression *divide(struct Expression *env, struct Expression *expr);

/*****************************************************************************
 *                              LOGIC FUNCTIONS
 *****************************************************************************/

/**
 * Returnes true if one of the parameters evaluates to !NIL.
 * Does not guarantee to evaluate all of the parameters!
 * @param env Current environment
 * @param list of expressions that are evaluated
 * @return T if one of the parameters is not NIL. NIL else.
 */
struct Expression * or (struct Expression * env, struct Expression *expr);

/**
 * Returnes true if all the parameters evaluates to not NIL.
 * Does not guarantee to evaluate all of the parameters!
 * @param env Current environment
 * @param list of expressions that are evaluated
 * @return T if one of the parameters is not NIL. NIL else.
 */
struct Expression * and (struct Expression * env, struct Expression *expr);

/**
 * Returnes T only if sole argument evaluates not to NIL;
 * @param env Current environment
 * @param expr argument list
 * @return T exactly if sole argument evaluates not to NIL
 */
struct Expression * not(struct Expression * env, struct Expression *expr);

/*****************************************************************************
 *                            COMPARISON OPERATORS
 *****************************************************************************/

/**
 * Returns T if and only if all numbers given to this function are equal.
 * @param env the environment to be used
 * @param expr list of numbers
 * @returns T if all given arguments are numbers and equal, NIL otherwise
 */
struct Expression *numEqual(struct Expression *env, struct Expression *expr);

/**
 * Returns T if and only if all numbers given to this function are ordered in a
 * strictly descending manner, i.e. if for (< a b c ...) holds a < b < c < ...
 * @pararm env environment to be used
 * @param expr list of numbers
 * @return T if if numbers are ordered strictly ascending
 */
struct Expression *numSmaller(struct Expression *env, struct Expression *expr);

/*****************************************************************************
 *                                 PREDICATS
 *****************************************************************************/

/**
 * @return T if and only if ALL of the arguments are NIL
 */
struct Expression *nilP(struct Expression *env, struct Expression *expr);

/**
 * Checks whether all arguments are of type integer. If none are given, T is
 * returned as well.
 * @param env The current environment
 * @param expr list of elements to check
 */
struct Expression *integerP(struct Expression *env, struct Expression *expr);

/**
 * Checks whether all arguments are of type float. If none are given, T is
 * returned as well.
 * @param env The current environment
 * @param expr list of elements to check
 */
struct Expression *floatP(struct Expression *env, struct Expression *expr);

/**
 * Checks whether all arguments are of type character. If none are given, T is
 * returned as well.
 * @param env The current environment
 * @param expr list of elements to check
 */
struct Expression *characterP(struct Expression *env, struct Expression *expr);

/**
 * Checks whether all arguments are of type nativeFunction. If none are given, T
 * is returned as well.
 * @param env The current environment
 * @param expr list of elements to check
 */
struct Expression *nativeFunctionP(struct Expression *env,
                                   struct Expression *expr);

/**
 * Checks whether all arguments are of type string. If none are given, T is
 * returned as well.
 * @param env The current environment
 * @param expr list of elements to check
 */
struct Expression *stringP(struct Expression *env, struct Expression *expr);

/**
 * Checks whether all arguments are of type symbol. If none are given, T is
 * returned as well.
 * @param env The current environment
 * @param expr list of elements to check
 */
struct Expression *symbolP(struct Expression *env, struct Expression *expr);

/**
 * Checks whether all arguments are of type symbol. If none are given, T is
 * returned as well.
 * @param env The current environment
 * @param expr list of elements to check
 */
struct Expression *consP(struct Expression *env, struct Expression *expr);

/**
 * Checks whether all arguments are of type lambda. If none are given, T is
 * returned as well.
 * @param env The current environment
 * @param expr list of elements to check
 */
struct Expression *lambdaP(struct Expression *env, struct Expression *expr);

/**
 * Checks whether all arguments are of type environment. If none are given, T is
 * returned as well.
 * @param env The current environment
 * @param expr list of elements to check
 */
struct Expression *environmentP(struct Expression *env,
                                struct Expression *expr);

/**
 * Returns the current environment
 * @param env the current environment
 */
struct Expression *getEnv(struct Expression *env, struct Expression *expr);

/**
 * Returns the parent environment of expr .
 * @param env the current environment
 * @param expr an environment
 */
struct Expression *getParentEnv(struct Expression *env,
                                struct Expression *expr);
#endif
