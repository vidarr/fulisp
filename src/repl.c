/**
 * (C) 2012 Michael J. Beer
 * This program is free software; you can
 * redistribute it and/or modify * it under the terms of the GNU General Public
 * License as published by * the Free Software Foundation; either version 3 of
 * the License, or * (at your option) any later version.2010 *2010 * This
 * program is distributed in the hope that it will be useful,2010 * but WITHOUT
 * ANY WARRANTY; without even the implied warranty of * MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the * GNU General Public License for
 * more details.2010 2010 * You should have received a copy of the GNU General
 * Public License * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,
 * USA.2010
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
        "   This program is free software: you can redistribute it and/or "
        "modify\n"
        "   it under the terms of the GNU General Public License as published "
        "by\n"
        "   Free Software Foundation, either version 3 of the License, or\n"
        "   your option) any later version.\n");
    printf(
        "\n"
        "   This program is distributed in the hope that it will be useful,\n"
        "   WITHOUT ANY WARRANTY; without even the implied warranty of\n"
        "   HANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
        "   General Public License for more details.\n"
        "\n"
        "   should have received a copy of the GNU General Public License\n"
        "   along with this program.  If not, see "
        "<http://www.gnu.org/licenses/>.\n"
        "\n");
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
