/*
 * (C) 2011 Michael J. Beer * This program is free software; you can
 * redistribute it and/or modify * it under the terms of the GNU General Public
 * License as published by * the Free Software Foundation; either version 3 of
 * the License, or * (at your option) any later version.2010 *2010 * This
 * program is distributed in the hope that it will be useful,2010 * but WITHOUT
 * ANY WARRANTY; without even the implied warranty of * MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the * GNU General Public License for
 * more details.2010 2010 * You should have received a copy of the GNU General
 * Public License * along with this program; if not, write to the Free Software
 * * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,
 * USA.2010
 */

#include "fulispreader.h"


#define MODULE_NAME "fulispreader.c"

#ifdef DEBUG_READER
#include "debugging.h"
#else
#include "no_debugging.h"
#endif


/*******************************************************************************
 *                           R E A D   M A C R O S
 *******************************************************************************/



/**
 * push toupper(char) to buffer
 */
static void rmStandard(struct Reader *reader, char sigle);

/**
 * read in an integer
 */
static void rmInteger(struct Reader *reader, char sigle); 

/**
 * read in a float
 */
static void rmFloat(struct Reader *reader, char sigle); 

/* read in a string */

static void rmString(struct Reader *reader, char sigle);

static void rmStringEnd(struct Reader *reader, char sigle);

static void rmUnexpectedEndOfString(struct Reader *reader, char sigle);

/* Read in a char */

static void rmCharacter(struct Reader *reader, char sigle);

static void rmGetNextAsCharacter(struct Reader *reader, char sigle);

/* read in cons */

static void rmClosingBraket(struct Reader *reader, char sigle);

static void rmUnexpectedEndOfCons(struct Reader *reader, char sigle);

static void rmOpeningBraket(struct Reader *reader, char sigle);

/* fulisp - specific terminator macro */

static void rmFuLispTerminator(struct Reader *reader, char sigle);



static void registerStandardReadMacros(struct Reader *reader);



/******************************************************************************
 *                        I M P L E M E N T A T I O N 
*******************************************************************************/



struct Reader *newFuLispReader(struct Expression *env, struct CharReadStream *stream) { 
    struct Reader *reader = newReader(env, stream);
    assert(stream);
    /* Now register all the specific read macros and sign handlers */
    registerStandardReadMacros(reader);
    /* Register constant symbols */
    hashTableSet(reader->symbolTable, ((NIL)->data.string), NIL);
    hashTableSet(reader->symbolTable, ((T)->data.string), T);
    hashTableSet(reader->symbolTable, ((REST)->data.string), REST);
    return reader;
}


static void registerStandardReadMacros(struct Reader *reader) {

    IF_DEBUG( struct ReadMacroLookUp *next;);

    assert(reader);

    /* This is the standard read macro */
    registerStandardReadMacro(reader, rmStandard);

    /* Register all the macros */
    registerReadMacro(reader, (unsigned char)' ', rmFuLispTerminator);
    registerReadMacro(reader, (unsigned char)'\r', rmFuLispTerminator);
    registerReadMacro(reader, (unsigned char)'\n', rmFuLispTerminator);
    registerReadMacro(reader, (unsigned char)'1', rmInteger);
    registerReadMacro(reader, (unsigned char)'2', rmInteger);
    registerReadMacro(reader, (unsigned char)'3', rmInteger);
    registerReadMacro(reader, (unsigned char)'4', rmInteger);
    registerReadMacro(reader, (unsigned char)'5', rmInteger);
    registerReadMacro(reader, (unsigned char)'6', rmInteger);
    registerReadMacro(reader, (unsigned char)'7', rmInteger);
    registerReadMacro(reader, (unsigned char)'8', rmInteger);
    registerReadMacro(reader, (unsigned char)'9', rmInteger);
    registerReadMacro(reader, (unsigned char)'0', rmInteger);
    registerReadMacro(reader, (unsigned char)'+', rmInteger);
    registerReadMacro(reader, (unsigned char)'-', rmInteger);
    registerReadMacro(reader, (unsigned char)'.', rmFloat);
    registerReadMacro(reader, (unsigned char)'"', rmString);
    registerReadMacro(reader, (unsigned char)0, rmFuLispTerminator);
    registerReadMacro(reader, (unsigned char)'(', rmOpeningBraket);
    registerReadMacro(reader, (unsigned char)')', rmClosingBraket);
    registerReadMacro(reader, (unsigned char)'\\', rmGetNextAsCharacter);

    IF_DEBUG( 
            fprintf(stderr, "Lookup looks like:\n");
            next = reader->lookup;
            while((next = next->next)) {
            fprintf(stderr, "Entry for %c\n", (next->sign) ? next->sign : '`');
            };
    );

}



