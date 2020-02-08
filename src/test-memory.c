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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,
 * USA.
 */

#include <stdlib.h>
#include <time.h>
#include "memory.h"
#include "test.h"

#define ADAPT_INDEX(i, j)                                                  \
    /* if i flows over, ensure that numbers dont repeat with period 255 */ \
    if (j > i) {                                                           \
        i += 256 - j;                                                      \
    }

#define GET_EXPRESSION(m, e, i, j) \
    ADAPT_INDEX(i, j)              \
    MEMORY_GET_EXPRESSION(m, e);   \
    e->type = i;

void initRandom(void) { srand(time(NULL)); }

int getTrueOrFalse(void) { return (rand() > RAND_MAX / 2); }

static int outOfMemory = 0;

void setOutOfMemory(void) { outOfMemory = 1; }

struct Expression *allocate(struct Memory *mem, int no) {
    unsigned char i, j;
    int total;
    struct Expression *expr, *start, *prev;

    i = j = total = 0;

    GET_EXPRESSION(mem, expr, i, j);
    EXPRESSION_STRING(expr) = (char *)NULL;
    start = prev = expr;
    for (i = 1, total = 1, j = 0; total < no; i++, j++, total++) {
        GET_EXPRESSION(mem, expr, i, j);
        if (outOfMemory) return NULL;
        EXPRESSION_STRING(prev) = (char *)expr;
        prev = expr;
    }

    EXPRESSION_STRING(prev) = (char *)NULL;
    return start;
}

int verify(struct Expression *expr, int no) {
    unsigned char i, j;
    i = j = 0;

    ADAPT_INDEX(i, j);
    if (expr->type != i) return TEST_FAILED;

    expr = (struct Expression *)EXPRESSION_STRING(expr);

    j = 0;
    i = 1;

    while (expr != NULL) {
        ADAPT_INDEX(i, j);
        if (expr->type != i) {
            return TEST_FAILED;
        }

        expr = (struct Expression *)EXPRESSION_STRING(expr);
        i++;
        j++;
    }

    return TEST_PASSED;
}

int releaseSome(struct Memory *mem, struct Expression *expr) {
    struct Expression *current;
    int no = 0;

    assert(mem && expr);

    current = expr;

    initRandom();

    while (current != NULL) {
        expr = (struct Expression *)EXPRESSION_STRING(current);
        if ((getTrueOrFalse())) {
            MEMORY_DISPOSE_EXPRESSION(mem, current);
            no++;
        }
        current = expr;
    }
    return no;
}

static struct Expression *expr = NULL;

int testAllocation(struct Memory *mem, int no) {
    expr = allocate(mem, no);
    if (expr == NULL) return TEST_FAILED;
    return verify(expr, no);
}

int testReleaseAndAlloc(struct Memory *mem) {
    int noReleased = 0;
    noReleased = releaseSome(mem, expr);
    expr = allocate(mem, noReleased);
    if (expr == NULL) return TEST_FAILED;
    return verify(expr, noReleased);
}

int main(int argc, char **argv) {
    struct Memory *mem;
    int result;

    DECLARE_TEST(memory.c);

#ifndef MEMORY_USE_PREALLOCATION

    testWarn("Built without memory preallocation - skipping memory tests");
    exit(0);

#endif

    mem = newMemory();
    mem->outOfMemory = setOutOfMemory;

    result = test(testAllocation(mem, MEMORY_BLOCK_SIZE),
                  "Allocate less memory than available");
    if (result == TEST_PASSED)
        result =
            test(testReleaseAndAlloc(mem), "Release and reallocate memory");
    else {
        testWarn("Skipping Release and reallocate memory");
        return result;
    }

    deleteMemory(mem);

#ifdef MEMORY_AUTOEXTEND

    mem = newMemory();

    result = test(testAllocation(mem, 2 * MEMORY_BLOCK_SIZE + 3),
                  "Allocate memory with autoexpansion");
    if (result == TEST_PASSED)
        result = test(testReleaseAndAlloc(mem),
                      "Release and reallocate memory with autoexpansion");
    else
        testWarn(
            "Skipping Release and reallocate memory with autoexpanded memory");

#else

    testWarn("Skipping testing autoexpansion");

#endif

    return result;
}
