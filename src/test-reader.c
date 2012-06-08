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
char *inputBuffer;
struct CharReadStream *readStream;
struct Reader *reader;
struct Expression *env;


char readCharFromInput(void *v) {
    DEBUG_PRINT_PARAM("readCharFromInput(): %s\n", input);
    return *input ? *(input++) : 0;
}


void iOError(void) {
    perror(strerror(errno));
    exit(1);
}


char *chomp(char *str) {
    int i = 0;
    DEBUG_PRINT_PARAM("Chomping %s\n", str);
    while(str[i] != '\0') i++;
    if(i > 1) {
        if((str[--i] == '\n') || (str[i] == '\r'))  str[i] = '\0';
        if((str[--i] == '\n') || (str[i] == '\r'))  str[i] = '\0';
    };
    DEBUG_PRINT_PARAM("Chomped %s\n", str);
    return str;
}


int checkInput(char *str, char *reference) {
    struct Expression *expr = 0;
    struct CharBufferedReadStream *bufStream;
    input = inputBuffer;
    memcpy(inputBuffer, str, INPUT_BUFFER_SIZE);
    bufStream = makeCharBufferedReadStream(readStream);
    reader = newFuLispReader(env, bufStream);
    expr = fuRead(reader);
    deleteReader(reader);
    disposeCharBufferedReadStream(bufStream);

    if(!expr) { DEBUG_PRINT("expr is 0!\n");
        return 1;
    }

    expressionToString(env, inputBuffer, INPUT_BUFFER_SIZE, expr);
    /* printed = fuPrint(env, expr); */
    expressionDispose(env, expr);
    /* printf("read: %s| reference: %s|\n", EXPRESSION_STRING(printed), reference); */
    IF_DEBUG(fprintf(stderr, "read: %s| reference: %s|\n", inputBuffer, reference););
    if(strcmp(inputBuffer, reference) == 0)  {
        /* expressionDispose(env, printed); */
        return 0;
    }
    /* expressionDispose(env, printed); */
    return 1;
}


int main(int argc, char **argv) {
    FILE *inFile;
    char *inBuffer, *refBuffer;
    struct Environment *env = malloc(sizeof(struct Environment));
    readStream = (struct CharReadStream *)malloc(sizeof(struct CharReadStream));
    readStream->getNext = readCharFromInput;
    readStream->intConfig = 0;
    inputBuffer = (char *)malloc(sizeof(char) * INPUT_BUFFER_SIZE);
    inBuffer = (char *)malloc(sizeof(char) * INPUT_BUFFER_SIZE);
    refBuffer = (char *)malloc(sizeof(char) * INPUT_BUFFER_SIZE);

    inFile = fopen(INPUT_FILE, "r");
    if(inFile == NULL) iOError();
    DEBUG_PRINT("File open\n");
    while(!feof(inFile)) {
        if(NULL == fgets((void *)inBuffer, INPUT_BUFFER_SIZE, inFile)) iOError();
        if(NULL == fgets((void *)refBuffer, INPUT_BUFFER_SIZE, inFile)) iOError();
        chomp(inBuffer);
        chomp(refBuffer);
        test(checkInput(inBuffer, refBuffer), inBuffer);
    }
    fclose(inFile);

    free(inputBuffer);
    free(readStream); 
    free(env);
    free(inBuffer);
    free(refBuffer);
    return 0;
}

