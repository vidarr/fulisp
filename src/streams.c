/*
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
  
#include "assert.h"
#include "stdlib.h"
#include "streams.h"
#include "safety.h"
#include "error.h"



char getNextCharFromCFile(struct CharReadStream *stream) {
    int got;

    assert(stream);

    if(!stream->intConfig) return 0;
    if(feof((FILE *)stream->intConfig)) {
        got = 0;
    } else {
        got = getc((FILE *)(stream->intConfig));
        if(got == EOF) {
            got = 0;
        };
    }
    return (char)got;
}


void disposeCStreamCharReadStream(struct CharReadStream *stream) {
    assert(stream);
    free(stream);
}


int statusCStream(struct CharReadStream *stream) {
    FILE *file = stream->intConfig;

    assert(stream);

    if(!file)         return STREAM_STATUS_INTERNAL_ERROR;
    if(ferror(file))  return STREAM_STATUS_GENERAL_ERROR;
    if(feof(file))    return STREAM_STATUS_EOS;
    return STREAM_STATUS_OK;
}


struct CharReadStream *makeCStreamCharReadStream(FILE *s) {
    struct CharReadStream *stream;

    assert(s);

    stream            = malloc(sizeof(struct CharReadStream));
    stream->getNext   = getNextCharFromCFile;
    stream->dispose   = disposeCStreamCharReadStream;
    stream->status    = statusCStream;
    stream->intConfig = (void *)s;
    return stream;
}



/*****************************************************************************
 *                             charBufferedStream
 *****************************************************************************/



struct InternalCharBufferedStream {
    struct CharReadStream *readStream;
    char *current;
    char *buffer;
};


char getNextWrapper(struct CharBufferedReadStream *stream) {
    struct InternalCharBufferedStream *internalStruct;
    struct CharReadStream *readStream;

    assert(stream && stream->intConfig);

    internalStruct = (struct InternalCharBufferedStream *)(stream->intConfig);

    if(internalStruct->current != internalStruct->buffer) 
        return *((--internalStruct->current));

    readStream = internalStruct->readStream; 

    assert(readStream);

    return STREAM_NEXT(readStream);
}


int statusReadStreamWrapper(struct CharBufferedReadStream *stream) {
    struct InternalCharBufferedStream *intStruct;
    int status;

    assert(stream);

    intStruct = (struct InternalCharBufferedStream *)stream->intConfig;
     
    assert(intStruct && intStruct->readStream);

    status = STREAM_STATUS(intStruct->readStream);
    if(status == STREAM_STATUS_OK && 
            intStruct->current != intStruct->buffer) 
        return STREAM_STATUS_OK;

    return status;
}


void charPushBack(struct CharBufferedReadStream *stream, char c) {
    struct InternalCharBufferedStream *internalStruct;

    assert(stream && stream->intConfig);

    internalStruct = (struct InternalCharBufferedStream *)(stream->intConfig);
    IF_SAFETY_HIGH( \
        if(internalStruct->current - internalStruct->buffer >= \
            BUFFERED_STREAM_BUFFER_STREAM) { \
            ERROR(ERR_BUFFER_OVERFLOW, "charPushBack(): Unable to push more chars onto read buffer"); \
            return; \
        });

    *(internalStruct->current++) = c;
}


struct CharBufferedReadStream *resetCharBufferedReadStream(struct CharBufferedReadStream
        *stream) {
    struct InternalCharBufferedStream *internal;

    assert(stream && stream->intConfig);

    internal = stream->intConfig;
    internal->current = internal->buffer;
    return stream;
}


void disposeCharBufferedReadStream(struct CharBufferedReadStream *stream) {
    struct InternalCharBufferedStream *internalStruct;

    assert(stream && stream->intConfig);

    internalStruct = (struct InternalCharBufferedStream *)stream->intConfig;
    free(internalStruct->buffer);
    free(internalStruct);
    free(stream);
}


struct CharBufferedReadStream *makeCharBufferedReadStream(struct CharReadStream
        *stream) {
    struct CharBufferedReadStream *bufStream = (struct CharBufferedReadStream *)
        malloc(sizeof(struct CharBufferedReadStream));
    struct InternalCharBufferedStream *intStream = 
        (struct InternalCharBufferedStream *) 
        malloc(sizeof(struct InternalCharBufferedStream));
    intStream->current = intStream->buffer = 
        (char *)malloc(
            sizeof(char) * BUFFERED_STREAM_BUFFER_STREAM);
    intStream->readStream = stream;
    bufStream->intConfig  = (void *)intStream;
    bufStream->pushBack   = charPushBack;
    bufStream->getNext    = getNextWrapper;
    bufStream->dispose    = disposeCharBufferedReadStream;
    bufStream->status     = statusReadStreamWrapper;
    return bufStream;
} 



