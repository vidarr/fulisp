/*
 * (C) 2012 Michael J. Beer
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
 * Release resources directly held by Lambda struct, currently the memory itself
 */
void lambdaRelease(struct Expression *env, struct Lambda *lambda);

#endif
