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
#include <errno.h>
#include <stdio.h>
#include "benchmarking.h"
#include "config.h"
#include "environment.h"
#include "error.h"
#include "eval.h"
#include "fulispreader.h"
#include "garbage_collector.h"
#include "print.h"
#include "fulisp.h"

#ifdef DEBUG
#include "debugging.h"
#else
#include "no_debugging.h"
#endif

int dontExit;

void welcome(FILE *out) {
    fprintf(out,
            "       _\n"
            "      /      /     __  ___\n"
            "     /  / / /   / /_  /  /\n"
            "   _/  /_/ /__ / __/ /  /\n"
            "   /                /\n"
            "  /                /\n\n\n"
            "    fuLisp " VERSION "  " COPYRIGHT
            "\n"
            "    This program comes with ABSOLUTELY NO WARRANTY;"
            " for details type `(license)'.\n"
            "    This is free software,"
            " and you are welcome to redistribute it\n"
            "    under certain conditions;"
            " type `(license)' for details.\n");
}

struct Expression *license(struct Expression *env, struct Expression *expr) {
    printf(
        "\n"
        "   fuLisp " VERSION
        " - A minimalistic Lisp interpreter\n"
        "   " COPYRIGHT
        "\n"
        "\n"
        "(C) 2012 Michael J. Beer\n"
         "All rights reserved.\n");
    printf(
                 "Redistribution  and use in source and binary forms, with or with‐\n"
         "out modification, are permitted provided that the following  con‐\n"
         "ditions are met:\n"
         "\n");
    printf(
         "1. Redistributions of source code must retain the above copyright\n"
         "notice, this list of conditions and the following disclaimer.\n"
         "\n");
    printf(
         "2. Redistributions in binary form must reproduce the above  copy‐\n"
         "right  notice,  this  list  of  conditions and the following dis‐\n"
         "claimer in the documentation and/or other materials provided with\n"
         "the distribution.\n"
         "\n");
    printf(
         "3.  Neither the name of the copyright holder nor the names of its\n"
         "contributors may be used to endorse or promote  products  derived\n"
         "from this software without specific prior written permission.\n"
         "\n");
    printf(
         "THIS  SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBU‐\n"
         "TORS \"AS IS\" AND ANY EXPRESS OR  IMPLIED  WARRANTIES,  INCLUDING,\n"
         "BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND\n"
         "FITNESS FOR A PARTICULAR PURPOSE  ARE  DISCLAIMED.  IN  NO  EVENT\n"
         "SHALL  THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DI‐\n"
         "RECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR  CONSEQUENTIAL\n");
    printf(
         "DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE\n"
         "GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS IN‐\n"
         "TERRUPTION)  HOWEVER  CAUSED  AND  ON  ANY  THEORY  OF LIABILITY,\n"
         "WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING  NEGLI‐\n"
         "GENCE  OR  OTHERWISE)  ARISING  IN ANY WAY OUT OF THE USE OF THIS\n"
         "SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.\n\n");

    return T;
}

struct Expression *quit(struct Expression *env, struct Expression *expr) {
    printf("Exiting...\n");
    dontExit = 0;
    return NIL;
}

int main(int argc, char **argv) {
    struct Expression *env;
    struct Expression *expr, *res;
    struct CharWriteStream *outStream;
    struct CharReadStream *bufStream;
    struct CharReadStream *readStream;
    struct Reader *reader;
    int lastError;

    BENCHMARK_DECLARE_VAR(bmTime, bmTemp, bmTimeSt);

    lastError = ERR_OK;

    BENCHMARK_INIT(bmTime, bmTemp, bmTimeSt);

    welcome(stdout);

    env = fuOpen();

    ADD_NATIVE_FUNCTION_EXPRESSION(env, "QUIT", quit);
    ADD_NATIVE_FUNCTION_EXPRESSION(env, "LICENSE", license);

    readStream = makeCStreamCharReadStream(stdin);
    outStream = makeCStreamCharWriteStream(1024, stdout);

    bufStream = makeCharReadStream(readStream);
    reader = newFuLispReader(env, bufStream);

    expr = 0;
    dontExit = 1;

    while (dontExit) { /* Loop until "quit" is called  */
        printf("\nfuLisp:%i$ ", lastError);
        expr = fuRead(reader);
        resetCharReadStream(bufStream);
        resetReader(reader);
        if (!NO_ERROR) {
            fprintf(stderr, "    FUBAR: %s\n\n", lispErrorMessage);
            lastError = lispError;
            ERROR_RESET;
        } else {
            BENCHMARK_CONTINUE(bmTime, bmTemp, bmTimeSt);
            res = eval(env, expr);
            BENCHMARK_INTERRUPT(bmTime, bmTemp, bmTimeSt);
            expressionDispose(env, expr);
            if (!NO_ERROR) {
                fprintf(stderr, "    FUBAR: %s\n\n", lispErrorMessage);
                lastError = lispError;
                ERROR_RESET;
            }
            printToStream(env, outStream, res);
            expressionDispose(env, res);
        }
        printf("\n");
    }

    BENCHMARK_DO(
        fprintf(stderr, "Benchmark: Total time measured: %li \n", bmTime));

    deleteReader(reader);

    STREAM_DISPOSE(bufStream);
    STREAM_DISPOSE(readStream);
    STREAM_DISPOSE(outStream);

    /* Actually, if lambda expressions have been bound to variables, the
     * environment wont be disposed as it will have some cicular structure:
     * The env contains a reference to a lambda, this lambda contains a
     * reference to the env it has been created in
     */

    fuClose(env);

    return 0;

}
