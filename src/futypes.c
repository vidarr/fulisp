/*
 * (C) 2014 Michael J. Beer
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,
 * USA.
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
    expressionDispose(env, evaluatedExpr);
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
            result = expressionAssign(env, evaluatedExpr);
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
    expressionDispose(env, evaluatedExpr);
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
            result = expressionAssign(env, evaluatedExpr);
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
    expressionDispose(env, evaluatedExpr);
    return result;
}
