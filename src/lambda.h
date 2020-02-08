/*
 * (C) 2012 Michael J. Beer
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

/*
 * Deals with lambda forms
 */
#ifndef __LAMBDA_H__
#define __LAMBDA_H__

#include "config.h"
#include "cons.h"
#include "eval.h"
#include "expression.h"
#include "lisp_internals.h"

/**
 * Data structure representing a lambda form
 */
struct Lambda {
    struct Expression *body;
    struct Expression *env;
    struct Expression *argList;
    struct Expression *rest;
};

/*****************************************************************************
 *                                 Functions                                 *
 *****************************************************************************/

/**
 * Create a new lambda form
 * @param env Environment this lambda form should be created int
 * @param expr Cons cell containing as car list of parameters and as cadr the
 * body.
 * @return a new lambda form
 */
struct Expression *lambdaCreate(struct Expression *env,
                                struct Expression *expr);

/**
 * Invoke lambda form on concrete arguments
 * @param lambda lambda form to evaluate
 * @param args list of values for the arguments to use
 * @return expression being the result of the evaluation
 */
struct Expression *lambdaInvoke(struct Expression *oldEnv,
                                struct Lambda *lambda, struct Expression *args);

/**
 * Delete a lambda form
 * @param lambda the lambda form to dispose
 */
void lambdaDispose(struct Expression *env, struct Lambda *lambda);

#endif
