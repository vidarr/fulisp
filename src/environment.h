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
 
struct Environment {
  struct HashTable *lookup;
};


#include "lisp_internals.h"
#include "hash.h"
#include "symboltable.h"
#include "error.h"
#include "expression.h"



/*******************************************************************************
 * MACROS
 *******************************************************************************/


/**
 * Try to resolve a symbol within an environment
 * @param env the environment to use
 * @param sym the symbol to look up
 * @return whatever sym resolves to 
 */
#define ENVIRONMENT_SYMBOL_LOOKUP(env, sym) (hashTableGet((env)->lookup, EXPRESSION_STRING(sym)))



/**
 * Try to resolve a string within an environment
 * @param env the environment to use
 * @param sym the string to look up
 * @return whatever sym resolves to 
 */
#define ENVIRONMENT_STRING_LOOKUP(env, sym) (hashTableGet((env)->lookup, sym))



/**
 * Define a symbol within environment 
 * Does not check whether the symbol already exists
 * @param env the environment to define the symbol in
 * @param string the name of the new symbol
 * @param value the value to set the symbol to
 * @return old value of the symbol or NULL if previously not set
 */
#define ENVIRONMENT_ADD_STRING(env, string, value) hashTableSet((env)->lookup, string, (value))



/**
 * Define a symbol within environment 
 * Does not check whether the symbol already exists
 * @param env the environment to define the symbol in
 * @param sym the symbol
 * @param value the value to set the symbol to
 * @return old value of the symbol or NULL if previously not set
 */
#define ENVIRONMENT_ADD_SYMBOL(env, sym, value) ENVIRONMENT_ADD_STRING(env, EXPRESSION_STRING(sym), (value))



/*******************************************************************************
                               F U N C T I O N S 
 *******************************************************************************/



/**
 * Create new empty environment
 * @return the new environment
 */
struct Environment *environmentCreate(void);



/**
 * Create new environment with standard symbols defined
 * @return the new environment
 */
struct Environment *environmentCreateStdEnv(void);



/**
 * Disposes of an environment
 * @param env the environment to dispose
 */
void environmentDispose(struct Environment *env);


#endif
