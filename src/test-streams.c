/**
 * (C) 2010 Michael J. Beer
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
  
#include "streams.h"
#include "stdlib.h"
#include "stdio.h"


char *input;

int testCharReadStream(struct CharReadStream *stream) {
   char *current = input;
   *(current++) = STREAM_NEXT(stream);
   while(*(current - 1) != 'q') {
       *(current++) = STREAM_NEXT(stream);
   };
   *current = 0;
   printf("Read: %s\n", input);
   return 1;
}


int testCharBufferedReadStream(struct CharBufferedReadStream *stream) {
   int flag;
   char *current = input;
   flag = 1;
   *(current++) = STREAM_NEXT(stream);
   while(*(current - 1) != 'q') {
       *(current++) = STREAM_NEXT(stream);
       if(flag) stream->pushBack(stream->intConfig, *(current - 1));
       flag = 1 - flag;
   };
   *current = 0;
   printf("Read: %s\n", input);
   return 1;
}


int testCharWriteStream(struct CharWriteStream *stream) {
    char *current = input;
    while(*current) {
        STREAM_WRITE(stream, *current++);
    }
    return 1;
}



int main(int argc, char **argv) {
    struct CharWriteStream *writeStream;
    struct CharBufferedReadStream *bufStream;
    struct CharReadStream *readStream = makeCStreamCharReadStream(stdin);
    input = (char *)malloc(sizeof(char) * 2000);
    testCharReadStream(readStream);
    
    bufStream =
    makeCharBufferedReadStream(readStream);
    testCharBufferedReadStream(bufStream);
    
    disposeCharBufferedReadStream(bufStream);
    free(readStream);

    writeStream = makeCStreamCharWriteStream(0, stdout);
    testCharWriteStream(writeStream);
    disposeCStreamCharWriteStream(writeStream);
    writeStream = makeCStreamCharWriteStream(10, stdout);
    testCharWriteStream(writeStream);
    STREAM_WRITE(writeStream, 0);
    disposeCStreamCharWriteStream(writeStream);

    free(input);

    return 0;
}
   
        
