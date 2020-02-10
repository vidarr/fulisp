/*
 * (C) 2010, 2012 Michael J. Beer
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
 * Provides functions for dealing with expressions.
 * This peovides basic functions for
 * Creation
 * Disposal
 * Access
 */
#ifndef __EXPRESSION_H__
#define __EXPRESSION_H__

#include <assert.h>
#include "safety.h"

/*******************************************************************************
  E X R E S S I O N   S T R U C T U R E
 *******************************************************************************/

/**
 * Represents a lisp expression.
 * Actual implementation is provided within included header, see the very bottom
 * of this file.
 */
struct Expression;

typedef struct Expression *(NativeFunction)(struct Expression *env,
                                            struct Expression *);

#include "environment.h"
#include "garbage_collector.h"
#include "lambda.h"
#include "lisp_internals.h"

/*******************************************************************************
                              CONSTANT EXPRESSIONS
 *******************************************************************************/

/**
 * The NIL atom
 */
#define NIL __NIL

/**
 * The T atom
 */
#define T __T

/**
 * The symbol that denotes the variable to store abundand arguments of lambda
 * calls
 */
#define REST __REST

/**
 * Symbol that denotes the type 'INTEGER'
 */
#define TYPE_INTEGER __TYPE_INTEGER

/**
 * Symbol that denotes the type 'FLOAT'
 */
#define TYPE_FLOAT __TYPE_FLOAT

/**
 * Symbol that denotes the type 'CHAR'
 */
#define TYPE_CHARACTER __TYPE_CHARACTER

/**
 * Symbol that denotes the type 'STRING'
 */
#define TYPE_STRING __TYPE_STRING

/**
 * Symbol that denotes the type 'SYMBOL'
 */
#define TYPE_SYMBOL __TYPE_SYMBOL

/**
 * Symbol that denotes the type 'CONS'
 */
#define TYPE_CONS __TYPE_CONS

/**
 * Symbol that denotes the type 'LAMBDA'
 */
#define TYPE_LAMBDA __TYPE_LAMBDA

/**
 * Symbol that denotes the type 'NATIVE FUNCTION'
 */
#define TYPE_NATIVE_FUNC __TYPE_NATIVE_FUNC

/**
 * Symbol that denotes the type 'ENVIRONMENT'
 */
#define TYPE_ENVIRONMENT __TYPE_ENVIRONMENT

/*******************************************************************************
  D E A L I N G   W I T H   E X R P R E S S I O N   T Y P E S
 *******************************************************************************/

/**
 * Gives the type of the expression
 */
#define EXPRESSION_TYPE(x) __EXPRESSION_TYPE(x)

/**
 * Checks whether a given expression is of a certain kind
 * This macro takes any kind of the following constants, so
 * one can check whether the expression contains a pointer by providing
 * EXPR_POINTER, one can check whether the expression is particular pointer type
 * like a string by providing EXPR_STRING e.g.
 * Attention: This macro cannot be used to detect NIL as this could be
 * represented by 0x0 pointer.
 * @param x a pointer to an expression
 * @param y unsigned integer indicating the type
 */
#define EXPR_OF_TYPE(x, y) __EXPR_OF_TYPE(x, y)

/**
 * Checks whether x is a pointer type
 */
#define EXPR_IS_POINTER(x) __EXPR_IS_POINTER(x)

/**
 * Checks whether x is a Cons cell
 */
#define EXPR_IS_CONS(x) EXPR_OF_TYPE(x, EXPR_CONS)

/**
 * Checks whether x is NIL
 */
#define EXPR_IS_NIL(x) __EXPR_IS_NIL(x)

/**
 * Checks whether x is a valid (initialized) expression
 */
#define EXPR_IS_VALID(x) (x && !EXPR_OF_TYPE(x, EXPR_NO_TYPE))

/**
 * Sets the type of the expression
 * @param expr pointer to expression
 * @param type the new type
 */
#define EXPR_SET_TYPE(expr, type) __EXPR_SET_TYPE(expr, type)

/*
   Internally, every expression keeps a type - field
   This field is built up this way:
   Bits above 6 reserved (e.g. for Garbage collector)
   Bit 6 indicates a pointer to be freed
   Bit 5 indicates no pointer to be freed (an "atomic type")
   */

/**
 * Defines the bytes to be used for determining the type
 */
