/*
 * (C) 2011 Michael J. Beer
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
 *
 * Safe code will only be generated if GENERATE_SAFETY_CODE has been set
 * Else, the following defs will have *no* effekt
 */
  
#ifndef __SAFETY_H__
#define __SAFETY_H__

#include "config.h"


/**
 * This determines the number of safety checks that will be performed
 */
extern int safetyLevel;


/*****************************************************************************
 *        Security levels
 *****************************************************************************/

/**
 * Perform all safety tests
 */
#define SAFETY_HIGH 255

/**
 * Perform no safety checks
 */
#define SAFETY_LOW    0



/******************************************************************************
 *      Macros to execute code if some safety level has been set
 *****************************************************************************/

/**
 * Execute code only if safety level is set to SAFETY_HIGH
 */
#define IF_SAFETY_HIGH(x) __IF_SAFETY_HIGH(x)

/** 
 * Compiles the code given if GENERATE_SAFE_CODE is set
 */
#define IF_SAFETY_CODE(x) __IF_SAFETY_CODE(x)

/**
 * Compilse the given code if GENERATE_SAFE_CODE is not set
 */
#define IF_NOT_SAFETY_CODE(c) __IF_NOT_SAFETY_CODE(c)


/*****************************************************************************
 *                           Manipulate safety level
 *****************************************************************************/

/**
 * Set safety level
 */
#define SET_SAFETY_LEVEL(x) __SET_SAFETY_LEVEL(x)

/**
 * Set safety level to SAFETY_LOW
 */
#define SET_SAFETY_LOW SET_SAFETY_LEVEL(SAFETY_LOW)


/**
 * Set safety level to SAFETY_HIGH
 */
#define SET_SAFETY_HIGH SET_SAFETY_LEVEL(SAFETY_HIGH)


/*****************************************************************************
 *                  PREVENT USING UNSAFE STRING FUNCTIONS
 *
 * In order to prevent buffer overflows, this code should enforce the use of 
 * safe string functions. These functions take a length argument and will
 * cause the program to terminate if a function would operate outside of 
 * this length.
 *
 *****************************************************************************/


/* #define strlen unsafe_strlen   */
/* #define strcpy unsafe_strcpy */
/* #define strcmp unsafe_strcmp */
#define sprintf unsafe_sprintf 

/**
 * Safe version of sprintf(3) .
 * Should always be preferred to sprintf(3) / snprintf(3)
 * Semtantics as snprintf(3) :
 * int virtualStrlen = 
 *    SAFE_SPRINTF(targetStr, targetStrMaxLen, FORMAT_STRING, ...)
 */
#define SAFE_SPRINTF             safeSprintf

/** 
 * strnlen(3) replacement as it is not available in C89
 * Its not always recommended to use this instead of strlen(3) !
 */
#define SAFE_STRLEN(x, len)      __SAFE_STRLEN(x, len)

/** 
 * strlcpy(3) replacement as it is not available in C89
 */
#define SAFE_STRCPY(d, s, len)   __SAFE_STRCPY(d, s, len)

/** 
 * strncmp(3) replacement as it is not available in C89
 */
#define SAFE_STRCMP(s1, s2, len) __SAFE_STRCMP(s1, s2, len)


/*===========================================================================*
 *                             I N T E R N A L S
 *===========================================================================*/

int safeSprintf(char *str, size_t size, const char *format, ...);


#ifdef GENERATE_SAFETY_CODE


#define __IF_SAFETY_HIGH(x) { if(safetyLevel == SAFETY_HIGH) {x}}
#define __IF_SAFETY_CODE(x) {x}
#define __IF_NOT_SAFETY_CODE(c) 
#define __SET_SAFETY_LEVEL(x) {safetyLevel = x;}

#define __SAFE_STRLEN(x, len)      safeStrlen(x, len)
#define __SAFE_STRCPY(d, s, len)   safeStrcpy(d, s, len)
/* Has not been implemented yet */
#define __SAFE_STRCMP(s1, s2, len) strcmp(s1, s2)

size_t safeStrlen(const char *str, size_t maxLen);
char * safeStrcpy(char *dest,const char *src, size_t maxLen);

#else
/* if no safe code should be generated, just remove all safety macros */

#define __IF_SAFETY_HIGH(x)
#define __IF_SAFETY_CODE(x)
#define __IF_NOT_SAFETY_CODE(c) {c}
#define __SET_SAFETY_LEVEL(x)

#define __SAFE_STRLEN(x, len)      strlen(x)
#define __SAFE_STRCPY(d, s, len)   strcpy(d, s)
#define __SAFE_STRCMP(s1, s2, len) strcmp(s1, s2)


#endif


#endif
