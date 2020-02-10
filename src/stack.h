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
#ifndef __CALL_STACK_H__
#define __CALL_STACK_H__

#include "config.h"
#include "error.h"
#include "expression.h"

typedef void *StackEntry;

struct Stack {
    StackEntry *stack;
    StackEntry *next;
    StackEntry *first;
    StackEntry *last;
    int error;
};

/**
 * Creates a stack
 * @param size Number of elements the stack can hold (unsupported yet)
 */
struct Stack *stackCreate(int size);

/**
 * Frees stack structure.
 * @param stack The stack to be freed.
 */
void stackFree(struct Stack *stack);

/**
 * Pushes an element onto the stack.
 * Any element of size of a void pointer can be pushed.
 * @param stack Stack to push to
 * @param el Element to be pushed onto the stack
 * @return Element that has been pushed onto the stack or NULL if stack is full.
 */
void *stackPush(struct Stack *stack, StackEntry el);

/**
 * Pops an element from the stack.
 * @param stack the stack to be popped from
 * @return pointer to StackEntry or NULL if the stack is empty.
 */
StackEntry stackPop(struct Stack *stack);

/**
 * Returns error state from last stack operation.
 */
int stackError(struct Stack *stack);

/**
 * Set error state of stack back to ERR_OK
 */
void stackResetError(struct Stack *stack);

#endif
