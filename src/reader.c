/*
 * (C) 2010 Michael J. Beer * This program is free software; you can
 * redistribute it and/or modify * it under the terms of the GNU General Public
 * License as published by * the Free Software Foundation; either version 3 of
 * the License, or * (at your option) any later version.2010 *2010 * This
 * program is distributed in the hope that it will be useful,2010 * but WITHOUT
 * ANY WARRANTY; without even the implied warranty of * MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the * GNU General Public License for
 * more details.2010 2010 * You should have received a copy of the GNU General
 * Public License * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,
 * USA.2010
 */

#include "reader.h"
#include "error.h"
#include "config.h"


#define MODULE_NAME "reader.c"

#ifdef DEBUG_READER
#include "debugging.h"
#else
#include "no_debugging.h"
#endif



/******************************************************************************
 *                              Public Functions
 ******************************************************************************/



struct Reader *newReader(struct Expression *env, struct CharReadStream *stream) { 
    struct Reader *reader = malloc(sizeof(struct Reader));
    assert(env && stream);
    reader->type = EXPR_SYMBOL;
    reader->expr = (void *)0;
    reader->env = env;
    reader->lookup = malloc(sizeof(struct ReadMacroLookUp));
    registerStandardReadMacro(reader, rmIdentity);
    reader->lookup->next = 0;
    reader->lookup->sign = 0;
    reader->stream = stream;
    reader->buffer = (char *)malloc(sizeof(char) * READ_BUFFER_SIZE);
    reader->symbolTable = symbolTableCreate(READER_GET_ENVIRONMENT(reader));
    resetReader(reader);
    return reader;
}


void deleteReader(struct Reader *reader) {
    struct ReadMacroLookUp * entry, *nextEntry;

    assert(reader);

    entry = reader->lookup;

    if(reader) {
        if(reader->lookup) {
            while(entry) {
                nextEntry = entry->next;
                free(entry);
                entry = nextEntry;
            };
        };
        if(reader->buffer) free(reader->buffer);
        /* free all stored symbols */
        symbolTableDispose(READER_GET_ENVIRONMENT(reader), reader->symbolTable);
        free(reader);
    };
    reader = 0;
} 


void resetReader(struct Reader *reader) {
    assert(reader);
    if(reader->expr) {
        expressionDispose(READER_GET_ENVIRONMENT(reader), reader->expr); 
        reader->expr = (void *)0;
    };
    reader->type = EXPR_NO_TYPE;
    reader->current = reader->buffer;
}


struct Expression *fuRead(struct Reader *reader) {

    char readChar;
    NativeReadMacro macro;
    struct Expression *expr = 0;

    assert(reader);

    while(!reader->expr && (readChar = READ_NEXT_CHAR(reader))) {
        DEBUG_PRINT_PARAM(" Got '%c'\n", readChar);
        macro = LOOKUP_READ_MACRO((char)readChar, reader);
        DEBUG_PRINT_NATIVE_FUNC(macro);
        if(macro) {
            macro(reader, readChar);
            /* if the macro returned an expression, we are done  */
            if(reader->expr) {
                DEBUG_PRINT("fuRead() returns expr : ");
                DEBUG_PRINT_EXPR(READER_GET_ENVIRONMENT(reader), reader->expr);
                expr = reader->expr;
                reader->expr = 0;
                return expr;
            }
            /* Any Read macro should take care to restore the status of the
             * reader lookup to the state when it was invoked */
        } else {
            /* Any char not triggering any macro indicates that the read string
             * is indeed a symbol */
            /* Should never ever be reached !  */
            DEBUG_PRINT("read was unable to deal with readChar!");
        };
    };

    /* if not looked up yet, try to evaluate the input */
    if((macro = LOOKUP_READ_MACRO(0, reader))) {
        macro(reader, 0);
    } else 
        rmTerminator(reader, 0); /* If none registered, take the standard
                                    terminating macro */
    DEBUG_PRINT(" reached end of function\n"); 
    DEBUG_PRINT_EXPR(READER_GET_ENVIRONMENT(reader), reader->expr);

    return expressionAssign(READER_GET_ENVIRONMENT(reader), reader->expr);
}


NativeReadMacro lookupReadMacro(struct Reader *reader, char c) {
    struct ReadMacroLookUp *entry;

    assert(reader);
    if(!reader->lookup) return 0;
    /* if lookup does not contain any special macros, so give back the standard
       one */
    if(!reader->lookup->next) return reader->lookup->macro;
    entry = reader->lookup->next;
    while(entry->next && entry->sign != c) {
#ifdef DEBUG_READER
#ifdef DEBUG
        fprintf(stderr, "Search %c for %c\n", entry->sign, c);
#endif
#endif
        entry = entry->next;
    }

    /* if none found, return the standard read macro, i.e. the first in the list
     * */
    if(entry->sign != c) entry = reader->lookup;
#ifdef DEBUG
#ifdef DEBUG_READER
    fprintf(stderr, "lookupReadMacro(): %c is '%c'\n", c, entry->sign);
#endif
#endif
    return entry->macro;
}


