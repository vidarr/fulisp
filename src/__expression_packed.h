/*
 * (C) 2012 Michael J. Beer
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
 * Provides actual implementation for expression.h - packed version
 */
#ifndef __EXPRESSION_H__

#error("This Header is intended tor internal use only and is not meant to " \
        "be used directly")

#else

#include "garbage_collector.h"

/*******************************************************************************
  E X R E S S I O N   S T R U C T U R E
 *******************************************************************************/

/* Advantage of THIS implementation:
 * Single pointer derefencing to access car/cdr
 * No second struct to wrap car/cdr
 * 'Extended' types possible (i.e. some BIGNUM with head = mantisse,
 * tail =
 exponent
 Disadvantages:
 * Atomic types seize one int that is not used
 */
struct Expression {
    int type;
    union {
        char *string;
        int integer;
        char character;
        double floating;
        struct Expression *(*nativeFunc)(struct Expression *env,
                                         struct Expression *);
        struct Environment *env;
        struct Lambda *lambda;
        struct Expression *car;
    } data;
    union {
        struct Expression *cdr;
        size_t length; /* Used for length of string */
    } extension;
    GC_DEFINE_EXPR_INFO
};

/******************************************************************************
  D E A L I N G   W I T H   E X R P R E S S I O N   T Y P E S
 ******************************************************************************/

#define __EXPRESSION_TYPE(x) (x->type & EXPR_TYPE_AREAL)

#define __EXPR_OF_TYPE(x, y) (x && __EXPRESSION_TYPE(x) == y)

#define __EXPR_IS_POINTER(x) (x->type & EXPR_POINTER)

#define __EXPR_IS_NIL(x) (x == NIL)

#define __EXPR_IS_VALID(x) (x && !EXPR_OF_TYPE(x, EXPR_NO_TYPE))

#define __EXPR_SET_TYPE(expr, type) \
    (expr->type = ((expr->type & ~EXPR_TYPE_AREAL) | (type & EXPR_TYPE_AREAL)))

/******************************************************************************
                              CONSTANT EXPRESSIONS
 ******************************************************************************/

extern struct Expression expressionNil;
extern struct Expression expressionT;
extern struct Expression expressionRest;

#define __T (&expressionT)
#define __NIL (&expressionNil)
#define __REST (&expressionRest)

extern struct Expression expressionInteger;
extern struct Expression expressionFloat;
extern struct Expression expressionCharacter;
extern struct Expression expressionString;
extern struct Expression expressionSymbol;
extern struct Expression expressionCons;
extern struct Expression expressionLambda;
extern struct Expression expressionNativeFunc;
extern struct Expression expressionEnvironment;
extern struct Expression lispLibraryPath;

#define __TYPE_INTEGER (&expressionInteger)
#define __TYPE_FLOAT (&expressionFloat)
#define __TYPE_CHARACTER (&expressionCharacter)
#define __TYPE_STRING (&expressionString)
#define __TYPE_SYMBOL (&expressionSymbol)
#define __TYPE_CONS (&expressionCons)
#define __TYPE_LAMBDA (&expressionLambda)
#define __TYPE_NATIVE_FUNC (&expressionNativeFunc)
#define __TYPE_ENVIRONMENT (&expressionEnvironment)

/******************************************************************************
  A C C E S S
 ******************************************************************************/

#define __EXPRESSION_INTEGER(expr) ((expr)->data.integer)

#define __EXPRESSION_FLOATING(expr) ((expr)->data.floating)

#define __EXPRESSION_CHARACTER(expr) ((expr)->data.character)

#define __EXPRESSION_STRING(expr) ((expr)->data.string)

#define __EXPRESSION_NATIVE_FUNC(expr) ((expr)->data.nativeFunc)

#define __EXPRESSION_ENVIRONMENT(expr) ((expr)->data.env)

#define __EXPRESSION_LAMBDA(expr) ((expr)->data.lambda)

#define __EXPRESSION_CAR(expr) ((expr)->data.car)

#define __EXPRESSION_CDR(expr) ((expr)->extension.cdr)

#define __EXPRESSION_SET_CAR(expr, expr2) (expr)->data.car = expr2;

#define __EXPRESSION_SET_CDR(expr, expr2) (expr)->extension.cdr = expr2;

/*******************************************************************************
 *                                    CASTS
 *******************************************************************************/

/*******************************************************************************
 *                             I N T E R N A L S
 *******************************************************************************/

/* There is nothing like an explicit cons struct in this implementation, thus
   ensure nobody tries to get it */
#ifdef __EXPRESSION_CONS

#undef __EXPRESSION_CONS

#endif

#define __EXPRESSION_DISPOSE_CONS_STRUCT(mem, expr)

#endif
