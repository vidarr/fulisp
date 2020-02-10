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

#include "nativefunctions.h"
#include "error.h"
#include "expression.h"
#include "garbage_collector.h"
#include "safety.h"
#include "operating_system.h"
#include "fulisp.h"
#include <errno.h>

#define MODULE_NAME "nativefunctions.c"

#ifdef DEBUG_NATIVE_FUNCTIONS
#include "debugging.h"
#else
#include "no_debugging.h"
#endif

/*****************************************************************************
 *                                  BASIC FUNCTIONS
 *****************************************************************************/

struct Expression *quote(struct Expression *env, struct Expression *expr) {
    INIT_NATIVE_FUNCTION("quote", env, expr);
    DEBUG_PRINT_EXPR(env, expr);
    if (EXPR_IS_NIL(expr)) return NIL;
    return expressionAssign(env, intCar(env, expr));
}

struct Expression *set(struct Expression *env, struct Expression *expr) {
    struct Expression *sym, *val, *rest = NULL;

    INIT_NATIVE_FUNCTION("set", env, expr);

    ASSIGN_2_PARAMS_WITH_REST(env, expr, sym, val, rest,
                              "set(): 2 arguments expected");
    if (!EXPR_OF_TYPE(sym, EXPR_SYMBOL)) {
        ERROR(ERR_UNEXPECTED_TYPE, "set(): expects SYMBOL VALUE as argument");
        return NULL;
    };

    val = eval(env, val);
    DEBUG_PRINT("Setting \n");
    DEBUG_PRINT_EXPR(env, sym);
    DEBUG_PRINT_EXPR(env, val);

    if (!EXPR_IS_NIL(rest)) {
        DEBUG_PRINT("set(): Using non-standard environment");
        rest = eval(env, rest);
        if (!EXPR_OF_TYPE(rest, EXPR_ENVIRONMENT)) {
            expressionDispose(env, rest);
            ERROR(ERR_UNEXPECTED_TYPE,
                  "set(): expects ENVIRONMENT VALUE as argument");
            return NIL;
        }
        environmentUpdate(rest, sym, val);
        expressionDispose(env, rest);
    } else {
        environmentUpdate(env, sym, val);
    }

    return val;
}

struct Expression *define(struct Expression *env, struct Expression *expr) {
    struct Expression *sym, *val, *rest, *pEnv = env;

    INIT_NATIVE_FUNCTION("define", env, expr);

    ASSIGN_2_PARAMS_WITH_REST(env, expr, sym, val, rest,
                              "define(): 2 arguments expected");
    if (!EXPR_OF_TYPE(sym, EXPR_SYMBOL)) {
        ERROR(ERR_UNEXPECTED_TYPE,
              "define(): expects SYMBOL VALUE as argument");
        return NIL;
    };
    val = eval(env, val);

    DEBUG_PRINT("Setting \n");
    DEBUG_PRINT_EXPR(env, sym);
    DEBUG_PRINT_EXPR(env, val);

    if (!EXPR_IS_NIL(rest)) {
        DEBUG_PRINT("define(): Using non-standard environment\n");
        rest = eval(env, rest);
        DEBUG_PRINT("define(): Environment expression evaluated to\n");
        DEBUG_PRINT_EXPR(env, rest);
        if (!EXPR_OF_TYPE(rest, EXPR_ENVIRONMENT)) {
            expressionDispose(env, rest);
            ERROR(ERR_UNEXPECTED_TYPE,
                  "define(): expects ENVIRONMENT VALUE as argument");
            return NIL;
        }
        ENVIRONMENT_ADD_SYMBOL(rest, sym, val);
        expressionDispose(env, rest);
    } else {
        /* Use global environment */
        rest = env;
        while ((pEnv = ENVIRONMENT_GET_PARENT(EXPRESSION_ENVIRONMENT(rest))) &&
               EXPR_OF_TYPE(pEnv, EXPR_ENVIRONMENT))
            rest = pEnv;
        ENVIRONMENT_ADD_SYMBOL(rest, sym, val);
    }

    return val;
}

struct Expression *cond(struct Expression *env, struct Expression *expr) {
    struct Expression *iter, *condition, *body;

    INIT_NATIVE_FUNCTION("cond", env, expr);

    ITERATE_LIST(env, expr, iter, {
        ASSIGN_2_PARAMS(env, iter, condition, body, "2 arguments expected");
        condition = eval(env, condition);
        IF_SAFETY_CODE(if (!condition) return NIL;)
        if (!EXPR_IS_NIL(condition)) {
            expressionDispose(env, condition);
            body = eval(env, body);
            IF_SAFETY_CODE(if (!body) return NIL;);
            return body;
        };
    };);
    return NIL;
}