NativeReadMacro registerStandardReadMacro(struct Reader *reader, NativeReadMacro macro) {
    NativeReadMacro old;
    assert(reader);
    old = reader->lookup->macro;
    reader->lookup->macro = macro;
    return old;
}


NativeReadMacro registerReadMacro(struct Reader *reader, unsigned char c, 
        NativeReadMacro macro) {

    NativeReadMacro old;
    struct ReadMacroLookUp *before, *entry = reader->lookup; 
    assert(reader);

    before = reader->lookup;

    while(entry->next && entry->next->sign != c) {
        before = entry;
        entry = entry->next;
    };

    if(entry->next && entry->next->sign == c) {
        DEBUG_PRINT_PARAM("registerLookup: Searched for '%c'", c);
        DEBUG_PRINT_PARAM("   (%u)  ", (unsigned int)c);
        DEBUG_PRINT_PARAM(" Found '%c'\n", entry->sign);
        if(!macro) {
            DEBUG_PRINT_PARAM("Removed '%c\n' from Read Macro Lookup\n", c);
            before->next->next = entry->next->next;
            old = entry->next->macro;
            free(entry->next);
            return old;
        };
        DEBUG_PRINT_PARAM("Replaced '%c\n' from Read Macro Lookup\n", c);
        old = entry->next->macro;
        entry->next->macro = macro;
        return old;
    };
    /* Macro entry does not yet exist - create it */
    DEBUG_PRINT_PARAM("Created entry for '%c\n' in Read Macro Lookup\n", c);
    entry->next = malloc(sizeof(struct ReadMacroLookUp));
    entry = entry->next;
    entry->macro = macro;
    entry->sign = c;
    entry->next = 0;
    return 0;
}



/**
 * Cast the content of reader->buffer to whatever type is given as reader->type
 * and store the expression in reader->expr
 */
void setExprOfReader(struct Reader *reader) {
    int i;
    float f;

    assert(reader);

    DEBUG_PRINT("setExprOfReader() invoked\n");

    /* if buffer is empty, there is nothin to convert anyways */
    if(reader->buffer == reader->current) {
        reader->expr = GET_SYMBOL(reader, "");
        return;
    };

    switch(reader->type) {
        case EXPR_SYMBOL:
            *reader->current = 0;
            reader->expr = GET_SYMBOL(reader, reader->buffer);
            DEBUG_PRINT_PARAM("   Created %s\n", reader->expr->data.string);
            break;

        case EXPR_CHARACTER:
            reader->expr = CREATE_CHAR_EXPRESSION(READER_GET_ENVIRONMENT(reader), *(reader->current - 1));
            DEBUG_PRINT_PARAM("   Created %c\n", reader->expr->data.character);
            *reader->current = 0;
            break;

        case EXPR_STRING:
            *reader->current = 0;
            reader->expr = CREATE_STRING_EXPRESSION(READER_GET_ENVIRONMENT(reader), reader->buffer); /* expressionCreate(reader->type, str); */
            DEBUG_PRINT_PARAM("   Created %s\n", reader->expr->data.string);
            break;

        case EXPR_INTEGER:
            *reader->current = 0;
            i = atoi(reader->buffer);
            reader->expr = CREATE_INT_EXPRESSION(READER_GET_ENVIRONMENT(reader), i); /* expressionCreate(reader->type, (void *)&i); */
            DEBUG_PRINT_PARAM("   Created %i\n", \
                    reader->expr->data.integer);
            break;

        case EXPR_FLOAT:
            *reader->current = 0;
            f = atof(reader->buffer);
            reader->expr = CREATE_FLOAT_EXPRESSION(READER_GET_ENVIRONMENT(reader), f);/* expressionCreate(reader->type, (void *)&f); */
            DEBUG_PRINT_PARAM("   Created %f\n", reader->expr->data.floating);
            break;

        case EXPR_NO_TYPE:
            reader->expr = CREATE_INVALID_EXPRESSION(READER_GET_ENVIRONMENT(reader));
            break;

        default:
            ERROR(ERR_UNEXPECTED_VAL, "Could not determine type of read expression");
    };

    *reader->current = 0;
    DEBUG_PRINT_PARAM("   Found  %s\n", reader->buffer);
    DEBUG_PRINT_PARAM("   Expr set: %s\n", (reader->expr) ? "yes" : "no");
    return;
}


void printLookup(struct Reader *reader) {
    struct ReadMacroLookUp *next;

    assert(reader && reader->lookup);

    next = reader->lookup;
    fprintf(stderr, "Lookup looks like :   ");

    while(next->next) {
        fprintf(stderr, "%c ", next->next->sign);
        next = next->next;
    };

    fprintf(stderr, "\n");
}


void rmIgnore(struct Reader *reader, char sigle) {
    /* well, yes, thats exactly what it does - nothing */
}


void rmIdentity(struct Reader *reader, char sigle) {
    assert(reader);
    PUSH_INTO_BUFFER(sigle, reader);
}


void rmTerminator(struct Reader *reader, char sigle) {
    assert(reader != 0);

    DEBUG_PRINT_PARAM("Read macro rmTerminator called with %c\n", sigle);
    if(reader->buffer != reader->current) {
        setExprOfReader(reader);
    };
}
