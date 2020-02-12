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
#include "benchmarking.h"
#include "eval.h"
#include "expression.h"
#include "fulispreader.h"
#include "garbage_collector.h"
#include "fulisp.h"
#include "streams.h"
#include "test.h"

#ifdef DEBUG_GARBAGE_COLLECTOR
#include "debugging.h"
#else
#include "no_debugging.h"
#endif

#if GARBAGE_COLLECTOR == GC_MARK_AND_SWEEP

BENCHMARK_DECLARE_VAR(bmTime, bmTemp, bmTimeSt)

#define STR_BUFFER_LEN 255
static char *strBuffer;

static void getGcBenchmarkVars(struct Expression *env, size_t *reclaimed,
                               size_t *marked) {
    struct Environment *environ = EXPRESSION_ENVIRONMENT(env);
    *reclaimed = environ->gcInfo.noReclaimedExpr;
    *marked = environ->gcInfo.noMarkedExpr;
}

static int performTest(int(check)(struct Expression *)) {
    int result;
    struct Expression *env;

    env = fuOpen();

    BENCHMARK_INIT(bmTime, bmTemp, bmTimeSt);
    result = check(env);
    BENCHMARK_DO();

    fuClose(env);

    return result;
}

static void printUnexpectedNumberOfExpressions(const char *kind, size_t actual,
                                               size_t expected) {
    SAFE_SPRINTF(strBuffer, STR_BUFFER_LEN,
                 "Unexpected number of %s expressions: %lu  Expected %lu", kind,
                 actual, expected);
    testWarn(strBuffer);
}

static int ensureReclaimed(struct Expression *env, size_t no) {
    size_t reclaimed;
    size_t marked;
    getGcBenchmarkVars(env, &reclaimed, &marked);
    if (reclaimed != no) {
        printUnexpectedNumberOfExpressions("reclaimed", reclaimed, no);
        return TEST_FAILED;
    }
    return TEST_PASSED;
}

static int ensureMarked(struct Expression *env, size_t no) {
    size_t reclaimed;
    size_t marked;
    getGcBenchmarkVars(env, &reclaimed, &marked);
    if (marked != no) {
        printUnexpectedNumberOfExpressions("marked", marked, no);
        return TEST_FAILED;
    }
    return TEST_PASSED;
}

static int testAtomicExpressions(struct Expression *env) {
    int result;
    size_t index;
    size_t maxNo, usedNo;
    GC_RUN(env);
    getGcBenchmarkVars(env, &maxNo, &usedNo);
    /* Needed due to constant symbols in symbol table that are only marked
     * once at the very first run of the GC */
    getGcBenchmarkVars(env, &maxNo, &usedNo);
    for (index = 0; index < maxNo; index++) {
        EXPRESSION_CREATE_ATOM(env, EXPR_INTEGER, &index);
    }
    GC_RUN(env);
    result = ensureReclaimed(env, maxNo);
    if (result != TEST_PASSED) return result;
    result = ensureMarked(env, usedNo - 11);
    return result;
}

static int testConsExpressions(struct Expression *env) {
    int result;
    size_t index;
    size_t maxNo, freeNo, usedNo;
    struct Expression *car, *cdr;
    GC_RUN(env);
    getGcBenchmarkVars(env, &freeNo, &usedNo);
    maxNo = freeNo / 3 - 1;
    for (index = 0; index < maxNo; index++) {
        car = EXPRESSION_CREATE_ATOM(env, EXPR_INTEGER, &index);
        cdr = EXPRESSION_CREATE_ATOM(env, EXPR_INTEGER, &index);
        EXPRESSION_CREATE_CONS(env, car, cdr);
    }
    GC_RUN(env);
    result = ensureMarked(env, usedNo);
    if (result != TEST_PASSED) return result;
    result = ensureReclaimed(env, freeNo);
    if (result != TEST_PASSED) return result;
    cdr = NIL;
    for (index = 0; index < maxNo; index++) {
        car = EXPRESSION_CREATE_ATOM(env, EXPR_INTEGER, &index);
        cdr = EXPRESSION_CREATE_CONS(env, car, cdr);
        EXPRESSION_CREATE_CONS(env, car, cdr);
    }
    GC_RUN(env);
    result = ensureMarked(env, usedNo);
    if (result != TEST_PASSED) return result;
    result = ensureReclaimed(env, freeNo);
    if (result != TEST_PASSED) return result;
    return result;
}

