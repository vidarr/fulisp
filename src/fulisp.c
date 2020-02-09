/*
 * (C) 2020 Michael J. Beer
 *
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
