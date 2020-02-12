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

#include "expression.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"
#include "environment.h"
#include "print.h"

#define MODULE_NAME "expression.c"

#ifdef DEBUG_EXPRESSION
#include "debugging.h"
#else
#include "no_debugging.h"
#endif

void expressionRelease(struct Expression *env, struct Expression *expr) {

    if(0 == expr) return;

    struct Memory *mem = ENVIRONMENT_GET_MEMORY(env);

    if(EXPR_DONT_FREE & expr->type) return;

    if (EXPR_IS_POINTER(expr)) {
        if (EXPR_OF_TYPE(expr, EXPR_ENVIRONMENT)) {
/*            environmentRelease(env, EXPRESSION_ENVIRONMENT(expr)); */
        } else if (EXPR_OF_TYPE(expr, EXPR_STRING) ||
                   EXPR_OF_TYPE(expr, EXPR_SYMBOL)) {
            free(EXPRESSION_STRING(expr));
        } else if (EXPR_OF_TYPE(expr, EXPR_LAMBDA)) {
/*            lambdaDispose(env, EXPRESSION_LAMBDA(expr)); */
        }
    }

    MEMORY_DISPOSE_EXPRESSION(mem, expr);

}

void expressionDispose(struct Expression *env, struct Expression *expr) {

#if GARBAGE_COLLECTOR == GC_REFERENCE_COUNTING

    if (!expr || EXPR_IS_NIL(expr) || expr == T) return;
    if (!EXPR_IS_VALID(expr)) {
        MEMORY_DISPOSE_EXPRESSION(ENVIRONMENT_GET_MEMORY(env), expr);
        return;
    }
    /* ALERT: Does not work with non-valid expressions - occur together with
     * parsing of conses */
    DEBUG_PRINT_PARAM("Dispose: Old counter : %u\n",
                      GC_GET_REF_COUNT(env, expr));
    DEBUG_PRINT_PARAM("   type %u \n", (int)EXPRESSION_TYPE(expr));
    DEBUG_PRINT_EXPR(env, expr);

    if (GC_GET_REF_COUNT(env, expr) > 0) {
        GC_DEC_REF_COUNT(env, expr);
    } else {
        expressionForceDispose(env, expr);
    }

#endif

}

void expressionForceDispose(struct Expression *env, struct Expression *expr) {

#if GARBAGE_COLLECTOR == GC_REFERENCE_COUNTING

#ifdef MEMORY_USE_PREALLOCATION
    struct Memory *mem;
#endif

    DEBUG_PRINT_PARAM("Disposing type %u \n", (int)EXPRESSION_TYPE(expr));

#ifdef MEMORY_USE_PREALLOCATION
    mem = ENVIRONMENT_GET_MEMORY(env);
#endif

    if(EXPR_DONT_FREE & expr->type) return;

    if (EXPR_IS_POINTER(expr)) {
        if (EXPR_OF_TYPE(expr, EXPR_CONS)) {
            DEBUG_PRINT("   expr is a cons cell - recursive disposing...\n");
            if (EXPRESSION_CAR(expr))
                expressionDispose(env, EXPRESSION_CAR(expr));
            if (EXPRESSION_CDR(expr))
                expressionDispose(env, EXPRESSION_CDR(expr));
            __EXPRESSION_DISPOSE_CONS_STRUCT(mem, expr);
        } else if (EXPR_OF_TYPE(expr, EXPR_ENVIRONMENT)) {
            environmentDispose(env, EXPRESSION_ENVIRONMENT(expr));
        } else if (EXPR_OF_TYPE(expr, EXPR_STRING) ||
                   EXPR_OF_TYPE(expr, EXPR_SYMBOL)) {
            free(EXPRESSION_STRING(expr));
        } else if (EXPR_OF_TYPE(expr, EXPR_LAMBDA)) {
            lambdaDispose(env, EXPRESSION_LAMBDA(expr));
        }
    }

    MEMORY_DISPOSE_EXPRESSION(mem, expr);

#endif

}

struct Expression *expressionCreate(struct Expression *env, unsigned char type,
                                    void *content, void *extension) {
    struct Expression *expr = 0;
    /* expr = malloc(sizeof(struct Expression)); */
    MEMORY_GET_EXPRESSION(ENVIRONMENT_GET_MEMORY(env), expr);
    GC_INIT_EXPRESSION(env, expr);
    expr->type = type;
    if (EXPR_IS_CONS(expr)) {
        /* Allocate mem for struct Cons */
        expressionAssign(env, content);
        expressionAssign(env, extension);
        __EXPRESSION_SET_CAR(expr, content);
        __EXPRESSION_SET_CDR(expr, extension);
    } else if (EXPR_IS_POINTER(expr)) {
        __EXPRESSION_STRING(expr) = (char *)content;
    } else {
        switch (type) {
            case EXPR_INTEGER:
                __EXPRESSION_INTEGER(expr) = *((int *)content);
                break;
            case EXPR_FLOAT:
                __EXPRESSION_FLOATING(expr) = *((float *)content);
                break;
            case EXPR_CHARACTER:
                __EXPRESSION_CHARACTER(expr) = *((char *)content);
                break;
            case EXPR_NATIVE_FUNC:
#ifndef STRICT_NATIVE_FUNCS
                __EXPRESSION_NATIVE_FUNC(expr) =
                    ((struct Expression *
                      (*)(struct Expression *, struct Expression *)) content);
#else
                ERROR(ERR_UNEXPECTED_TYPE,
                      "Cannot create native function expression via "
                      "expressionCreate. Use expressionCreateNativeFunc "
                      "instead!");
                return NIL;

#endif
                break;
            case EXPR_NO_TYPE:
                __EXPRESSION_STRING(expr) = 0;
        };
    }

    DEBUG_PRINT("Created new expression\n");
    return expr;
}

