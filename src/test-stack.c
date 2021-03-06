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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,
 * USA.
 */

#include <stdio.h>
#include "stack.h"
#include "test.h"

static struct Stack *stack;
static int size;

int createStack(void) {
    if ((stack = stackCreate(size))) return 0;
    return 1;
}

int fillStack(struct Stack *stack, int no) {
    long i = 0;
    assert(stack);
    for (i = 0; i < no; i++) {
        if (stackPush(stack, (void *)i) == NULL) {
            if (stackError(stack) != ERR_OK) return 1;
        }
    }
    return 0;
}

int drainStack(struct Stack *stack, int no, int firstNo) {
    long i = 0;
    assert(stack);
    for (i = 0; i < no; i++) {
        if (stackPop(stack) == NULL) {
            if (stackError(stack) != ERR_OK) return 1;
        }
    }
    return 0;
}

/* int fillEmptyWithinLimits(void) {  */
/*   if(fillStack(stack, size / 2))  */
/*     return 1;  */
/*   if(drainStack(stack, size / 4, size - 1))  */
/*      return 1;  */
/*   if(fillStack(stack, size / 4))  */
/*      return 1;  */
/*   if(drainStack(stack, size / 2,size  - 1 + size / 4))  */
/*      return 1;  */
/*   return 0;  */
/* }       */
/*     */
/*   */
/* int fillEmptyOutOfLimits(void) {  */
/*   if(!fillStack(stack, size + 2))  */
/*     return 1;  */
/*   if(!drainStack(stack, size + 1, size + 1))  */
/*      return 1;  */
/*   if(fillStack(stack, size / 4))  */
/*      return 1;  */
/*   if(drainStack(stack, size / 4, size  - 1 + size / 4))  */
/*      return 1;  */
/*   return 0;  */
/* }       */

int freeStack(void) {
    stackFree(stack);
    return 0;
}

int main(int argc, char **argv) {
    size = 20;

    DECLARE_TEST(stack.c);

    test(createStack(), "Create stack with 20 elements");
    test(fillStack(stack, 10), "Fill stack with 10 elements");
    test(drainStack(stack, 5, 5), "Pop 5 elements off of stack");
    test(fillStack(stack, 15), "Fill stack");
    test(!stackPush(stack, (StackEntry)21) && !(stackError(stack) != ERR_OK),
         "Push beyond limits");
    test(!stackPush(stack, (StackEntry)21) && !(stackError(stack) != ERR_OK),
         "Push beyond limits");
    stackResetError(stack);
    test(drainStack(stack, 21, 21), "Pop all elements off of stack");
    test(!stackPop(stack) && !(stackError(stack) != ERR_OK) &&
             !stackPop(stack) && !(stackError(stack) != ERR_OK),
         "Pop beyond limits");
    stackResetError(stack);
    test(stackPush(stack, (StackEntry)15l) != (StackEntry)15l &&
             !(stackError(stack) != ERR_OK) &&
             stackPop(stack) != (StackEntry)15,
         "Push onto potentially tainted stack");

    test(freeStack(), "Disposing stack");
    return 0;
}
