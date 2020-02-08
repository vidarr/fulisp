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

#include <stdio.h>
#include <errno.h>
#include "config.h"
#include "print.h"
#include "error.h"
#include "fulispreader.h"
#include "test.h"

#ifdef DEBUG

#    include "debugging.h"

#else 

#    include "no_debugging.h"

#endif


#define INPUT_BUFFER_SIZE 200
#define INPUT_FILE "../tests/test-reader.txt"

char *input;
struct CharReadStream *readStream;
struct Reader *reader;


char readCharFromInput(void *v) {
    DEBUG_PRINT_PARAM("readCharFromInput(): %s\n", input);
    return *input ? *(input++) : 0;
}


int main(int argc, char **argv) {
    struct Expression *env;
    struct Expression *expr;
    struct CharWriteStream *outStream;
    struct CharBufferedReadStream *bufStream;
    assert(argc > 1 && argv);
    env = (struct Expression *)malloc(sizeof(struct Expression*));
    readStream = (struct CharReadStream *)malloc(sizeof(struct CharReadStream));
    readStream->getNext = readCharFromInput;
    readStream->intConfig = 0;
    input = argv[1];

    expr = 0;
    bufStream = makeCharBufferedReadStream(readStream);
    reader = newFuLispReader(env, bufStream);
    expr = fuRead(reader);
    deleteReader(reader);
    disposeCharBufferedReadStream(bufStream);

    outStream = makeCStreamCharWriteStream(1024, stdout);
    printToStream(env, outStream, expr);
    printf("\n lispError: %i    lispWarning: %i\n", lispError, lispWarning);
    disposeCStreamCharWriteStream(outStream);

    expressionDispose(env, expr);
    free(readStream); 
    free(env);
    return 0;
}

