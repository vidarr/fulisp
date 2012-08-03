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
