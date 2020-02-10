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
#include "environment.h"
#include "eval.h"



#ifdef DEBUG

#    include "debugging.h"

#else 

#    include "no_debugging.h"

#endif


#define INPUT_BUFFER_LEN 200


char *input;
struct CharReadStream *readStream;
struct Reader *reader;


char readCharFromInput(void *v) {
    DEBUG_PRINT_PARAM("readCharFromInput(): %s\n", input);
    return *input ? *(input++) : 0;
}


struct Expression *quit(struct Expression *env, struct Expression *expr) {
    printf("Exiting...\n");
    exit(0);
}

int main(int argc, char **argv) {
    struct Expression *env;
    struct Expression *expr, *res;
    struct CharWriteStream *outStream;
    struct CharBufferedReadStream *bufStream;
    int lastError = ERR_OK;
    env = environmentCreateStdEnv();
    ADD_NATIVE_FUNCTION_EXPRESSION(env, "QUIT", quit);
    
    readStream = (struct CharReadStream *)malloc(sizeof(struct CharReadStream));
    readStream->getNext = readCharFromInput;
    readStream->intConfig = 0;
    input = (char *)malloc(sizeof(char) * (1 + INPUT_BUFFER_LEN));

    expr = 0;
    bufStream = makeCharBufferedReadStream(readStream);
    reader = newFuLispReader(env, bufStream);
    while(1) { /* Loop until "quit" is called */
        printf("\nfuLisp:%i$ ", lastError);
        while(NULL ==  fgets(input, INPUT_BUFFER_LEN, stdin)) {
        };
        resetCharBufferedReadStream(bufStream);
        expr = fuRead(reader);
        if(!NO_ERROR) {
            fprintf(stderr, "    FOOBAR: %s\n\n", lispErrorMessage);
            lastError = lispError;
            ERROR_RESET;
        } else {
            res = eval(env, expr);
            expressionDispose(env, expr);
            if(!NO_ERROR) {
                fprintf(stderr, "    FOOBAR: %s\n\n", lispErrorMessage);
                lastError = lispError;
                ERROR_RESET;
            }
            outStream = makeCStreamCharWriteStream(1024, stdout);
            printToStream(env, outStream, res);
            expressionDispose(env, res);
        }
        printf("\n");
    }
    deleteReader(reader);
    disposeCharBufferedReadStream(bufStream);
    disposeCStreamCharWriteStream(outStream);

    expressionDispose(env,env);
    free(readStream); 
    free(input);
    return 0;
}

