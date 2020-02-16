/*
 * (C) 2014 Michael J. Beer
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
#include "futypes.h"

#define MODULE_NAME "futypes.c"

#ifdef DEBUG_TYPES
#include "debugging.h"
#else
#include "no_debugging.h"
#endif

struct Expression *stringToIntExpression(struct Expression *env,
                                         const char *str) {
    struct Expression *result;
    char *endPointer;
    int iResult;
    double dResult = strtod(str, &endPointer);
    if (dResult > INT_MAX || dResult < INT_MIN) {
        WARNING(ERR_CONVERSION_FAILED, "fuInt(): Out of integer range");
        result = NIL;
    } else {
        iResult = dResult;
        DEBUG_PRINT_PARAM("fuInt(): Converted to %i\n", iResult);
        if (*endPointer != '\0') {
            ERROR(ERR_CONVERSION_FAILED,
                  "fuInt(): Could not convert STRING to INT");
            result = NIL;
        } else {
            result = CREATE_INT_EXPRESSION(env, iResult);
        }
    }
    return result;
}

struct Expression *fuType(struct Expression *env, struct Expression *expr) {
    struct Expression *evaluatedExpr;
    struct Expression *type = NULL;
    INIT_NATIVE_FUNCTION("fuTypes", env, expr);
    evaluatedExpr = eval(env, intCar(env, expr));
    if (EXPR_IS_NIL(evaluatedExpr)) {
        return NIL;
    }
    switch (EXPRESSION_TYPE(evaluatedExpr)) {
        case EXPR_INTEGER:
            type = TYPE_INTEGER;
            break;
        case EXPR_FLOAT:
            type = TYPE_FLOAT;
            break;
        case EXPR_CHARACTER:
            type = TYPE_CHARACTER;
            break;
        case EXPR_STRING:
            type = TYPE_STRING;
            break;
        case EXPR_SYMBOL:
            type = TYPE_SYMBOL;
            break;
        case EXPR_CONS:
            type = TYPE_CONS;
            break;
        case EXPR_LAMBDA:
            type = TYPE_LAMBDA;
            break;
        case EXPR_NATIVE_FUNC:
            type = TYPE_NATIVE_FUNC;
            break;
        case EXPR_ENVIRONMENT:
            type = TYPE_ENVIRONMENT;
            break;
    };
    assert(type != NULL);
    DEBUG_PRINT("fuType(): Got Expression of type :");
    DEBUG_PRINT_EXPR(env, type);
    return type;
}

struct Expression *fuInt(struct Expression *env, struct Expression *expr) {
    int iResult;
    struct Expression *evaluatedExpr;
    struct Expression *result = NULL;
    INIT_NATIVE_FUNCTION("fuInt", env, expr);
    evaluatedExpr = eval(env, intCar(env, expr));
    DEBUG_PRINT("fuFloat(): Evaluated to \n");
    DEBUG_PRINT_EXPR(env, evaluatedExpr);
    if (EXPR_IS_NIL(evaluatedExpr)) {
        ERROR(ERR_CONVERSION_FAILED, "fuInt(): Got NIL");
        return NIL;
    }
    switch (EXPRESSION_TYPE(evaluatedExpr)) {
        case EXPR_INTEGER:
            DEBUG_PRINT("fuInt(): Got INTEGER\n");
            result = evaluatedExpr;
            break;
        case EXPR_FLOAT:
            DEBUG_PRINT("fuInt(): Got FLOAT\n");
            iResult = EXPRESSION_FLOATING(evaluatedExpr);
            DEBUG_PRINT_PARAM("fuInt(): Converted to %i\n", iResult);
            result = CREATE_INT_EXPRESSION(env, iResult);
            break;
        case EXPR_STRING:
            DEBUG_PRINT("fuInt(): Got STRING\n");
            result =
                stringToIntExpression(env, EXPRESSION_STRING(evaluatedExpr));
            break;
        default:
            result = NIL;
    }
    return result;
}

struct Expression *fuFloat(struct Expression *env, struct Expression *expr) {
    float fResult;
    double dResult;
    char *endPointer;
    struct Expression *evaluatedExpr;
    struct Expression *result = NULL;
    INIT_NATIVE_FUNCTION("fuInt", env, expr);
    evaluatedExpr = eval(env, intCar(env, expr));
    DEBUG_PRINT("fuFloat(): Evaluated to \n");
    DEBUG_PRINT_EXPR(env, evaluatedExpr);
    if (EXPR_IS_NIL(evaluatedExpr)) {
        ERROR(ERR_CONVERSION_FAILED, "fuFloat(): Got NIL");
        return NIL;
    }
    switch (EXPRESSION_TYPE(evaluatedExpr)) {
        case EXPR_INTEGER:
            DEBUG_PRINT("fuFloat(): Got INTEGER\n");
            fResult = EXPR_TO_FLOAT(evaluatedExpr);
            DEBUG_PRINT_PARAM("fuFloat(): Converted to %f\n", fResult);
            result = CREATE_FLOAT_EXPRESSION(env, fResult);
            break;
        case EXPR_FLOAT:
            DEBUG_PRINT("fuFloat(): Got FLOAT\n");
            result = evaluatedExpr;
            break;
        case EXPR_STRING:
            DEBUG_PRINT("fuFloat(): Got STRING\n");
            dResult = strtod(EXPRESSION_STRING(evaluatedExpr), &endPointer);
            DEBUG_PRINT_PARAM("fuFloat(): Converted to %f\n", (float)dResult);
            if (*endPointer != '\0') {
                ERROR(ERR_CONVERSION_FAILED,
                      "fuFloat(): Could not convert STRING to FLOAT");
                result = NIL;
            } else {
                fResult = (float)dResult;
                result = CREATE_FLOAT_EXPRESSION(env, fResult);
            }
            break;
        default:
            result = NIL;
    }
    return result;
}
