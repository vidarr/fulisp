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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,
 USA.
 */



#ifndef __NATIVEFUNCTIONS_H__
#define __NATIVEFUNCTIONS_H__


#include "environment.h"
#include "eval.h"
#include "lambda.h"


/******************************************************************************
                                   FUNCTIONS
 ******************************************************************************/



/**
 * Identity 
 * Returnes the expression given to this functions without alteration
 * @parame env the environment to be used
 * @param expr
 * @returns expr
 */
struct Expression *quote(struct Expression *env, struct Expression *expr);


/**
 * Returns the sum of the arguments given
 * @param env the environment to be used
 * @param a list of numbers 
 * @returnes sum of the arguments
 */
struct Expression *add(struct Expression *env, struct Expression *expr);


/**
 * Prints out a String representation of the expression given as parameter
 * @param env the environment to be used
 * @param expr an expression to print
 * @returns T
 */
struct Expression *print(struct Expression *env, struct Expression *expr);


/**
 * Bind symbol to a value. Currently, if there is no binding for this symbol
 * yet, a new binding is created, thus explicit call to "define" or something is
 * unneccessary. 
 * @param env the environment to be used
 * @param expr a cons list with two entries: first one should be the symbol to
 * bind, second the value to be bound to the symbol.
 * @returns new value bound to the symbol
 */
struct Expression *set(struct Expression *env, struct Expression *expr) ;


/** 
 * Bind a symbol to a value within current environment, if it exists already,
 * create new binding.
 * @param  env environment create binding within
 * @param  sym symbol to bind 
 * @param  expr expr to bind to symbol
 * @return new value 
 */
struct Expression *define(struct Expression *env, struct Expression *expr);


#endif