/*****************************************************************************
 * ENVIRONMENT TEST
 *****************************************************************************/

static char *createSymbolName(size_t no) {
    char *str = SAFE_STRING_NEW(30);
    SAFE_SPRINTF(str, 30, "s%lu", no);
    return str;
}

static struct Expression *createDefineStatement(struct Expression *env,
                                                char *name, char *val) {
    /* Creates 6 expressions */
    struct Expression *nameExpr;
    struct Expression *valExpr;
    struct Expression *defineCall;
    struct Expression *defineExpr = CREATE_SYMBOL(env, "DEFINE");
    nameExpr = CREATE_SYMBOL(env, name);
    valExpr = CREATE_STRING_EXPRESSION(env, val);
    defineCall = EXPRESSION_CREATE_CONS(env, valExpr, NIL);
    defineCall = EXPRESSION_CREATE_CONS(env, nameExpr, defineCall);
    defineCall = EXPRESSION_CREATE_CONS(env, defineExpr, defineCall);
    return defineCall;
}

static struct Expression *createSymbolExpression(struct Expression *env,
                                                 size_t no) {
    /* Creates 1 expression */
    char *name;
    name = createSymbolName(no);
    return CREATE_SYMBOL(env, name);
}

static void createLookupEntry(struct Expression *env, size_t no) {
    /* Creates 7 expressions
     * 6 are inserted into lookup table and should not be garbage collected
     * 1 is placed in environment->current and wont be garbage collected
     * On subsequent calls, the old value of env->current WILL be colected */
    struct Expression *lispStatement;
    char *symbolName = createSymbolName(no);
    lispStatement = createDefineStatement(env, symbolName, symbolName);
    eval(env, lispStatement);
}

static int testEnvironment(struct Expression *env) {
    int result;
    size_t index;
    size_t freeNo, usedNo;
    GC_RUN(env);
    getGcBenchmarkVars(env, &freeNo, &usedNo);
    /* will create 10 * 7 expressions
     * 10 * 6 should be garbage-collected */
    freeNo -= 10;
    usedNo += 10;
    for (index = 0; index < 10; index++) {
        createLookupEntry(env, index);
    }
    GC_RUN(env);
    result = ensureMarked(env, usedNo);
    if (result != TEST_PASSED) return result;
    result = ensureReclaimed(env, freeNo);
    if (result != TEST_PASSED) return result;
    /* Creates 10 expressions, all should be garbage collected */
    for (index = 0; index < 10; index++) {
        createSymbolExpression(env, index + 10);
    }
    GC_RUN(env);
    result = ensureMarked(env, usedNo);
    if (result != TEST_PASSED) return result;
    result = ensureReclaimed(env, freeNo);
    if (result != TEST_PASSED) return result;
    /* will create 15 * 7 expressions
     * 15 * 6 - 1 should be garbage-collected */
    freeNo -= 15;
    usedNo += 15;
    for (index = 0; index < 15; index++) {
        createLookupEntry(env, index + 111);
    }
    GC_RUN(env);
    result = ensureMarked(env, usedNo);
    if (result != TEST_PASSED) return result;
    result = ensureReclaimed(env, freeNo);
    if (result != TEST_PASSED) return result;
    return result;
}

/*****************************************************************************
 *                               C O M P L E X
 *****************************************************************************/

static struct Expression *execute(struct Expression *env, char *statement) {
    struct Expression *expr;
    struct Reader *reader;
    struct CharReadStream *bufStream;
    struct CharReadStream *stream = makeStringCharReadStream(statement);
    bufStream = makeCharReadStream(stream);
    reader = newFuLispReader(env, bufStream);
    expr = fuRead(reader);