/*****************************************************************************k
 *                                Read Macros   
 *****************************************************************************/



static void rmStandard(struct Reader *reader, char sigle) {
    char b;

    assert(reader);

    DEBUG_PRINT_PARAM("Read macro rmStandard called with %c\n", sigle);

    reader->type = EXPR_SYMBOL;
    b = toupper(sigle);
    PUSH_INTO_BUFFER(b, reader);
}




static void rmInteger(struct Reader *reader, char sigle) {
    assert(reader);

    DEBUG_PRINT_PARAM("Read macro rmInteger called with %c\n", sigle);
    /* If its the first sign OR the expression has been of type INTEGER before, 
       it is an integer */
    if(reader->current == reader->buffer) {
        reader->type = EXPR_INTEGER;
    } else 
        if(sigle == '+' || sigle == '-')
            /* if a plus or minus appears in the middle of a sequence of digits,
             * its no number any more... */
            reader->type = EXPR_SYMBOL;
    PUSH_INTO_BUFFER(sigle, reader);
}


static void rmFloat(struct Reader *reader, char sigle) {
    assert(reader);
    DEBUG_PRINT_PARAM("Read macro rmFloat called with %c\n", sigle);
    if(sigle == '.') {
        if(reader->type == EXPR_INTEGER ||
                reader->buffer == reader->current)
            /* If it is the first point within the expression, its a float */
            reader->type = EXPR_FLOAT;
        else 
            reader->type = EXPR_SYMBOL;
    } else
        reader->type = EXPR_FLOAT;

    PUSH_INTO_BUFFER(sigle, reader);
}


static void rmString(struct Reader *reader, char sigle) {
    struct Reader *nestedReader;
    assert(reader);
    DEBUG_PRINT_PARAM("Read macro rmString called with %c\n", sigle);

    if(!BUFFER_IS_EMPTY(reader)) {
        ERROR(ERR_SYNTAX_ERROR, "rmString(): Leading characters before String - Missing ' '?");
        reader->expr = NIL; /* GET_SYMBOL(reader, "NIL"); */
        return;
    }
    /* We need a new, 'clean' reader to prevent any char to trigger anything
     * except the end of the String */
    nestedReader = newReader(READER_GET_ENVIRONMENT(reader), reader->stream);
    reader->type = nestedReader->type = EXPR_STRING;

    registerStandardReadMacro(nestedReader, rmIdentity);
    registerReadMacro(nestedReader, sigle, rmStringEnd);
    registerReadMacro(nestedReader, '\0', rmUnexpectedEndOfString);
    reader->expr = fuRead(nestedReader);
    deleteReader(nestedReader);
}


static void rmStringEnd(struct Reader *reader, char sigle) {
    assert(reader);
    DEBUG_PRINT_PARAM("Read macro rmStringEnd called with %c\n", sigle);

    registerReadMacro(reader, sigle, rmString);
    rmFuLispTerminator(reader, sigle);
}


static void rmUnexpectedEndOfString(struct Reader *reader, char sigle) {
    ERROR(ERR_UNEXPECTED_END_OF_STRING, "String terminated without ending \"");
    reader->expr = NIL; /* GET_SYMBOL(reader, "NIL"); */
}


