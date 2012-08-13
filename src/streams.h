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
  
#include "expression.h"



#ifndef __STREAMS_H__
#define __STREAMS_H__

#include <stdio.h>
#include "config.h"



#define BUFFERED_STREAM_BUFFER_STREAM READ_BUFFER_SIZE



/**
 * In here, streams are considered streams of some data
 * they are represented by a function returning subsequent bits of this data
 * flow
 * You could also call them 'iterators' 
 * All of them contain a field intConfig, a void pointer.
 * In it, the streams internal data should be stored
 * For example, the stream reading from a FILE object keeps a pointer to its
 * FILE in it
 *
 *  a function getNext(void *) returns the next element of the stream
 * It takes the pointer stored in intConfig, thus getting the next element
 * collapses to the call stream->getNext(stream->intConfig)
 * To simpligy this call, use STREAM_NEXT to shorten this call:
 * STREAM_NEXT(stream)
 *
 * Buffered streams provide another function:
 * pushBack(el, intConfig)
 * They shovel back one element into the stream to be returned as the next
 * element
 *
 * If streams do not support buffering, i.e. do not provide the pushBack() -
 * function, they should instead install a dummy function that signals the error
 * ERR_UNIMPLEMENTED.
 *
 */



/*****************************************************************************
                             GENERAL STREAM MACROS
 *****************************************************************************/



/**
 * Get the next item from a stream
 * @param stream pointer to  the Stream to be read from
 * @return anything the stream got as return type
 */
#define STREAM_NEXT(stream) ((stream)->getNext((stream)))


/**
 * Push back a char onto a buffered stream
 * @param s buffered read stream
 * @param x entity to push back (in case of CharReadStream, a char)
 */
#define STREAM_PUSH_BACK(s, x) ((s)->pushBack((s), x))


/**
 * Get rid of a stream.
 * @param stream Pointer to a stream to dispose
 */
#define STREAM_DISPOSE(stream) ((stream)->dispose(stream))


/**
 * Get status of stream.
 * @param stream Pointer to a stream to dispose
 * @return int value being the status of the stream.
 */
#define STREAM_STATUS(stream) ((stream)->status(stream))



/*****************************************************************************
 *                                 ReadStream
 *****************************************************************************/



struct CharReadStream {
    char (*getNext) (struct CharReadStream *);
    void (*dispose) (struct CharReadStream *);
    int  (*status)  (struct CharReadStream *);
    void (*pushBack)(struct CharReadStream *, char); 
    void *intConfig;
};


/**
 * Character to return in case of some exceptional behaviour, i.e. an error or
 * end of stream...
 */
#define STREAM_RETURN_CHAR_ON_ERROR ((char)0)


/**
 * Create a charReadStream to read from a C-Stream
 * Will read and return chars
 */
struct CharReadStream *makeCStreamCharReadStream(FILE *s);



/**
 * Reset CharReadStream. Next read will start from beginning of buffer
 * again.
 * @param str stream to reset
 * @return the stream that has been reset
 */
struct CharReadStream *resetCharReadStream(struct CharReadStream *stream);


/**
 * Create a buffered stream that wraps around a charReadStream.
 * @param stream Pointer to a CharReadStream to be wrapped within a buffered
 * stream
 */
struct CharReadStream *makeCharReadStream(struct CharReadStream *stream);



/******************************************************************************
 *                                Write Streams
 ******************************************************************************/


/**
 * Generic macro to write to a stream
 */
#define STREAM_WRITE(stream, x)  {stream->write(stream->intConfig, x);}


struct CharWriteStream{
    int (*status)(struct CharWriteStream *);
    void (*dispose)(struct CharWriteStream *);
    int (*write)(void *, char);
    void *intConfig;
};


/**
 * Creates a WriteStream that writes to a C file
 * @param bufferSize the size for the buffer, if zero buffering is omitted
 * @param file pointer to an open c file, must be writeable
 */
struct CharWriteStream *makeCStreamCharWriteStream(int bufferSize, FILE *file);


/**
 * Creates a stream that writes into a string buffer
 * @param stringLength maximum length of the string 
 * @param string buffer to write to
 * @return pointer to CharWriteStream 
 */ 
struct CharWriteStream *makeStringCharWriteStream(int stringLength, char *string);



/******************************************************************************
 *                              STATUS CONSTANTS
 ******************************************************************************/


/**
 * Stream is ready to be used.
 */
#define STREAM_STATUS_OK ((int)0)

/** 
 * Stream has reached end of stream (EOS).
 */
#define STREAM_STATUS_EOS ((int)100)

/**
 * Stream has encountered an internal error. 
 */
#define STREAM_STATUS_INTERNAL_ERROR ((int)-1)

/** 
 * Stream has encountered an error that is not further specified.
 */
#define STREAM_STATUS_GENERAL_ERROR ((int)1)



/******************************************************************************
 *                            Unimplemented stuff
 ******************************************************************************/



/** 
 * This represents an ordinary stream to be read from
 * Stream.getNext() will deliver another item from the stream
 */
/* struct ReadStream { */
/*     struct Expression *(*getNext)(struct ReadStream *); */
/*     void (*dispose)(struct ReadStream *); */
/*     int (*status)(struct ReadStream *); */
/*     void *intConfig; */
/* }; */


/**
 * Convert a charReadStream into an ordinary Stream
 * @param cStream the char stream to convert
 * @return a ReadStream wrapping the charStream
 */
/* struct ReadStream *charReadStreamToReadStream(struct CharReadStream *cStream); */


/* struct BufferedReadStream { */
/*     struct Expression *(*getNext)(struct BufferedReadStream *); */
/*     void (*dispose)(struct BufferedReadStream *); */
/*     int (*status)(struct BufferedReadStream *); */
/*     void (*pushBack)(struct BufferedReadStream *, struct Expression *); */
/*     void *intConfig; */
/* }; */


/**
 * a write stream basically takes one item and writes it whereever the stream
 * aims at 
 */
/* struct WriteStream { */
/*     int (*status)(struct WriteStream *); */
/*     void (*dispose)(struct WriteStream *); */
/*     int (*write)(void *, struct Expression *); */
/*     void *intConfig; */
/* }; */



#endif
