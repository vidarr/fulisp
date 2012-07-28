/**
 * (C) 2010, 2012 Michael J. Beer
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,
 * USA.
 */
  
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include "streams.h"
#include "test.h"


char *testText =  
" /** \n" \
"  * (C) 2010,2012 Michael J. Beer \n" \
"  * This program is free software; you can redistribute it and/or modify \n" \
"  * it under the terms of the GNU General Public License as published by \n" \
"  * the Free Software Foundation; either version 3 of the License, or \n" \
"  * (at your option) any later version. \n" \
"  * \n" \
"  * This program is distributed in the hope that it will be useful, |";


char *copyText = NULL;


/* Used for debugging */
/* void copyContent(FILE *in, FILE *out) { */
/*     while(!feof(in)) { */
/*         fputc(fgetc(in), out); */
/*     }; */
/*     fflush(out); */
/* } */


int randomMax(int upper) {
    return (int) (upper ? rand() % upper : 0);
}


int compareStrings(char *a, char *b, int len) {
    while(*a == *b && len > 0) {
        a++; b++; len--;
    };
    return len;
}


int testStringCharWriteStream(char *target, char *origin, int len) {
    struct CharWriteStream *stream;
    char *lOrigin = origin;

    stream = makeStringCharWriteStream(len, target);

    while(*lOrigin != '\0') {
        STREAM_WRITE(stream, *lOrigin);
        lOrigin++;
    };
    STREAM_DISPOSE(stream);
    /* TODO: Check whether this is actually right */
    target[len - 1] = '\0';

    if(compareStrings(origin, target, strlen(target)) != 0) return 1;

    return 0;
}


int testCStreamCharWriteStream(FILE *file, char *text, int len) {
    struct CharWriteStream *stream = makeCStreamCharWriteStream(len, file);

    while(*text != '\0') {
        STREAM_WRITE(stream, *text);
        text++;
    };
    /* Mark end of stream - once stream->status(stream) will be available, this
     * should be used to indicate end of stream rather than anything else... */
    STREAM_WRITE(stream, '\0');
    STREAM_DISPOSE(stream);
    return 0;
}


int testCStreamCharReadStream(FILE *file, char *text, int len) {
    char next;
    char *reference = text;
    struct CharReadStream *stream = makeCStreamCharReadStream(file);

    while((next = STREAM_NEXT(stream)) != 0) {
        if(next != *reference++) {
            STREAM_DISPOSE(stream);
            return 1;
        }
    };
    STREAM_DISPOSE(stream);
    return 0;
}


int testCharBufferedReadStream(FILE *file, char *text, int len) {
    char next;
    int count, countOrig;
    struct CharReadStream *intStream;
    char *reference = text;
    struct CharBufferedReadStream *stream;
    count = countOrig = randomMax(len);
    intStream = makeCStreamCharReadStream(file);
    stream =  makeCharBufferedReadStream(intStream);
    while((next = STREAM_NEXT(stream)) != 0) {
        if(next != *reference++) {
            STREAM_DISPOSE(intStream);
            STREAM_DISPOSE(stream);
            return 1;
        }
        if(!count--) {
            count = randomMax(countOrig);
            while(count--) {
                STREAM_PUSH_BACK(stream, *(reference - 1));
                reference--;
            }
            count = randomMax(text - reference);
        }
    };
    STREAM_DISPOSE(intStream);
    STREAM_DISPOSE(stream);
    return 0;
}


int main(int argc, char **argv) {
    FILE *ioFile = NULL;
    int result = 0;
    int len = strlen(testText);
    fprintf(stdout, "Testing streams.c\n");
    copyText = (char *)malloc(sizeof(char) * (len + 1));

    srand((unsigned int)time(NULL));

    /* Testing writing to/reading from a string */

    result  = test(testStringCharWriteStream(copyText, testText, strlen(testText) + 1), 
            "CStreamCharWriteStream / buffer large enough");;
    result |= test(testStringCharWriteStream(copyText, testText, strlen(testText) - 1), 
            "CStreamCharWriteStream / buffer too small");

    free(copyText);

    if((ioFile = tmpfile()) == NULL) {
        fprintf(stderr, "Could not retrieve temporary file - aborting");
        exit(1);
    } 

    /* Testing writing chars to FILE objects */

    result  = test(testCStreamCharWriteStream(ioFile, testText, 0), 
            "CStreamCharWriteStream / without buffering");

    rewind(ioFile);

    result |= test(testCStreamCharReadStream(ioFile, testText, 0), 
            "CStreamCharReadStream / without buffering");

    /* The same with buffering turned on */

    rewind(ioFile);

    result |= test(testCStreamCharWriteStream(ioFile, testText,20), 
            "CStreamCharWriteStream / with buffering");

    rewind(ioFile);

    result |= test(testCStreamCharReadStream(ioFile, testText, 20), 
            "CStreamCharReadStream / with buffering");

    rewind(ioFile);

    result |=  test(testCharBufferedReadStream(ioFile, testText, 25),
            "CharBufferedReadStream");

    fclose(ioFile);

    return result;
}

