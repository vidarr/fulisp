/**
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
  if(stack->next <= stack->last) {
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
  if(stack->next > stack->first) {
      DEBUG_PRINT("Popping...\n");
      stack->next--;
      return *(stack->next);
  }
  DEBUG_PRINT("Stack would underflow!\n");
  stack->error = ERR_STACK_UNDERFLOW;
  return NULL;
}


int stackError(struct Stack *stack) {
  return stack->error;
}


void stackResetError(struct Stack *stack) {
  stack->error = ERR_OK;
}

