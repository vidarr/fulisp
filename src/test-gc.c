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

#include "eval.h"
#include "benchmarking.h"
#include "test.h"
#include "garbage_collector.h"
#include "expression.h"
#include <stdio.h>


BENCHMARK_DECLARE_VAR(bmTime, bmTemp, bmTimeSt)

#define STR_BUFFER_LEN 255
static char *strBuffer;

static int performTest(int (check)(struct Expression*)) {
    int result;
    struct Expression *env;
    struct Memory *mem;

    DECLARE_TEST(eval.c);

    BENCHMARK_INIT(bmTime, bmTemp, bmTimeSt);

    mem = newMemory();
    env = environmentCreateStdEnv(mem);

    result =  check(env);

    expressionDispose(env, env);

    BENCHMARK_DO( \
        fprintf(stderr, "Benchmark: Total time measured: %li \n", bmTime) \
    );

    deleteMemory(mem);
    return result;
}


#define PRINT_UNEXPECTED_NUMBER_OF_EXPRESSIONS(kind, actual, expected) {      \
        sprintf(strBuffer,                                                    \
                "Unexpected number of %s expressions: %lu   "                 \
                "Expected %lu\n",                                             \
                kind, (actual), (expected));                                  \
    testWarn(strBuffer);                                                      \
}


static int ensureReclaimed(struct Expression *env, size_t no) {      
    size_t reclaimed;
    struct Environment * environ = EXPRESSION_ENVIRONMENT(env);                
    reclaimed = environ->gcInfo.noReclaimedExpr;
    if( reclaimed != no) {
        PRINT_UNEXPECTED_NUMBER_OF_EXPRESSIONS("reclaimed", reclaimed, no);
        return TEST_FAILED;
    }
    return TEST_PASSED;
}


static int ensureMarked(struct Expression *env, size_t no) {      
    size_t marked;
    struct Environment * environ = EXPRESSION_ENVIRONMENT(env);                
    marked = environ->gcInfo.noMarkedExpr;
    if(marked != no) {
        PRINT_UNEXPECTED_NUMBER_OF_EXPRESSIONS("marked", marked, no);
        return TEST_FAILED;
    }
    return TEST_PASSED;
}


#undef PRINT_UNEXPECTED_NUMBER_OF_EXPRESSIONS


static int testAtomicExpressions(struct Expression *env) {
    int result;
    size_t index;
    size_t maxNo, usedNo;
    struct Environment * environ;
    GC_RUN(env);
    environ = EXPRESSION_ENVIRONMENT(env);
    maxNo  = environ->gcInfo.noReclaimedExpr;
    usedNo = environ->gcInfo.noMarkedExpr;
    for(index = 0; index  < maxNo; index++ ) {
        EXPRESSION_CREATE_ATOM(env, EXPR_INTEGER, &index);
    }
    GC_RUN(env);
    result = ensureReclaimed(env, maxNo);
    if(result != TEST_PASSED) return result;
    result = ensureMarked(env, usedNo);
    return result;
}


static int testConsExpressions(struct Expression *env) {
    int result;
    size_t index;
    size_t maxNo, usedNo;
    struct Environment * environ;
    struct Expression  * car, * cdr;
    GC_RUN(env);
    environ = EXPRESSION_ENVIRONMENT(env);
    maxNo  = environ->gcInfo.noReclaimedExpr / 3;
    maxNo -= 1;
    usedNo = environ->gcInfo.noMarkedExpr;
    for(index = 0; index  < maxNo; index++ ) {
        car = EXPRESSION_CREATE_ATOM(env, EXPR_INTEGER, &index);
        cdr = EXPRESSION_CREATE_ATOM(env, EXPR_INTEGER, &index);
        EXPRESSION_CREATE_CONS(env, car, cdr); 
    }
    GC_RUN(env);
    maxNo += 1;
    result = ensureMarked(env, usedNo);
    if(result != TEST_PASSED) return result;
    result = ensureReclaimed(env, maxNo * 3);
    if(result != TEST_PASSED) return result;
    cdr = NIL;
    for(index = 0; index  < maxNo; index++ ) {
        car = EXPRESSION_CREATE_ATOM(env, EXPR_INTEGER, &index);
        cdr = EXPRESSION_CREATE_CONS(env, car, cdr); 
    }
    GC_RUN(env);
    testMessage("Checking CONS list\n");
    result = ensureMarked(env, usedNo);
    if(result != TEST_PASSED) return result;
    result = ensureReclaimed(env, maxNo * 3);
    if(result != TEST_PASSED) return result;
    return result;
}


int main(int argc, char **argv) {
    int result;
    DECLARE_TEST(test-gc.c);

#   if GARBAGE_COLLECTOR != GC_MARK_AND_SWEEP

    testWarn("Garbage Collector tests only suitable for MARK_AND_SWEEP");
    exit(0);

#   endif

    strBuffer = malloc(255 * sizeof(char));
    result = test(performTest(testAtomicExpressions), 
            "Check allocating atomic expressions");
    result = test(performTest(testConsExpressions), 
            "Check allocating cons expressions");
    free(strBuffer);
    return result; 
} 
