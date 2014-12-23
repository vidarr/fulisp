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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */ 

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "memory.h"
#include "config.h"
#include "garbage_collector.h"


#define MODULE_NAME "memory.c"

#ifdef DEBUG_MEMORY
#include "debugging.h"
#else
#include "no_debugging.h"
#endif


static char *outOfMemoryMessage = "Ran out of Memory!";


static void outOfMemory(void) {
    fprintf(stderr, "%s", outOfMemoryMessage);
    exit(EXIT_FAILURE);
}

void allocateNewExpressionBlock(struct Memory *mem) {
    int i;
    struct ExpressionBlock *block;
   
    block = (struct ExpressionBlock *)malloc(sizeof(struct ExpressionBlock));
    if(block == NULL) outOfMemory();
    block->memory = (struct Expression *)
        malloc(sizeof(struct Expression) * MEMORY_BLOCK_SIZE);
    if(block->memory == NULL) outOfMemory();
 
#   ifdef MEMORY_AUTOEXTEND
        block->nextBlock = mem->exprBlocks;
#   endif
    mem->exprBlocks = block;

    mem->nextExpr = NULL;
    for(i = 0; i < MEMORY_BLOCK_SIZE; i++) {
        MEMORY_DISPOSE_EXPRESSION(mem, (block->memory + i));
    }
}


void allocateNewConsBlock(struct Memory *mem) {
#   if EXPRESSION_FORMAT == EXPRESSION_FORMAT_EXPANDED
        int i;
        struct ConsBlock *block = (struct ConsBlock *)malloc(sizeof(struct ConsBlock));
        if(block == NULL) outOfMemory();
        block->memory = (struct Cons *)malloc(sizeof(struct Cons) * MEMORY_BLOCK_SIZE);
        if(block->memory == NULL) outOfMemory();
    
#       ifdef MEMORY_AUTOEXTEND
            block->nextBlock = mem->consBlocks;
#       endif
        mem->consBlocks = block;
        mem->nextCons = NULL;
        for(i = 0; i < MEMORY_BLOCK_SIZE; i++) {
            MEMORY_DISPOSE_CONS(mem, (block->memory + i));
        }
#   endif   /* EXPRESSION_FORMAT */

}


struct Memory *newMemory(void) {
#   ifdef MEMORY_USE_PREALLOCATION
       struct Memory *mem = (struct Memory *)malloc(sizeof(struct Memory));
       if(mem == NULL) outOfMemory(); 
       mem->exprBlocks = NULL;
       mem->consBlocks = NULL;
       allocateNewExpressionBlock(mem);
       allocateNewConsBlock(mem);
       mem->outOfMemory = outOfMemory;
       return mem;
#   else
       return NULL;
#   endif
}


void deleteMemory(struct Memory *mem) {
#   ifdef MEMORY_USE_PREALLOCATION

       assert(mem);

       if(mem->exprBlocks) {
           if(mem->exprBlocks->memory) free(mem->exprBlocks->memory);
           free(mem->exprBlocks);
       }
#        if EXPRESSION_FORMAT == EXPRESSION_FORMAT_EXPANDED
            if(mem->consBlocks) {
                if(mem->consBlocks->memory) free(mem->consBlocks->memory);
                free(mem->consBlocks);
            }
#        endif   /* EXPRESSION_FORMAT */
       free(mem);
#   endif   /* MEMORY_USE_PREALLOCATION */
}

