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
#include "stack.h"
#include <stdlib.h>

#define MODULE_NAME "stack.c"

#ifdef DEBUG_STACK
#include "debugging.h"
#else
#include "no_debugging.h"
#endif

struct Stack *stackCreate(int size) {
    struct Stack *stack = (struct Stack *)SAFE_MALLOC(sizeof(struct Stack));
    stack->stack = (StackEntry *)SAFE_MALLOC(sizeof(StackEntry) * (size + 1));
    stack->first = stack->next = stack->stack;
    stack->last = stack->stack + size; /* - 1; */
    stackResetError(stack);
    return stack;
}

void stackFree(struct Stack *stack) {
    free(stack->stack);
    free(stack);
}

void *stackPush(struct Stack *stack, StackEntry el) {
    if (stack->next <= stack->last) {
        DEBUG_PRINT("Pushing...\n");
        *(stack->next) = el;
        stack->next++;
        return el;
    }
    DEBUG_PRINT("Stack would overflow!\n");
    stack->error = ERR_STACK_OVERFLOW;
    return NULL;
}

StackEntry stackPop(struct Stack *stack) {
    if (stack->next > stack->first) {
        DEBUG_PRINT("Popping...\n");
        stack->next--;
        return *(stack->next);
    }
    DEBUG_PRINT("Stack would underflow!\n");
    stack->error = ERR_STACK_UNDERFLOW;
    return NULL;
}

int stackError(struct Stack *stack) { return stack->error; }

void stackResetError(struct Stack *stack) { stack->error = ERR_OK; }