struct Expression *expressionCreateNativeFunc(struct Expression *env,
                                              NativeFunction *content) {
    struct Expression *expr;
    /* expr = malloc(sizeof(struct Expression)); */
    MEMORY_GET_EXPRESSION(ENVIRONMENT_GET_MEMORY(env), expr);
    GC_INIT_EXPRESSION(env, expr);
    expr->type = EXPR_NATIVE_FUNC;
    EXPRESSION_NATIVE_FUNC(expr) = content;
    DEBUG_PRINT("Created new expression containing native function.\n");
    return expr;
}

struct Expression *expressionCreateString(struct Expression *env, char *str) {
    char *buf;
    assert(str);

    /*TODO: As soon as the global lookup for symbols is installed,
      look up str in it and assign the data-slot a pointer to the entry in the
      table */
    buf = SAFE_STRING_NEW(strlen(str));
    strcpy(buf, str);
    return EXPRESSION_CREATE_ATOM(env, EXPR_STRING, buf);
}

struct Expression *createSymbol(struct Expression *env, char *str) {
    char *buf;
    assert(str);

    DEBUG_PRINT_PARAM("createSymbol(): Got '%s'\n", str);

    /*TODO: As soon as the global lookup for symbols is installed,
      look up str in it and assign the data-slot a pointer to the entry in the
      table */
    buf = SAFE_STRING_NEW(strlen(str));
    strcpy(buf, str);
    DEBUG_PRINT_PARAM("createSymbol(): Copied to  %s\n", buf);
    return EXPRESSION_CREATE_ATOM(env, EXPR_SYMBOL, buf);
}

struct Expression *expressionAssign(struct Expression *env,
                                    struct Expression *expr) {
    if (expr) GC_INC_REF_COUNT(env, expr);
    return expr;
}

struct Expression *createEnvironmentExpression(struct Environment *env) {
    struct Expression *expr;
    MEMORY_GET_EXPRESSION(env->memory, expr);
    expr->type = EXPR_ENVIRONMENT;
    GC_INIT_ENVIRONMENT(env);
    EXPRESSION_ENVIRONMENT(expr) = env;
    GC_INIT_EXPRESSION(expr, expr);
    return expr;
}

char nil[] = "NIL";

char true[] = "T";

char rest[] = "&REST";

struct Expression expressionNil = {
    EXPR_DONT_FREE | EXPR_SYMBOL, {nil}, {NULL} GC_INIT_EXPR_INFO(0)};

struct Expression expressionT = {
    EXPR_DONT_FREE | EXPR_SYMBOL, {true}, {NULL} GC_INIT_EXPR_INFO(0)};

struct Expression expressionRest = {
    EXPR_DONT_FREE | EXPR_SYMBOL, {rest}, {NULL} GC_INIT_EXPR_INFO(1)};

#define DEFINE_CONSTANT_SYMBOL(constantName, SymbolName)            \
    char char##constantName[] = SymbolName;                         \
    struct Expression constantName = {EXPR_DONT_FREE | EXPR_SYMBOL, \
                                      {char##constantName},         \
                                      {NULL} GC_INIT_EXPR_INFO(1)}

DEFINE_CONSTANT_SYMBOL(expressionInteger, ":INT");
DEFINE_CONSTANT_SYMBOL(expressionFloat, ":FLOAT");
DEFINE_CONSTANT_SYMBOL(expressionCharacter, ":CHAR");
DEFINE_CONSTANT_SYMBOL(expressionString, ":STRING");
DEFINE_CONSTANT_SYMBOL(expressionSymbol, ":SYMBOL");
DEFINE_CONSTANT_SYMBOL(expressionCons, ":CONS");
DEFINE_CONSTANT_SYMBOL(expressionLambda, ":LAMBDA");
DEFINE_CONSTANT_SYMBOL(expressionNativeFunc, ":NATIVEFUNC");
DEFINE_CONSTANT_SYMBOL(expressionEnvironment, ":ENV");
DEFINE_CONSTANT_SYMBOL(lispLibraryPath, "LISP-LIBRARY-PATH");
