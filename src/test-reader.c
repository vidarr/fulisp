
#include "config.h"
#include "print.h"
#include "error.h"
#include "fulispreader.h"

char *input;

char readCharFromInput(void *v) {
    return *input ? *(input++) : 0;
}



int main(int argc, char **argv) {
    struct Reader *reader;
    struct CharReadStream *readStream;
    struct CharWriteStream *writeStream;
    struct CharBufferedReadStream *stream;
    struct Expression *expr = 0;

    struct Environment *env = malloc(sizeof(struct Environment));

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
    /* str =  print(expr); */
    deleteReader(reader);
    disposeCharBufferedReadStream(stream);
    free(readStream); 

    writeStream = makeCStreamCharWriteStream(0, stdout); 
    printToStream(env, writeStream, expr); 
    disposeCStreamCharWriteStream(writeStream);
    printf(" Error: %i\n", lispError); 
    /* expressionDispose(str); */
    expressionDispose(env, expr);

    free(env);

    fprintf(stderr, "TYPE_AREAL is %i\n", EXPR_TYPE_AREAL);
    fprintf(stderr, "TYPE of %i is %i\n", EXPR_STRING,
    EXPR_STRING & EXPR_TYPE_AREAL);
    fprintf(stderr, "TYPE of %i is %i\n", EXPR_CONS,
    EXPR_CONS & EXPR_TYPE_AREAL);

    return 0;
}
    
