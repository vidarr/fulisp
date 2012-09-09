/*
 * (C) 2012 Michael J. Beer
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
 * Provides actual implementation for expression.h - packed version
 */
#ifndef __EXPRESSION_H__


#   error("This Header is intended tor internal use only and is not meant to " \
        "be used directly")


#else



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
    unsigned int counter;
    union {
        char *string;
        int integer;
        char character;
        double floating;
        struct Expression *(*nativeFunc)(struct Expression *env, struct Expression *); 
        struct Environment *env;
        struct Lambda *lambda;
        struct Expression *car;
    } data;  
    union {
        struct Expression *cdr;
    } extension;  
};



/*******************************************************************************
  D E A L I N G   W I T H   E X R P R E S S I O N   T Y P E S
 *******************************************************************************/



#define __EXPRESSION_TYPE(x)             (x->type & EXPR_TYPE_AREAL)

#define __EXPR_OF_TYPE(x, y)             (x && __EXPRESSION_TYPE(x) == y)

#define __EXPR_IS_POINTER(x)             (x->type & EXPR_POINTER)

#define __EXPR_IS_NIL(x)                 (x == NIL)

#define __EXPR_IS_VALID(x)               (x && !EXPR_OF_TYPE(x, EXPR_NO_TYPE))

#define __EXPR_SET_TYPE(expr, type)            \
            (expr->type = (                    \
            (expr->type & ~EXPR_TYPE_AREAL) |  \
            (type & EXPR_TYPE_AREAL)))



/*******************************************************************************
                              CONSTANT EXPRESSIONS
 *******************************************************************************/



extern struct Expression expressionNil;
extern struct Expression expressionT ;
extern struct Expression expressionRest;


#define __T                            (&expressionT)

#define __NIL                          (&expressionNil)

#define __REST                         (&expressionRest)



/******************************************************************************
  A C C E S S
 ******************************************************************************/



#define __EXPRESSION_INTEGER(expr)               (expr->data.integer)


#define __EXPRESSION_FLOATING(expr)              (expr->data.floating)


#define __EXPRESSION_CHARACTER(expr)             (expr->data.character)


#define __EXPRESSION_STRING(expr)                (expr->data.string)


#define __EXPRESSION_NATIVE_FUNC(expr)           ((expr)->data.nativeFunc)


#define __EXPRESSION_ENVIRONMENT(expr)           ((expr)->data.env)


#define __EXPRESSION_LAMBDA(expr)                ((expr)->data.lambda)


#define __EXPRESSION_CAR(expr)                   ((expr)->data.car)


#define __EXPRESSION_CDR(expr)                   ((expr)->extension.cdr)


#define __EXPRESSION_SET_CAR(expr, expr2)        (expr)->data.car = expr2;


#define __EXPRESSION_SET_CDR(expr, expr2)        (expr)->extension.cdr = expr2;



/*******************************************************************************
 *                                    CASTS
 *******************************************************************************/



/*******************************************************************************
 *                             I N T E R N A L S
 *******************************************************************************/



/* There is nothing like an explicit cons struct in this implementation, thus
   ensure nobody tries to get it */
#ifdef __EXPRESSION_CONS

#   undef __EXPRESSION_CONS

#endif

#define __EXPRESSION_DISPOSE_CONS_STRUCT(mem, expr)  



#endif
