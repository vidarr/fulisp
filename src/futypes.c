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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */ 


#include"futypes.h"

#define MODULE_NAME "futypes.c"

#ifdef DEBUG_TYPES
#   include "debugging.h"
#else
#   include "no_debugging.h"
#endif


struct Expression *fuType(struct Expression *env, struct Expression *expr) {
    struct Expression *evaluatedExpr;
    struct Expression *type = NULL;
    INIT_NATIVE_FUNCTION("fuTypes", env, expr);
    evaluatedExpr = eval(env, intCar(env, expr));
    if(EXPR_IS_NIL(evaluatedExpr)) {
        return NIL;
    }
    switch(EXPRESSION_TYPE(evaluatedExpr)) {
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

