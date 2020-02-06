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

#include "streams.h"
#include "assert.h"
#include "error.h"
#include "safety.h"
#include "stdlib.h"

#define MODULE_NAME "streams.c"

#ifdef DEBUG_STREAMS
#include "debugging.h"
#else
#include "no_debugging.h"
#endif

#define CHECK_AND_RECOVER_BUFFER_SANITY(buf, bufLen, functionName) \
    do {                                                           \
        size_t localBufLen = bufLen;                               \
        if (SAFE_STRING_IS_TAINTED(buf, localBufLen)) {            \
            SAFE_STRING_RESET_TAINTED(buf, localBufLen);           \
            ERROR(ERR_BUFFER_OVERFLOW, functionName                \
                  ": "                                             \
                  "Unable to push more chars onto buffer");        \
        }                                                          \
    } while (0)

void reportPushBackUnimplemented(struct CharReadStream *stream, char c) {
    ERROR(ERR_UNIMPLEMENTED, "Push back not implemented by this reader!");
}

/*****************************************************************************
 *               Unbuffered CharReadStream Reading from String
 *****************************************************************************/

struct IntStringCharReadStreamData {
    char *originalString;
    char *currentPos;
};

char getNextCharFromString(struct CharReadStream *stream) {
    struct IntStringCharReadStreamData *intData;
    char got;

    assert(stream);

    intData = (struct IntStringCharReadStreamData *)stream->intConfig;
    got = *(intData->currentPos);
    if (got != '\0') {
        intData->currentPos++;
    } else {
        got = STREAM_RETURN_CHAR_ON_ERROR;
    }
    return got;
}

void disposeStringCharReadStream(struct CharReadStream *stream) {
    struct IntStringCharReadStreamData *intData;

    assert(stream);

    intData = (struct IntStringCharReadStreamData *)stream->intConfig;
    free(intData->originalString);
    free(intData);
    free(stream);
}

int statusString(struct CharReadStream *stream) {
    struct IntStringCharReadStreamData *intData;

    assert(stream);

    intData = (struct IntStringCharReadStreamData *)stream->intConfig;
    if (!intData) return STREAM_STATUS_INTERNAL_ERROR;
    if (!intData->originalString) return STREAM_STATUS_INTERNAL_ERROR;
    if (!intData->currentPos) return STREAM_STATUS_INTERNAL_ERROR;
    if (intData->currentPos == 0) return STREAM_STATUS_EOS;
    return STREAM_STATUS_OK;
}

struct CharReadStream *makeStringCharReadStream(char *s) {
    struct CharReadStream *stream;
    struct IntStringCharReadStreamData *intData;
    size_t strLen;

    assert(s);

    strLen = strlen(s);
    intData = (struct IntStringCharReadStreamData *)SAFE_MALLOC(
        sizeof(struct IntStringCharReadStreamData));
    intData->originalString = (char *)SAFE_MALLOC(sizeof(char) * (strLen + 1));
    strncpy(intData->originalString, s, strLen);
    intData->originalString[strLen] = '\0';
    intData->currentPos = intData->originalString;
    stream = SAFE_MALLOC(sizeof(struct CharReadStream));
    stream->intConfig = (void *)intData;
    stream->getNext = getNextCharFromString;
    stream->dispose = disposeStringCharReadStream;
    stream->status = statusString;
    stream->pushBack = reportPushBackUnimplemented;
    return stream;
}

/*****************************************************************************
 *                         Unbuffered CharReadStream
 *****************************************************************************/

char getNextCharFromCFile(struct CharReadStream *stream) {
    int got;

    assert(stream);

    if (!stream->intConfig) return 0;
    if (feof((FILE *)stream->intConfig)) {
        got = STREAM_RETURN_CHAR_ON_ERROR;
    } else {
        got = getc((FILE *)(stream->intConfig));
        if (got == EOF) {
            got = STREAM_RETURN_CHAR_ON_ERROR;
        };
    }
    return (char)got;
}

void disposeCStreamCharReadStream(struct CharReadStream *stream) {
    assert(stream);
    free(stream);
}

int statusCStream(struct CharReadStream *stream) {
    FILE *file;

    assert(stream);

    file = stream->intConfig;

    if (!file) return STREAM_STATUS_INTERNAL_ERROR;
    if (ferror(file)) return STREAM_STATUS_GENERAL_ERROR;
    if (feof(file)) return STREAM_STATUS_EOS;
    return STREAM_STATUS_OK;
}

struct CharReadStream *makeCStreamCharReadStream(FILE *s) {
    struct CharReadStream *stream;

    assert(s);

    stream = SAFE_MALLOC(sizeof(struct CharReadStream));
    stream->getNext = getNextCharFromCFile;
    stream->dispose = disposeCStreamCharReadStream;
    stream->status = statusCStream;
    stream->pushBack = reportPushBackUnimplemented;
    stream->intConfig = (void *)s;
    return stream;
}

/*****************************************************************************
 *                             Buffered CharReadStream
 *****************************************************************************/

struct InternalCharBufferedStream {
    struct CharReadStream *readStream;
    char *current;
    char *buffer;
};

