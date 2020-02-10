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