struct Expression *begin(struct Expression *env, struct Expression *expr) {
    struct Expression *iter, *res = NIL;

    INIT_NATIVE_FUNCTION("begin", env, expr);

    ITERATE_LIST(env, expr, iter, {
        expressionDispose(env, res);
        res = eval(env, iter);
    });

    return res;
}

struct Expression *gcRun(struct Expression *env, struct Expression *expr) {
    INIT_NATIVE_FUNCTION("gcRun", env, expr);

    return GC_RUN(env);
}

/*****************************************************************************
 *                           MATHEMATICAL FUNCTIONS
 *****************************************************************************/

struct Expression *add(struct Expression *env, struct Expression *expr) {
    struct Expression *car = 0;
    float res = 0;

    INIT_NATIVE_FUNCTION("add", env, expr);

    ITERATE_LIST(env, expr, car, { /* TODO: Check for conversion errors */
                                   car = eval(env, car);
                                   res += EXPR_TO_FLOAT(car);
                                   expressionDispose(env, car);
                                   DEBUG_PRINT_PARAM(
                                       "add():   IN while: res = %f\n", res);
    });

    DEBUG_PRINT_PARAM("ADD: %f\n", res);

    return EXPRESSION_CREATE_ATOM(env, EXPR_FLOAT, (void *)&res);
}

struct Expression *mul(struct Expression *env, struct Expression *expr) {
    struct Expression *car = NULL;
    float res = 1;

    INIT_NATIVE_FUNCTION("mul", env, expr);

    ITERATE_LIST(env, expr, car, { /* TODO: Check for conversion errors */
                                   car = eval(env, car);
                                   res *= EXPR_TO_FLOAT(car);
                                   expressionDispose(env, car);
                                   DEBUG_PRINT_PARAM(
                                       "mul():   IN while: res = %f\n", res);
    });

    DEBUG_PRINT_PARAM("MUL: %f\n", res);

    return EXPRESSION_CREATE_ATOM(env, EXPR_FLOAT, (void *)&res);
}

struct Expression *divide(struct Expression *env, struct Expression *expr) {
    struct Expression *car;
    struct Expression *cdr;
    float divident, divisor;

    INIT_NATIVE_FUNCTION("divide", env, expr);

    SECURE_CAR(env, expr, car, "Argument list flawed");
    car = eval(env, car);
    divident = EXPR_TO_FLOAT(car);
    expressionDispose(env, car);

    SECURE_CDR(env, expr, cdr, "Argument list flawed");
    if (EXPR_IS_NIL(cdr)) {
        if (divident == 0.0) {
            ERROR(ERR_DIVISION_BY_ZERO, "Division by zero");
            return NIL;
        }
        divident = 1.0 / divident;
        return EXPRESSION_CREATE_ATOM(env, EXPR_FLOAT, (void *)&divident);
    }

    cdr = mul(env, cdr);

    divisor = EXPR_TO_FLOAT(cdr);
    if (divisor == 0.0) {
        ERROR(ERR_DIVISION_BY_ZERO, "Division by zero");
        return NIL;
    };

    divident /= divisor;

    DEBUG_PRINT_PARAM("DIV: %f\n", divident);

    return EXPRESSION_CREATE_ATOM(env, EXPR_FLOAT, (void *)&divident);
}

/*****************************************************************************
 *                              LOGICAL FUNCTIONS
 *****************************************************************************/

struct Expression * and (struct Expression * env, struct Expression *expr) {
    struct Expression *car;
    INIT_NATIVE_FUNCTION("and", env, expr);
    ITERATE_LIST(env, expr, car, {
        car = eval(env, car);
        if (EXPR_IS_NIL(car)) return NIL;
        expressionDispose(env, car);
    });
    return T;
}

struct Expression * or (struct Expression * env, struct Expression *expr) {
    struct Expression *car;
    INIT_NATIVE_FUNCTION("or", env, expr);
    ITERATE_LIST(env, expr, car, {
        car = eval(env, car);
        if (!EXPR_IS_NIL(car)) {
            expressionDispose(env, car);
            return T;
        }
    });
    return NIL;
}

struct Expression * not(struct Expression * env, struct Expression *expr) {
    struct Expression *car, *res = NIL;
    INIT_NATIVE_FUNCTION("not", env, expr);
    if (EXPR_IS_NIL(expr)) return T;
    car = eval(env, intCar(env, expr));
    if (EXPR_IS_NIL(car)) {
        res = T;
    }
    expressionDispose(env, car);
    return res;
}

