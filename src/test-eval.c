
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

    struct Environment *env = environmentCreateStdEnv();

    if(argc < 2) { 
        return 1;
    };

    input = argv[1];

    readStream = (struct CharReadStream *)malloc(sizeof(struct CharReadStream));
    readStream->getNext = readCharFromInput;
    readStream->intConfig = 0;

    stream = makeCharBufferedReadStream(readStream);

    reader = newFuLispReader(env, stream);

    expr = read(reader);
    deleteReader(reader);
    disposeCharBufferedReadStream(stream);
    free(readStream); 

    writeStream = makeCStreamCharWriteStream(0, stdout); 
    printToStream(env, writeStream, expr); 
    printf(" Error: %i\n", lispError); 
    result = eval(env, expr);
    DEBUG_PRINT("main():   result = ");
    DEBUG_PRINT_EXPR(env, result, buf);
    DEBUG_PRINT_EXPR(env, result, buf);
    DEBUG_PRINT_EXPR(env, result, buf);

    printToStream(env, writeStream, result);
    STREAM_WRITE(writeStream, '\n');

    expressionDispose(env, result);
    expressionDispose(env, expr);
    environmentDispose(env);
    disposeCStreamCharWriteStream(writeStream);

    fprintf(stderr, "TYPE_AREAL is %i\n", EXPR_TYPE_AREAL);
    fprintf(stderr, "TYPE of %i is %i\n", EXPR_STRING,
    EXPR_STRING & EXPR_TYPE_AREAL);
    fprintf(stderr, "TYPE of %i is %i\n", EXPR_CONS,
    EXPR_CONS & EXPR_TYPE_AREAL);

    return 0;
}

