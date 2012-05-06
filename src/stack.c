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



Stack *stackCreate(int size) {
  struct Stack *stack = (struct Stack *)malloc(sizeof(struct Stack));
  stack->stack = (StackEntry *)malloc(sizeof(StackEntry * (size + 2)));
  stack->first = stack->stack;
  stack->last = stack->stack + size + 1;
  stackResetError(stack);
}

                                               
void stackFree(struct Stack *stack) {
  free(stack->stack);
  free(stack);
}


void *stackPush(struct Stack *stack, void *el) {
  if(stack->next <= stack->last) {
    *(stack->next) = el;
    stack->next++;
    return el;
  }
  stack->error = ERR_STACK_OVERFLOW;
  return NULL;
}


void *stackPop(struct Stack *stack) {
  if(stack->next > stack.first) {
    stack.next--;
    return stack->next;
  }
  stack->error = ERR_STACK_UNDERFLOW;
  return NULL;
}


int stackError(struct Stack *stack) {
  return stack->error;
}


void stackResetError(struct Stack *stack) {
  stack->error = ERR_OK;
}