#define EXPR_TYPE_AREAL \
    ((1 << 6) | (1 << 5) | (1 << 4) | (1 << 3) | (1 << 2) | 2 | 1)


/**
 * Expression is a constant, shall not be freed
 */

#define EXPR_DONT_FREE (1 << 8)
/**
 * The bit used for the GC to mark expressions
 */
#define EXPR_GC_MARKER_BIT (1 << 7)

/**
 * This represents just an uninitialised expression
 */
#define EXPR_NO_TYPE 0

/**
 * Expression contains a pointer (to be freed)
 */
#define EXPR_POINTER (1 << 6)

/**
 * Expression contains a an atomar value  (not to be freed)
 */
#define EXPR_ATOM (1 << 5)

/**
 * Expression is an integer
 */
#define EXPR_INTEGER (EXPR_ATOM | 1)

/**
 * Expression is a float
 */
#define EXPR_FLOAT (EXPR_ATOM | 2)

/**
 * Expression is an character
 */
#define EXPR_CHARACTER (EXPR_ATOM | 3)

/**
 * Expression is an string
 */
#define EXPR_STRING (EXPR_POINTER | 1)

/**
 * Expression is an cons cell
 */
#define EXPR_CONS (EXPR_POINTER | 3)

/**
 * Expression is a lambda expression
 */
#define EXPR_LAMBDA (EXPR_POINTER | 5)

/**
 * Expression is asymbol
 */
#define EXPR_SYMBOL (EXPR_POINTER | 2)

/**
 * Expression is an native function
 */
#define EXPR_NATIVE_FUNC (EXPR_ATOM | 4)

/**
 * Expression is an hash table
 */
#define EXPR_ENVIRONMENT (EXPR_POINTER | 6)

/******************************************************************************
                     C R E A T I O N   &   D I S P O S A L
 ******************************************************************************/

/**
 * disposes an expression
 * Takes care of the reference count
 * @param env the environment currently valid
 * @param expr the expression to dispose
 */
void expressionDispose(struct Expression *env, struct Expression *expr);

/**
 * Dispose an expression, bypass any kind of garbage collector.
 * @param env the environment currently valid
 * @param expr the expression to dispose
 */
void expressionForceDispose(struct Expression *env, struct Expression *expr);

/**
 * Creates an expression. To create an expression containing a native function,
 * usage of {@code expressionCreateNativeFunc} is recommended.
 * @param type type of the content
 * @param content pointer to the content that should go into the new expression
 * @param extension in case of cons cell, here goes the pointer to the cdr
 * @return the new expression
 *         or null in case of an error
 */
struct Expression *expressionCreate(struct Expression *env, unsigned char type,
                                    void *content, void *extension);

/**
 * Creates an expression
 * @param type type of the content
 * @param content
 * @return the new expression
 *         or null in case of an error
 */
struct Expression *expressionCreateNativeFunc(struct Expression *env,
                                              NativeFunction *content);

/**
 * Create an Expression struct around an environment struct.
 * Needed to create root environment
 */
struct Expression *createEnvironmentExpression(struct Environment *env);

/**
 * Create an expression that keeps an atom
 * @param env environment to use
 * @return a new expression being an atom
 */
#define EXPRESSION_CREATE_ATOM(env, type, content) \
    expressionCreate(env, type, content, NULL)

/**
 * Create an expression that keeps a cons cell
 * @param env environment to use
 * @return a new expression being a cons cell
 */
#define EXPRESSION_CREATE_CONS(env, car, cdr) \
    expressionCreate(env, EXPR_CONS, car, cdr)

/**
 * Creates an invalid expression
 */
#define CREATE_INVALID_EXPRESSION(env) \
    EXPRESSION_CREATE_ATOM(env, EXPR_NO_TYPE, 0)

/**
 * Creates a Symbol out of the string str
 */
#define CREATE_SYMBOL(env, str) createSymbol(env, str)

/**
 * Creates a float expression
 */
#define CREATE_FLOAT_EXPRESSION(env, x) \
    EXPRESSION_CREATE_ATOM(env, EXPR_FLOAT, (void *)&x)

/**
 * Creates a string expression
 */
#define CREATE_STRING_EXPRESSION(env, x) expressionCreateString(env, x)

/**
 * Creates a int expression
 */
#define CREATE_INT_EXPRESSION(env, x) \
    EXPRESSION_CREATE_ATOM(env, EXPR_INTEGER, (void *)&x)

