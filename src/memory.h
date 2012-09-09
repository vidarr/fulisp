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

#ifndef __MEMORY_H__
#define __MEMORY_H__

#include "config.h"
#include "expression.h"


struct Expression;

struct ExpressionBlock {
    struct Expression *memory;
#    ifdef MEMORY_AUTOEXTEND
    struct ExpressionBlock *nextBlock; 
#   endif
};


struct ConsBlock;


struct Memory {
    struct ExpressionBlock *exprBlocks;
    struct ConsBlock       *consBlocks;
    struct Expression      *nextExpr;
    struct Cons            *nextCons;
    void (* outOfMemory)(void);
};


/******************************************************************************
                                     MACROS
 ******************************************************************************/


/**
 * Get another Expression struct that can be used 
 * @param mem Pointer to Memory struct to use
 * @param epxr lvalue (variable) to receive pointer to new Expression struct
 */
#define MEMORY_GET_EXPRESSION(mem, expr) __MEMORY_GET_EXPRESSION(mem, expr) 

/**
 * Get another Cons struct that can be used
 * @param mem Pointer to Memory struct to use
 * @param cons lvalue (variable) to receive pointer to new Cons struct
 */
#define MEMORY_GET_CONS(mem, cons) __MEMORY_GET_CONS(mem, cons) 

/**
 * Recycle Expression struct
 */
#define MEMORY_DISPOSE_EXPRESSION(mem, expr) __MEMORY_DISPOSE_EXPRESSION(mem, expr) 

/**
 * Recycle Cons struct
 */
#define  MEMORY_DISPOSE_CONS(mem, cons) __MEMORY_DISPOSE_CONS(mem, cons)


/******************************************************************************
                                Public functions
 ******************************************************************************/


/**
 * Creates a new Memory structure
 * @return pointer to a new Memory structure
 */
struct Memory *newMemory(void);

/**
 * Release memory struct
 * @param mem the Memory struct to release
 */
void deleteMemory(struct Memory *mem);


/******************************************************************************
                                   INTERNALS
 *****************************************************************************/


#ifdef MEMORY_USE_PREALLOCATION

#    ifdef MEMORY_AUTOEXTEND

         void allocateNewExpressionBlock(struct Memory *mem);
         
         void allocateNewConsBlock(struct Memory *mem);

#        define __HANDLE_OUT_OF_EXPR_MEM(mem) allocateNewExpressionBlock(mem);

#        define __HANDLE_OUT_OF_CONS_MEM(mem) allocateNewConsBlock(mem);

#    else    /* MEMORY_AUTOEXTEND */

#        define __HANDLE_OUT_OF_EXPR_MEM(mem) (mem)->outOfMemory();

#        define __HANDLE_OUT_OF_CONS_MEM(mem) (mem)->outOfMemory();

#    endif   /* MEMORY_AUTOEXTEND */

#    define __MEMORY_GET_EXPRESSION(mem, expr) { \
        if((mem)->nextExpr == NULL) __HANDLE_OUT_OF_EXPR_MEM(mem); \
        expr = (mem)->nextExpr; \
        (mem)->nextExpr = (struct Expression *)EXPRESSION_STRING((mem)->nextExpr); \
     }    

#    define __MEMORY_DISPOSE_EXPRESSION(mem, expr) { \
        EXPRESSION_STRING(expr) = (char *)((mem)->nextExpr); \
        (mem)->nextExpr = expr; \
        IF_SAFETY_CODE( \
            expr->type = 0; \
        ); \
     }    


#    if EXPRESSION_FORMAT == EXPRESSION_FORMAT_EXPANDED

#        define __MEMORY_GET_CONS(mem, cons) { \
            if((mem)->nextCons == NULL) __HANDLE_OUT_OF_CONS_MEM(mem); \
            cons = (mem)->nextCons; \
            (mem)->nextCons = (struct Cons *)((mem)->nextCons->car); \
         }    

#        define __MEMORY_DISPOSE_CONS(mem, cons) { \
            (cons)->car = (struct Expression *)(mem)->nextCons; \
            (mem)->nextCons = cons; \
         }    

#    endif   /* EXPRESSION_FORMAT */


#else    /* MEMORY_USE_PREALLOCATION */


#    define __MEMORY_GET_EXPRESSION(mem, expr) { \
        expr = (struct Expression *)malloc(sizeof(struct Expression)); \
     }    
         
#    define __MEMORY_DISPOSE_EXPRESSION(mem, expr) \
        free(expr)



#    if EXPRESSION_FORMAT == EXPRESSION_FORMAT_EXTENDED


#        define __MEMORY_GET_CONS(mem, cons) { \
            cons = ((struct Cons *)malloc(sizeof(struct Cons))); \
         }    

#        define __MEMORY_DISPOSE_CONS(mem, cons) \
            free(cons)


#    endif   /* EXPRESSION_FORMAT */


#endif   /* MEMORY_USE_PREALLOCATION */


#if EXPRESSION_FORMAT == EXPRESSION_FORMAT_EXPANDED

struct ConsBlock {
    struct Cons *memory;
#    ifdef MEMORY_AUTOEXTEND
    struct ConsBlock *nextBlock; 
#   endif
};


#endif /* EXPRESSION_FORMAT */



#endif

