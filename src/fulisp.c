/*
 * (C) 2020 Michael J. Beer
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

#include "fulisp.h"
#include "fulispreader.h"

/*----------------------------------------------------------------------------*/

struct Expression *fuOpen(void) {

    struct Expression *env = 0;
    struct Memory *mem = 0;

    mem = newMemory();
    env = environmentCreateStdEnv(mem);

    return env;

}

/*----------------------------------------------------------------------------*/


void fuClose(struct Expression *env) {

    struct Memory *mem = 0;

    if(0 == env) return;

    mem = ENVIRONMENT_GET_MEMORY(env);

    environmentClear(env);
    GC_RUN(env); /* Required to recursively free all resources */
    expressionForceDispose(env, env);

    env = 0;

    deleteMemory(mem);

    mem = 0;

}

/*----------------------------------------------------------------------------*/

int fuEvalStream(struct Expression *env, struct CharReadStream *instream,
              struct CharWriteStream *outStream) {

    struct CharReadStream *bufStream = 0;
    struct Reader *reader = 0;

    struct Expression *expr = 0;
    struct Expression *res = 0;

    bufStream = makeCharReadStream(instream);
    reader = newFuLispReader(env, bufStream);

    while (0 != (expr = fuRead(reader))) {

        resetCharReadStream(bufStream);
        resetReader(reader);
        if (!NO_ERROR) {
            fprintf(stderr, "    FUBAR: %s\n\n", lispErrorMessage);
            goto ret_from_func;
        }
        res = eval(env, expr);

        expressionDispose(env, expr);
        if (!NO_ERROR) {
            fprintf(stderr, "    FUBAR: %s\n\n", lispErrorMessage);
            goto ret_from_func;
        }

        if (0 != outStream) {
            printToStream(env, outStream, res);
        }

        expressionDispose(env, res);
    }

ret_from_func:

    deleteReader(reader);
    STREAM_DISPOSE(bufStream);

    return lispError;
}

/*----------------------------------------------------------------------------*/
