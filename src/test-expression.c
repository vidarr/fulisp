/*
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,
 * USA.
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cons.h"
#include "expression.h"
#include "print.h"
#include "fulisp.h"
#include "test.h"

#define MAX_STR_BUFFER 65000
#define TEST_STR                                                           \
    "test is a test is a test hehehe crashing its gonna crash hash slash " \
    "cash - johnny?"

static int sigsegvOccured = 1;

void sigsegvHandler(int signal) {
    sigsegvOccured = 1;
    printf("SIGSEGV occured...ABORTING!\n");
    exit(-1);
}

int testExpression(struct Expression *env, struct Expression *expr) {
    struct Expression *ref1, *ref2, *ref3;

    char *str = SAFE_STRING_NEW(MAX_STR_BUFFER);

    ref1 = expressionAssign(env, expr);
    ref2 = expressionAssign(env, expr);
    ref3 = expressionAssign(env, expr);

    expressionToString(env, str, MAX_STR_BUFFER, ref1);
    printf("%s\n", str);
    expressionToString(env, str, MAX_STR_BUFFER, ref2);
    printf("%s\n", str);
    expressionToString(env, str, MAX_STR_BUFFER, ref3);
    printf("%s\n", str);

    /* Should fail */
    /* expressionToString(env, str, ref3);
    printf("%s\n", str); */
    free(str);
    return 0;
}

int main(int argc, char **argv) {
    int res;
    char *str;
    struct Memory *mem;
    /* Environment is needed to create/assign/dispose expressions for garbage
     * collection and access to memory */
    struct Expression *cons, *cons2;
    struct Expression *env;

    DECLARE_TEST(expression.c);

    mem = newMemory();
    env = environmentCreate(0, mem);

    res = 0;
    str = SAFE_STRING_NEW(strlen(TEST_STR) * sizeof(char));
    cons2 = EXPRESSION_CREATE_ATOM(env, EXPR_STRING, str);
    signal(SIGSEGV, sigsegvHandler);
    memcpy(str, TEST_STR, strlen(TEST_STR) + 1);
    cons = EXPRESSION_CREATE_CONS(env, cons2, NIL);
    cons = EXPRESSION_CREATE_CONS(env, expressionAssign(env, cons2),
                                  expressionAssign(env, cons));
    res = test(testExpression(env, cons), "Cons cell management");
    /* usually one should get rid of the cons expression, but this has happened
       within testExpression */

    fuClose(env);

    return res;
}