/**
 * Creates a char expression
 */
#define CREATE_CHAR_EXPRESSION(env, x) \
    EXPRESSION_CREATE_ATOM(env, EXPR_CHARACTER, (void *)&x)

/******************************************************************************
                                  A C C E S S
 ******************************************************************************/

/**
 * Get integer value of expression
 * @param expr pointer to an expression
 */
#define EXPRESSION_INTEGER(expr) __EXPRESSION_INTEGER(expr)

/**
 * Get floating value of expression
 * @param expr pointer to an expression
 */
#define EXPRESSION_FLOATING(expr) __EXPRESSION_FLOATING(expr)

/**
 * Get character value of expression
 * @param expr pointer to an expression
 */
#define EXPRESSION_CHARACTER(expr) __EXPRESSION_CHARACTER(expr)

/**
 * Get string value of expression
 * @param expr pointer to an expression
 */
#define EXPRESSION_STRING(expr) __EXPRESSION_STRING(expr)

/**
 * Get the maximum length of a string expression.
 * @param expr pointer to an expression
 */
#define EXPRESSION_STRING_LENGTH(expr) __EXPRESSION_STRING_LENGTH(expr)

/**
 * If you want to use this function, refrain from it and use
 * SAFE_STRING_IS_TAINTED (safety.h) instead (as the other
 * function performs some on-the-fly checks)
 * @param str and struct Expression resembling a string
 * Returns 1 if TAINTED_FLAG has been set, 0 otherwis
 *
 */
#define EXPRESSION_STRING_GET_TAINTED_BIT(str) \
    __EXPRESSION_STRING_GET_TAINTED_BIT(str)

/**
 * Get the pointer to native function
 * @param expr pointer to an expression
 */
#define EXPRESSION_NATIVE_FUNC(expr) __EXPRESSION_NATIVE_FUNC(expr)

/**
 * Get the pointer to an environment
 * @param expr pointer to an expression
 */
#define EXPRESSION_ENVIRONMENT(expr) __EXPRESSION_ENVIRONMENT(expr)

/**
 * Get the pointer to a lambda form
 */
#define EXPRESSION_LAMBDA(expr) __EXPRESSION_LAMBDA(expr)

/**
 * Get the car of a cons cell
 */
#define EXPRESSION_CAR(expr) __EXPRESSION_CAR(expr)

/**
 * Get the cdr of a cons cell
 */
#define EXPRESSION_CDR(expr) __EXPRESSION_CDR(expr)

/**
 * Takes care of the modification necessary to an expression
 * that is going to be assigned to anything
 * @param expr the expression that should be assigned
 * @return a pointer to the expression that is ready to be assigned to whatever
 */
struct Expression *expressionAssign(struct Expression *env,
                                    struct Expression *expr);

/**
 * Set the car of a cons cell
 * @param expr cons cell whose car should be set
 * @param expr2 any expression that should be set as the car of the cons cell
 */
#define EXPRESSION_SET_CAR(expr, expr2) __EXPRESSION_SET_CAR(expr, expr2)

/**
 * Set the cdr of a cons cell
 * @param expr cons cell whose cdr should be set
 * @param expr2 any expression that should be set as the cdr of the cons cell
 */
#define EXPRESSION_SET_CDR(expr, expr2) __EXPRESSION_SET_CDR(expr, expr2)

/*******************************************************************************
                                     CASTS
 *******************************************************************************/

/**
 * Tries to cast value of expr to float
 * Returns 0 in case of failure
 * TODO: Report error!
 */
#define EXPR_TO_FLOAT(expr)                \
    ((EXPR_OF_TYPE(expr, EXPR_INTEGER))    \
         ? (float)EXPRESSION_INTEGER(expr) \
         : (EXPR_OF_TYPE(expr, EXPR_FLOAT)) ? EXPRESSION_FLOATING(expr) : 0)

/*******************************************************************************
                               I N T E R N A L S
 *******************************************************************************/

/**
 * Creates a expression containing a string
 * @param str the string
 */
struct Expression *expressionCreateString(struct Expression *env, char *str);

/**
 * Creates a symbol
 * @param str the string resembling the symbol
 */
struct Expression *createSymbol(struct Expression *env, char *str);

/*****************************************************************************
                         INCLUDE ACTUAL IMPLEMENTATION
 *****************************************************************************/

#include "__expression_packed.h"

#endif
