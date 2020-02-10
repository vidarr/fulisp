/*
 * (C) 2010, 2012 Michael J. Beer
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

#include <stdio.h>
#include <stdlib.h>

#include "cons.h"
#include "constants.h"
#include "expression.h"
#include "print.h"

#define MODULE_NAME "print.c"

#ifdef DEBUG_PRINT
#include "debugging.h"
#else
#include "no_debugging.h"
#endif

#ifdef STRICT_NATIVE_FUNCS
#include "printnativefunctions.h"
#endif

#define STRING_SET_TO_END(str) \
    {                          \
        while (*(str++) != 0)  \
            ;                  \
        str--;                 \
    }

struct Expression *fuPrint(struct Expression *env, struct Expression *expr) {
    char *retStr, *buf = SAFE_STRING_NEW(READ_BUFFER_SIZE);
    struct Expression *res = EXPRESSION_CREATE_ATOM(
        env, EXPR_STRING, expressionToString(env, buf, READ_BUFFER_SIZE, expr));
    retStr = SAFE_STRING_NEW(strlen(res->data.string));
    SAFE_STRCPY(retStr, buf, READ_BUFFER_SIZE);
    res->data.string = retStr;
    free(buf);
    return res;
}

char *expressionToString(struct Expression *env, char *str, int sizeOfBuffer,
                         struct Expression *expr) {
    struct CharWriteStream *stream =
        makeStringCharWriteStream(sizeOfBuffer, str);
    printToStream(env, stream, expr);
    STREAM_DISPOSE(stream);
    return str;
}

void printToStream(struct Expression *env, struct CharWriteStream *stream,
                   struct Expression *expr) {
    /* Macros for printToStream()
       These macros rely on some variables to exist & be initialized properly!
       Should be used internally to printToStream() ONLY! */

#define PRINT_STREAM(x)                                \
    {                                                  \
        char *b = x;                                   \
        for (; *b != 0; b++) STREAM_WRITE(stream, *b); \
    }