static void rmCharacter(struct Reader *reader, char sigle) {
    assert(reader);
    DEBUG_PRINT_PARAM("rmCharacter: Called with %c\n", sigle);
    reader->type = EXPR_CHARACTER;
    PUSH_INTO_BUFFER(sigle, reader);
    /* Thats it, the expression is ready to be created ... */
    rmFuLispTerminator(reader, sigle);
    DEBUG_PRINT_PARAM("rmCharacter: Expression has been set: %s\n",
            reader->expr ? "yes" : "no");
}


static void rmGetNextAsCharacter(struct Reader *reader, char sigle) {
    struct ReadMacroLookUp *oldTable;
    NativeReadMacro oldStdMacro;

    assert(reader);

    oldStdMacro = registerStandardReadMacro(reader, rmCharacter);
    oldTable = reader->lookup->next;
    reader->lookup->next = 0;
    reader->expr = fuRead(reader);
    registerStandardReadMacro(reader, oldStdMacro);
    reader->lookup->next = oldTable;
}


static void rmClosingBraket(struct Reader *reader, char sigle) {
    assert(reader);

    ERROR(ERR_UNBALANCED_PARENTHESES,"Closing braket without opening braket found");
    reader->expr = NIL; /* GET_SYMBOL(reader, "NIL"); */
}


static void rmUnexpectedEndOfCons(struct Reader *reader, char sigle) {
    ERROR(ERR_UNEXPECTED_END_OF_STRING, "List terminated without ending \"");
    reader->expr = CREATE_INVALID_EXPRESSION(READER_GET_ENVIRONMENT(reader));
}


static void rmClosingBraketWithinCons(struct Reader *reader, char sigle) {
    DEBUG_PRINT("rmClosingBraketWithinCons called!\n");
    if(!BUFFER_IS_EMPTY(reader)) {
        rmFuLispTerminator(reader, 0);
        STREAM_PUSH_BACK(reader->stream, sigle);
        return;
    }
    /* Perhaps another rm without these lines could be act. if '.' within cons
     * is recognized to start building cons cells without implicit NIL at the
     * end? */
    /* reader->expr = !EXPR_IS_NIL(reader->expr) ? cons(reader->expr, */
            /* GET_SYMBOL(reader, "NIL")) : CREATE_SYMBOL("NIL"); */
    reader->expr = CREATE_INVALID_EXPRESSION(READER_GET_ENVIRONMENT(reader));
}