char getNextWrapper(struct CharReadStream *stream) {
    struct InternalCharBufferedStream *internalStruct;
    struct CharReadStream *readStream;

    assert(stream && stream->intConfig);

    internalStruct = (struct InternalCharBufferedStream *)(stream->intConfig);

    if (internalStruct->current != internalStruct->buffer)
        return *((--internalStruct->current));

    readStream = internalStruct->readStream;

    assert(readStream);

    return STREAM_NEXT(readStream);
}

int statusReadStreamWrapper(struct CharReadStream *stream) {
    struct InternalCharBufferedStream *intStruct;
    int status;

    assert(stream);

    intStruct = (struct InternalCharBufferedStream *)stream->intConfig;

    assert(intStruct && intStruct->readStream);

    status = STREAM_STATUS(intStruct->readStream);
    if (status == STREAM_STATUS_OK && intStruct->current != intStruct->buffer)
        return STREAM_STATUS_OK;

    return status;
}

void charPushBack(struct CharReadStream *stream, char c) {
    struct InternalCharBufferedStream *internalStruct;

    assert(stream && stream->intConfig);

    internalStruct = (struct InternalCharBufferedStream *)(stream->intConfig);
    IF_SAFETY_HIGH(if (internalStruct->current - internalStruct->buffer >=
                       BUFFERED_STREAM_BUFFER_STREAM) {
        ERROR(ERR_BUFFER_OVERFLOW,
              "charPushBack(): "
              "Unable to push more chars onto read buffer");
        return;
    });

    *(internalStruct->current++) = c;
    CHECK_AND_RECOVER_BUFFER_SANITY(
        internalStruct->buffer, BUFFERED_STREAM_BUFFER_STREAM, "charPushBack");
}

struct CharReadStream *resetCharReadStream(struct CharReadStream *stream) {
    struct InternalCharBufferedStream *internal;

    assert(stream && stream->intConfig);

    internal = stream->intConfig;
    internal->current = internal->buffer;
    return stream;
}

void disposeCharReadStream(struct CharReadStream *stream) {
    struct InternalCharBufferedStream *internalStruct;

    assert(stream && stream->intConfig);

    internalStruct = (struct InternalCharBufferedStream *)stream->intConfig;
    free(internalStruct->buffer);
    free(internalStruct);
    free(stream);
}

struct CharReadStream *makeCharReadStream(struct CharReadStream *stream) {
    struct CharReadStream *bufStream =
        (struct CharReadStream *)SAFE_MALLOC(sizeof(struct CharReadStream));
    struct InternalCharBufferedStream *intStream =
        (struct InternalCharBufferedStream *)SAFE_MALLOC(
            sizeof(struct InternalCharBufferedStream));
    intStream->current = intStream->buffer =
        SAFE_STRING_NEW(BUFFERED_STREAM_BUFFER_STREAM);
    intStream->readStream = stream;
    bufStream->intConfig = (void *)intStream;
    bufStream->pushBack = charPushBack;
    bufStream->getNext = getNextWrapper;
    bufStream->dispose = disposeCharReadStream;
    bufStream->status = statusReadStreamWrapper;
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
    CHECK_AND_RECOVER_BUFFER_SANITY(cfg->buffer, cfg->bufferSize,
                                    "writeBufferedCharToFile");
    if (cfg->current - cfg->buffer <= cfg->bufferSize) {
        *cfg->current = 0;
        length = fprintf(cfg->stream, "%s", cfg->buffer);
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
int writeCharToFile(void *intConfig, char c) {
    assert(intConfig);
    return fputc(c, (FILE *)intConfig);
}

void disposeCStreamCharWriteStream(struct CharWriteStream *stream) {
    struct CStreamCharWriteStream *cfg;
    assert(stream);

    if (stream->write != writeCharToFile) {
        cfg = (struct CStreamCharWriteStream *)stream->intConfig;
        if (cfg->buffer < cfg->current) {
            *cfg->current = 0;
        }
        fprintf(cfg->stream, "%s", cfg->buffer);
        free(cfg->buffer);
        free(cfg);
    }
    free(stream);
}

struct CharWriteStream *makeCStreamCharWriteStream(int bufferSize, FILE *file) {
    struct CStreamCharWriteStream *cfg;
    struct CharWriteStream *stream =
        (struct CharWriteStream *)SAFE_MALLOC(sizeof(struct CharWriteStream));
    assert(file);

    if (bufferSize == 0) {
        stream->intConfig = (void *)file;
        stream->write = writeCharToFile;
        stream->dispose = disposeCStreamCharWriteStream;
    } else {
        stream->intConfig = cfg =
            (void *)SAFE_MALLOC(sizeof(struct CStreamCharWriteStream));
        cfg->stream = (FILE *)file;
        cfg->buffer = cfg->current = SAFE_STRING_NEW(bufferSize);
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
    if (cfg->index < cfg->strLength - 1) {
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

struct CharWriteStream *makeStringCharWriteStream(int stringLength,
                                                  char *string) {
    struct CharWriteStream *stream =
        (struct CharWriteStream *)SAFE_MALLOC(sizeof(struct CharWriteStream));
    struct intStringWriteStream *intStream =
        (struct intStringWriteStream *)SAFE_MALLOC(
            sizeof(struct intStringWriteStream));
    stream->intConfig = intStream;
    stream->write = writeCharToString;
    stream->dispose = disposeStringCharWriteStream;
    intStream->index = 0;
    intStream->strLength = stringLength;
    intStream->strBuffer = string;
    return stream;
}
