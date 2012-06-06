/**
 * (C) 2010 Michael J. Beer
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

/**
 * Create textual descriptions of expressions in various ways
 */
#ifndef __PRINT_H__
#define __PRINT_H__

#include "config.h"

#include <stdio.h>
#include <string.h>
#include "expression.h"
#include "error.h"
#include "lisp_internals.h"
#include "streams.h"
#include "environment.h"

/* Read chars from C streams */
/* This macro is potentially dangerous - be sure to have the variable int
 * readChar defined ! */
#define READCHARFROMSTREAM(env, stream) (((readChar = fgetc(stream)) == EOF) ? \
                                       NIL : expressionCreate(env, EXPR_CHARACTER, \
                                       (void *)&readChar))

/**
 * Reads one char from a stream and returns it as an expression - or NIL if
 * something went wrong
 */
struct Expression *getFileNextChar(struct Expression *env, FILE *stream);

/**
 * Gives a string representation of an expression
 * @param expr the expression to be described
 * @return en expression containing an string describing the expression
 */
struct Expression *fuPrint(struct Expression *env, struct Expression *expr);


/**
 * TODO: Replace by appr. calls to printToStream()
 * Gives a string representation of an expression
 * @param str pointer to a buffer to be written to.
 *            Its size wont be altered so buffer overflows are possible !
 * @param sizeOfBuffer size of the buffer
 *                     if SAFETY_CODE is enabled, buffer overflows are prevented
 * @param expr pointer to the expression to be printed 
 * @return pointer to the string representation (should be the pointer given as
 *         'str'
 */
char *expressionToString(struct Expression *env, char *str, int sizeOfBuffer, struct Expression *expr);


/**
 * prints an expression to a write stream
 * @param stream the stream to write to
 * @param expr the expr to be printed
 */
void printToStream(struct Expression *env, struct CharWriteStream *stream, struct Expression *expr);


#endif
