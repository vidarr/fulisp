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

#include "benchmarking.h"
#include "eval.h"
#include "fulispreader.h"
#include "test.h"
#include "testfileinput.h"
#include "fulisp.h"

#define INPUT_FILE "../tests/test-eval.in"
#define INPUT_REF_FILE "../tests/test-eval.ref"

static struct Expression *env;

BENCHMARK_DECLARE_VAR(bmTime, bmTemp, bmTimeSt)

static struct Expression *getNextExpression(struct Reader *reader) {
    struct Expression *expr, *readExpr = fuRead(reader);

    BENCHMARK_CONTINUE(bmTime, bmTemp, bmTimeSt);

    expr = eval(env, readExpr);

    BENCHMARK_INTERRUPT(bmTime, bmTemp, bmTimeSt);

    return expr;
}

int main(int argc, char **argv) {
    int result;

    DECLARE_TEST(eval.c);

    BENCHMARK_INIT(bmTime, bmTemp, bmTimeSt);

    env = fuOpen();

    result = checkFromFiles(env, INPUT_FILE, INPUT_REF_FILE, getNextExpression);

    BENCHMARK_DO(
        fprintf(stderr, "Benchmark: Total time measured: %li \n", bmTime));

    fuClose(env);

    return result;
}
