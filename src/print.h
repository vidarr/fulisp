/*
 * (C) 2010 Michael J. Beer
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

/**
 * Create textual descriptions of expressions in various ways
 */
#ifndef __PRINT_H__
#define __PRINT_H__

#include "config.h"

#include <stdio.h>
#include <string.h>
#include "environment.h"
#include "error.h"
#include "expression.h"
#include "lisp_internals.h"
#include "streams.h"

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
char *expressionToString(struct Expression *env, char *str, int sizeOfBuffer,
                         struct Expression *expr);

/**
 * prints an expression to a write stream
 * @param stream the stream to write to
 * @param expr the expr to be printed
 */
void printToStream(struct Expression *env, struct CharWriteStream *stream,
                   struct Expression *expr);

#endif