/*****************************************************************************
 *                            COMPARISON OPERATORS
 *****************************************************************************/

struct Expression *numEqual(struct Expression *env, struct Expression *expr) {
    struct Expression *car = NULL;
    struct Expression *first = NULL;
    float res = 0;

    INIT_NATIVE_FUNCTION("numEqual", env, expr);

    SECURE_CARCDR(env, expr, first, expr, "Flawed argument list");
    first = eval(env, first);
    if (first == NIL) {
        return first;
    }
    res = EXPR_TO_FLOAT(first);
    expressionDispose(env, first);
    if (!NO_ERROR) return NIL;
    ITERATE_LIST(env, expr, car, {
        car = eval(env, car);
        if ((res != EXPR_TO_FLOAT(car)) || !NO_ERROR) {
            expressionDispose(env, car);
            return NIL;
        }
        expressionDispose(env, car);
        DEBUG_PRINT_PARAM("numEqual():   IN while: res = %f\n", res);
    });

    DEBUG_PRINT_PARAM("numEqual: %f\n", res);

    return T;
}

struct Expression *numSmaller(struct Expression *env, struct Expression *expr) {
    struct Expression *car = NULL;
    struct Expression *first = NULL;
    float res = 0, nextRes = 0;

    INIT_NATIVE_FUNCTION("numSmaller", env, expr);

    SECURE_CARCDR(env, expr, first, expr, "Flawed argument list");
    first = eval(env, first);
    if (first == NIL) {
        return first;
    }
    res = EXPR_TO_FLOAT(first);
    expressionDispose(env, first);
    if (!NO_ERROR) return NIL;

    ITERATE_LIST(env, expr, car, {
        car = eval(env, car);
        nextRes = EXPR_TO_FLOAT(car);
        expressionDispose(env, car);
        if ((res >= nextRes) || !NO_ERROR) {
            return NIL;
        }
        res = nextRes;
        DEBUG_PRINT_PARAM("numSmaller:   IN while: res = %f\n", res);
    });

    DEBUG_PRINT("numSmaller: returning T\n");

    return T;
}

/*****************************************************************************
 *                                 PREDICATS
 *****************************************************************************/

struct Expression *nilP(struct Expression *env, struct Expression *expr) {
    struct Expression *iterator;
    INIT_NATIVE_FUNCTION("nilP", env, expr);
    ITERATE_LIST(env, expr, iterator, {
        iterator = eval(env, iterator);
        if (!EXPR_IS_NIL(iterator)) {
            expressionDispose(env, iterator);
            return NIL;
        }
        expressionDispose(env, iterator);
    });
    return T;
}

