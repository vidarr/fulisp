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

#ifndef __MEMORY_H__
#define __MEMORY_H__

#include "config.h"
#include "expression.h"

struct Expression;

struct ExpressionBlock {
    struct Expression *memory;
    struct ExpressionBlock *nextBlock;
};

struct Memory {
    struct ExpressionBlock *exprBlocks;
    struct Expression *nextExpr;
    void (*outOfMemory)(void);
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
 * Recycle Expression struct
 */
#define MEMORY_DISPOSE_EXPRESSION(mem, expr) \
    __MEMORY_DISPOSE_EXPRESSION(mem, expr)

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

void resetMemory(struct Memory *mem);

void dumpFreeExpressions(struct Memory *mem);

/******************************************************************************
                                   INTERNALS
 *****************************************************************************/

#ifdef MEMORY_AUTOEXTEND

void allocateNewExpressionBlock(struct Memory *mem);

void allocateNewConsBlock(struct Memory *mem);

#define __HANDLE_OUT_OF_EXPR_MEM(mem) allocateNewExpressionBlock(mem);

#define __HANDLE_OUT_OF_CONS_MEM(mem) allocateNewConsBlock(mem);

#else /* MEMORY_AUTOEXTEND */

#define __HANDLE_OUT_OF_EXPR_MEM(mem) (mem)->outOfMemory();

#define __HANDLE_OUT_OF_CONS_MEM(mem) (mem)->outOfMemory();

#endif /* MEMORY_AUTOEXTEND */

#define __MEMORY_GET_EXPRESSION(mem, expr)                            \
    {                                                                 \
        fprintf(stderr, "mem: %p   mem->nextExpr: %p\n", (void *)mem, \
                (void *)(mem)->nextExpr);                             \
        if ((mem)->nextExpr == NULL) __HANDLE_OUT_OF_EXPR_MEM(mem);   \
        expr = (mem)->nextExpr;                                       \
        fprintf(stderr, "new mem->nextExpr: %p\n",                    \
                (void *)EXPRESSION_STRING((mem)->nextExpr));          \
        (mem)->nextExpr =                                             \
            (struct Expression *)EXPRESSION_STRING((mem)->nextExpr);  \
    }

#define __MEMORY_DISPOSE_EXPRESSION(mem, expr)               \
    {                                                        \
        fprintf(stderr, "(1)   expr->string: %p\n",          \
                (void *)EXPRESSION_STRING(expr));            \
        EXPRESSION_STRING(expr) = (char *)((mem)->nextExpr); \
        fprintf(stderr, "(2)   expr->string: %p\n",          \
                (void *)EXPRESSION_STRING(expr));            \
        (mem)->nextExpr = expr;                              \
        IF_SAFETY_CODE(expr->type = 0;);                     \
    }

/*----------------------------------------------------------------------------*/

#endif
