/*
 * (C) 2011 Michael J. Beer
 * All rights reserved.
 *
 * Redistribution  and use in source and binary forms, with or with‐
 * out modification, are permitted provided that the following  con‐
 * ditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above  copy‐
 * right  notice,  this  list  of  conditions and the following dis‐
 * claimer in the documentation and/or other materials provided with
 * the distribution.
 *
 * 3.  Neither the name of the copyright holder nor the names of its
 * contributors may be used to endorse or promote  products  derived
 * from this software without specific prior written permission.
 *
 * THIS  SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBU‐
 * TORS "AS IS" AND ANY EXPRESS OR  IMPLIED  WARRANTIES,  INCLUDING,
 * BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS FOR A PARTICULAR PURPOSE  ARE  DISCLAIMED.  IN  NO  EVENT
 * SHALL  THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DI‐
 * RECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR  CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS IN‐
 * TERRUPTION)  HOWEVER  CAUSED  AND  ON  ANY  THEORY  OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING  NEGLI‐
 * GENCE  OR  OTHERWISE)  ARISING  IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "memory.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"
#include "garbage_collector.h"

#define MODULE_NAME "memory.c"

#ifdef DEBUG_MEMORY
#include "debugging.h"
#else
#include "no_debugging.h"
#endif

static char *outOfMemoryMessage = "Ran out of Memory!";

void outOfMemory(void) {
    fprintf(stderr, "%s", outOfMemoryMessage);
    exit(EXIT_FAILURE);
}

void allocateNewExpressionBlock(struct Memory *mem) {
    int i;
    struct ExpressionBlock *block;

    DEBUG_PRINT("allocating one more block ...\n");

    block =
        (struct ExpressionBlock *)SAFE_MALLOC(sizeof(struct ExpressionBlock));
    if (block == NULL) outOfMemory();
    block->memory = (struct Expression *)SAFE_MALLOC(sizeof(struct Expression) *
                                                     MEMORY_BLOCK_SIZE);
    if (block->memory == NULL) outOfMemory();

    block->nextBlock = mem->exprBlocks;
    mem->exprBlocks = block;

    mem->nextExpr = NULL;
    for (i = 0; i < MEMORY_BLOCK_SIZE; i++) {
        MEMORY_DISPOSE_EXPRESSION(mem, (block->memory + i));
    }

}

struct Memory *newMemory(void) {
    struct Memory *mem = (struct Memory *)SAFE_MALLOC(sizeof(struct Memory));
    if (mem == NULL) outOfMemory();
    mem->exprBlocks = NULL;
    allocateNewExpressionBlock(mem);
    mem->outOfMemory = outOfMemory;
    return mem;
}

void deleteMemory(struct Memory *mem) {

    assert(mem);

    if (mem->exprBlocks) {
        if (mem->exprBlocks->memory) free(mem->exprBlocks->memory);
        free(mem->exprBlocks);
    }
    free(mem);

}

void resetMemory(struct Memory *mem) {

    (mem)->nextExpr = 0;

}

void dumpFreeExpressions(struct Memory *mem) {

    struct Expression *expr = 0;
    assert(mem);

    fprintf(stderr, "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n");

    for (expr = mem->nextExpr; 0 != expr;
         expr = (void *)EXPRESSION_STRING(expr)) {
        fprintf(stderr, "Free mem: %p\n", (void *)expr);
    }

    fprintf(stderr, ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");

}
