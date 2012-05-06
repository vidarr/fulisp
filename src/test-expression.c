/**
 * (C) 2010 Michael J. Beer
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


#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "expression.h"


#define MAX_STR_BUFFER 65000
#define TEST_STR "test is a test is a test hehehe crashing its gonna crash hash slash cash - johnny?"


static int sigsegvOccured = 1;


void sigsegvHandler(int signal) {
    sigsegvOccured = 1;
    printf("SIGSEGV occured...ABORTING!\n");
    exit(1);
}


int testExpression(struct Expression *expr) {
    struct Expression *ref1, *ref2, *ref3;

    char *str = (char *)malloc(sizeof(char) * MAX_STR_BUFFER);
    
    ref1 = expressionAssign(expr);
    ref2 = expressionAssign(expr);
    ref3 = expressionAssign(expr);

    expressionToString(str, ref1);
    printf("%s\n", str);
    expressionDispose(ref1);
    expressionToString(str, ref2);
    printf("%s\n", str);
    expressionDispose(ref2);
    expressionToString(str, ref3);
    printf("%s\n", str);
    expressionDispose(ref3);
    
    expressionDispose(expr);

    /* Should fail */
    /* expressionToString(str, ref3);
    printf("%s\n", str); */
    free(str);
    return -1;
}


int main (int argc, char **argv) {
    int res = 0;
    char *str = malloc((1 + strlen(TEST_STR)) * sizeof(char));
    struct Expression *cons, *cons2 = expressionCreate(EXPR_STRING, str);
    signal(SIGSEGV, sigsegvHandler); 
    memcpy(str, TEST_STR, strlen(TEST_STR) + 1);
    cons = expressionCreate(EXPR_CONS, intCons(cons2, 0));
    cons = expressionCreate(EXPR_CONS, intCons(expressionAssign(cons2),
    expressionAssign(cons)));
    res = test(testExpression(cons), "Cons cell management");
    /* usually one should get rid of the cons expression, but this has happened 
       within testExpression */
    expressionDispose(cons);
    return res;
}








