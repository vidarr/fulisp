/**
 * (C) 2011 Michael J. Beer
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

#ifndef __ENVIRONMENT_H__
#define __ENVIRONMENT_H__

/*******************************************************************************
 *                                 DATA TYPE
 *******************************************************************************/
 
struct Environment;

#include "lisp_internals.h"
#include "hash.h"
#include "error.h"
#include "expression.h"



struct Environment {
  struct HashTable *lookup;
  struct Expression *parent;
};



/*******************************************************************************
 * MACROS
 *******************************************************************************/


/**
 * Check if expr is of type environment and if not, set error appropriately and
 * return NIL
 */
#define ENSURE_ENVIRONMENT(expr) \
    IF_SAFETY_CODE(if(!EXPR_OF_TYPE((expr), EXPR_ENVIRONMENT)) { \
        ERROR(ERR_UNEXPECTED_TYPE, "Expected Environment"); \
        return NIL; \
        })


/**
 * Try to resolve a symbol within an environment
 * @param env expression containing the environment to use
 * @param sym the symbol to look up
 * @return whatever sym resolves to 
 */
#define ENVIRONMENT_SYMBOL_LOOKUP(env, sym) environmentLookup((env), (sym))


/**
 * Try to resolve a string within an environment
 * @param env the environment to use
 * @param sym the string to look up
 * @return whatever sym resolves to 
 */
#define ENVIRONMENT_STRING_LOOKUP(env, sym) \
    (hashTableGet((EXPRESSION_ENVIRONMENT(env))->lookup, (sym)))


/**
 * Define a symbol within environment 
 * Does not check whether the symbol already exists
 * @param env the environment to define the symbol in
 * @param string the name of the new symbol
 * @param expr the expression to set the symbol to
 * @return old value of the symbol or NULL if previously not set
 */
#define ENVIRONMENT_ADD_STRING(env, string, expr) {\
    void *old = hashTableSet((EXPRESSION_ENVIRONMENT(env))->lookup, (string), (expr)); \
    if(old != NULL) expressionDispose(env, (struct Expression *)old); \
    expressionAssign((env), (expr));};


/**
 * Define a symbol within environment 
 * Does not check whether the symbol already exists
 * @param env the environment to define the symbol in
 * @param sym the symbol
 * @param expression the epxression to set the symbol to
 * @return old value of the symbol or NULL if previously not set
 */
#define ENVIRONMENT_ADD_SYMBOL(env, sym, expr) \
    ENVIRONMENT_ADD_STRING(env, EXPRESSION_STRING(sym), (expr))


/**
 * Create a new expression containing a native function and insert it into the
 * lookup table of an environment
 * @param env the environment to instert to
 * @param string name of the function 
 * @param func the function to add
 */
#define ADD_NATIVE_FUNCTION_EXPRESSION(env, string, func) {\
    struct Expression *f = expressionCreateNativeFunc((env), (func)); \
    ENVIRONMENT_ADD_STRING((env), (string), f); expressionDispose((env), f); }



/*******************************************************************************
                               F U N C T I O N S 
 *******************************************************************************/



/**
 * Create new empty environment
 * @param parent either expression containing parent environment or 0
 * @return the new environment
 */
struct Expression *environmentCreate(struct Expression *parent);


/**
 * Create new environment with standard symbols defined
 * @return the new environment
 */
struct Expression *environmentCreateStdEnv(void);


/**
 * Disposes of an environment. <b>If you want to get rid of a environment, use
 * expressionDispose. This function is to be used internally/by
 * expressionDispose only!</b>
 * @param surrEnv surrounding environment
 * @param env the environment to dispose
 */
void environmentDispose(struct Expression *surrEnv, struct Environment *env);


/**
 * Look up the binding of a symbol within the current environment
 * @param env the environment to look up symbol
 * @param sym the symbol to look up
 * @return expression bound to the symbol or NIL if not found
 */
struct Expression *environmentLookup(struct Expression *env, struct Expression *sym);


#endif