    /* expressionToString(env, buffer, sizeof(buffer), expr);
    fprintf(stderr, "Read '%s'\n", buffer); */

    assert(expr);
    expr = eval(env, expr);
    deleteReader(reader);
    STREAM_DISPOSE(stream);
    STREAM_DISPOSE(bufStream);
    return expr;
}

static int testComplex(struct Expression *env) {
#define CHECK_STRICT_SMALLER(a, b, c, message) \
    if (a < b && b < c) {                      \
        return TEST_PASSED;                    \
    } else {                                   \
        testWarn(message);                     \
        return TEST_FAILED;                    \
    }

    size_t reclaimed, marked;
    size_t reclaimed2, marked2;
    assert(env);
    GC_RUN(env);
    getGcBenchmarkVars(env, &reclaimed2, &marked2);
    execute(env, "(define adder (lambda (x y) (+ x y)))");
    GC_RUN(env);
    getGcBenchmarkVars(env, &reclaimed, &marked);
    if (reclaimed2 < reclaimed) {
        testWarn("Reclaimed number unexpected\n");
        return TEST_FAILED;
    }
    if (marked2 > marked) {
        testWarn("Marked number unexpected\n");
        return TEST_FAILED;
    }
    execute(env, "(+ 17 5)");
    DEBUG_PRINT("execute(env, \"(+ 4 5)\")\n");
    GC_RUN(env);
    getGcBenchmarkVars(env, &reclaimed2, &marked2);
    reclaimed--; /* current contained ADDER which is still in lookup */
    marked++;    /* see above */
    if (reclaimed != reclaimed2) {
        testWarn("Reclaimed number not equal\n");
        return TEST_FAILED;
    }
    if (marked != marked2) {
        testWarn("Marked number not equal\n");
        return TEST_FAILED;
    }
    execute(env, "(set! adder nil)");
    DEBUG_PRINT("execute(env, \"(set adder nil)\")\n");
    GC_RUN(env);
    getGcBenchmarkVars(env, &reclaimed, &marked);
    if (reclaimed < reclaimed2) {
        testWarn("Reclaimed number decreased - should be increased\n");
        return TEST_FAILED;
    }
    if (marked > marked2) {
        testWarn("Marked number increased - should be decreased\n");
        return TEST_FAILED;
    }
    execute(env, "(define two 2)");
    DEBUG_PRINT("execute(env, \"(define two 2)\")\n");
    GC_RUN(env);
    getGcBenchmarkVars(env, &reclaimed2, &marked2);
    if (reclaimed2 > reclaimed) {
        testWarn("Reclaimed number unexpected\n");
        return TEST_FAILED;
    }
    if (marked2 < marked) {
        testWarn("Marked number unexpected\n");
        return TEST_FAILED;
    }
    return TEST_PASSED;

#undef CHECK_STRICT_SMALLER
}

int main(int argc, char **argv) {
    int result;
    DECLARE_TEST(test - gc.c);

    strBuffer = SAFE_STRING_NEW(STR_BUFFER_LEN);
    result = test(performTest(testAtomicExpressions),
                  "Check allocating atomic expressions");
    if (result != TEST_PASSED) return 0;
    result = test(performTest(testConsExpressions),
                  "Check allocating cons expressions");
    if (result != TEST_PASSED) return 0;
    result =
        test(performTest(testEnvironment), "Check dealing with environment");
    if (result != TEST_PASSED) return 0;
    result = test(performTest(testComplex), "Check Complex operations");
    free(strBuffer);
    return result;
}

#else /* GARBAGE_COLLECTOR == GC_MARK_AND_SWEEP */

int main(int argc, char **argv) {
    testWarn("Garbage Collector tests only suitable for MARK_AND_SWEEP");
    exit(0);
}

#endif /* GARBAGE_COLLECTOR == GC_MARK_AND_SWEEP */
