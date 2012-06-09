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
 * All of them feature
 * and contain a field intConfig, a void pointer
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
 */


/** 
 * This represents an ordinary stream to be read from
 * Stream.getNext() will deliver another item from the stream
 */
struct ReadStream {
    struct Expression *(*getNext)(void *);
    void *intConfig;
};


/**
 * Get the next item from a stream
 * @param stream pointer to  the Stream to be read from
 * @return anything the stream got as return type
 */
#define STREAM_NEXT(stream) (stream->getNext(stream->intConfig))


/** 
 * The stdin stream - will return char Expressions
 */
/* extern struct ReadStream cStdIn; */

struct CharReadStream {
    char (*getNext)(void *);
    void *intConfig;
};


/**
 * Convert a charReadStream into an ordinary Stream
 * @param cStream the char stream to convert
 * @return a ReadStream wrapping the charStream
 */
struct ReadStream *charReadStreamToReadStream(struct CharReadStream *cStream);


/**
 * Create a charReadStream to read from a C-Stream
 * Will read and return chars
 */
struct CharReadStream *makeCStreamCharReadStream(FILE *s);



/******************************************************************************
 *                              Buffered Readers
 ******************************************************************************/



/**
 * A buffered stream is a stream that is able to push back signs to reread them
 * later on
 */
struct BufferedStream {
    struct Expression *(*getNext)(void *);
    void (*pushBack)(void *, struct Expression *);
    void *intConfig;
};


struct CharBufferedReadStream {
    char (*getNext)(void *);
    void (*pushBack)(void *, char); 
    void *intConfig;
};


/**
 * Push back a char onto a buffered stream
 * @param s buffered read stream
 * @param x entity to push back (in case of CharBufferedReadStream, a char)
 */
#define STREAM_PUSH_BACK(s, x) (s->pushBack(s->intConfig, x))


/**
 * Reset CharBufferedReadStream. Next read will start from beginning of buffer
 * again.
 * @param str stream to reset
 * @return the stream that has been reset
 */
struct CharBufferedReadStream *resetCharBufferedReadStream(struct CharBufferedReadStream *stream);


/**
 * This is the StdIn stream buffered
 */
struct CharBufferedReadStream *cBufStdIn;


struct CharBufferedReadStream *makeCharBufferedReadStream(struct CharReadStream *stream);


void disposeCharBufferedReadStream(struct CharBufferedReadStream *stream);



/******************************************************************************
 *                                Write Streams
 ******************************************************************************/


/**
 * Generic macro to write to a stream
 */
#define STREAM_WRITE(stream, x)  {stream->write(stream->intConfig, x);}


/**
 * a write stream basically takes one item and writes it whereever the stream
 * aims at 
 */
struct WriteStream {
    /**
     * writes an expression to this stream
     * @param intConfig 
     * @param expr
     * @return number of written bytes
     */
    int (*write)(void *, struct Expression *);
    void *intConfig;
};


struct CharWriteStream{
    /**
     * Writes a character to this stream
     * @param intConfig 
     * @param c character to write
     * @return number of written characters
     */
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
 * Disposes a CharWriteStream that was created with makeCStreamCharWriteStream
 * Remember: The FILE object wont be closed or even freed - you have to care for
 * this!
 * @param stream the stream to dispose
 */
void disposeCStreamCharWriteStream(struct CharWriteStream *stream);


/**
 * Creates a stream that writes into a string buffer
 * @param stringLength maximum length of the string 
 * @param string buffer to write to
 * @return pointer to CharWriteStream 
 */ 
struct CharWriteStream *makeStringCharWriteStream(int stringLength, char *string);


/**
 * Disposes a StringCharWriteStream 
 * @param  stream pointer to CharWriteStream to dispose
 */
void disposeStringCharWriteStream(struct CharWriteStream *stream);


/******************************************************************************
 *                             INTERNAL FUNCTIONS
 ******************************************************************************/


#endif