#define DEFINE_PREDICATE_FUNCTION(predicateName, iterator, test) \
    struct Expression *predicateName(struct Expression *env,     \
                                     struct Expression *expr) {  \
        struct Expression *iterator;                             \
        INIT_NATIVE_FUNCTION(#predicateName, env, expr);         \
        ITERATE_LIST(env, expr, iterator, {                      \
            iterator = eval(env, iterator);                      \
            if (EXPR_IS_NIL(iterator) || !(test)) {              \
                expressionDispose(env, iterator);                \
                return NIL;                                      \
            }                                                    \
            expressionDispose(env, iterator);                    \
        });                                                      \
        return T;                                                \
    }

DEFINE_PREDICATE_FUNCTION(integerP, iter, (EXPR_OF_TYPE(iter, EXPR_INTEGER)))

DEFINE_PREDICATE_FUNCTION(floatP, iter, (EXPR_OF_TYPE(iter, EXPR_FLOAT)))

DEFINE_PREDICATE_FUNCTION(characterP, iter,
                          (EXPR_OF_TYPE(iter, EXPR_CHARACTER)))

DEFINE_PREDICATE_FUNCTION(nativeFunctionP, iter,
                          (EXPR_OF_TYPE(iter, EXPR_NATIVE_FUNC)))

DEFINE_PREDICATE_FUNCTION(stringP, iter, (EXPR_OF_TYPE(iter, EXPR_STRING)))

/* SYMBOL? needs special treatment, because in here, NIL will cause T to be
 * returned */
struct Expression *symbolP(struct Expression *env, struct Expression *expr) {
    struct Expression *iterator;
    INIT_NATIVE_FUNCTION("symbolP", env, expr);
    ITERATE_LIST(env, expr, iterator, {
        iterator = eval(env, iterator);
        if (!(EXPR_OF_TYPE(iterator, EXPR_SYMBOL))) {
            expressionDispose(env, iterator);
            return NIL;
        }
        expressionDispose(env, iterator);
    });
    return T;
}

DEFINE_PREDICATE_FUNCTION(consP, iter, (EXPR_OF_TYPE(iter, EXPR_CONS)))

DEFINE_PREDICATE_FUNCTION(lambdaP, iter, (EXPR_OF_TYPE(iter, EXPR_LAMBDA)))

DEFINE_PREDICATE_FUNCTION(environmentP, iter,
                          (EXPR_OF_TYPE(iter, EXPR_ENVIRONMENT)))

#undef DEFINE_PREDICATE_FUNCTION

struct Expression *getEnv(struct Expression *env, struct Expression *expr) {
    INIT_NATIVE_FUNCTION("getEnv", env, expr);
    return env;
}

struct Expression *getParentEnv(struct Expression *env,
                                struct Expression *expr) {
    struct Expression *envToLookAt, *envToReturn;
    INIT_NATIVE_FUNCTION("getParentEnv", env, expr);
    SECURE_CAR(env, expr, envToLookAt, "getParentEnv expects argument");
    envToLookAt = eval(env, envToLookAt);
    DEBUG_PRINT("getParentEnv(): Got \n");
    DEBUG_PRINT_EXPR(env, envToLookAt);
    if (EXPR_IS_NIL(envToLookAt)) {
        return NIL;
    } else if (!EXPR_OF_TYPE(envToLookAt, EXPR_ENVIRONMENT)) {
        expressionDispose(env, envToLookAt);
        ERROR(ERR_UNEXPECTED_TYPE,
              "getParentEnvironment expects ENVIRONMENT VALUE as argument");
        return NIL;
    }
    envToReturn = ENVIRONMENT_GET_PARENT(EXPRESSION_ENVIRONMENT(envToLookAt));
    expressionDispose(env, envToLookAt);
    return envToReturn;
}

/******************************************************************************
 *                                   IMPORT 
 ******************************************************************************/

static struct Expression *importFile(struct Expression *env,
                                     char const *libraryPath,
                                     char const *fileName) {

    struct CharReadStream *fileStream = 0;
    struct Expression *retVal = NIL;
    FILE *f = 0;

    assert(0 != libraryPath);
    assert(0 != fileName);

    printf("importing %s/%s\n", libraryPath, fileName);

    f = file_open(libraryPath, fileName, "r");
    if (0 == f) {
        DEBUG_PRINT_PARAM("Reading file %s failed\n", fileName);
        ERROR(ERR_NO_RESOURCE, strerror(errno));
        return NIL;
    }

    printf("Opened %s/%s\n", libraryPath, fileName);

    fileStream = makeCStreamCharReadStream(f);

    if(0 == fileStream) {
        ERROR(ERR_NIL_VALUE, "Could not create file stream");
        goto error;
    }

    printf("Built CStreamCharReadStream %s/%s\n", libraryPath, fileName);

    fuEvalStream(env, fileStream, 0);

    printf("Evaluated\n");

    if(NO_ERROR) {
        retVal = T;
    }

error:

    if (0 != fileStream) {
        STREAM_DISPOSE(fileStream);
        fileStream = 0;
    }

    fclose(f);

    return retVal;
}

struct Expression *import(struct Expression *env, struct Expression *expr) {
    struct Expression *res = 0;
    struct Expression *importResult = 0;
    struct Expression *fileName = 0;
    struct Expression *libraryPath = 0;

    char const *fileNameStr = 0;
    char const *libraryPathStr = 0;

    INIT_NATIVE_FUNCTION("import", env, expr);

    libraryPath = eval(env, &lispLibraryPath);

    if ((NIL != libraryPath) && (!EXPR_OF_TYPE(libraryPath, EXPR_STRING))) {
        expressionDispose(env, libraryPath);
        ERROR(ERR_UNEXPECTED_TYPE,
              "LISP-LIBRARY-PATH is not defined / not a String");
    }

    if (NIL != libraryPath) {
        libraryPathStr = EXPRESSION_STRING(libraryPath);
    }

    ITERATE_LIST(env, expr, fileName, {
        expressionDispose(env, res);
        res = eval(env, fileName);
        if (!EXPR_OF_TYPE(res, EXPR_STRING)) {
            ERROR(ERR_UNEXPECTED_TYPE, "Expected string as file name");
        }
        fileNameStr = EXPRESSION_STRING(res);
        importResult = importFile(env, libraryPathStr, fileNameStr);
        expressionDispose(env, res);
        if (NIL == importResult) {
            expressionDispose(env, libraryPath);
            return NIL;
        }
    });

    expressionDispose(env, libraryPath);

    return T;
}
