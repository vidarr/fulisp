/*
 * (C) 2012 Michael J. Beer
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */
 

#ifndef __ERROR_H__
#define __ERROR_H__
#include <stdlib.h>

/**
 * Reset error state
 */
#define NO_ERROR (lispError == ERR_OK)

/** 
 * Signal an error
 */
#ifdef NO_MASK_ERROR 

#    define ERROR_BASIC(e, m) {if(NO_ERROR) { \
        lispFile = __FILE__; lispLine = __LINE__; lispError = e; \
        lispErrorMessage = m; \
     }}

#else 

#    define ERROR_BASIC(e, m) {lispFile = __FILE__; lispLine = __LINE__; lispError = e; \
        lispErrorMessage = m;}

#endif

#ifdef EXIT_ON_ERROR
#    define ERROR(e, m) {ERROR_BASIC(e, m); abort();}
#else
#    define ERROR(e, m) ERROR_BASIC(e, m)
#endif


#define ERROR_RESET { \
    lispError = 0; lispErrorMessage = 0;};

/**
 * Signal a warning
 */
#define WARNING(w, m) {lispFile = __FILE__; lispLine = __LINE__; lispWarning = w; lispWarningMessage = m;}

/**
 * Signal an unrecoverable error - abort the process
 */
#define PANIC(p, m) {fprintf(stderr, "fuLisp Panic %3i: %s\n", p, m); exit(1);}

/**
 * The error codes 
 */
#define ERR_OK 0
#define ERR_NIL_VALUE 1
#define ERR_SYNTAX_ERROR 2
#define ERR_BUFFER_OVERFLOW 100
#define ERR_UNEXPECTED_TYPE 200
#define ERR_UNEXPECTED_VAL  300
#define ERR_EXPECTED_FUNCTION 310
#define ERR_UNEXPECTED_END_OF_STRING  500
#define ERR_UNBALANCED_PARENTHESES 600
#define ERR_UNEVEN_SYM_VAL 700
#define ERR_UNRESOLVABLE 1100
#define ERR_STACK_OVERFLOW 1210
#define ERR_STACK_UNDERFLOW 1211
#define ERR_UNIMPLEMENTED 2000
#define ERR_DIVISION_BY_ZERO 3000

extern int lispError;
extern char *lispErrorMessage;
extern int lispWarning;
extern char *lispWarningMessage;

extern char *lispFile;
extern int lispLine;
 

#endif