static void rmOpeningBraket(struct Reader *reader, char sigle) {
    struct Expression *retVal, *current, *next, *expr, *nil;
    NativeReadMacro oldMacro, oldClosingBraket;
    int dottedPair = 0;
    assert(reader);

    if(!BUFFER_IS_EMPTY(reader)) {
        rmFuLispTerminator(reader, 0);
        STREAM_PUSH_BACK(reader->stream, sigle);
        return;
    }
    nil = NIL;

    if(reader->type != EXPR_NO_TYPE) {
        ERROR(ERR_SYNTAX_ERROR, "Unexpected opening braket");
        /* counter of nil has already the correct value */
        reader->expr = nil;
        return;
    }

    reader->type = EXPR_CONS;

    DEBUG_PRINT_MACRO_LOOKUP(reader);

    /* save old terminal macro */
    oldMacro = registerReadMacro(reader, 0, rmUnexpectedEndOfCons);
    oldClosingBraket =  registerReadMacro(reader, ')', rmClosingBraketWithinCons);

    DEBUG_PRINT_MACRO_LOOKUP(reader);

    resetReader(reader);
    expr = fuRead(reader);
    if(!NO_ERROR) {
        expressionDispose(READER_GET_ENVIRONMENT(reader), expr);
        /* counter of nil has already the correct value */
        reader->expr = nil; 
        return; 
    }
    if(!EXPR_IS_VALID(expr)) {
        expressionDispose(READER_GET_ENVIRONMENT(reader), expr);
        /* counter of nil has already the correct value */
        reader->expr = EXPRESSION_CREATE_CONS(READER_GET_ENVIRONMENT(reader),  
                 NIL, NIL);
        return; 
    }
    DEBUG_PRINT(" Create first cons...");
    retVal = current = EXPRESSION_CREATE_CONS(READER_GET_ENVIRONMENT(reader), 
                                              expr, NIL);
        /* intCons(READER_GET_ENVIRONMENT(reader), expr, nil); */
    expressionDispose(READER_GET_ENVIRONMENT(reader), expr);
    
    resetReader(reader);
    while((expr = fuRead(reader)) && EXPR_IS_VALID(expr)) { 
        resetReader(reader);

        if(!NO_ERROR) {
            expressionDispose(READER_GET_ENVIRONMENT(reader), retVal);
            expressionDispose(READER_GET_ENVIRONMENT(reader), expr);
            reader->expr = NIL;
            return;
        }

        /* If dottedPair >= 2, it means that there have been more than one
         * expression behind the 'dot' of a dotted pair */
        if((dottedPair >= 2)) {
            expressionDispose(READER_GET_ENVIRONMENT(reader), retVal);
            expressionDispose(READER_GET_ENVIRONMENT(reader), expr);
            /* Dispose of rest of this list ... */
            while((expr = fuRead(reader)) && EXPR_IS_VALID(expr)) {
                resetReader(reader);
                expressionDispose(READER_GET_ENVIRONMENT(reader), expr); 
            };
            reader->expr = NIL;
            ERROR(ERR_SYNTAX_ERROR, "Dotted Pair: Expected exactly one expression after dot, got more");
            return;
        }

        if(dottedPair == 1) {
            EXPRESSION_SET_CDR(current, expr);
            DEBUG_PRINT_EXPR(READER_GET_ENVIRONMENT(reader), retVal);
            dottedPair = 2;
        } else if(EXPR_OF_TYPE(expr, EXPR_SYMBOL) && (strcmp(EXPRESSION_STRING(expr), DOTTED_PAIR_MARKER_STRING) == 0)) {
            dottedPair = 1;
        } else {
            /* Here, the counter of nil is raised. It has been one too much already,
             * so remember to perform one expressionDispose on it after the loop or
             * recycle nil somehow without raising the counter */
            next = EXPRESSION_CREATE_CONS(READER_GET_ENVIRONMENT(reader),
                                          expr, NIL);
            expressionDispose(READER_GET_ENVIRONMENT(reader), expr);
            expressionAssign(READER_GET_ENVIRONMENT(reader), next);
            EXPRESSION_SET_CDR(current, next);
            current = next;
            expressionDispose(READER_GET_ENVIRONMENT(reader), next);
            DEBUG_PRINT_EXPR(READER_GET_ENVIRONMENT(reader), retVal);
        }
   }

    /* Prevent nested brakets to get confused, e.g. if '))' is encountered */
    *reader->current = 0;
    /* Get rid of the last 'empty' expr that signaled the end of list  */
    expressionDispose(READER_GET_ENVIRONMENT(reader), expr);
    expressionDispose(READER_GET_ENVIRONMENT(reader), nil);
    reader->expr = retVal;

    /* Restore old terminal macro */
    registerReadMacro(reader, 0, oldMacro);
    registerReadMacro(reader, ')', oldClosingBraket);
}


static void rmFuLispTerminator(struct Reader *reader, char sigle) {
    char c = 0;
    if(reader->type == EXPR_INTEGER && reader->current != reader->buffer) {
        c = *(reader->current - 1);
        if(c == '+' || c == '-') {
            reader->type = EXPR_SYMBOL;
        }
    }  else if (reader->type == EXPR_FLOAT) {
        c = *(reader->current - 1);
        if((c == '.') && (c == *(reader->buffer))) {
            reader->type = EXPR_SYMBOL;
        }
    }
    rmTerminator(reader, sigle);
}
 