#define PRINT_EXPR(type, strType, data)                       \
    {                                                         \
        size_t bufLen = MAX_BYTES_PER_##type + 6 + 2;         \
        buf = SAFE_STRING_NEW(bufLen);                        \
        SAFE_SPRINTF(buf, bufLen, #type ": " #strType, data); \
        PRINT_STREAM(buf);                                    \
        free(buf);                                            \
    }

#ifdef STRICT_NATIVE_FUNCS

    unsigned char *fp;
    int i;
    unsigned char low, high;

#define PRINT_NATIVE_FUNC(x)                             \
    {                                                    \
        fp = (unsigned char *)&(x);                      \
        for (i = 0; i < sizeof(NativeFunction *); i++) { \
            /* Convert char to hex representation */     \
            low = fp[i] & (16 | 8 | 4 | 2 | 1);          \
            high = fp[i] >> 4;                           \
            low = TO_HEX(low);                           \
            high = TO_HEX(high);                         \
            STREAM_WRITE(stream, ' ');                   \
            STREAM_WRITE(stream, high);                  \
            STREAM_WRITE(stream, low);                   \
        };                                               \
        STREAM_WRITE(stream, '\0');                      \
    }

#else

#define PRINT_NATIVE_FUNC(x) PRINT_EXPR(pointer, % p, (void *)(x))

#endif

    int exprType;
    char *buf;
#define BUFTOWRITETO_LENGTH 200
    char bufToWriteTo[BUFTOWRITETO_LENGTH + 1];
    struct Expression *e1, *e2;

    assert(stream);
    bufToWriteTo[BUFTOWRITETO_LENGTH] = '\0';

    if (!expr) {
        DEBUG_PRINT("expressionToString(): NIL");
        PRINT_STREAM("INVALID");
        return;
    }

    switch (EXPRESSION_TYPE(expr)) {
        case EXPR_INTEGER:
            DEBUG_PRINT_PARAM("expressionToString(): INT: %i\n",
                              &(expr->data.integer));
            PRINT_EXPR(int, % i, expr->data.integer);
            break;
        case EXPR_FLOAT:
            DEBUG_PRINT_PARAM("expressionToString(): FLT: %f\n",
                              expr->data.floating);
            PRINT_EXPR(float, % f, expr->data.floating);
            break;
        case EXPR_CHARACTER:
            DEBUG_PRINT_PARAM("expressionToString(): CHR: %ci\n",
                              expr->data.character);
            buf = "CHR: ";
            PRINT_STREAM(buf);
            STREAM_WRITE(stream, expr->data.character);
            break;
        case EXPR_STRING:
            DEBUG_PRINT_PARAM("expressionToString(): STR: %s\n",
                              expr->data.string);
            buf = "STR: ";
            PRINT_STREAM(buf);
            PRINT_STREAM(expr->data.string);
            break;
        case EXPR_SYMBOL:
            DEBUG_PRINT_PARAM("expressionToString(): SYM: %s\n",
                              expr->data.string);
            buf = "SYM: ";
            PRINT_STREAM(buf);
            PRINT_STREAM(expr->data.string);
            break;
        case EXPR_NATIVE_FUNC:
            DEBUG_PRINT_PARAM("expressionToString(): FNC: %p\n",
                              expr->data.nativeFunc);
            buf = "FNC: ";
            PRINT_STREAM(buf);
            PRINT_NATIVE_FUNC(expr->data.nativeFunc);
            break;
        case EXPR_LAMBDA:
            DEBUG_PRINT_PARAM("expressionToString(): LAMBDA Expression: %p\n",
                              expr->data.lambda);
            buf = "LMB: ";
            PRINT_STREAM(buf);
            PRINT_EXPR(pointer, % p, ((void *)expr->data.lambda));
            break;
        case EXPR_CONS:
            DEBUG_PRINT("expressionToString(): CONS");
            STREAM_WRITE(stream, '(');
            e1 = intCar(env, expr);
            IF_DEBUG(
                if (!e1) { DEBUG_PRINT("expressionToString(): car is 0!"); });
            printToStream(env, stream, e1);
            e1 = intCdr(env, expr);
            while (EXPR_IS_CONS(e1)) {
                e2 = intCar(env, e1);
                STREAM_WRITE(stream, ' ');
                printToStream(env, stream, e2);
                e2 = intCdr(env, e1);
                e1 = e2;
            }
            if (!EXPR_IS_NIL(e1)) {
                STREAM_WRITE(stream, ' ');
                buf = DOTTED_PAIR_MARKER_STRING;
                PRINT_STREAM(buf);
                STREAM_WRITE(stream, ' ');
                printToStream(env, stream, e1);
            };
            STREAM_WRITE(stream, ')');
            break;
        case EXPR_ENVIRONMENT:
            DEBUG_PRINT_PARAM("expressionToString(): ENV Expression: %p\n",
                              expr->data.env);
            buf = "ENV: ";
            PRINT_STREAM(buf);
            PRINT_EXPR(pointer, % p, ((void *)expr->data.env));
            break;
        case EXPR_NO_TYPE:
            DEBUG_PRINT(
                "expressionToString(): "
                "INVALID Expression\n");
            buf = "INVALID EXPRESSION";
            PRINT_STREAM(buf);
            break;
        default:
            exprType = EXPRESSION_TYPE(expr);
            DEBUG_PRINT_PARAM(
                "expressionToString(): "
                "UNKNOWN Expression of type %ui\n",
                exprType);
            SAFE_SPRINTF(bufToWriteTo, BUFTOWRITETO_LENGTH,
                         "UNKNOWN expression type %ui", exprType);
            PRINT_STREAM(bufToWriteTo);
            break;
    };

/* Prevent to use macros 'internal' to printToStream() elsewhere,
 * would probably not be intented! */
#undef PRINT_STREAM
#undef PRINT_EXPR
#undef PRINT_NATIVE_FUNC
}
