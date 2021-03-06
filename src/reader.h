/*
 * (C) 2012 Michael J. Beer
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
#ifndef __READER_H__
#define __READER_H__

#include "config.h"

#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cons.h"
#include "constants.h"
#include "environment.h"
#include "expression.h"
#include "futypes.h"
#include "lisp_internals.h"
#include "print.h"
#include "streams.h"
#include "symboltable.h"

/******************************************************************************
 *                               Structs
 ******************************************************************************/

struct Reader;

/**
 * A read macro is a function that takes over parsing the input provided by a
 * reader
 * @param reader the Reader to read from and store the result to
 * @param sigle  the character that caused the invokation of this macro. This
 * char does not appear within the buffer, so if this char should be kept, the
 * read macro has to place it there
 */
typedef void (*NativeReadMacro)(struct Reader *, char);

/**
 * Lookup entry for read macros
 * such a lookup is a linked list
 * the very first entry is not included in the search
 * its macro resembles the 'default' macro
 * returned if none fitting could be found
 * inserted/ deleted is at the beginning of the list
 */
struct ReadMacroLookUp {
    struct ReadMacroLookUp *next;
    char sign;
    NativeReadMacro macro;
};

/**
 * A Reader is a set of read macros that have been assigned a char as identifier
 * getNextChar resembles the stream that is read from
 * type is the current estimated type of the expression that will be returned
 * buffer is the input buffer. The length of this buffer indicates the max.
 * length of symbols/strings
 * expr provides the posibility to handle return values
 * every NativeReadMacro can either build an expression or just transform the
 * input from one char into some string and place it in the input buffer
 * therefor, the results are stored within the reader rather than returning them
 */
struct Reader {
    struct ReadMacroLookUp *lookup;
    struct HashTable *symbolTable;
    struct Expression *env;
    struct Expression *expr;
    struct CharReadStream *stream;
    char *buffer;
    char *current;
    int type;
};

/******************************************************************************
 *                              Public Functions
 ******************************************************************************/

struct Expression *fuRead(struct Reader *reader);

/**
 * Create a new reader environment
 * @param getNextChar function that wraps the stream to be read from
 */
struct Reader *newReader(struct Expression *env, struct CharReadStream *stream);

/**
 * Frees Reader structure
 * @param reader the reader to be freed
 */
void deleteReader(struct Reader *reader);

/**
 * Resets the Reader structure
 * @param reader the reader to be reset
 */
void resetReader(struct Reader *reader);

/**
 * Lookup the given char, if not found, the standard macro is returned
 * @param reader the reader used
 * @param c the character to be looked up
 * @return the ReadMacro associated with character c or the standard macro if
 *         none has been assigned
 */
NativeReadMacro lookupReadMacro(struct Reader *reader, char c);

/**
 * Registers the read macro to call if none other does apply
 * @param reader the reader to register to
 * @param macro  the macro to become the standard macro
 */
NativeReadMacro registerStandardReadMacro(struct Reader *reader,
                                          NativeReadMacro macro);

/**
 * Inserts a read macro for sign c into reader reader
 * if the sign has been assigned before, the assigned macro will be replaced
 * if the given macro is 0, the entry for sign c will be removed if found
 * @return the macro found within lookup for sign c or 0 if there has not been
 * one
 */
NativeReadMacro registerReadMacro(struct Reader *reader, unsigned char c,
                                  NativeReadMacro macro);

void printLookup(struct Reader *reader);

/* coerce read string to expression */

void setExprOfReader(struct Reader *reader);

/**
 * ReadMacro that ignores read character
 */
void rmIgnore(struct Reader *reader, char sigle);

/**
 * Standard ReadMacro
 * just push read char to buffer
 */
void rmIdentity(struct Reader *reader, char sigle);

/**
 * Finish reading from stream
 */
void rmTerminator(struct Reader *reader, char sigle);

/******************************************************************************
 *                                M A C R O S
 ******************************************************************************/

#define LOOKUP_READ_MACRO(x, r) lookupReadMacro(r, x)

#ifdef DEBUG_READER
#ifdef DEBUG
#define PUSH_INTO_BUFFER(x, r)                         \
    {                                                  \
        fprintf(stderr, "Pushed %c into buffer\n", x); \
        *(r->current) = x;                             \
        (r->current)++;                                \
    }

#define DEBUG_PRINT_MACRO_LOOKUP(r) printLookup(r)

#endif
#endif

#ifndef PUSH_INTO_BUFFER

#define PUSH_INTO_BUFFER(x, r) \
    {                          \
        *(r->current) = x;     \
        (r->current)++;        \
    }

#define DEBUG_PRINT_MACRO_LOOKUP(r)

#endif

#define BUFFER_IS_EMPTY(r) (r->current == r->buffer)

#define PUSH_INTO_BUFFER_X(x, r)                                          \
    {                                                                     \
        IF_SAFETY_HIGH(if ((r->current - r->buffer) > READ_BUFFER_SIZE) { \
            ERROR(ERR_BUFFER_OVERFLOW);                                   \
        });                                                               \
        *(r->current) = x;                                                \
        (r->current)++;                                                   \
    }

#define READ_NEXT_CHAR(r) STREAM_NEXT(r->stream)

#define GET_SYMBOL(r, x) \
    symbolTableGetSymbol(READER_GET_ENVIRONMENT(r), r->symbolTable, x)

#define READER_GET_ENVIRONMENT(reader) (reader->env)

#endif
