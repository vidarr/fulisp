
#include "config.h"
#include "fulispreader.h"
#include "print.h"
#include "error.h"
#include "eval.h"

#include "debugging.h"


char *input;

char readCharFromInput(void *v) {
    return *input ? *(input++) : 0;
}



int main(int argc, char **argv) {
    struct Reader *reader;
    struct CharReadStream *readStream;
    struct CharWriteStream *writeStream;
    struct CharBufferedReadStream *stream;
    char *buf;
    struct Expression *expr = 0, *result = 0;

    struct Expression *env = environmentCreateStdEnv();

    if(argc < 2) { 
        return 1;
    };

    input = argv[1];

    readStream = (struct CharReadStream *)malloc(sizeof(struct CharReadStream));
    readStream->getNext = readCharFromInput;
    readStream->intConfig = 0;
    stream = makeCharBufferedReadStream(readStream);
    reader = newFuLispReader(env, stream);

    expr = fuRead(reader);

    deleteReader(reader);
    disposeCharBufferedReadStream(stream);
    free(readStream); 

    writeStream = makeCStreamCharWriteStream(0, stdout); 
    printToStream(env, writeStream, expr); 
    printf(" Error: %i\n", lispError); 
    result = eval(env, expr);
    DEBUG_PRINT("main():   result = ");
    DEBUG_PRINT_EXPR(env, result, buf);
    DEBUG_PRINT("\n");
    printToStream(env, writeStream, result);
    STREAM_WRITE(writeStream, '\n');
    disposeCStreamCharWriteStream(writeStream);
    printf(" Error: %i\n", lispError); 
    if(lispError != ERR_OK) {
        printf("Error occured within: %s:%i\n", lispFile, lispLine);
    }

    printf("Disposing result...\n");
    expressionDispose(env, result);
    printf("Disposing expr...\n");
    expressionDispose(env, expr);
    printf("Disposing env...\n");
    expressionDispose(env, env);
    /* environmentDispose(env); */

    return 0;
}

