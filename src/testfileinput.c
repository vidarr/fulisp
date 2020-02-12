/*
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
#include "config.h"
/* Exit if any kind of error occurs */
#define EXIT_ON_ERROR
#include "environment.h"
#include "error.h"
#include "fulispreader.h"
#include "print.h"
#include "test.h"

#ifdef DEBUG

#include "debugging.h"

#else

#include "no_debugging.h"

#endif

#define INPUT_BUFFER_SIZE 200
#define IGNORE_SIGLE ";   *IGNORE*   ;"

static char *inputBuffer;

static void iOError(void) {
    perror("Something went wrong: ");
    perror(strerror(errno));
    exit(1);
}

static char *chomp(char *str) {
    int i = 0;
    DEBUG_PRINT_PARAM("Chomping %s\n", str);
    while (str[i] != '\0') i++;
    if (i > 1) {
        if ((str[--i] == '\n') || (str[i] == '\r')) str[i] = '\0';
        if ((str[--i] == '\n') || (str[i] == '\r')) str[i] = '\0';
    };
    DEBUG_PRINT_PARAM("Chomped %s\n", str);
    return str;
}

static int checkInput(struct Expression *env, struct Expression *expr,
                      char *reference) {
    if (!expr) {
        DEBUG_PRINT("expr is 0!\n");
        return 1;
    }

    if (strcmp(reference, IGNORE_SIGLE) == 0) return 0;
    expressionToString(env, inputBuffer, INPUT_BUFFER_SIZE, expr);
    if (strcmp(inputBuffer, reference) == 0) {
        /* expressionDispose(env, printed); */
        return 0;
    }
    /* expressionDispose(env, printed); */
    return 1;
}

int checkFromFiles(struct Expression *env, char *inFileName, char *refFileName,
                   struct Expression *(*getNextExpr)(struct Reader *reader)) {
    char *refBuffer;
    FILE *inFile;
    FILE *refFile;
    int result = 0;
    struct CharReadStream *readStream;
    struct CharReadStream *bufStream;
    struct Reader *reader;
    struct Expression *expr = 0;

    inputBuffer = SAFE_STRING_NEW(INPUT_BUFFER_SIZE);
    refBuffer = SAFE_STRING_NEW(INPUT_BUFFER_SIZE);

    inFile = fopen(inFileName, "r");
    if (inFile == NULL) iOError();
    DEBUG_PRINT("File open\n");

    refFile = fopen(refFileName, "r");
    if (refFile == NULL) iOError();
    DEBUG_PRINT("File open\n");

    readStream = makeCStreamCharReadStream(inFile);
    bufStream = makeCharReadStream(readStream);
    reader = newFuLispReader(env, bufStream);

    while (!feof(refFile)) {
        if (NULL == fgets((void *)refBuffer, INPUT_BUFFER_SIZE, refFile)) {
            if (errno != 0)
                iOError();
            else
                break;
        }
        errno = 0;
        chomp(refBuffer);
        expr = getNextExpr(reader);
        result = result ? 1 : test(checkInput(env, expr, refBuffer), refBuffer);
        expressionDispose(env, expr);
        ERROR_RESET;
        resetReader(reader);
        resetCharReadStream(bufStream);
    }

    deleteReader(reader);
    STREAM_DISPOSE(bufStream);
    fclose(inFile);
    fclose(refFile);
    free(readStream);

    free(refBuffer);
    free(inputBuffer);
    return result;
}