/******************************************************************************
 *                           CStreamCharWriteStream
 ******************************************************************************/


struct CStreamCharWriteStream {
    FILE *stream;
    char *buffer;
    char *current;
    int bufferSize;
};


/** 
 * Write a char to a file 
 * @param intConfig pointer to a fileStruct
 * @param c the char to write to
 * @return the number of chars written, thus 0 if an error occured or 1 if
 * successful 
 */
int writeBufferedCharToFile(void *intConfig, char c) {
    int length = 1;
    struct CStreamCharWriteStream *cfg = 
        (struct CStreamCharWriteStream *)intConfig;
    assert(intConfig);
    *(cfg->current++) = c;
    if(cfg->current - cfg->buffer <= cfg->bufferSize) {
        *cfg->current = 0;
        length = fprintf(cfg->stream,"%s", cfg->buffer);
        fflush(cfg->stream);
        cfg->current = cfg->buffer;
    }
    return length;
}


/**
 * Write a char to a file (unbuffered)
 * @param intConfig pointer to a FILE
 * @param c the char to write
 * @return the number of chars written, thus 1 if successful, 0 if unsuccessful
 */
int writeCharToFile(void *intConfig, char c){
    assert(intConfig);
    return fputc(c, (FILE *)intConfig);
}


void disposeCStreamCharWriteStream(struct CharWriteStream *stream) {
    struct CStreamCharWriteStream *cfg;
    assert(stream);

    if(stream->write != writeCharToFile) {
       cfg = (struct CStreamCharWriteStream *)stream->intConfig; 
       if(cfg->buffer < cfg->current) 
           *cfg->current = 0;
           fprintf(cfg->stream,"%s", cfg->buffer);
           free(cfg->buffer);
           free(cfg);
    }
    free(stream);
}


struct CharWriteStream *makeCStreamCharWriteStream(int bufferSize, FILE *file) {
    struct CStreamCharWriteStream *cfg;
    struct CharWriteStream *stream = 
        (struct CharWriteStream *)malloc(sizeof(struct CharWriteStream));
    assert(file);

    if(bufferSize == 0) {
        stream->intConfig = (void *)file;
        stream->write = writeCharToFile;
        stream->dispose = disposeCStreamCharWriteStream;
    } else {
        stream->intConfig = cfg = (void *)malloc(
                sizeof(struct CStreamCharWriteStream));
        cfg->stream =(FILE *)file;
        cfg->buffer = cfg->current = 
        (char *)malloc(sizeof(char) * (bufferSize + 1));
        stream->write = writeBufferedCharToFile;
        stream->dispose = disposeCStreamCharWriteStream;
        cfg->bufferSize = bufferSize;
    }
    return stream;
}



/******************************************************************************
 *       Write to a string
 ******************************************************************************/


struct intStringWriteStream {
    int index;
    int strLength;
    char *strBuffer;
};


int writeCharToString(void *intConfig, char c) {
    struct intStringWriteStream *cfg = (struct intStringWriteStream *)intConfig;
    if(cfg->index < cfg->strLength - 1) {
        cfg->strBuffer[cfg->index] = c;
        cfg->index++;
        cfg->strBuffer[cfg->index] = '\0';
    } else {
        ERROR(ERR_BUFFER_OVERFLOW, "writeCharToString: string size exceeded");
        return 0;
    }
    return 1;
}


void disposeStringCharWriteStream(struct CharWriteStream *stream) {
    free(stream->intConfig);
    free(stream);
}
        

struct CharWriteStream *makeStringCharWriteStream(int stringLength, char
        *string) {
    struct CharWriteStream *stream = (struct CharWriteStream
            *)malloc(sizeof(struct CharWriteStream));
    struct intStringWriteStream *intStream = (struct intStringWriteStream
            *)malloc(sizeof(struct intStringWriteStream));
    stream->intConfig = intStream;
    stream->write = writeCharToString;
    stream->dispose = disposeStringCharWriteStream;
    intStream->index = 0;
    intStream->strLength = stringLength;
    intStream->strBuffer = string;
    return stream;
}

