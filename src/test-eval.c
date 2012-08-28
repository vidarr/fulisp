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

#include "testfileinput.h"
#include "fulispreader.h"
#include "eval.h"

#define INPUT_FILE     "../tests/test-eval.in"
#define INPUT_REF_FILE "../tests/test-eval.ref"


static struct Expression *env;


static struct Expression *getNextExpression(struct Reader *reader) {
    struct Expression *expr, *readExpr = fuRead(reader);
    expr = eval(env, readExpr);
    expressionDispose(env, readExpr);
    return expr;
}


int main(int argc, char **argv) {
    int result;
    struct Memory *mem;

    printf("\nTesting eval.c\n\n");

    mem = newMemory();
    env = environmentCreateStdEnv(mem);
    result = checkFromFiles(env, INPUT_FILE, INPUT_REF_FILE, getNextExpression);
    expressionDispose(env, env);
    deleteMemory(mem);
    return result;
}

